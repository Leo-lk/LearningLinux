#!/usr/bin/env python3
import matplotlib.pyplot as plt

x_values = list(range(1, 1001))
y_values = [x**2 for x in x_values]
# s：设置点的大小
# edgecolor: 删除数据点的轮廓，使点不粘连
# c：设置颜色，也可以使用RGB配色：c(0, 0, 0.8),越接近1色越浅
#plt.scatter(x_values, y_values, s=1, edgecolor='none', c='red')

# 使用颜色映射实现渐变色
plt.scatter(x_values, y_values, s=1, edgecolor='none', c=y_values, cmap=plt.cm.Blues)

plt.title("Square numbers", fontsize=24)
plt.xlabel("value", fontsize=14)
plt.ylabel("Square of value", fontsize=14)
# 刻度字号
plt.tick_params(axis='both', which='major', labelsize=10)

# 设置每个坐标轴的取值范围
plt.axis([0, 1100, 0, 1100000])

plt.show()

