---@class ngx
ngx = {}

ngx.OK = 200
ngx.ERROR = -1
ngx.HTTP_OK = 200
ngx.HTTP_MOVED_TEMPORARILY = 302
ngx.HTTP_FORBIDDEN = 403
ngx.HTTP_NOT_FOUND = 404
ngx.HTTP_INTERNAL_SERVER_ERROR = 500

---@class ngx.req
ngx.req = {}

---@return table<string, string|string[]>
function ngx.req.get_headers() end

---@return string
function ngx.req.get_method() end

---@return string|nil
function ngx.req.get_body_data() end

function ngx.req.read_body() end

---@class ngx.var
ngx.var = {}

---@class ngx.ctx
ngx.ctx = {}

---@class ngx.header
ngx.header = {}

---@param status integer
function ngx.exit(status) end

---@param ... any
function ngx.say(...) end

---@param ... any
function ngx.print(...) end

---@param level integer
---@param ... any
function ngx.log(level, ...) end

---@param uri string
function ngx.exec(uri) end

---@param uri string
function ngx.redirect(uri) end

---@param ms integer
function ngx.sleep(ms) end

---@class ngx.re
ngx.re = {}

---@return string|nil
function ngx.re.match(subject, regex, options) end

---@return string|nil
function ngx.re.gsub(subject, regex, replace, options) end

---@return string|nil
function ngx.re.sub(subject, regex, replace, options) end
