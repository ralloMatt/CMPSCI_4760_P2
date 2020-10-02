# CMPSCI_4760_P2
Operating systems project 2.


NOTE:
	I believe the project runs correctly. When I did n = 5 and s = 1 I got 25,000. The project 
	should run as described in Project 2.
	
Troubles:
	Had trouble with signal handling at first. Was using time.h which threw me way off. Once 
	I checked out book I saw the function alarm() which saved me. 

Brief Overview of Files:
	1. Makefile - creates two executables to use.
	2. master.c - the main program as described in project.
	3. worker.c - the child process as described in project.
	4. README - the file you are reading. 
	
How to Run the program:
	It should run as described in the project with the command: 
		"./master -n <integer> -s <integer>"
	Using "./master -h" also shows how to run it.
	
Version Control:
	I used git again, just like the last project. You can use "git log" to see 
	my progress throughout the project.
	
*Don't really need to read below it's all commented in code. 

Overview of master.c:
	Going from the top down. I included the header files used. Defined my shared memory key
	and the buff size for it, just like the sanjiv's document on shared memory. Created some 
	global variables so I can kill the processes and define my alarm handler functions.
	Inside main I started with seeing what options were given in argument. I stored n and s if 
	-n and -s were after ./master. Then I set the signal alarm handlers for if two seconds passed 
	by using alarm(2) and for ctrl c with signal(SIGINT, intHandler). Then I just created 
	shared memory and checked for errors. Then I created the clock variables of seconds and 
	milliseconds. Then I used the for loop described in class and it worked perfectly for 
	creating the child processes. Then I just printed results and deallocated memory.
	The functions for alarm handlers are prettry straight forward.
	
Overview of worker.c:
	Again going from top down. I first got the argument passed, then I got access to the 
	shared memory from before. The clock is what is in shared memory. Then I added to the 
 	clock in milliseconds of n times 1 million times. If I hit 1000 I set milliseconds to  
	zero and added one to seconds. That's pretty much it.
	
