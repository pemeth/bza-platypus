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

    # Shows good separation between high energy instructions (clflush, rdrand, fscale, rdtsc),
    # but no separation between imul, add, mov (r, mem)
    data = np.split(data, runs)
    data = np.median(data, axis=1)
    data = data[(data < 2)]
    data = data[(data > 0.05)]

    hist, bins = np.histogram(data, bins=500)
    plt.plot(bins[1:], hist, label=input)

plt.ylabel("Density")
plt.xlabel("Energy in \u03bcJ")
#plt.xlabel("Energy in pJ")

plt.title("Instruction Comparison")
plt.legend()
plt.grid()
plt.show()
