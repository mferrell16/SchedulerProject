/*****************************************************************
*    main.c - Gusty - began with code from https://codereview.stackexchange.com/questions/67746/simple-shell-in-c
********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "types.h"
#include "defs.h"
#include "proc.h"

void parseCmd(char* cmd, char** params, int *nparams);
int executeCmd(char** params, int nparams);

#define MAX_COMMAND_LENGTH 100
#define MAX_NUMBER_OF_PARAMS 10

enum cmds        { FORK=0, SETPID,   SHOWPID,   WAIT,   EXIT,   SLEEP,   WAKEUP,   PS,   SCHEDULE,  TIMER,    HELP,   QUIT, LOTTERY };
char *cmdstr[] = {"fork", "Setpid", "currpid",  "wait", "exit", "sleep", "wakeup", "ps",   "schedule", "timer", "help", "quit", "lottery"};

int curr_proc_id = 0;

int local_scheduler() {
    scheduler();
    struct proc *p = curr_proc;
    return p->pid;
}

int main()
{
	printf("***** NOTES TO KNOW ******\n");
	printf("you can now enter the command 'lottery'. It will tell you usage.\n");
	printf("Proc object now has an extra varibale for ticket and global variable TotalTickets was added to Proc.C \n"); 
	printf("In the scheduler() function, if you uncomment the print statements it is easier to see how the code runs.\n");
    pinit(); // initialize process table
    curr_proc_id = userinit(); // create first user process
    char cmd[MAX_COMMAND_LENGTH + 1];
    char* params[MAX_NUMBER_OF_PARAMS + 1];
    int cmdCount = 0, nparams = 0;

    while(1) {
        nparams = 0; // > Fork 4 command sets nparams to 2
        char* username = getenv("USER");
        printf("%s@shell %d> ", username, ++cmdCount);
        if(fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        if(cmd[strlen(cmd)-1] == '\n') cmd[strlen(cmd)-1] = '\0';
        parseCmd(cmd, params, &nparams);
        if(strcmp(params[0], "Quit") == 0) break;
        if(executeCmd(params, nparams) == 0) break;
    }

    return 0;
}

// Split cmd into array of parameters
void parseCmd(char* cmd, char** params, int *nparams)
{       
    for(int i = 0; i < MAX_NUMBER_OF_PARAMS; i++) {
        params[i] = strsep(&cmd, " ");
        if(params[i] == NULL) break;
        (*nparams)++;
    }
}

int executeCmd(char** params, int nparams)
{
    int pid, rc = 1, chan;
    int ncmds = sizeof(cmdstr) / sizeof(char *);
    int cmd_index;
    for (cmd_index = 0; cmd_index < ncmds; cmd_index++)
        if (strcmp(params[0], cmdstr[cmd_index]) == 0){
            break;
		}
	
    //for (int i = 0; i < nparams; i++)
        //printf("Param %d: %s\n", i, params[i]);
    //printf("ncmds: %d, cmd_index: %d\n", ncmds, cmd_index);
    
    switch (cmd_index) {
    case FORK:
        if (nparams > 1)
            pid = atoi(params[1]);
        else
            pid = curr_proc->pid;
        int fpid = Fork(pid);
        printf("pid: %d forked: %d\n", pid, fpid);
        break;
    case SETPID:
        if (nparams == 1)
            printf("setpid cmd requires pid parameter\n");
        else
            curr_proc_id = atoi(params[1]);
        break;
    case SHOWPID:
        //printf("Current pid: %d\n", curr_proc_id);
        printf("Current pid: %d\n", curr_proc->pid);
        break;
    case WAIT:
        if (nparams > 1)
            pid = atoi(params[1]);
        else
            pid = curr_proc->pid;
        int wpid = Wait(pid);
        if (wpid == -1)
            printf("pid: %d has no children to wait for.\n", pid);
        else if (wpid == -2)
            printf("pid: %d has children, but children still running.\n", pid);
        else
            printf("pid: %d child %d has terminated.\n", pid, wpid);
        break;
    case EXIT:
        if (nparams > 1)
            pid = atoi(params[1]);
        else
            pid = curr_proc->pid;
        pid = Exit(pid);
        printf("Exit Status:: %d .\n", pid);
        break;
    case SLEEP:
        if (nparams < 2)
            printf("Sleep chan [pid]\n");
        else {
            chan = atoi(params[1]);
            if (nparams > 2)
                pid = atoi(params[2]);
            else
                pid = curr_proc->pid;
            pid = Sleep(pid, chan);
            printf("Sleep Status:: %d .\n", pid);
        }
        break;
    case WAKEUP:
        if (nparams < 2)
            printf("Wakeup chan\n");
        else {
            chan = atoi(params[1]);
            Wakeup(chan);
        }
        break;
    case PS:
        procdump();
        break;
    case SCHEDULE:
        pid = local_scheduler();
        printf("Scheduler selected pid: %d\n", pid);
        break;
    case TIMER:
        if (nparams < 2)
            printf("timer quantums\n");
        else {
            int quantums = atoi(params[1]);
            for (int i = 0; i < quantums; i++) {
                pid = local_scheduler();
                printf("Scheduler selected pid: %d\n", pid);
            }
        }
        break;
    case HELP:
        printf("Commands: fork, wait, exit, ps, Setpid, currpid, sleep, wakeup, timer, help\n");
        break;
    case QUIT:
        rc = 0;
        break;
    case LOTTERY:
		if(nparams ==1){
			printf("To use this command, enter lottery <pid> to increment by 1 ticket and lottery <pid> <numTickets> to increment by specific number of tickets\n");
			}
		else if( nparams ==2){
			TicketIncrease(atoi(params[1]), 1);
			}
		else if( nparams == 3){
			TicketIncrease(atoi(params[1]), atoi(params[2]));
			}
		break;
    default:
        printf("Invalid command! Enter Help to see commands.\n");
    }
    
    
    return rc;
}
