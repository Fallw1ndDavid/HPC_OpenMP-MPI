# Compiler definitions
CC = gcc             # GCC compiler for OpenMP version
MPICC = mpicc        # MPI compiler wrapper for GCC
ICC = icc            # Intel compiler for OpenMP version
MPIICC = mpiicc      # Intel compiler wrapper for MPI

# Compilation flags
CFLAGS = -fopenmp -O3 -std=c99       # Flags for GCC OpenMP
ICFLAGS = -qopenmp -O3 -std=c99      # Flags for Intel OpenMP

# Default target to build all versions
all: gccnearly gcccomplete iccnearly icccomplete

# 1. Nearly-serial version compiled with GCC
gccnearly:
	$(CC) stencil.c main-nearly.c file-reader.c $(CFLAGS) -o stencil-nearly-gcc -lm

# 2. Complete MPI version compiled with mpicc (GCC)
gcccomplete:
	$(MPICC) stencil.c main-mpi.c file-reader.c -O3 -std=c99 -o stencil-complete-gcc -lm

# 3. Nearly-serial version compiled with Intel compiler
iccnearly:
	$(ICC) stencil.c main-nearly.c file-reader.c $(ICFLAGS) -o stencil-nearly-icc -lm

# 4. Complete MPI version compiled with Intel MPI compiler
icccomplete:
	$(MPIICC) stencil.c main-mpi.c file-reader.c $(ICFLAGS) -o stencil-complete-icc -lm

# Cleanup target to remove compiled executables and temporary files
clean:
	rm -f stencil-* output_*.dat *.o *.exe
