import pandas as pd

# 10个新域名列表
domains = [
    "v5-se-ex-mc-default.365yg.com",
    "v5-se-ex-mc-reading-video-a.qznovelvod.com",
    "v95-zjbwz-mc-reading-video.qznovelvod.com",
    "v95-zjbjx-mc-reading-video.qznovelvod.com",
    "v95-bjb-mc-reading-video.qznovelvod.com",
    "v5-se-ex-mc-awememusicpgc.amemv.com",
    "v5-se-ex-mc-wha.douyinvod.com",
    "v95-zjbwz-mc-wha.douyinvod.com",
    "v95-zjbjx-mc-wha.douyinvod.com",
    "v95-bjb-mc-wha.douyinvod.com"
]

data = []

for dom in domains:
    # 按照模板定义的 7 个步骤
    steps = [
        [dom, dom, "修改", "节点L1", "/etc/trafficserver/parent.config", "1", "7"],
        [dom, dom, "修改", "节点L1", "/etc/trafficserver/remap.config", "2", "6"],
        [dom, dom, "修改", "节点L1", f"/etc/nginx/sites-enabled/{dom}.conf", "3", "5"],
        [dom, dom, "修改", "节点L2", "/etc/trafficserver/remap.config", "4", "4"],
        [dom, dom, "修改", "节点L2", f"/etc/nginx/sites-enabled/{dom}-l2.conf", "5", "3"],
        [dom, dom, "修改", "节点L3", f"/etc/nginx/sites-enabled/{dom}3-l2.conf", "6", "2"],
        [dom, dom, "修改", "节点L3", "/etc/trafficserver/remap.config", "7", "1"]
    ]
    data.extend(steps)

# 创建 DataFrame
columns = ["域名1", "域名2", "操作类型", "节点级别", "完整路径", "步骤ID", "优先级"]
df = pd.DataFrame(data, columns=columns)

# 输出为 Excel 文件
output_file = "字节新域名配置清单.xlsx"
df.to_excel(output_file, index=False)

print(f"成功！文件已生成：{output_file}")