#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

// Function declarations from file-reader.c
int read_num_dims(char *filename);
int *read_dims(char *filename, int num_dims);
float *read_array(char *filename, int *dims, int num_dims);
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims);
int product(int *dims, int num_dims);

// External stencil function
extern void stencil(float *input_vec, float *output_vec, float *filter_vec, int m, int n, int k, int b);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);  // Initialize the MPI environment

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get current process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

    // Check for proper number of arguments
    if (argc != 4) {
        if (rank == 0)
            printf("Usage: %s <input_file> <filter_file> <output_file>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    // Parse input arguments
    char *input_file = argv[1];
    char *filter_file = argv[2];
    char *output_file = argv[3];

    // Read input and filter dimensions
    int input_dims_num = read_num_dims(input_file);
    int filter_dims_num = read_num_dims(filter_file);

    int *input_dims = read_dims(input_file, input_dims_num);
    int *filter_dims = read_dims(filter_file, filter_dims_num);

    // Input shape: [b][m][n]
    int b = input_dims[0];
    int m = input_dims[1];
    int n = input_dims[2];

    // Filter shape: [k][k]
    int k = filter_dims[0];

    // Allocate input/output only in rank 0
    float *full_input = NULL;
    float *output_data = NULL;
    if (rank == 0) {
        full_input = read_array(input_file, input_dims, input_dims_num);
        output_data = malloc(sizeof(float) * b * m * n);
    }

    // Every process loads filter data locally and broadcast it
    float *filter_data = read_array(filter_file, filter_dims, filter_dims_num);
    MPI_Bcast(filter_data, k * k, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Compute number of batches each process should handle (irregular distribution)
    int local_b_base = b / size;
    int remainder = b % size;

    int local_b;
    if (rank < remainder)
        local_b = local_b_base + 1;  // Some processes get one more image
    else
        local_b = local_b_base;

    int local_count = local_b * m * n;

    // Set up counts and displacements for Scatterv/Gatherv on rank 0
    int *sendcounts = NULL, *displs = NULL;
    if (rank == 0) {
        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        int offset = 0;
        for (int i = 0; i < size; i++) {
            int proc_b = local_b_base + (i < remainder ? 1 : 0);
            sendcounts[i] = proc_b * m * n;
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    // Allocate space for local input chunk
    float *local_input = malloc(local_count * sizeof(float));
    MPI_Scatterv(full_input, sendcounts, displs, MPI_FLOAT,
                 local_input, local_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Allocate local output
    float *local_output = malloc(local_count * sizeof(float));

    // Start timing AFTER file reading, BEFORE writing
    double start_time = MPI_Wtime();
    
    // Compute stencil
    stencil(local_input, local_output, filter_data, m, n, k, local_b);
    
    // Gather all local outputs to rank 0
    MPI_Gatherv(local_output, local_count, MPI_FLOAT,
                output_data, sendcounts, displs, MPI_FLOAT,
                0, MPI_COMM_WORLD);
                
    // Stop timing BEFORE file writing
    double end_time = MPI_Wtime();

    // Rank 0 writes output and prints time
    if (rank == 0) {
        printf("STENCIL_TIME: %f\n", end_time - start_time);
        write_to_output_file(output_file, output_data, input_dims, input_dims_num);
    }

    // Free memory
    if (rank == 0) {
        free(full_input);
        free(output_data);
        free(sendcounts);
        free(displs);
    }
    free(local_input);
    free(local_output);
    free(input_dims);
    free(filter_dims);
    free(filter_data);

    MPI_Finalize();  // Finalize MPI environment
    return 0;
}
