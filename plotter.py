import argparse
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import sys

LOOPS_STATS_FILE = "loop_stats.txt"

parser = argparse.ArgumentParser()
parser.add_argument('-i', nargs='+')
parser.add_argument('-o', nargs='+')
parser.add_argument('-r', nargs='+')
parser.add_argument('-c', nargs=1)

args = parser.parse_args()

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
    if not args.r:
        return

    for input in args.r:
        data = np.loadtxt(input, delimiter=',', skiprows=1)

        data = data * 1000000 # to microseconds

        hist, bins = np.histogram(data, bins=50)
        plt.plot(bins[1:], hist, label=(input.split(".")[0]))

    plt.ylabel("Počet prípadov")
    plt.xlabel("Čas [\u03bcs]")
    plt.legend()
    plt.grid()
    plt.show()

def plot_instruction_energy():
    if not args.i:
        return

    for input in args.i:
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
    plt.legend()
    plt.grid()
    plt.show()

def plot_operand_energy():
    if not args.o:
        return

    for input in args.o:
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
    plt.legend()
    plt.grid()
    plt.show()

def plot_covert_channel():
    if not args.c:
        return

    f = args.c[0]

    data = np.loadtxt(f)

    plt.plot(data)
    plt.ylabel("Energia v \u03bcJ")

    #window = 10
    #avg = np.convolve(data, np.ones(window), 'valid') / window
    #plt.plot(avg, label="rolling average")
    plt.grid()
    plt.show()

plot_rapl_refresh_rates()
plot_instruction_energy()
plot_operand_energy()
plot_covert_channel()
