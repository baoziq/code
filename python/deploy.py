import socket
import shutil
import subprocess
from pathlib import Path

base_dir = "../SNSPLLua"
def get_type():
    hostname = socket.gethostname()
    index = hostname.find("SNS")
    if index == -1:
        return "CCS"
    else:
        return "SNS"

def get_path(domain):
    base = Path(base_dir)
    for d in base.iterdir():
        if d.is_dir() and d.name == domain:
            return str(d.resolve())
        
    return None


def get_layer():
    base = Path(base_dir)
    type = get_type()
    dirs = [d.name for d in base.iterdir() if d.is_dir()]

    if "CCS" in dirs and "SNS" in dirs:
        if type == "CCS":
            return "CCS"
        else:
            return "SNS"
    else:
        if type == "CCS":
            return "L2"
        else:
            return "L1"


def update_nginx(domain):
    src = base_dir + "/" + get_path(domain) + "/" + get_layer() + "/nginx"
    dst = "/etc/nginx/sites-enabled/"
    shutil.copy(src, dst)
    subprocess.run(["nginx", "-s", "reload"])

def update_ats(domain):
    src = base_dir + "/" + get_path(domain) + "/" + get_layer() + "/ats"
    dst = "/etc/trafficserver/"
    src_dir = Path(src)
    dst_dir = Path(dst)
    for src_file in src_dir.iterdir():
        if src_file.is_file():
            dst_file = dst_dir / src_file.name
        with open(src_file, "r", encoding = "utf-8") as f_src, \
                open(dst_file, "w", encoding = "utf-8") as f_dst:
            content = f_src.read()
            f_dst.write("\n")
            f_dst.write(content)

        print(f"✅ 已插入 {src_file.name} 到 {dst_file}")

    subprocess.run(["traffic_ctl", "server", "reload"])
    subprocess.run(["tail", "-f", "/var/log/trafficserver/diags.log"])

if __name__ == "__main__":
    print("输入域名")
    domain = input()
    if (get_path(domain) is None):
        print("域名不存在")
    else:
        update_nginx(domain)
        update_ats(domain)
