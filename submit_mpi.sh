#!/bin/bash
#SBATCH --job-name=mpi_8
#SBATCH --output=output_mpi_%j.txt
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=8              
#SBATCH --time=00:10:00
#SBATCH -D ./
#SBATCH --export=ALL
#SBATCH --nodes=8 

# Load required modules
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

echo "Running MPI stencil weak scaling tests"

# Loop over different number of processes
for procs in 1 2 4 8 16 32
do
    echo "MPI_PROCS=$procs"
    input="linked_datafiles/input_${procs}_3840_2048.dat"
    output="output_mpi_${procs}.dat"

    # Run stencil program using mpirun with specified number of processes
    mpirun -np $procs ./stencil-complete-gcc $input linked_datafiles/filter_5.dat $output
done

