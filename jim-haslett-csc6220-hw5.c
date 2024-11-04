/**
 * Copyright 2024 Jim Haslett
 *
 * This work part of a university assignment.  If you are taking the course
 * this work was assigned for, do the right thing, and solve the assignment
 * yourself!
 *
 */

#include <stdio.h>
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

    /* Initialize MPI */
    MPI_Init(&argc, &argv);

    /* Fetch rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number_of_processes; /* Number of processes running */

    /* Calculate number of rows per process */
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    int rows_per_process = MATRIX_SIZE / number_of_processes;
    int element_count = MATRIX_SIZE * rows_per_process;

    /* receive status variable */
    MPI_Status status;

    if(rank == 0) {
        /* This process is rank 0, therefore it's the controller */

        /* set up timers */
        double start_time, end_time;
        start_time = MPI_Wtime();

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

        /* Distribute submatricies to other processes */
        int *buffer_ptr_a = matrix_a;
        int *buffer_ptr_b = matrix_b;
        for(int i = 1; i < number_of_processes; i++) {
            /**
             * buffer_ptr_a + (element_count * i) will send a pointer to the location in the main array
             * offset by element_count * i, where element_count is the number of elements to send to each
             * process.  element_count is calcuated by multiplying the number of elements per row (MATRIX_SIZE)
             * by the number of rows calculated to send to each process (rows_per_process).
             * 
             * This is a bit of C "trickery" as an alternate way to access/refer to an array.
             */
            MPI_Send( buffer_ptr_a + (element_count * i), element_count , MPI_INT , i , 0 , MPI_COMM_WORLD); // tag 0 for matrix_a
            MPI_Send( buffer_ptr_b + (element_count * i), element_count , MPI_INT , i , 1 , MPI_COMM_WORLD); // tab 1 for matrix_b
        }

        /* process our sub matricies */
        add_sub_matrix_chunks(matrix_a, matrix_b, matrix_c, rows_per_process);  // this works on the main matricies only because rank 0 processes the first rows

        /* Collect results from other processes */
        int *buffer_ptr_c = matrix_c;
        for(int i = 1; i < number_of_processes; i++) {
            MPI_Recv( buffer_ptr_c + (element_count * i), element_count , MPI_INT , i , 2 , MPI_COMM_WORLD , &status); // tag 2 for matrix_c
        }

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

        /* output execution time to file */
        outfile = fopen(REPORTFILENAME, "a");
        fprintf(outfile, "MPI %d process time: %f\n", number_of_processes, end_time - start_time);
        fclose(outfile);

    } // if rank==0
    else {
        /* This process is not rank 0, therefore only a worker */

        /* establish matrix chunks */
        int sub_matrix_a[element_count];
        int sub_matrix_b[element_count];
        int sub_matrix_c[element_count];

        MPI_Recv( &sub_matrix_a , element_count , MPI_INT , 0 , 0 , MPI_COMM_WORLD , &status); // tag 0 for matrix_a
        MPI_Recv( &sub_matrix_b , element_count , MPI_INT , 0 , 1 , MPI_COMM_WORLD , &status); // tag 1 for matrix_b
        add_sub_matrix_chunks(sub_matrix_a, sub_matrix_b, sub_matrix_c, rows_per_process);
        MPI_Send( sub_matrix_c, element_count , MPI_INT , 0 , 2 , MPI_COMM_WORLD); // tag 2 for matrix_c      
    }

    printf("Process %d: Done\n", rank);

    MPI_Finalize();
    return 0;
}