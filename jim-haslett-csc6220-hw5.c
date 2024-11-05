/**
 * Copyright 2024 Jim Haslett
 *
 * This work part of a university assignment.  If you are taking the course
 * this work was assigned for, do the right thing, and solve the assignment
 * yourself!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

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

int main(int argc,char* argv[]) {
    
    /* MPI rank */
    int rank;

    /* Number of processes running */
    int number_of_processes;

    /* Timer variables */
    double start_time, end_time, c_start_time, c_end_time;

    /* establish matricies */
    int *matrix_a = NULL;
    int *matrix_b = NULL;
    int *matrix_c = NULL;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);

    /* Fetch rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Calculate number of rows per process */
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    int rows_per_process = MATRIX_SIZE / number_of_processes;
    int element_count = MATRIX_SIZE * rows_per_process;

    /* establish sub matrix chunks */
    int sub_matrix_a[element_count];
    int sub_matrix_b[element_count];
    int sub_matrix_c[element_count];   

    if(rank == 0) {
        /* This process is rank 0, therefore it's the root */

        /* allocate space for matricies */
        matrix_a = (int *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        matrix_b = (int *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        matrix_c = (int *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));

        /* Fill matricies */
        int val;
        for (int i=0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
            val = i % MATRIX_SIZE + 1;
            matrix_a[i] = val;
            matrix_b[i] = val;
        }

        /* Record start time */
        start_time = MPI_Wtime();

    } // if rank==0
 
    /* Distribute submatricies to all processes */
    MPI_Scatter( matrix_a, element_count, MPI_INT, sub_matrix_a, element_count, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter( matrix_b, element_count, MPI_INT, sub_matrix_b, element_count, MPI_INT, 0, MPI_COMM_WORLD);

    /* Record calculation start time */
    c_start_time = MPI_Wtime();

    /* process our sub matricies */
    add_sub_matrix_chunks(sub_matrix_a, sub_matrix_b, sub_matrix_c, rows_per_process);
    
    /* Record calculation end time */
    c_end_time = MPI_Wtime();

    /* Per instructions, local summation is complete, print "Process i: Done" */
    printf("Process %d: Done\n", rank);

    /* Collect results from all processes */
    MPI_Gather( sub_matrix_c, element_count, MPI_INT, matrix_c, element_count, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        /* This process is rank 0, therefore it's the root */

        /* record end time */
        end_time = MPI_Wtime();

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


        /* free allocated memory */
        free(matrix_a);
        free(matrix_b);
        free(matrix_c);

        /* calculate times */
        double total_time = end_time - start_time;
        double calc_time = c_end_time - c_start_time;
        double comm_time = total_time - calc_time;

        /* output execution time to file */
        outfile = fopen(REPORTFILENAME, "a");
        fprintf(outfile, "MPI %d process time: %f  Calculation time: %f  Communication Time: %f\n", number_of_processes, total_time, calc_time, comm_time);
        fclose(outfile);

        /* Print execution time to screen*/
        printf("MPI %d process time: %f  Calculation time: %f  Communication Time: %f\n", number_of_processes, total_time, calc_time, comm_time);

    } // if rank==0



    MPI_Finalize();
    return 0;
}