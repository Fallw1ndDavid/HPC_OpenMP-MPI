#!/bin/bash
#SBATCH --job-name=omp_8
#SBATCH --output=output_omp_%j.txt
#SBATCH --cpus-per-task=8
#SBATCH --time=00:10:00
#SBATCH -D ./
#SBATCH --export=ALL
#SBATCH -p course

# Load compiler and MPI modules
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

echo "Running OpenMP stencil strong scaling tests"

# Loop over different thread counts to evaluate strong scaling
for threads in 1 2 4 8 16 32
do
    export OMP_NUM_THREADS=$threads        # Set number of OpenMP threads
    echo -n "OMP_NUM_THREADS=$threads "
    
    # Run the stencil program on single input with fixed size and varying thread count
    ./stencil-nearly-gcc linked_datafiles/input_1_3840_2048.dat linked_datafiles/filter_5.dat output_omp_${threads}.dat
done

