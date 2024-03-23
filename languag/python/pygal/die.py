from random import randint 

class Die():
    # 默认是6面骰子
    def __init__(self, num_sides = 6):
        self.num_sides = num_sides
    """掷骰子"""
    def roll(self):
        return randint(1, self.num_sides)

