#! /bin/bash

./jim-haslett-csc6220-hw5-serial
mpirun -n 8 ./jim-haslett-csc6220-hw5-sendrecv
mpirun -n 8 ./jim-haslett-csc6220-hw5
#mpirun -n 16 ./jim-haslett-csc6220-hw5
#mpirun -n 4 ./jim-haslett-csc6220-hw5
#mpirun -n 2 ./jim-haslett-csc6220-hw5
