#include <stdio.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <stdbool.h>

#define RANGE 500000
#define DELAY 1000000

int main(int argc, char** argv){
	srand(time(NULL) ^ getpid());
	int id = atoi(argv[0]);
	FILE* fs;
	FILE* fr;
	char* studentNum;
	char* studentText;
	char* rubricText;
	int* array;
	int* counter;
	char filename[30];
	long duration;
	if ((studentText = (char *)shmat(atoi(argv[1]), NULL, 0)) == (char *)-1) {
               	perror("shmat failed");
               	return 1;
	}
	if ((counter = (int*)shmat(atoi(argv[4]), NULL, 0)) == (int*)-1) {
               	perror("shmat failed");
               	return 1;
       	}
	if ((rubricText = (char *)shmat(atoi(argv[2]), NULL, 0)) == (char *)-1) {
        	perror("shmat failed");
                return 1;
        }
	if ((array = (int *)shmat(atoi(argv[3]), NULL, 0)) == (int *)-1) {
               	perror("shmat failed");
               	return 1;
       	}
	if (*counter == 0){
		*counter = 1;
    		fs = fopen("Student1.txt", "r");
		if (fs == NULL) {
            		printf("initial file open fail");
			perror("Error opening file");
			return 1;
		}
		fgets(studentText, sizeof(char) * 5, fs);
		studentNum = studentText;
		printf("Accessed exam of student %s. (TA ID: %d)\n", studentNum, id);
		if (fclose(fs) != 0) {
       			perror("Error closing file");
			return 1;
		}
		fr = fopen("rubric.txt", "r");
		if (fr == NULL) {
                	printf("file open fail rubric\n");
                       	perror("Error opening file");
                       	return 1;
		}
		fread(rubricText, sizeof(char), 20, fr);
		rubricText[19] = '\0';
		if (fclose(fr) != 0) {
                        perror("Error closing file");
			return 1;
		}
	}
	int index;
	while(1){
		studentNum = studentText;
		for (int i = 0; i < 5; i++){
			duration = DELAY - rand() % RANGE;
			usleep(duration);
			if (rand() % 100 == 0){
				printf("correcting rubric. (TA ID: %d)\n", id);
				index = (i * 4) + 2;
				if (rubricText[index] == 'E') rubricText[index] = 'A';
				else rubricText[index] += 1;
				printf("%s\n", rubricText);
				fr = fopen("rubric.txt", "w");
                        	if (fr == NULL) {
                               		printf("file open fail for rubric.\n");
                               		perror("Error opening file");
                               		return 1;
                       		}
				if (fputs(rubricText, fr) == EOF) {
				        perror("Error writing new content to file");
					return 1;
				}
				if (fclose(fr) != 0) {
                               		perror("Error closing file");
                       			return 1;
				}
				printf("finished correcting rubric. (TA ID: %d)\n", id);
			}
		}
		printf("Finshed looking at rubric. (TA ID: %d)\n", id);
		int sum = 0;
		for(int x = 0; x < 5; x++){
			if (array[x] == 0){
				array[x] = 1;
				duration = DELAY + rand() % (RANGE * 2);
				usleep(duration);
				printf("Question %d has been graded. Exam of student %s. TA ID: %d\n", x + 1, studentNum, id);
				array[x] = 2;
			}
		}

		while(1){
			for (int i = 0; i < 5; i++){
                 	       sum += array[i];
                	}
			if (sum < 5){
				studentNum = studentText;
				sum = 0;
				printf("WORKED!!!!");
				break;
			}
			if (sum >= 5 && sum < 10){
				sum = 0;
				continue;
			}
			if (sum == 10){
				sum = 0;
 				*counter += 1;
				printf("student %s has been graded. (TA ID: %d)\n", studentNum, id);
				sprintf(filename, "Student%d.txt", *counter);
				fs = fopen(filename, "r");
				if (*counter == 34){
					for (int i = 0; i < 5; i++) array[i] = 0;
					break;
				}
				if (fs == NULL) {
                        		printf("loop file open fail");
                        		perror("Error opening file");
                        		return 1;
                		}
                		fgets(studentText, sizeof(char) * 5, fs);
		                studentNum = studentText;
        		        printf("Accessed exam of student %s. (TA ID: %d)\n", studentNum, id);
        		        if (fclose(fs) != 0) {
                		        perror("Error closing file");
                		        return 1;
                		}
				for (int i = 0; i < 5; i++) array[i] = 0;
				break;
			}
		}
		if (*counter == 34) break;
	}
}
