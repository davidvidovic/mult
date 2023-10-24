#!/bin/sh

# To run: ./run.sh <source file> <number of cores>
# Example: .run.sh a.c 8

echo "Compiling $1 ..."
mpicc -o exe $1
echo "Executing..."
mpiexec -n $2 exe
rm exe
