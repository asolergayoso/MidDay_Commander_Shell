#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


struct process_info{
	char* pname;
	int pid;
	int pnum;
	struct timeval start;	
	struct rusage usage1;
};
	
int newP (int pid, char* command, char* envp[]);
int operationsNumber (int option);
int operationsLetter (char* letter, char* commandList[], int* counter, int flag, struct process_info** ps, int* counter2);
int printNewCommand(char* commandList[], int* counter);
int opsAddComm (int option, char* commandList[], int* counter, struct process_info** ps, int* counter2);
int runningP(char* newCommand, char* commandList[], int option, struct process_info** ps, int* counter2);
void print_back_proc (struct process_info* pinfo);
int* decreaseBParray(int* counter2, struct process_info* pinfo, struct process_info** ps);
void statistics(struct timeval start, struct timeval end, long pageFault, long pageReclaimed);


#define MAX_NEW_COMMANDS 50

int main(){

	printf("==== Mid-Day Commander, v0 ====\n");
	char* commandList[MAX_NEW_COMMANDS];
	int x = 0;
	int y = 0;
	int* counter = &x;
	int flag = 1;
	int* counter2 = &y;
	long pageFault1, pageFault2, pageFault;
	long pageReclaimed1, pageReclaimed2, pageReclaimed;
	struct rusage usage2;
	struct process_info** ps = (struct process_info**) malloc(10*sizeof(ps));

	while (1){		

		printf("G'day, Commander! What command would you like to run?\n"); 
        	printf("   0. whoami : Prints out the result of the whoamicommand\n");
        	printf("   1. last   : Prints out the result of the last command\n");
        	printf("   2. ls     : Prints out the result of a listing on a user-specified path\n");
		printNewCommand(commandList, counter);
		printf("   a. add command : Adds a new command to the menu\n");
                printf("   c. change directory : Changes process working directory\n");
		printf("   e. exit : Leave Mid-Day Commander\n");
		printf("   p. pwd : Prints working directory\n");
		printf("   r. running processes : Print list of running processes\n");
        	printf("Choose option: ");

        	char* p = (char*) malloc(sizeof(char*));

        	scanf("%s", p);
        	
        	while(1){
		
			pid_t t = wait3(NULL, WNOHANG, &usage2);
			
			
			if(t == 0) {
				//printf("\nBackground Processes still working\n");
				flag = 0;
				break;
			}
			if (t > 0) {
			
				//printf("test\n");
				struct timeval end;
				gettimeofday(&end, NULL);
				//getrusage(RUSAGE_CHILDREN, &usage2); 			
			
				int posi;
			
				for (int i = 0; i < *counter2; i++){
					if (ps[i]->pid == t){
					posi = i;
					}
				}
			
				struct timeval start = ps[posi]->start;
			
				struct rusage usage1 = ps[posi]->usage1;
				pageFault1 = usage1.ru_majflt, pageReclaimed1 = usage1.ru_minflt;
				getrusage(RUSAGE_CHILDREN, &usage2); 			
				pageFault2 = usage2.ru_majflt, pageReclaimed2 = usage2.ru_minflt;	
				pageFault = (pageFault2 - pageFault1);
				pageReclaimed = (pageReclaimed2 - pageReclaimed1);
				
				flag = 1;
				printf("\n -- Job Complete [%d] --\n", ps[posi]->pnum);
				printf("Process ID: %d\n", t);
				printf("Command: %s\n",  ps[posi]->pname);
				statistics(ps[posi]->start, end, pageFault, pageReclaimed);	
				//decreaseBParray(counter2, ps[posi], ps);
				
			
			}			
			if (t == -1){	
				break;
			}
				
		}

		if (isdigit(*p) == 0){
			operationsLetter (p, commandList, counter, flag, ps, counter2);

		}else{
			int option = atoi(p);

			if(option > 2){
				opsAddComm(option,commandList, counter, ps, counter2);

			}else{
        			operationsNumber (option);

			}
		}
		free(p);
	}
	return 0;
}



int operationsNumber (int option){	

	struct timeval start;
	struct timeval end;
	struct rusage info;
	long pageFault1, pageFault2, pageFault;
	long pageReclaimed1, pageReclaimed2, pageReclaimed;

	switch(option){	

	 	case 0:
			printf("\n-- Who Am I? --\n");

			gettimeofday(&start, NULL);			

			int pid1 = fork(); //creates a new process to handle the command

			getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
			pageFault1 = info.ru_majflt, pageReclaimed1 = info.ru_minflt;

			char* command1 = "whoami";

			char* envp1[] = {"whoami", NULL};			

			newP(pid1, command1, envp1);
			
			getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
			pageFault2 = info.ru_majflt, pageReclaimed2 = info.ru_minflt;	
			pageFault = (pageFault2 - pageFault1);
			pageReclaimed = (pageReclaimed2 - pageReclaimed1);

			gettimeofday(&end, NULL);

			statistics(start, end, pageFault, pageReclaimed);			

			break;                

        	case 1:

			printf("\n-- Last Logins --\n");

			gettimeofday(&start, NULL);			

                	int pid2 = fork(); //creates a new process to handle the command
			
			getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
			pageFault1 = info.ru_majflt, pageReclaimed1 = info.ru_minflt;

			char* command2 = "last";

			char* envp2[] = {"last", "-n 1", NULL};			

			newP(pid2, command2, envp2);

			gettimeofday(&end, NULL);
		
			getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
			pageFault2 = info.ru_majflt, pageReclaimed2 = info.ru_minflt;	
			pageFault = (pageFault2 - pageFault1);
			pageReclaimed = (pageReclaimed2 - pageReclaimed1);

			statistics(start, end, pageFault, pageReclaimed);			

			break;		

		 case 2:

			printf("-- Directory Listing --\n");

			char* argument = (char*) malloc(20);			

			printf("Argument: ");

			scanf("%s", argument);           

			char* path = (char*) malloc(200);

			printf("Path: ");

			scanf("%s", path);

			char* command3 = "ls";

			char* envp3[] = {"ls", argument, path, NULL}; //creates an evirnonmrnt to be used by execle		

			gettimeofday(&start, NULL);			

			int pid3 = fork(); //creates a new process to handle the command
		
			getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
			pageFault1 = info.ru_majflt, pageReclaimed1 = info.ru_minflt;


			newP(pid3, command3, envp3);

			free(argument);

			free(path);

			gettimeofday(&end, NULL);

			getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
			pageFault2 = info.ru_majflt, pageReclaimed2 = info.ru_minflt;	
			pageFault = (pageFault2 - pageFault1);
			pageReclaimed = (pageReclaimed2 - pageReclaimed1);

			break;		

		default:

			fprintf(stderr, "\n ERROR: Please, type the number of one of the listed options!\n\n");

			//gettimeofday(&end, NULL);

			break;		

	}
	return 0;	

}



void statistics(struct timeval start, struct timeval end, long pageFault, long pageReclaimed/*struct rusage* usage*/){
	

	printf("--- Satistics ---\n");		

	//long pageFault = usage->ru_majflt, pageReclaimed = usage->ru_minflt;	

	float timeElap =  ((float) end.tv_usec - (float) start.tv_usec)/1000 + ((float)end.tv_sec - (float) start.tv_sec)*1000;
		
	//getrusage(RUSAGE_CHILDREN, usage); //to calculate the info of the children

	printf("Elapsed Time: %.3f milliseconds\n", timeElap);//, timeElap.tv_usec);	

	printf("Page Faults: %ld\n", pageFault);

	printf("Page Faults (reclaimed): %ld\n\n", pageReclaimed);
}


int newP (int pid, char* command, char* envp[]){	

	if (pid < 0){    //failed duplication of process

		fprintf(stderr,  "creation of new process failed\n");

		exit(1);		

	}else if (pid == 0){	//this is the child process	

		execvp(command, envp); //it runs the command whoami

	}else{   //this would logically be the parent process			

		wait(NULL);

		printf("\n");						

	}
	return 0;
}


int operationsLetter (char* letter, char* commandList[], int* counter, int flag, struct process_info** ps, int* counter2){

	switch(*letter){		

		case 'a':

			printf("\n-- Add Command --\n");	

			printf("Command to add: ");
			
			size_t buffer = 100;

			char* newCommand = (char*) malloc(buffer*sizeof(char));//scanf("%s", newCommand);

			getchar();

			size_t t = getline(&newCommand, &buffer,  stdin);
			
			if(newCommand[t - 1] == '\n') newCommand[t - 1] = '\0';	

			commandList[*counter] = (char*) malloc(buffer*sizeof(char));

			strcpy(commandList[*counter], newCommand);
			
			printf("Okay, added with ID %d\n\n", (*counter + 3));

			*counter = *counter + 1;	
				
			buffer = 0;

			free(newCommand);

			break;	

		case 'c':

			printf("\n-- Change Directory --\n");

			printf("New Directory: ");

			char* newDir = (char*) malloc(sizeof(newDir));

			scanf("%s", newDir);

			chdir(newDir);

			printf("\n");

			break;


		case 'p':

			printf("\n-- Current Directory --\n");

			char* command = "pwd";

			char* envp[] = {"pwd", NULL};

			int pid = fork();

			newP(pid, command, envp);

			break;	
		

		case 'e':
			
			if (flag == 0){
				printf(" You need to stay Commander, there are still background processes running!\n\n");
			}else{
			
				printf("Logging you out, Commander!\n");
				exit(1);
			}

			break;
			
		case 'r':
			
			printf("\n-- Background Processes --\n");
			for (int i = 0; i < *counter2;	i++){
				print_back_proc (ps[i]);
			}
			printf("\n");
			break;		


		default:

			fprintf(stderr, "\n ERROR: Please, type the number of one of the listed options!\n\n");

			break;
	}
	return 0;
}



int parseCommand (char* command, char* fullCommand[]){	

	char delim[3] = " \n";

	char* token = strtok(command, delim);

	int i = 0;//COUNTER



	while(token != NULL){

		fullCommand[i] = (char*) malloc(sizeof(token));

		strcpy(fullCommand[i], token);

		token = strtok(NULL, delim);

		i++;
	}

	fullCommand[i] = NULL;

	//printf("fullCommand[0], fullCommand[1], fullCommand[2]: %s, %s, %s\n", fullCommand[0],fullCommand[1],fullCommand[2]);

	return i;
}	

		

int printNewCommand(char* commandList[], int* counter){

	if(*counter == 0){

		 return 1;

	}else{
		for(int i = 0; i < *counter; i++){	
		printf("   %d. %s   : User added Command\n", (i + 3), commandList[i]);
		}
	}
	return 0;
}



int opsAddComm (int option, char* commandList[], int* counter, struct process_info** ps, int* counter2){

	struct timeval start;
	struct timeval end;
	struct rusage info;
	long pageFault1, pageFault2, pageFault;
	long pageReclaimed1, pageReclaimed2, pageReclaimed;
	char* newCommand = (char*) malloc(sizeof(commandList[option - 3]));
	strcpy(newCommand, commandList[option - 3]);	

	if (option > (*counter+2)){

		fprintf(stderr, "\n ERROR: Please, type the number of one of the listed options!\n\n");

		 return 1;

	}else if (strchr(newCommand, '&') != NULL){
			//char* newCommand1 = (char*) malloc(sizeof(char*));
			//strcpy(newCommand1, newCommand);
			runningP(newCommand, commandList, option, ps, counter2);
	}else{
		
		printf("\n-- Command: %s --\n", newCommand);		

		char** fullCommand = (char**) calloc(strlen(commandList[option - 3]), sizeof(char*)); 

		int argNum = parseCommand(newCommand, fullCommand);

		//printf("argnum: %d\n", argNum);

		char* command = (char*) malloc(sizeof(fullCommand[0]));

		strcpy(command, fullCommand[0]);
		
		gettimeofday(&start, NULL);

		int pid = fork();

		getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
		pageFault1 = info.ru_majflt, pageReclaimed1 = info.ru_minflt;		

		newP(pid, command, fullCommand);

		gettimeofday(&end, NULL);

		getrusage(RUSAGE_CHILDREN, &info); //to calculate the info of the children
		pageFault2 = info.ru_majflt, pageReclaimed2 = info.ru_minflt;	
		pageFault = (pageFault2 - pageFault1);
		pageReclaimed = (pageReclaimed2 - pageReclaimed1);

		statistics(start, end, pageFault, pageReclaimed);

		for (int c = argNum; c = 0; c--){
			free(fullCommand[c]);
		}
	}
	return 1;

}

int runningP(char* newCommand, char* commandList[], int option, struct process_info** ps, int* counter2){

	printf("\n-- Command: %s --\n", newCommand);
	int flag = 1;
	struct process_info* pinfo = (struct process_info*) malloc(sizeof(pinfo));	
	pinfo->pname = (char*) malloc(sizeof(newCommand));
	strcpy(pinfo->pname, newCommand); 
	char** fullCommand = (char**) calloc(strlen(commandList[option - 3]), sizeof(char*));	
	int argNum = parseCommand(newCommand, fullCommand);
	char* command = (char*) malloc(sizeof(fullCommand[0]));
	strcpy(command, fullCommand[0]);
	for(int i = 0; i < argNum; i++){
		if (strchr(fullCommand[i], '&') != NULL){
			 fullCommand[i] = NULL;
		}	
	}
	struct timeval start;
	gettimeofday(&start, NULL);
	pinfo->start = start;
	
	int pid = fork();
	struct rusage usage;
	getrusage(RUSAGE_CHILDREN, &usage);
	pinfo->usage1=usage;
	
	pinfo->pid = pid;
	pinfo->pnum = *counter2 + 1;
	
	//printf("test\n");
	int c, d;
	//struct rusage* usage;
	
	
	
	if (pid < 0){    //failed duplication of process
		fprintf(stderr,  "creation of new process failed\n");
		exit(1);
	}		

	if  (pid == 0){	//this is the child process		
		
		execvp(command, fullCommand); //it runs the command whoami	
	}
	if (pid > 0){
		printf("[%d] %d\n", pinfo->pnum, pinfo->pid);
	}
	
	ps[*counter2] = pinfo;
	*counter2 = *counter2 + 1;
	printf("\n");					
	return flag;
}


void print_back_proc (struct process_info* pinfo){
	printf("[%d] %d %s\n", pinfo->pnum, pinfo->pid, pinfo->pname);
}


int* decreaseBParray(int* counter2, struct process_info* pinfo, struct process_info** ps){
	
	int p = pinfo->pnum - 1;
	
	for (p; p < *counter2; p++){
		 ps[p] = ps[p + 1];
	}
	*counter2 = *counter2 - 1;
	return counter2;
}
	
	
	
	
	









