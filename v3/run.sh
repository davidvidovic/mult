#!/bin/sh

# To run: ./run.sh <number_of_cores> 
# Example: ./run.sh 8 

# echo "Compiling $1..."
mpic++ z4.cpp -o exe 
# echo "Executing..."
mpiexec exe $1 input_vector.txt input_matrix.txt
rm exe
g++ seq.cpp -o exe
./exe input_vector.txt input_matrix.txt
rm exe
