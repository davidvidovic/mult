#!/bin/sh

# To run: ./run.sh <source_file> <number_of_cores>
# Example: ./run.sh z1.c 8

echo "Compiling $1..."
mpicc -o exe $1 -lm
echo "Executing..."
mpiexec -n $2 exe
rm exe
