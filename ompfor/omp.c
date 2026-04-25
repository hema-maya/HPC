#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
    const int N = 1000;
    const int PRINT_ROWS = 20; /* set to N to print every iteration */
    int *A = malloc((size_t)N * sizeof(int));
    int *B = malloc((size_t)N * sizeof(int));
    int *C = malloc((size_t)N * sizeof(int));
    if (!A || !B || !C) { perror("malloc"); return 1; }

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        A[i] = i;
        B[i] = 2*i;
    }

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        C[i] = A[i] + B[i];
    }

    printf("Index\tA\tB\tValue\n");
    for (int i = 0; i < PRINT_ROWS && i < N; ++i) {
        printf("%d\t%d\t%d\t%d\n", i,A[i],B[i],C[i]);
    }

    free(A); free(B); free(C);
    return 0;
}