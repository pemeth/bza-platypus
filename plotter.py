import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
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

def plot_rapl_refresh_rates():
    for input in sys.argv[1:]:
        data = np.loadtxt(input, delimiter=',', skiprows=1)

        data = data * 1000000 # to microseconds

        hist, bins = np.histogram(data, bins=50)
        plt.plot(bins[1:], hist, label=(input.split(".")[0]))

    plt.ylabel("Počet prípadov")
    plt.xlabel("Čas [\u03bcs]")

def plot_instruction_energy():
    for input in sys.argv[1:]:
        data = np.loadtxt(input, delimiter=',', skiprows=1)

        data = np.split(data, runs)
        data = np.median(data, axis=1)

        data = data / loop_iterations
        data = data - energy_per_iteration_micro

        # For consistent zoom-in on the plot
        #data = data[(data <= 0.5)]

        #sns.kdeplot(data, label=(input.split(".")[0]))

        hist, bins = np.histogram(data, bins='auto')
        plt.plot(bins[1:], hist, label=(input.split(".")[0]))

    plt.ylabel("Počet prípadov")
    plt.xlabel("Energia v \u03bcJ")

def plot_operand_energy():
    for input in sys.argv[1:]:
        data = np.loadtxt(input, delimiter=',', skiprows=1)

        data = np.split(data, runs)
        data = np.median(data, axis=1)

        data = data / loop_iterations
        data = data - energy_per_iteration_micro

        # For consistent zoom-in on the plot
        data = data[(data <= 0.5)]

        sns.kdeplot(data, label=(input.split(".")[0]))

    plt.ylabel("Odhad hustoty rozloženia (KDE)")
    plt.xlabel("Energia v \u03bcJ")

#plot_rapl_refresh_rates()
plot_instruction_energy()
#plot_operand_energy()

#plt.title("Porovnanie inštrukcií")
plt.legend()
plt.grid()
plt.show()
