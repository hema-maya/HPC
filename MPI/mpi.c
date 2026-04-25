#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int roll_no;
    float total_marks;
    char grade;
} Student;

void calculate_grade(Student *s) {
    if (s->total_marks >= 90) s->grade = 'A';
    else if (s->total_marks >= 75) s->grade = 'B';
    else if (s->total_marks >= 50) s->grade = 'C';
    else s->grade = 'D';
}

int main(int argc, char **argv) {
    int rank, size, n_students;
    Student students[MAX_STUDENTS];
    Student local_students[MAX_STUDENTS];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        FILE *input_file = fopen("student_info.txt", "r");
        if (input_file == NULL) {
            perror("Error opening input file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        n_students = 0;
        while (fscanf(input_file, "%s %d %f", students[n_students].name, &students[n_students].roll_no, &students[n_students].total_marks) == 3) {
            n_students++;
        }
        fclose(input_file);
    }

    // Broadcast the number of students
    MPI_Bcast(&n_students, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Broadcast the students' data
    MPI_Bcast(students, sizeof(Student) * MAX_STUDENTS, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Calculate chunk size for each process
    int chunk_size = n_students / size;
    // Handle potential remainder if number of students is not evenly divisible
    int remainder = n_students % size;

    // Scatter the students' data
    MPI_Scatter(students, sizeof(Student) * chunk_size, MPI_BYTE, local_students, sizeof(Student) * chunk_size, MPI_BYTE, 0, MPI_COMM_WORLD);

    clock_t start = clock();

    // Each process except for rank 0 will process its assigned students
    for (int i = 0; i < chunk_size; i++) {
        calculate_grade(&local_students[i]);
        printf("Process %d: Name: %s, Roll No: %d, Marks: %.2f, Grade: %c\n",
               rank, local_students[i].name, local_students[i].roll_no, local_students[i].total_marks, local_students[i].grade);
    }

    // If this process is the last to handle the remainder, do that here
    if (rank == size - 1 && remainder > 0) {
        for (int i = 0; i < remainder; i++) {
            calculate_grade(&students[n_students - remainder + i]);
            printf("Process %d: Name: %s, Roll No: %d, Marks: %.2f, Grade: %c\n",
                   rank, students[n_students - remainder + i].name, students[n_students - remainder + i].roll_no,
                   students[n_students - remainder + i].total_marks, students[n_students - remainder + i].grade);
        }
    }

    clock_t end = clock();
    double elapsed_time = ((double)(end - start)) * 1000000000 / CLOCKS_PER_SEC;

    // Gather the results back to Process 0
    MPI_Gather(local_students, sizeof(Student) * chunk_size, MPI_BYTE, students, sizeof(Student) * chunk_size, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Process 0 writes results to file
    if (rank == 0) {
        FILE *output_file = fopen("grades_output.txt", "w");
        for (int i = 0; i < n_students; i++) {
            fprintf(output_file, "Name: %s, Roll No: %d, Marks: %.2f, Grade: %c\n",
                    students[i].name, students[i].roll_no, students[i].total_marks, students[i].grade);
        }
        fclose(output_file);
    }

    printf("Process %d execution time: %.f ns\n", rank, elapsed_time);

    MPI_Finalize();
    return 0;
}