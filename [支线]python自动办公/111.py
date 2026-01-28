import pandas as pd
import glob
import os


folder_path = r"C:\Users\23286\Desktop\入库文件"
output_file = os.path.join(folder_path, "merged_smart.xlsx")
target_keywords = ["测绘长度", "管道材质", "断面数据", "设施标识码", "项目名称", "缺陷等级"]
excel_files = glob.glo
all_data = []
for file in excel_files:
    print(f"正在处理文件：{file}")
    try:
        df = pd.read_excel(file, dtype=str)  # dtype=str 避免数字列丢失前导零
        df.columns = [str(col).strip().lower() for col in df.columns]
        matched_cols = []
        for kw in target_keywords:
            kw_lower = kw.lower()
            for col in df.columns:
                if kw_lower in col:  # 列名中包含关键字就匹配
                    matched_cols.append(col)
        matched_cols = list(set(matched_cols))  # 去重
        if matched_cols:
            all_data.append(df[matched_cols])
            print(f"✅ 匹配列：{matched_cols}")
        else:
            print(f"⚠️ 文件 {os.path.basename(file)} 没有匹配列，跳过")
    except Exception as e:
        print(f"❌ 文件 {os.path.basename(file)} 读取失败，跳过。错误：{e}")
if all_data:
    merged_df = pd.concat(all_data, ignore_index=True)
    merged_df.to_excel(output_file, index=False)
    print(f"合并完成！输出文件：{output_file}")
else:
    print("没有成功读取任何文件，请检查文件路径或列名关键字。")

