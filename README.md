# HPC_OpenMP-MPI
This project implements a high-performance stencil computation over 2D matrices using both shared-memory (OpenMP) and distributed-memory (MPI) parallelism.

The project involves:

A multi-threaded stencil solver using OpenMP
A nearly-serial program to read data, invoke the solver, and write output
A fully distributed MPI solution capable of handling large-scale input across multiple processes
A Makefile supporting multiple compilation targets with both GCC and Intel compilers
Performance evaluation scripts and plots for strong and weak scaling experiments

The project was developed and tested on the Barkla HPC cluster, using both synthetic and real benchmark data.
Components:

stencil.c: OpenMP implementation
main-nearly.c: I/O + nearly-serial orchestration
main-mpi.c: MPI-distributed implementation
Makefile: Multi-backend build support
report.pdf: Summary of implementation, performance plots, and discussion

