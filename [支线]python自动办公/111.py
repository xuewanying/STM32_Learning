"""
Excel文件自动合并处理脚本
功能：自动扫描指定文件夹中的Excel文件，提取关键列并合并到一个文件中
作者：不知周
日期：2025-01-29
"""

# ============ 导入必要的库 ============
import pandas as pd  # 用于数据处理和分析的主要库
import glob  # 用于文件路径模式匹配
import os  # 用于操作系统相关功能，如路径操作

# ============ 第一部分：内窥检测记录表合并 ============
"""
功能：合并多个内窥检测记录表，提取指定的关键列
"""

# 定义要处理的文件夹路径
# r"" 表示原始字符串，避免转义字符被解析
folder_path = r"C:\Users\23286\Desktop\内窥检测记录表"

# 定义输出文件的完整路径
# os.path.join() 智能拼接路径，确保在不同操作系统下的兼容性
output_file = os.path.join(folder_path, "merged_smart.xlsx")

# 定义需要提取的关键字列表
# 这些关键字用于匹配Excel表中的列名
target_keywords = ["管段长度", "材质", "管径", "录像文件", "道路名称", "缺陷等级", "缺陷名称"]

# 使用glob模块获取文件夹中所有Excel文件
# *.xls* 匹配所有.xls和.xlsx文件（包括.xlsm等）
excel_files = glob.glob(os.path.join(folder_path, "*.xls*"))

# 初始化一个空列表，用于存储所有文件的数据
all_data = []

# 遍历每个Excel文件
for file in excel_files:
    print(f"正在处理文件：{file}")  # 显示当前处理的文件

    try:
        # 读取Excel文件
        # dtype=str 确保所有数据以字符串形式读取，避免数字格式问题（如丢失前导零）
        df = pd.read_excel(file, dtype=str)

        # 清洗列名：将所有列名转换为小写并去除首尾空格
        # 这有助于后续的关键字匹配，避免大小写或空格导致匹配失败
        df.columns = [str(col).strip().lower() for col in df.columns]

        # 初始化匹配列列表
        matched_cols = []

        # 遍历每个目标关键字
        for kw in target_keywords:
            kw_lower = kw.lower()  # 将关键字也转换为小写，确保匹配一致性

            # 遍历数据框的每一列
            for col in df.columns:
                # 如果关键字出现在列名中，则认为匹配成功
                # 使用"in"而不是"=="是为了模糊匹配（如"道路名称"匹配"道路名称1"、"道路名称2"等）
                if kw_lower in col:
                    matched_cols.append(col)

        # 去除重复的列名（同一列可能匹配多个关键字）
        matched_cols = list(set(matched_cols))

        # 如果找到了匹配的列
        if matched_cols:
            # 提取这些列的数据并添加到总数据列表中
            all_data.append(df[matched_cols])
            print(f"✅ 匹配列：{matched_cols}")  # 显示匹配成功的列
        else:
            # 如果没有匹配到任何列，跳过该文件
            print(f"⚠️ 文件 {os.path.basename(file)} 没有匹配列，跳过")

    except Exception as e:
        # 捕获并处理读取文件时可能出现的异常
        print(f"❌ 文件 {os.path.basename(file)} 读取失败，跳过。错误：{e}")

# 检查是否有成功读取的数据
if all_data:
    # 使用pandas的concat函数合并所有数据
    # ignore_index=True 重置索引，避免重复的索引值
    merged_df = pd.concat(all_data, ignore_index=True)

    # 将合并后的数据保存到新的Excel文件
    # index=False 不保存行索引
    merged_df.to_excel(output_file, index=False)
    print(f"合并完成！输出文件：{output_file}")
else:
    # 如果没有成功读取任何数据，给出提示
    print("没有成功读取任何文件，请检查文件路径或列名关键字。")