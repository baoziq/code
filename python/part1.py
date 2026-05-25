import re
import pandas as pd 

datas = []
with open("data.txt", "r", encoding="utf-8") as f:
    s = [line.strip() for line in f if line.strip()]

for data in s:
    desc = re.search(r'[\u4e00-\u9fa5]+', data).group()
    domain = re.search(r'[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)+', data).group()
    if [domain, desc] not in datas:
        datas.append([domain, desc])
    


df = pd.DataFrame(datas)
df.to_excel(
        "result.xlsx",
        index=False,
        header=False,
        engine="openpyxl"
)
