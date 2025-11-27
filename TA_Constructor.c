#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/shm.h>


int main(int argc, char** argv){
	int studentKey;
	int rubricKey;
	int arrayKey;
	int counterKey;
	int* counter;
	int* array;
	if ((studentKey = shmget(IPC_PRIVATE, sizeof(char) * 5 , IPC_CREAT | 0666)) < 0){
		perror("student key creation fail");
			return 1;
	}
	if ((rubricKey = shmget(IPC_PRIVATE, sizeof(char) * 20 , IPC_CREAT | 0666)) < 0){
                perror("rubric key creation fail");
                return 1;
	}
	if ((arrayKey = shmget(IPC_PRIVATE, sizeof(int) * 5 , IPC_CREAT | 0666)) < 0){
                perror("array key creation fail");
                return 1;
	}
	if ((array = (int *)shmat(arrayKey, NULL, 0)) == (int *)-1) {
        	perror("shmat failed");
        	return 1;
	}
	for (int x = 0; x< 5; x++){
		array[x] = 0;
	}
	if ((counterKey = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
                perror("array key creation fail");
                return 1;
	}
	if ((counter = (int*)shmat(counterKey, NULL, 0)) == (int*)-1) {
               perror("shmat failed");
               return 1;
	}
	*counter = 0;
	for (int x=0; x<atoi(argv[1]); x++){
		int p = fork();
		if (p==0){
			char TA_id[30];
                	char semaphoreID[30];
			char sKey[30];
			char rKey[30];
			char aKey[30];
			char cKey[30];
                	sprintf(TA_id,"%d",x);
			sprintf(sKey, "%d", studentKey);
                        sprintf(rKey, "%d", rubricKey);
                        sprintf(aKey, "%d", arrayKey);
                        sprintf(cKey, "%d", counterKey);
			int execTest = execlp("./TA_Body", TA_id, sKey, rKey, aKey, cKey, NULL);
                        if (execTest == -1){
				perror("EXEC FAIL");
				break;
			}
		}
	}
	for (int x=0; x<atoi(argv[1]); x++){
    		wait(NULL); 
	}
}
