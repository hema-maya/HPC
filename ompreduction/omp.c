#include<stdio.h>
#include<omp.h>

double f(double x) {
    return x*x;
}

int main() {
    int n, i, thread_count;
    double a, b, h;
    double sum = 0.0, global_sum = 0.0;

    printf("Enter Lower Limit(a): ");
    scanf("%lf", &a);
    printf("Enter Upper Limit(b): ");
    scanf("%lf", &b);
    printf("Enter number of Trapezoids: ");
    scanf("%d", &n);
    printf("Enter Thread Count: ");
    scanf("%d", &thread_count);

    h = (b - a) / n;
    printf("\nStep 1: h = (b-a)/n = (%.2lf - %.2lf)/%d = %.4lf\n", b, a, n, h);

    sum = (f(a) + f(b)) / 2.0;
    printf("\nStep 2: (f(a)+f(b))/2 = (%.2lf + %.2lf)/2 = %.4lf\n", f(a), f(b), sum);

    printf("\nStep 3: Parallel computation with %d threads:\n", thread_count);
    printf("\n");

    #pragma omp parallel num_threads(thread_count) reduction(+:global_sum)
    {
        int my_thread = omp_get_thread_num();
        double local_sum = 0.0;  // Each thread has its own local sum

        #pragma omp for
        for(i = 1; i < n; i++) {
            double x = a + i * h;
            double fx = f(x);
            local_sum += fx;  // Accumulate in local sum

            #pragma omp critical
            {
                printf("Thread %d: i=%d, x=%.4lf, f(x)=%.4lf\n", my_thread, i, x, fx);
            }
        }

        // Print local sum for each thread after the loop
        #pragma omp critical
        {
            printf("\nThread %d: Local Sum = %.4lf\n", my_thread, local_sum);
        }

        // Add local sum to global sum (handled by reduction)
        global_sum += local_sum;
    }


    printf("\nStep 4: Global Sum (combined from all threads) = %.4lf\n", global_sum);

    sum += global_sum;
    printf("\n Step 5: Total sum = %.4lf\n", sum);

    double integral = sum * h;
    printf("\nStep 6: Integral = sum * h = %.4lf * %.4lf = %.4lf\n", sum, h, integral);
    printf("\nEstimated Integral = %.4lf\n", integral);

    return 0;
}