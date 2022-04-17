import numpy as np
import matplotlib.pyplot as plt
import sys

LOOPS_STATS_FILE = "loop_stats.txt"

with open(LOOPS_STATS_FILE) as f:
    runs = int((f.readline().split(":"))[1])
    loop_iterations = int((f.readline().split(":"))[1])
    empty_loop_iterations = int((f.readline().split(":"))[1])
    loop_energy = float((f.readline().split(":"))[1])
    loop_time = float((f.readline().split(":"))[1])

loop_time_per_iteration = loop_time / empty_loop_iterations
energy_per_iteration = loop_energy / empty_loop_iterations

energy_per_iteration_micro = energy_per_iteration * 1000000

for input in sys.argv[1:]:
    data = np.loadtxt(input, delimiter=',', skiprows=1)
    #data = data[data != 0]
    data = data / loop_iterations
    data = data - energy_per_iteration_micro

    # For consistent zoom-in on the plot
    data = data[(data <= 1.5)]

    hist, bins = np.histogram(data, bins=100)
    plt.plot(bins[1:], hist, label=(input.split(".")[0]))

plt.ylabel("Počet prípadov")
plt.xlabel("Energia v \u03bcJ")
#plt.xlabel("Energy in pJ")

#plt.title("Porovnanie inštrukcií")
plt.legend()
plt.grid()
plt.show()
