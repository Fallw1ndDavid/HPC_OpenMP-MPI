#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Function declarations for file reading/writing
int read_num_dims(char *filename);
int *read_dims(char *filename, int num_dims);
float *read_array(char *filename, int *dims, int num_dims);
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims);
int product(int *dims, int num_dims);

// Declare the stencil function to be called
extern void stencil(float *input_vec, float *output_vec, float *filter_vec, int m, int n, int k, int b);

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 4) {
        printf("Usage: %s <input_file> <filter_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Parse file paths
    char *input_file = argv[1];
    char *filter_file = argv[2];
    char *output_file = argv[3];

    // Read input and filter dimensions from files
    int input_dims_num = read_num_dims(input_file);
    int filter_dims_num = read_num_dims(filter_file);

    int *input_dims = read_dims(input_file, input_dims_num);
    int *filter_dims = read_dims(filter_file, filter_dims_num);

    // Read actual input and filter data
    float *input_data = read_array(input_file, input_dims, input_dims_num);
    float *filter_data = read_array(filter_file, filter_dims, filter_dims_num);

    // Extract dimensional information
    int b = input_dims[0];
    int m = input_dims[1];
    int n = input_dims[2];
    int k = filter_dims[0];

    // Allocate space for output
    float *output_data = malloc(sizeof(float) * b * m * n);

    // Measure execution time of stencil computation
    double start_time = omp_get_wtime();
    stencil(input_data, output_data, filter_data, m, n, k, b);
    double end_time = omp_get_wtime();
    printf("STENCIL_TIME: %f\n", end_time - start_time);

    // Write result to output file
    write_to_output_file(output_file, output_data, input_dims, input_dims_num);

    // Free allocated memory
    free(input_dims);
    free(filter_dims);
    free(input_data);
    free(filter_data);
    free(output_data);

    return 0;
}
