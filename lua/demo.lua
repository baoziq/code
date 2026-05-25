local http = require "utils.httpc.http"
local resty_aes = require "common_aes"
local _M = {}

local DEFAULT_KEY = string.char(
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f
)

local AUTH_MAX_AGE_SECONDS = 300
local HW_FACTORY = "HW"
local ZTE_FACTORY = "ZTE"

local UPSTREAM_ZTE = { name = "ZTE", scheme = "http", host = "10.12.7.141", port = 804, timeout = 3000 }
local UPSTREAM_HW = { name = "HW", scheme = "http", host = "10.12.7.141", port = 805, timeout = 3000 }

local function split(str, sep)
    local out = {}
    if str == "" then
        return out
    end
    for part in string.gmatch(str, "([^" .. sep .. "]+)") do
        out[#out + 1] = part
    end
    return out
end

local function pack_ipv4(ip)
    local octets = split(ip, "%.")
    if #octets ~= 4 then
        return nil
    end

    local bytes = {}
    for i = 1, 4 do
        local n = tonumber(octets[i])
        if not n or n < 0 or n > 255 then
            return nil
        end
        bytes[i] = string.char(n)
    end
    return table.concat(bytes)
end

local function ipv4_groups(ip)
    local octets = split(ip, "%.")
    if #octets ~= 4 then
        return nil
    end

    local a, b, c, d = tonumber(octets[1]), tonumber(octets[2]), tonumber(octets[3]), tonumber(octets[4])
    if not a or not b or not c or not d then
        return nil
    end

    return {
        string.format("%x", a * 256 + b),
        string.format("%x", c * 256 + d),
    }
end

local function pack_ipv6(ip)
    local left, right = ip:match("^(.-)::(.-)$")
    local left_groups = left ~= nil and (left ~= "" and split(left, ":") or {}) or split(ip, ":")
    local right_groups = left ~= nil and (right ~= "" and split(right, ":") or {}) or {}

    if #right_groups > 0 and right_groups[#right_groups]:find("%.") then
        local groups = ipv4_groups(right_groups[#right_groups])
        if not groups then
            return nil
        end
        right_groups[#right_groups] = groups[1]
        right_groups[#right_groups + 1] = groups[2]
    elseif #left_groups > 0 and left_groups[#left_groups]:find("%.") then
        local groups = ipv4_groups(left_groups[#left_groups])
        if not groups then
            return nil
        end
        left_groups[#left_groups] = groups[1]
        left_groups[#left_groups + 1] = groups[2]
    end

    local missing = 8 - (#left_groups + #right_groups)
    if (left == nil and missing ~= 0) or missing < 0 then
        return nil
    end

    local groups = {}
    for _, v in ipairs(left_groups) do
        groups[#groups + 1] = v
    end
    for _ = 1, missing do
        groups[#groups + 1] = "0"
    end
    for _, v in ipairs(right_groups) do
        groups[#groups + 1] = v
    end
    if #groups ~= 8 then
        return nil
    end

    local bytes = {}
    for i = 1, 8 do
        local n = tonumber(groups[i], 16)
        if not n or n < 0 or n > 0xffff then
            return nil
        end
        bytes[#bytes + 1] = string.char(math.floor(n / 256))
        bytes[#bytes + 1] = string.char(n % 256)
    end
    return table.concat(bytes)
end

local function client_ip_binary()
    local ip = ngx.var.remote_addr or ""
    return ip:find(":") and pack_ipv6(ip) or pack_ipv4(ip)
end

local function build_auth_source_candidates(plain)
    local candidates = {}
    local total_len = #plain
    local fixed_suffix = 32 + 16

    for _, ip_len in ipairs({ 4, 16 }) do
        for _, ts_len in ipairs({ 8, 16 }) do
            local stb_len = total_len - ip_len - fixed_suffix - ts_len
            if stb_len and stb_len > 0 then
                local pos = 1
                candidates[#candidates + 1] = {
                    stb_id = plain:sub(pos, pos + stb_len - 1),
                    ip_bin = plain:sub(pos + stb_len, pos + stb_len + ip_len - 1),
                    content_id = plain:sub(pos + stb_len + ip_len, pos + stb_len + ip_len + 31),
                    ts_hex = plain:sub(pos + stb_len + ip_len + 32, pos + stb_len + ip_len + 32 + ts_len - 1),
                    md5_bin = plain:sub(pos + stb_len + ip_len + 32 + ts_len, pos + stb_len + ip_len + 32 + ts_len + 15),
                }
            end
        end
    end

    return candidates
end

local function md5_matches(candidate)
    if not candidate.ts_hex:match("^[0-9A-Fa-f]+$") then
        return false
    end
    return ngx.md5_bin(candidate.stb_id .. candidate.ip_bin .. candidate.content_id .. candidate.ts_hex) ==
        candidate.md5_bin
end

local function timestamp_valid(ts_hex)
    local ts = tonumber(ts_hex, 16)
    return ts and math.abs(ngx.time() - ts) <= AUTH_MAX_AGE_SECONDS or false
end

local function content_id_matches(candidate, args)
    local request_content_id = args.ContentID or args.contentid or args.contentId
    return request_content_id and request_content_id ~= "" and request_content_id == candidate.content_id or false
end

local function decode_and_decrypt_auth(authinfo)
    local cipher_bin = ngx.decode_base64(ngx.unescape_uri(authinfo))
    if not cipher_bin then
        return nil, "failed to base64 decode authinfo"
    end

    local aes = resty_aes:new(DEFAULT_KEY, nil, resty_aes.cipher(128, "ecb"), {
        padding = resty_aes.pkcs7_padding
    })
    if not aes then
        return nil, "failed to init aes"
    end

    local plain = aes:decrypt(cipher_bin)
    if not plain then
        return nil, "failed to decrypt authinfo"
    end
    return plain
end

local function validate_request()
    local args = ngx.req.get_uri_args()
    local authinfo = args.Authinfo or args.authinfo
    if not authinfo or authinfo == "" then
        return false, "missing Authinfo", ngx.HTTP_FORBIDDEN
    end

    local client_ip = client_ip_binary()
    if not client_ip then
        return false, "unsupported client ip", ngx.HTTP_FORBIDDEN
    end

    local plain, err = decode_and_decrypt_auth(authinfo)
    if not plain then
        return false, err, ngx.HTTP_FORBIDDEN
    end

    for _, candidate in ipairs(build_auth_source_candidates(plain)) do
        if candidate.ip_bin == client_ip
            and content_id_matches(candidate, args)
            and timestamp_valid(candidate.ts_hex)
            and md5_matches(candidate) then
            return true
        end
    end

    return false, "auth validation failed", ngx.HTTP_FORBIDDEN
end

-- 提取 HW 返回的 Location，支持绝对 http URL 和相对路径。
local function parse_location(location, upstream)
    if not location or location == "" then
        return nil, "missing location"
    end

    local scheme, host, port, path = location:match("^(https?)://([^/:?]+):?(%d*)(.*)$")
    if scheme and host then
        -- if scheme ~= "http" then
        --     return nil, "unsupported location scheme: " .. scheme
        -- end
        if path == "" then
            path = "/"
        elseif path:sub(1, 1) == "?" then
            path = "/" .. path
        elseif path:sub(1, 1) ~= "/" then
            return nil, "invalid location path: " .. location
        end
        local default_port = scheme == "https" and 443 or 80
        return {
            scheme = scheme,
            host = host,
            port = tonumber(port) or default_port,
            path = path,
        }
    end

    if location:sub(1, 1) == "/" then
        return {
            scheme = upstream.scheme,
            host = upstream.host,
            port = upstream.port,
            path = location,
        }
    end

    return nil, "invalid location: " .. location
end

local function hw_resolve_headers()
    local headers = {
        ["Host"] = ngx.var.host,
        ["User-Agent"] = ngx.var.http_user_agent,
        ["X-Real-IP"] = ngx.var.remote_addr,
        ["X-Forwarded-For"] = ngx.var.proxy_add_x_forwarded_for,
        ["Client-Factory"] = HW_FACTORY,
    }

    for k, v in pairs(headers) do
        if not v or v == "" then
            headers[k] = nil
        end
    end

    return headers
end

local function resolve_hw_target(request_uri)
    local httpc = http.new()
    httpc:set_timeout(UPSTREAM_HW.timeout)

    local ok, err = httpc:connect(UPSTREAM_HW.host, UPSTREAM_HW.port)
    if not ok then
        return nil, err
    end

    local res
    res, err = httpc:request({
        method = "GET",
        path = request_uri,
        headers = hw_resolve_headers(),
    })
    httpc:close()

    if not res then
        return nil, err
    end

    if res.status ~= 301 and res.status ~= 302 then
        return nil, "HW resolve failed with status " .. res.status, res.status
    end

    local location = res.headers and (res.headers["Location"] or res.headers["location"])
    local target, parse_err = parse_location(location, UPSTREAM_HW)
    if not target then
        return nil, parse_err, res.status
    end

    return target, nil, res.status
end

local function set_target(prefix, upstream, target)
    ngx.var[prefix .. "_host"] = target.host
    ngx.var[prefix .. "_scheme"] = target.scheme
    ngx.var[prefix .. "_port"] = tostring(target.port)
    ngx.var[prefix .. "_uri"] = target.path
    ngx.var[prefix .. "_name"] = upstream.name
end

local function zte_target(request_uri)
    return {
        host = UPSTREAM_ZTE.host,
        port = UPSTREAM_ZTE.port,
        path = request_uri,
        scheme = UPSTREAM_ZTE.scheme,
    }
end

local function resolve_target(upstream, request_uri)
    if upstream.name == UPSTREAM_HW.name then
        return resolve_hw_target(request_uri)
    end

    if upstream.name == UPSTREAM_ZTE.name then
        return zte_target(request_uri), nil, 200
    end

    return nil, "unknown upstream: " .. tostring(upstream.name)
end

-- 上游选择
local function upstream_order()
    local factory = ngx.req.get_headers()["Client-Factory"]
    if factory == HW_FACTORY then
        return { UPSTREAM_HW, UPSTREAM_ZTE }
    elseif factory == ZTE_FACTORY then
        return { UPSTREAM_ZTE, UPSTREAM_HW }
    end
    return nil
end

local function upstream_by_name(name)
    if name == UPSTREAM_HW.name then
        return UPSTREAM_HW
    end
    return UPSTREAM_ZTE
end

function _M.run()
    local ok, validation_err, validation_status = validate_request()
    if not ok then
        ngx.status = validation_status
        ngx.say(validation_err)
        return ngx.exit(validation_status)
    end

    local order = upstream_order()
    if not order then
        ngx.status = ngx.HTTP_FORBIDDEN
        ngx.say("unsupported Client-Factory")
        return ngx.exit(ngx.HTTP_FORBIDDEN)
    end

    local primary_upstream = order[1]
    local fallback_upstream = order[2]
    local target, err, status = resolve_target(primary_upstream, ngx.var.request_uri)

    if target then
        set_target("target", primary_upstream, target)
        ngx.var.fallback_name = fallback_upstream.name
        return ngx.exec("@origin_proxy")
    end

    ngx.log(ngx.WARN, "resolve primary upstream ", primary_upstream.name, " failed. err=", err or "", ", status=",
        status or "nil")

    target, err, status = resolve_target(fallback_upstream, ngx.var.request_uri)
    if target then
        set_target("target", fallback_upstream, target)
        ngx.var.fallback_name = ""
        return ngx.exec("@origin_proxy")
    end

    ngx.log(ngx.WARN, "resolve fallback upstream ", fallback_upstream.name, " failed. err=", err or "", ", status=",
        status or "nil")

    ngx.status = ngx.HTTP_BAD_GATEWAY
    ngx.say("all upstream resolves failed")
    return ngx.exit(ngx.HTTP_BAD_GATEWAY)
end

function _M.fallback()
    local fallback_name = ngx.var.fallback_name
    if not fallback_name or fallback_name == "" then
        ngx.status = ngx.HTTP_BAD_GATEWAY
        ngx.say("fallback upstream unavailable")
        return ngx.exit(ngx.HTTP_BAD_GATEWAY)
    end

    local upstream = upstream_by_name(fallback_name)
    local target, err, status = resolve_target(upstream, ngx.var.request_uri)
    if not target then
        ngx.log(ngx.WARN, "resolve fallback upstream ", upstream.name, " failed. err=", err or "", ", status=",
            status or "nil")
        ngx.status = ngx.HTTP_BAD_GATEWAY
        ngx.say("fallback upstream failed")
        return ngx.exit(ngx.HTTP_BAD_GATEWAY)
    end

    set_target("fallback", upstream, target)
    return ngx.exec("@fallback_proxy")
end

return _M
