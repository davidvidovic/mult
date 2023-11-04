#!/bin/sh

# To run: ./run.sh <number_of_cores> <dimension>
# Example: ./run.sh 8 100
# Runs MPI program on 8 cores with vector dim = 100 and matrix dim = 100x100

# echo "Compiling $1..."
mpic++ z4.cpp -o exe 
# echo "Executing..."
mpiexec exe $1 input_vector.txt input_matrix.txt $2
rm exe
g++ seq.cpp -o exe
./exe input_vector.txt input_matrix.txt
rm exe
