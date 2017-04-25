
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
