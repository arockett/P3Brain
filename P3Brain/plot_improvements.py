"""
Simple script to plot the points from an output.dat file.

output.dat files have two space separated numbers on each line.
The first number is the best score achieved so far by a solution.
The second number is the total number of evaluations done to that point.
An evaluation is when a solution gets a score from the test world.
"""

import sys
import matplotlib.pyplot as plt

datafile = sys.argv[1]

X = []
y = []
with open(datafile, 'r') as f:
    for line in f.readlines():
        if not line.startswith('#'):
            fitness, iteration = line.strip().split()
            X.append(iteration)
            y.append(fitness)

plt.plot(X, y)
plt.show()
