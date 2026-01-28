import pandas as pd
import glob
import os

# ========= 配置 =========
folder = r"C:\Users\23286\Desktop\设施标识码需合并\新增1"
output = os.path.join(folder, "项目编码汇总.xlsx")

# 模糊关键字（越短越稳）
KEY_PROJECT = ["项目名称"]
KEY_START = ["起点编码"]
KEY_END = ["终点编码"]

# =======================

def find_col(cols, keywords):
    """在列名中智能匹配关键字"""
    for k in keywords:
        for c in cols:
            if k in c:
                return c
    return None

files = glob.glob(os.path.join(folder, "*.xls*"))

rows = []
success, fail = 0, 0

for f in files:
    try:
        df = pd.read_excel(f, dtype=str)
        df.columns = [str(c).strip().lower() for c in df.columns]

        col_proj = find_col(df.columns, KEY_PROJECT)
        col_start = find_col(df.columns, KEY_START)
        col_end = find_col(df.columns, KEY_END)

        if not all([col_proj, col_start, col_end]):
            raise ValueError("缺少必要列")

        tmp = pd.DataFrame({
            "项目名称": df[col_proj],
            "编码区间": df[col_start].fillna("") + "-" + df[col_end].fillna("")
        })

        rows.append(tmp)
        success += 1
        print(f"✅ 成功：{os.path.basename(f)}")

    except Exception as e:
        fail += 1
        print(f"❌ 失败：{os.path.basename(f)} | {e}")

# 合并输出
if rows:
    pd.concat(rows, ignore_index=True).to_excel(output, index=False)

print(f"\n📊 总文件 {len(files)} 个 | 成功 {success} | 失败 {fail}")
