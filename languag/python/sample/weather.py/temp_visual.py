'''
Get weather data from weather.csv, and painting line chart for MaxTemperature and MinTemperature
'''
import csv
from matplotlib import pyplot as plt

filename = "weather.csv"
"""在.csv文件中每一行都是一个列表, 列表中的元素以逗号分隔"""
with open(filename) as f:
    reader = csv.reader(f)
    header_row = next(reader)
    # enumerate ：获取元素的索引和值
    for index, column_header in enumerate(header_row):
        print(index , column_header)
    
    maxTemp, minTemp = [], []
    for row in reader:
        # 防止.csv出现缺漏数据导致程序报错
        try:
            high = float(row[1])
            low = float(row[0])
        except ValueError:
            print(row, "missing data")
        else:
            maxTemp.append(high) 
            minTemp.append(low) 
"""开始绘图"""
# 图表大小
fig = plt.figure(figsize = (10, 6))
# 加入数据
days = [x for x in range(1, len(maxTemp)+1)]
plt.plot(maxTemp, c='red', alpha = 0.5)
plt.plot(minTemp, c='blue', alpha = 0.5)
# 填充最高和最低温度之间的图形
plt.fill_between(days, maxTemp, minTemp, facecolor='blue', alpha=0.1)

# 设置图表样式
plt.title("Temp in a year", fontsize=24)
plt.xlabel("day", fontsize=14)
plt.ylabel("Temp", fontsize=14)
# 刻度字号
plt.tick_params(axis='both', labelsize=10)

plt.show()
