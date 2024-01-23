#include<stdio.h>
#include<mpi.h>

int main(int argc, char **argv)
{
    int myRank, numOfProcs, tag;
    MPI_Status status;

    int x = 10, y = 5;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    tag = 999;


    if(myRank == 0)
    {   
    // printf("Enter two numbers: ");
    // scanf("%d %d", &x, &y);
        printf("MyRank = %d\n", myRank);
        printf("Summation of %d and %d = %d\n", x, y, (x + y));
    }
    else if(myRank == 1)
    {
        printf("MyRank = %d\n", myRank);
        printf("Subtraction of %d and %d = %d\n", x, y, (x - y));
    }
    else if(myRank == 2)
    {
        printf("MyRank = %d\n", myRank);
        printf("Multiplication of %d and %d = %d\n", x, y, (x * y));
    }
    else if(myRank == 3)
        printf("MyRank = %d\n", myRank);
        printf("Division of %d and %d = %d\n", x, y, (x / y));
    {

    }

    MPI_Finalize();
}