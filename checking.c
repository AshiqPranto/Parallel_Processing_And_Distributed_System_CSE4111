#include<stdio.h>
#include "mpi.h"
#include<string.h>
main(int argc, char **argv)
{
    int MyRank, Numprocs, tag, ierror, i;
    MPI_Status status;
    char send_message[20], recv_message[20];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &Numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
    tag=100;
    strcpy(send_message, "Hello-Participants");
    if (MyRank==0)
    {
        printf("My rank = %d\n", MyRank);
        for(i=1;i<Numprocs;i++)
        {
            MPI_Recv(recv_message,20,MPI_CHAR,i,tag,MPI_COMM_WORLD,&status);
            printf("sleeping\n");
            sleep(1);
            printf("node %d : %s \n",i,recv_message);
        }
    }
    else
    {
        printf("My rank = %d\n", MyRank);
        MPI_Send(send_message,20,MPI_CHAR,0,tag,MPI_COMM_WORLD);

    }
    MPI_Finalize();
}

// "D:\Pranto\study\4_1\Parallel_Processing\Lab\GCC_File\mingw64\bin\gcc.exe"

// C:\\MinGW\\bin\\gcc.exe