#!/usr/bin/env python3

import matplotlib.pyplot as plt

from random_walk import RandomWalk

while True:
    rw = RandomWalk(50000)
    rw.fill_walk()
    # 设置显示区域大小（/英寸）
    plt.figure(figsize=(10, 6))
    
    # 按先后顺序着色
    point_numbers = list(range(rw.num_points))
    plt.scatter(rw.x_values, rw.y_values, s=15, c = point_numbers, cmap = plt.cm.Blues, edgecolor = 'none')
    # 突出起点和终点
    plt.scatter(0, 0, c = 'green', edgecolors='none', s=100)
    plt.scatter(rw.x_values[-1], rw.y_values[-1], c='red', edgecolors='none', s=100)
    # 隐藏坐标轴
    plt.axis('off')
    
    plt.show()

    keep_running = input("Make another walk? (y/n): \n")
    if keep_running == 'n':
        break

