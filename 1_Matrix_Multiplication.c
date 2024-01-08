#include<stdio.h>
#include "mpi.h"
#include<time.h>

#define N 512 // Size of the matrix

MPI_Status status;

// These are our matrix holders
double matrix_a[N][N], matrix_b[N][N], matrix_c[N][N];

int main(int argc, char **argv)
{
	int processId, numOfProcess, numOfSlaveProcess, numOfRowsForSlave, offset;
	double startTime, endTime;
	MPI_Init(&argc, &argv); // Initiating the mpi environment
	MPI_Comm_rank(MPI_COMM_WORLD, &processId); // get the current process id
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcess); // get the total number of process

	numOfSlaveProcess = numOfProcess - 1; // One process is the master process others are slave process

	// Master process
	if(processId == 0){

		// Record the start time
		startTime = MPI_Wtime();


		printf("number of process = %d\n", numOfProcess);
		// Generate matrix a and b with some random number
		srand(time(NULL));
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				matrix_a[i][j] = rand()%10;
				matrix_b[i][j] = rand()%10;
			}
		}
		printf("\n\t Matrix - Matrix Multiplication using MPI\n");

		// print matrix A
		printf("\n Matrix A\n\n");
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				printf("%0.f\t", matrix_a[i][j]);
			}
			printf("\n");
		}
		// print matrix B
		printf("\n Matrix B\n\n");
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				printf("%0.f\t", matrix_b[i][j]);
			}
			printf("\n");
		}

		// Now from this process we will distribute task to all the slave process;
		// So we have to decide the #rows of the Matrix A, Which we will sent to each slave process
		numOfRowsForSlave = N / numOfSlaveProcess;
		offset = 0; // this variable is the starting row which we will sent to slave process

		// Assigning the calculation details to slave process, start from process 1
		// Each message tag is 1
		for(int dest = 1; dest <= numOfSlaveProcess; dest++)
		{
			MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&numOfRowsForSlave, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&matrix_a[offset][0], numOfRowsForSlave * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&matrix_b, N * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
			offset = offset + numOfRowsForSlave;
		}
		for(int i = 1; i<= numOfSlaveProcess; i++)
		{
			int source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&numOfRowsForSlave, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&matrix_c[offset][0], numOfRowsForSlave*N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
		}
		// Print the result matrix
		printf("\n Resultant Matrix C = A * B\n\n");
		for(int i = 0; i < N; i++)
		{
			printf("\n");
			for(int j = 0; j < N; j++)
			{
				printf("%0.f\t", matrix_c[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		// Record the end time
		endTime = MPI_Wtime();
		printf("\n Elapsed Time = %f second\n\n", endTime - startTime);
	}
	// Slave process
	if(processId > 0){
		int source = 0;
		// Now the slave process should receive the data that is sent by the master process
		// Each process will receive this data seperately and will execute the task and 
		// store the result in matrix C then resend to master process
		MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&numOfRowsForSlave, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&matrix_a, numOfRowsForSlave * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&matrix_b, N * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

		// Matrix multiplication
		for(int k = 0; k < N; k++)
		{
			for(int i = 0; i < numOfRowsForSlave; i++)
			{
				matrix_c[i][k] = 0;
				for(int j = 0; j < N; j++)
				{
					matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
				}
			}
		}
		// Send the calculated result back to master process
		MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&numOfRowsForSlave, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&matrix_c, numOfRowsForSlave * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
		
	}

	MPI_Finalize();
}

// Expected output
/*
For N = 512

Number of process = 2
Elapsed Time = 0.921194 second

Number of process = 5
Elapsed Time = 0.639368 second
*/