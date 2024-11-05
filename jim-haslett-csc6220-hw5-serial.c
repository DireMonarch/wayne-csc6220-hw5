/**
 * Copyright 2024 Jim Haslett
 *
 * This work part of a university assignment.  If you are taking the course
 * this work was assigned for, do the right thing, and solve the assignment
 * yourself!
 *
 */

#include <stdio.h>
#include <time.h>

#define MATRIX_SIZE 2048
#define OUTFILENAME "result.txt"
#define REPORTFILENAME "report.txt"

void add_sub_matrix_chunks(int *sub_matrix_a, int *sub_matrix_b, int *sub_matrix_c, int number_of_rows) {
    for(int j = 0; j < number_of_rows; j++) {
        for(int i = 0; i < MATRIX_SIZE; i++) {
            sub_matrix_c[j*MATRIX_SIZE + i] = 
                sub_matrix_a[j*MATRIX_SIZE + i] + 
                sub_matrix_b[j*MATRIX_SIZE + i];
        }
    }
}

double wtime() {
    /**
     * wtime function used to generate a wall time, in seconds, similar to MPI_wtime()
     * 
     * Based entirely on the MPI_wtime implementation:
     * https://github.com/open-mpi/ompi/blob/main/ompi/mpi/c/wtime.c
     */

    double wtime;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    wtime  = (double)tp.tv_nsec/1.0e+9;
    wtime += tp.tv_sec;
    return wtime;
}

int main(int argc,char* argv[]) {
    


    /* establish matricies */
    int matrix_a[MATRIX_SIZE * MATRIX_SIZE];
    int matrix_b[MATRIX_SIZE * MATRIX_SIZE];
    int matrix_c[MATRIX_SIZE * MATRIX_SIZE];

    /* Fill matricies */
    int val;
    for (int i=0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        val = i % MATRIX_SIZE + 1;
        matrix_a[i] = val;
        matrix_b[i] = val;
    }
    
    /* set up timers */
    double start_time, end_time;
    start_time = wtime();

    /* process our sub matricies */
    add_sub_matrix_chunks(matrix_a, matrix_b, matrix_c, MATRIX_SIZE);

    end_time = wtime();

    /* Output result matrix to file */
    FILE *outfile; /* File Pointer for output */
    outfile = fopen(OUTFILENAME, "w");
    for (int j=0; j < MATRIX_SIZE; j++) {
        for (int i=0; i < MATRIX_SIZE; i++) {
            fprintf(outfile, "%4d ", matrix_c[j*MATRIX_SIZE + i]);
        }
        fprintf(outfile, "\n");
    }
    fclose(outfile);        
    
    /* output execution time to file */
    outfile = fopen(REPORTFILENAME, "a");
    fprintf(outfile, "Serial process time: %f\n", end_time - start_time);
    fclose(outfile);

    /* Print execution time to screen*/
    printf("Serial process time: %f\n", end_time - start_time);

    return 0;
}