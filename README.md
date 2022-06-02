# x64BareBones - starting a 64 Bits OS almost from scratch

x64BareBones is a basic setup to develop operating systems for the Intel 64 bits architecture.

This project is based on a UNIX based operating system where we implement many key OS features.

## Environment setup: 

1- Install the following packages before building the Toolchain and Kernel:

    user@linux:$ nasm qemu gcc make

2- Compile the kernel

Execute the following commands on the x64BareBones project directory. This will compile the proyect inside the following docker container: `agodio/itba-so:1.0`

    user@linux:~/SO/$ ./compile.sh

3- Run the kernel

From the x64BareBones project directory run:

    user@linux:~/SO/$ ./run.sh
