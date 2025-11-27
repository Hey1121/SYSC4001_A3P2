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
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		void *__buf;
	};


	union semun arg;
	int studentKey;
	int rubricKey;
	int arrayKey;
	int counterKey;
	int* counter;
	int* array;
	int semaphoreID;

	key_t key1 = ftok("TA_Constructor.c", 'S');
	key_t key2 = ftok("TA_Constructor.c", 'R');
 	key_t key3 = ftok("TA_Constructor.c", 'A');
 	key_t key4 = ftok("TA_Constructor.c", 'C');
	key_t key5 = ftok("TA_Constructor.c", 'L');

	if ((studentKey = shmget(key1, sizeof(char) * 5 , IPC_CREAT | 0666)) < 0){
		perror("student key creation fail");
			return 1;
	}
	if ((rubricKey = shmget(key2, sizeof(char) * 20 , IPC_CREAT | 0666)) < 0){
                perror("rubric key creation fail");
                return 1;
	}
	if ((arrayKey = shmget(key3, sizeof(int) * 5 , IPC_CREAT | 0666)) < 0){
                perror("array key creation fail");
                return 1;
	}
	if ((array = (int *)shmat(arrayKey, NULL, 0)) == (int *)-1) {
        	perror("array shmat failed");
        	return 1;
	}
	for (int x = 0; x< 5; x++){
		array[x] = 0;
	}
	if ((counterKey = shmget(key4, sizeof(int), IPC_CREAT | 0666)) < 0){
                perror("array key creation fail");
                return 1;
	}
	if ((counter = (int*)shmat(counterKey, NULL, 0)) == (int*)-1) {
               perror("shmat failed");
               return 1;
	}
	*counter = 0;
	if ((semaphoreID = semget(IPC_PRIVATE, 4, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        	perror("semget failed");
    		return -1;
    	}
	unsigned short semSet[4] = {1, 1, 1, 1};
	arg.array = semSet;
	if (semctl(semaphoreID, 0, SETALL, arg) == -1){
		perror("semaphore set fail");
		return 1;
	}
	for (int x=0; x<atoi(argv[1]); x++){
		int p = fork();
		if (p==0){
			char TA_id[30];
			char sKey[30];
			char rKey[30];
			char aKey[30];
			char cKey[30];
         		char semID[30];
			sprintf(TA_id,"%d",x);
			sprintf(sKey, "%d", studentKey);
                        sprintf(rKey, "%d", rubricKey);
			sprintf(aKey, "%d", arrayKey);
                        sprintf(cKey, "%d", counterKey);
			sprintf(semID, "%d", semaphoreID);
			int execTest = execlp("./TA_Body2", TA_id, sKey, rKey, aKey, cKey, semID, NULL);
                        if (execTest == -1){
				perror("EXEC FAIL");
				break;
			}
		}
	}
	for (int x=0; x<atoi(argv[1]); x++){
    		wait(NULL); 
	semctl(semaphoreID, 0, IPC_RMID);
	}
}
