#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


// Function to print a matrix
void printMatrix(int rows, int cols, int matrix[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char** argv) {
    
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int NUM_MATRICES;
    int MATRIX_ROWS;
    int MATRIX_COLS;
    int MATRIX_B_COLS; // Number of columns in matrix B


    double startTime, endTime;
    if(rank == 0){
        printf("Enter Number of matrices: \n");
        scanf("%d", &NUM_MATRICES);
        printf("Enter Number of rows in matrix A: \n");
        scanf("%d", &MATRIX_ROWS);
        printf("Enter Number of columns in matrix A: \n");
        scanf("%d", &MATRIX_COLS);
        printf("Enter Number of columns in matrix B: \n");
        scanf("%d", &MATRIX_B_COLS);
    }

    MPI_Bcast(&NUM_MATRICES, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MATRIX_ROWS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MATRIX_COLS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MATRIX_B_COLS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (NUM_MATRICES % size != 0) {
        printf("Number of matrices must be divisible by the number of processes.\n");
        MPI_Finalize();
        return 1;
    }
    int root = 0;
    int matrices[NUM_MATRICES][MATRIX_ROWS][MATRIX_COLS];
    int matrixB[NUM_MATRICES][MATRIX_COLS][MATRIX_B_COLS];
    int resultMatrices[NUM_MATRICES][MATRIX_ROWS][MATRIX_B_COLS];

    if (rank == root) {
        // Initialize the matrices in the root process
        for (int k = 0; k < NUM_MATRICES; k++) {
            for (int i = 0; i < MATRIX_ROWS; i++) {
                for (int j = 0; j < MATRIX_COLS; j++) {
                    matrices[k][i][j]  = rand() % 10;
                }
            }
            for(int i = 0; i < MATRIX_COLS; i++){
                for(int j = 0; j < MATRIX_B_COLS; j++){
                    matrixB[k][i][j] = rand() % 10;
                }
            }                
        }
    }

    // Barrier to synchronize all processes before timing starts
    MPI_Barrier(MPI_COMM_WORLD);
    startTime = MPI_Wtime();

    // Buffer to store the portion of the matrices assigned to each process
    int localMatrices[NUM_MATRICES / size][MATRIX_ROWS][MATRIX_COLS];
    int localMatrixB[NUM_MATRICES / size][MATRIX_COLS][MATRIX_B_COLS];
    // Scatter matrices from the root process to all processes
    MPI_Scatter(matrices, (NUM_MATRICES / size) * MATRIX_ROWS * MATRIX_COLS, MPI_INT, localMatrices, (NUM_MATRICES / size) * MATRIX_ROWS * MATRIX_COLS, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Scatter(matrixB, (NUM_MATRICES / size) * MATRIX_COLS * MATRIX_B_COLS, MPI_INT, localMatrixB, (NUM_MATRICES / size) * MATRIX_COLS * MATRIX_B_COLS, MPI_INT, root, MPI_COMM_WORLD);
    // Each process multiplies its matrix with matrix B
    for (int k = 0; k < NUM_MATRICES / size; k++) {
        for (int i = 0; i < MATRIX_ROWS; i++) {
            for (int j = 0; j < MATRIX_B_COLS; j++) {
                resultMatrices[k][i][j] = 0;
                for (int l = 0; l < MATRIX_COLS; l++) {
                    resultMatrices[k][i][j] += localMatrices[k][i][l] * localMatrixB[k][l][j];
                }
            }
        }
    }

    // Barrier to synchronize all processes before timing ends
    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    // Print timing information for each process
    printf("Process %d: Time taken = %f seconds\n", rank, endTime - startTime);

    // Gather result matrices from all processes to the root process
    int gatheredMatrices[NUM_MATRICES][MATRIX_ROWS][MATRIX_B_COLS];
    MPI_Gather(resultMatrices, (NUM_MATRICES / size) * MATRIX_ROWS * MATRIX_B_COLS, MPI_INT, gatheredMatrices, (NUM_MATRICES / size) * MATRIX_ROWS * MATRIX_B_COLS, MPI_INT, root, MPI_COMM_WORLD);

    // Root process prints all 15 result matrices
    if (rank == root) {
        printf("\n All %d Result Matrices:\n", NUM_MATRICES);
        for (int k = 0; k < NUM_MATRICES; k++) {
            printf("Matrix %d:\n", k);
            printMatrix(MATRIX_ROWS, MATRIX_B_COLS, gatheredMatrices[k]);
        }
    }

    MPI_Finalize();

    return 0;
}