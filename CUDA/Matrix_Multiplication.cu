// %%cu
#include <stdio.h>
#include <stdlib.h>

// Function to initialize a matrix with random values
void initializeMatrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = rand() % 10;
    }
}

// Function to print a matrix
void printMatrix(const int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

// CUDA kernel for element-wise matrix multiplication
__global__ void matrixMultiply(int *a, int *b, int *c, int n, int m, int p) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < p) {
        int sum = 0;
        for (int i = 0; i < m; ++i) {
            sum += a[blockIdx.z * n * m + row * m + i] * b[blockIdx.z * m * p + i * p + col];
        }
        c[blockIdx.z * n * p + row * p + col] = sum;
    }
}

int main() {
    // Matrix dimensions
    int k = 13, N = 3, M = 3, P = 3;

    // Get matrix dimensions from the user
    /*
    printf("Enter the number of matrices (k): ");
    scanf("%d", &k);

    printf("Enter the number of rows for matrices A (N): ");
    scanf("%d", &N);

    printf("Enter the number of columns for matrices A and rows for matrices B (M): ");
    scanf("%d", &M);

    printf("Enter the number of columns for matrices B (P): ");
    scanf("%d", &P);
    */

    // Host matrices
    int *h_A, *h_B, *h_C;
    // Device matrices
    int *d_A, *d_B, *d_C;

    // Allocate memory on the host
    h_A = (int *)malloc(k * N * M * sizeof(int));
    h_B = (int *)malloc(k * M * P * sizeof(int));
    h_C = (int *)malloc(k * N * P * sizeof(int));

    // Initialize matrices with random values
    for (int i = 0; i < k; ++i) {
        initializeMatrix(&h_A[i * N * M], N, M);
        initializeMatrix(&h_B[i * M * P], M, P);
    }

    // Allocate memory on the device
    cudaMalloc((void**)&d_A, k * N * M * sizeof(int));
    cudaMalloc((void**)&d_B, k * M * P * sizeof(int));
    cudaMalloc((void**)&d_C, k * N * P * sizeof(int));

    // Copy data from host to device
    cudaMemcpy(d_A, h_A, k * N * M * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, k * M * P * sizeof(int), cudaMemcpyHostToDevice);

    // Define grid and block dimensions
    dim3 threadsPerBlock(10, 10);
    dim3 blocksPerGrid((N + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (P + threadsPerBlock.y - 1) / threadsPerBlock.y,
                       k); // Add k as the third dimension for handling multiple matrices

     // Record start time
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // Launch the CUDA kernel for element-wise matrix multiplication
    matrixMultiply<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N, M, P);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    printf("Time taken: %f milliseconds\n", milliseconds);

    // Copy result from device to host
    cudaMemcpy(h_C, d_C, k * N * P * sizeof(int), cudaMemcpyDeviceToHost);

    // Print the matrices and result for each pair
    for (int i = 0; i < k; ++i) {
        printf("\nMatrix A%d:\n", i + 1);
        printMatrix(&h_A[i * N * M], N, M);

        printf("\nMatrix B%d:\n", i + 1);
        printMatrix(&h_B[i * M * P], M, P);

        printf("\nResult Matrix C%d (Multiplication of A%d and B%d):\n", i + 1, i + 1, i + 1);
        printMatrix(&h_C[i * N * P], N, P);
    }

    // Free allocated memory
    free(h_A);
    free(h_B);
    free(h_C);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}

