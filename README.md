<h1>Parallel Processing Lab</h1>

1. Write a program to multiply K different matrices A of dimension MxN with matrices B of dimension NxP dimension matrices. Where K is the number of matrices. 
K * M * N <= 10^6; K * N * P<= 10^6; K * M * P <= 10^6;
(a). Using MPI
(b). Using CUDA

    Input: K, M, N, P
Output: Time taken for multiplication

2. Write a program to count the words in a file and sort it in descending order of frequency of words i.e., highest occurring word must come first and the least occurring word must come last.
(a). Using MPI
(b). Using CUDA

    Input: No. of processes, (Text input from file)
Output: Total time, top 10 occurrences

3. A phonebook is given as a file. Write a program to search for all the contacts matching a name.
(a). Using MPI
(b). Using CUDA

    Input: No. of processes, (phonebook from file)
Output: Total time, Matching names and contact numbers

4. Given a paragraph and a pattern like %x%, write a program to find out the number of occurrences of the given pattern inside the text.
(a). Using MPI
(b). Using CUDA

    Input: No. of processes, (paragraph from file)
Output: Total time, No. of occurrences of the pattern