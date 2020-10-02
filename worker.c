#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMKEY	823566   // key for parent and child.
#define BUFF_SZ	sizeof ( int ) //size of memory used

int shmid;

int main (int argc, char *argv[]) {
	
	int n = atoi(argv[1]); //n which was passed from exec
	
	//access shared memory
	shmid = shmget ( SHMKEY, BUFF_SZ, 0777 ); 
	if(shmid == -1){ //checking error
		perror("Error in shmget\n\n");
		exit (1);
    }
	
	int *shm = (int *)(shmat( shmid, 0, 0));//pointer to shared memory
	char *sharedClockAddress = (char *) shm; //address to sharedClock
	int *sharedClockSeconds = (int *)(sharedClockAddress); //address for seconds
	int *sharedClockMilliSeconds = sharedClockSeconds + 1; //address for milliseconds

	
	//Now to change clock
	int value = n * 1000000; //n times 1 million
	int count = 0; //keep track of how many times clock was added to
	
	while(count != value){ //add one to milliseconds in clock "n million times"
		*sharedClockMilliSeconds += 1; //add millisecond
		if(*sharedClockMilliSeconds == 1000){ //when we reach 1000
			*sharedClockSeconds += 1; //add one second to clock
			*sharedClockMilliSeconds = 0; //set milliseconds to zero
		}
		count++; 
	}
	
	return 0; 
}