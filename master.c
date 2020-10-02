#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include  <signal.h>

#define SHMKEY	823566   // key for parent and child.
#define BUFF_SZ	sizeof ( int ) //size of memory used

int shmid; //so I can free memory with signal handlers
pid_t child_pid; //so I can kill child process if needed
void alarmHandler(int sig); //used to deallocate memory and kill child processes if two seconds pass
void intHandler(int sig); //used to deallocate memory and kill child processes if ctrl c is pressed

int main (int argc, char *argv[]) {
	
	int opt; //option

	int n;
	int s;
	
	if (argc < 2){   //if only one argument is given "./master"
		printf("Error! Only one argument!\n");
		fprintf(stderr, "Usage: ./master -n <integer> -s <integer>\n", argv[0]);
		return 1; 
	}  
	
	while ((opt = getopt (argc, argv, "hns")) != -1){ //get option
	
		switch (opt) { //check cases 
			case 'h':
				printf("\nExpected input:    ");
				printf("./master -n <integer> -s <integer>\n");
				printf("Where 'n' is the maximum number of processes to fork off.\n");
				printf("And 's' is the number of children that can exist at the same time.\n\n");
				return 0;
				break;
			case 'n':
				if (argc == 5){ 
					//set variables to use for later
					n = atoi(argv[2]);
					s = atoi(argv[4]);
				}
				else {
					printf("Please give value for -s as well.\n");
					printf("Refer to -h for help.\n");
					return 0;
				}
				break;
			case 's':
				if (argc != 5){
					printf("Invalid option! Refer to -h for help.\n");
					return 0;
				}
				else {
					break;
				}
			default: //invalid option
				return 0;
		}
		
		
	}
	

	
	//Using alarm so program ends if it runs for more than 2 seconds (Number 4 on order) 
	signal(SIGALRM, alarmHandler); //create signal handler
	alarm(2); //alarm goes off after two seconds
	
	//signal handler to catch ctrl c
	signal(SIGINT, intHandler);
		
	//create shared memory (should be in parent)
	shmid = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT );
	
	if (shmid == -1){ //checking error
		perror("Error in shmget\n");
		exit (1);
    }
	
	//Get pointer to shared memory
	char *shm = shmat(shmid, 0, 0);
	
	if(shm == (char *) -1){
		perror("Error in shmat\n");
		exit(1);
	}
	
	//What I want to share in memory for now
	char *sharedClockAddress = (char *) shm; //the clock address
	
	//initializing the clock for seconds and milliseconds
	int *sharedClockSeconds = (int *)(sharedClockAddress); //address for seconds
	*sharedClockSeconds = 0; //seconds for clock
	int *sharedClockMilliSeconds = sharedClockSeconds + 1; //address for milliseconds
	*sharedClockMilliSeconds = 0; //milliseconds for clock
	
	//now to call child which is the worker	
	char arg1[10];
	snprintf(arg1,10,"%d", n);
	
	int i = 0;
	
	//n is the number of workers (processes to fork off)
	//s is the number of processes that can run simultaneously 
	
	for(i=0; i <= n - s; i++){
		wait();
		child_pid = fork();
		//fork off a copy
		if (child_pid == 0) { //in child		 
			//make exec call
			execlp("./worker","./worker",arg1,(char *)NULL);

			// If we get here, exec failed
			fprintf(stderr,"%s failed to exec worker!\n\n",argv[0]);
			exit(-1);
		}
	}
	
	wait(); //wait for child processes to ends
	//print results :P
	printf("\nClock seconds is %d\n", *sharedClockSeconds);
	printf("sharedClock milliseconds is %d\n\n", *sharedClockMilliSeconds);
	printf("Clock = %d : %d \n\n", *sharedClockSeconds, *sharedClockMilliSeconds);
	
	//detach and deallocate shared memory and check for errors	
	if(shmdt(shm) == -1)
		perror("Error in shmdt\n");	
	if(	shmctl(shmid, IPC_RMID,NULL) == -1)
		perror("Error in shmctl\n");
	
	
	return 0; 
}

void alarmHandler(int sig){ //program ends if two seconds pass
	signal(sig, SIG_IGN);
	printf("Two seconds already passed!!!\n");	
	printf("Program Terminated!!!\n");	

	//kill children
	kill(child_pid, SIGKILL);
	//free up any memory
	if(	shmctl(shmid, IPC_RMID,NULL) == -1) //deallocate shared memory and check for error
		perror("Error in shmctl\n");
	exit(0);
}

void intHandler(int sig){ //program ends if ctrl c is pressed
	signal(sig, SIG_IGN);
	printf("\nYou've pressed ctrl c!!!\n");	
	printf("Program Terminated!!!\n");	

	//kill children
	kill(child_pid, SIGKILL);
	//free up any memory
	if(	shmctl(shmid, IPC_RMID,NULL) == -1) //deallocate shared memory and check for error
		perror("Error in shmctl\n");
	exit(0);
}
