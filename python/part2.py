import re
from collections import defaultdict
import pandas as pd 

data = []

with open("data.txt", "r", encoding="utf-8") as f:
    s = [line.strip() for line in f if line.strip()]

rows = []

domain_total = defaultdict(int)

for line in s:
    domain = re.search(r'[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)+', line).group()
    domain_total[domain] += 1

domain_order = defaultdict(int)

for idx, line in enumerate(s, start=1):
    # 域名
    domain = re.search(r'[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)+', line).group()
    domain_order[domain] += 1 
    forward = domain_order[domain]
    reverse = domain_total[domain] - forward + 1 

    # 节点 L1 / L2
    level = re.search(r'/L([12])/', line).group(1)
    node = f"节点L{level}"

    # 配置文件名
    filename = re.search(
        r'/(ats|nginx)/([^/\s]+)',
        line
    ).group(2)
    #print(filename)

    # etc 路径
    etc_path = re.search(r'(/etc/[^"]+)', line).group(1)
    print(etc_path) 
    full_path = f"{etc_path}{filename}"
    #print(full_path)
    rows.append([
        domain,               # 域名
        domain,               # 域名
        "新增",               # 操作
        node,                 # 节点
        full_path,            # 配置路径
        forward,                  # 正序号
        reverse       # 倒序号
    ])

df = pd.DataFrame(rows)
df.to_excel("result1.xlsx", index=False, header=False)
