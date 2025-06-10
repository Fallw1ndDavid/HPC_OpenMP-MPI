#include <omp.h>
#include <stdio.h>

// Stencil function that performs convolution-like computation
void stencil(float *input_vec, float *output_vec, float *filter_vec, int m, int n, int k, int b) {
    // Cast the flat 1D arrays into 3D and 2D arrays for readability
    float (*input)[m][n] = (float (*)[m][n]) input_vec;
    float (*filter)[k] = (float (*)[k]) filter_vec;
    float (*output)[m][n] = (float (*)[m][n]) output_vec;

    // Calculate padding sizes for convolution
    int lower = (k - 1) / 2;
    int upper = (k - 1) - lower;

    // Parallelize over batch, rows, and columns using OpenMP
    #pragma omp parallel for collapse(3)
    for (int batch = 0; batch < b; batch++) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {

                // If the pixel is at the boundary, copy input directly to output
                if (i < lower || i >= m - upper || j < lower || j >= n - upper) {
                    output[batch][i][j] = input[batch][i][j];
                } else {
                    // Perform convolution
                    float sum = 0.0f;
                    for (int u = 0; u < k; u++) {
                        for (int v = 0; v < k; v++) {
                            // Apply the filter to the corresponding neighborhood
                            sum += input[batch][i + u - lower][j + v - lower] * filter[u][v];
                        }
                    }
                    output[batch][i][j] = sum;
                }
            }
        }
    }
}
