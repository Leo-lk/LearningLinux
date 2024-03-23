import pygal
from die import Die

roll_times = 1000
die1 = Die(8)
die2 = Die(8)
"""获取掷骰子的结果"""
results = [die1.roll() + die2.roll() for roll_num in range(1,roll_times)]
"""获取各面的出现频次"""
max_num_sides = die1.num_sides + die2.num_sides
frequencies = [results.count(value) for value in range(2, max_num_sides + 1)]

hist = pygal.Bar()
hist.title = "Result of roll two D6 {1} times".format(format, roll_times, grouping=False, monetary=False)
hist.x_labels = [x for x in range(2, max_num_sides + 1)]
hist.x_title = "result"
hist.y_title = "Frequence of result"

hist.add('D6 + D6', frequencies)
hist.render_to_file('die_visual_svg')



