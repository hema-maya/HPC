#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include<sys/time.h>
#include<time.h>

const int MAX_STRING = 100;
int isPalindrome(char str[]) {
    int i = 0, j = strlen(str) - 1;
    while (i < j) {
        if (str[i] != str[j])
            return 0;
        i++;
        j--;
    }
    return 1;
}
int main(void) {
    char message[MAX_STRING];
    int comm_sz;
    int my_rank;
    MPI_Status status;
    char *string_list[] = {
        "hello",
        "level",
        "world",
        "madam",
        "computer",
        "radar"
    };
    int list_size = 6;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank != 0) {
        strcpy(message, string_list[my_rank % list_size]);
        if (my_rank % 2 == 1) {
            MPI_Send(message, strlen(message) + 1,
                     MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        } else {
            MPI_Send(message, strlen(message) + 1,
                     MPI_CHAR, 0, 2, MPI_COMM_WORLD);
        }
    }
    else {
        char recv_msg[MAX_STRING];
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(recv_msg, MAX_STRING, MPI_CHAR,
                     MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);


            if (status.MPI_TAG == 1) {
                struct timeval start, end;
                double time_used;
                gettimeofday(&start, NULL);

                for (int i = 0; recv_msg[i]; i++)
                    recv_msg[i] = toupper(recv_msg[i]);

                printf("\nProcess %d  Type 1 --- Uppercase: %s\n",
                       status.MPI_SOURCE, recv_msg);
                gettimeofday(&end, NULL);

               time_used = (end.tv_sec - start.tv_sec) * 1e6;
               time_used = (time_used + (end.tv_usec - start.tv_usec)) * 1e-6;
                // Convert to milliseconds

               time_used *= 1000;

               printf("Execution time %f\n",time_used);

            }

            else if (status.MPI_TAG == 2) {
                struct timeval start, end;
                double time_used;
                gettimeofday(&start, NULL);

                if (isPalindrome(recv_msg))
                    printf("\nProcess %d  Type 2 --- Palindrome: %s\n",
                           status.MPI_SOURCE, recv_msg);
                else
                    printf("\nProcess %d  Type 2 --- Not Palindrome: %s\n",
                           status.MPI_SOURCE, recv_msg);
                gettimeofday(&end, NULL);

               time_used = (end.tv_sec - start.tv_sec) * 1e6;
               time_used = (time_used + (end.tv_usec - start.tv_usec)) * 1e-6;
                // Convert to milliseconds

               time_used *= 1000;

               printf("Execution time %f\n",time_used);


            }

        }
    }
    MPI_Finalize();
    return 0;
}