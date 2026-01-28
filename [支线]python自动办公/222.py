"""
功能：合并多个设施标识码文件，提取项目名称和编码区间
"""

# 再次导入库（虽然已导入，但这是脚本分隔的习惯）
import pandas as pd
import glob
import os

# ========= 配置参数 =========
# 定义要处理的文件夹路径
folder = r"C:\Users\23286\Desktop\设施标识码需合并"

# 定义输出文件的完整路径
output = os.path.join(folder, "项目编码汇总.xlsx")

# 定义用于匹配列名的关键字列表
# 使用较短的关键字可以提高匹配成功率
KEY_PROJECT = ["项目名称"]  # 匹配项目名称相关的列
KEY_START = ["起点编码"]  # 匹配起点编码相关的列
KEY_END = ["终点编码"]  # 匹配终点编码相关的列


# ===========================

def find_col(cols, keywords):
    """
    智能列名匹配函数

    参数：
    cols: 数据框的列名列表
    keywords: 要匹配的关键字列表

    返回值：
    匹配到的列名，如果没有匹配则返回None

    功能说明：
    遍历关键字列表和数据框列名，进行模糊匹配
    """
    for k in keywords:  # 遍历每个关键字
        for c in cols:  # 遍历每个列名
            if k in c:  # 如果关键字出现在列名中
                return c  # 返回匹配到的列名
    return None  # 如果没有匹配到任何列，返回None


# 获取文件夹中所有Excel文件
files = glob.glob(os.path.join(folder, "*.xls*"))

# 初始化存储合并数据的列表
rows = []

# 初始化成功和失败计数器
success, fail = 0, 0

# 遍历每个Excel文件
for f in files:
    try:
        # 读取Excel文件（全部作为字符串读取）
        df = pd.read_excel(f, dtype=str)

        # 清洗列名：转换为字符串、去除空格、转为小写
        df.columns = [str(c).strip().lower() for c in df.columns]

        # 使用find_col函数匹配需要的列
        col_proj = find_col(df.columns, KEY_PROJECT)  # 项目名称列
        col_start = find_col(df.columns, KEY_START)  # 起点编码列
        col_end = find_col(df.columns, KEY_END)  # 终点编码列

        # 检查是否所有必要的列都匹配到了
        if not all([col_proj, col_start, col_end]):
            raise ValueError("缺少必要列")  # 抛出异常

        # 创建临时数据框，包含项目名称和编码区间
        # 编码区间 = 起点编码 + "-" + 终点编码
        # fillna("") 将空值填充为空字符串，避免拼接时出现"nan"
        tmp = pd.DataFrame({
            "项目名称": df[col_proj],  # 项目名称列
            "编码区间": df[col_start].fillna("") + "-" + df[col_end].fillna("")  # 编码区间列
        })

        # 将临时数据框添加到总列表中
        rows.append(tmp)


        # 打印成功信息
        print(f"成功：{os.path.basename(f)}")

    except Exception as e:
        # 处理异常情况
        print(f"失败：{os.path.basename(f)} | {e}")  # 打印失败信息

# 合并所有数据并输出
if rows:  # 如果成功读取了数据
    # 合并所有临时数据框
    pd.concat(rows, ignore_index=True).to_excel(output, index=False)

