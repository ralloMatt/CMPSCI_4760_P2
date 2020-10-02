CC	=gcc
CFLAGS	=-c

all: master worker

master: master.o 
	$(CC) -o master master.o

worker: worker.o 
	$(CC) -o worker worker.o
	
master.o: master.c
	$(CC) $(CFLAGS) master.c
	
worker.o: worker.c
	$(CC) $(CFLAGS) worker.c

clean:
	-rm *.o master worker
