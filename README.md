# BZA project
# Replication of parts of the Platypus publication

## Gathering statistics
The MSR kernel module needs to be loaded. Modular system may not load it
automatically, in which case the `/dev/cpu/*/msr` devices will not be mounted.
Run `modprobe msr` if that is your case.

Sudo is required by this program.
Compile and run with `make run` to gather statistics on energy consumption of
various instructions, imul operands, RAPL refresh rates and for a covert channel demo.
If you wish to generate only some of the statistics, refer to this usage information:

    Usage: sudo taskset -c 0 ./bza_platypus {-i|-o|-r|-c}
    Options:
        -i Run measurements on various instructions and save them to files {instruction-name}.csv
        -o Run measurements on the imul instruction with operands of differring Hamming weights
        -r Run measurements on the RAPL refresh rates
        -c Run a covert channel demo and save the measured data to covert_channel.csv
    Note: The 'msr' kernel module may have to be loaded manually by running 'modprobe msr'.

## Visualisations
For plotting the gathered data, Python 3 is required. Generate a virtual environment by
running `make venv` and the activate it by running `source ./.venv/bin/activate`.
Install the required Python packages via `pip install -r requirements.txt`.

To plot specific datasets, refer to this usage information:

    Usage: python3 plotter.py [-h] [-i {files...}] [-o {files...}] [-r {files...}] [-c file]
    Options:
        -i files Show plots on instruction energy consumptions from data contained in files
        -o files Show plots on instruction operand energy consumptions from data contained in files
        -r files Show plots on RAPL refresh rates from data contained in files
        -c file  Visualise bits send over the covert channel
