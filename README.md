# x64BareBones - starting a 64 Bits OS almost from scratch

x64BareBones is a basic setup to develop operating systems for the Intel 64 bits architecture.

The final goal of the project is to provide an entry point for a kernel and the possibility to load extra binary modules separated from the main kernel.

## Environment setup: 

1- Install the following packages before building the Toolchain and Kernel:

    user@linux:$ nasm qemu gcc make

2- Compile the kernel

Execute the following commands on the x64BareBones project directory. This will compile the proyect inside the following docker: agodio/itba-so:1.0 

    user@linux:~/SO/$ ./compile.sh

3- Run the kernel

From the x64BareBones project directory run:

    user@linux:~/SO/$ ./run.sh
