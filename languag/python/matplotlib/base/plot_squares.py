#!/usr/bin/env python3
import matplotlib.pyplot as plt

input_values = [1, 2, 3, 4, 5]
squares = [1, 4, 9, 16, 25]
plt.plot(input_values,squares, linewidth=5)

plt.title("Square numbers", fontsize=24)
plt.xlabel("value", fontsize=14)
plt.ylabel("Square of value", fontsize=14)
# 刻度字号
plt.tick_params(axis='both', labelsize=10)

plt.show()

# 自动保存图表：使用plt.savefig()替换plt.show
# bbox_inches='tight'表示将图表多余的空白区域裁剪掉
#plt.savefig('squares_plot.png', bbox_inches='tight')
