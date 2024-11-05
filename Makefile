CC	=	/opt/ohpc/pub/mpi/openmpi3-gnu7/3.1.0/bin/mpicc
CCLINK	=	/opt/ohpc/pub/mpi/openmpi3-gnu7/3.1.0/bin/mpicc
SHELL	=	/bin/sh

EXEC	= jim-haslett-csc6220-hw5

main: jim-haslett-csc6220-hw5.c
	$(CC) -o $(EXEC) $(EXEC).c

clean:
	/bin/rm -f $(EXEC) $(EXEC)*.o $(EXEC)*.s jim-haslett-csc6220-hw5-serial jim-haslett-csc6220-hw5-sendrecv

serial:
	gcc -o jim-haslett-csc6220-hw5-serial jim-haslett-csc6220-hw5-serial.c

sendrecv: jim-haslett-csc6220-hw5.c
	$(CC) -o jim-haslett-csc6220-hw5-sendrecv jim-haslett-csc6220-hw5-sendrecv.c

all: main serial sendrecv
