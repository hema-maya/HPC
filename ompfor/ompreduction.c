#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define CONSTANT 2.0

void process_array(long n) {
    // Memory Allocation
    double *A = (double *)malloc(n * sizeof(double));
    if (A == NULL) return;

    // Initialization
    for (long i = 0; i < n; i++) {
        A[i] = (double)i;
    }

    // Parallel Constant Addition & Timing
    double start = omp_get_wtime();

    #pragma omp parallel for schedule(static)
    for (long i = 0; i < n; i++) {
        A[i] += CONSTANT;
    }

    double end = omp_get_wtime();

    // Output Results using Tabs
    printf("Size:\t%ld\n", n);
    printf("Time:\t%f seconds\n", end - start);

    // Print 3 indices: Start, Middle, and End using tabs
    long indices[] = {0, n / 2, n - 1};
    printf("Index\tOriginal\tConstant\tResult\n");
    for (int k = 0; k < 3; k++) {
        long idx = indices[k];
        printf("%ld\t%.1f\t\t+ %.1f\t\t= %.1f\n", idx, (double)idx, CONSTANT, A[idx]);
    }
    printf("\n"); // New line for next size block

    free(A);
}

int main() {
    long sizes[] = {10000, 1000000, 10000000};

    for (int i = 0; i < 3; i++) {
        process_array(sizes[i]);
    }

    return 0;
}