#include<stdio.h>
#include<mpi.h>
#include<string.h>

#define MAX_WORD_LENGTH 50

int main(int argc, char **argv){
    int myRank, numOfProcs, tag;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);

    if(numOfProcs != 2){
        if(myRank == 0){
            printf("This program requires exactly 2 MPI ranks.\n");
        }
        MPI_Finalize();
        return 1;
    }
    if(myRank == 0){
        char word[MAX_WORD_LENGTH];
        printf("Enter a word: ");
        scanf("%s", &word);
        tag = 0;
        MPI_Send(&word, MAX_WORD_LENGTH, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        tag = 1;
        MPI_Recv(&word, MAX_WORD_LENGTH, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
        printf("The word after Alteration is: %s\n", word);
    }
    else{ 
        char word[MAX_WORD_LENGTH];
        tag = 0;
        MPI_Recv(&word, MAX_WORD_LENGTH, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        int length = strlen(word);
        for(int i = 0; i < length; i++){
            if(word[i] >= 'a' && word[i] <= 'z'){
                word[i] = word[i] - 32;
            }
            else if(word[i] >= 'A' && word[i] <= 'Z'){
                word[i] = word[i] + 32;
            }
        }
        tag = 1;
        MPI_Send(&word, MAX_WORD_LENGTH, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}


/*
Sample Input/Output:
Input:
HEllo
Output:
The word after Alteration is: heLLO
*/ 