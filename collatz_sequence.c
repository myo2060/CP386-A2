#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAX 150

int main(){
    int startnum[MAX];
    int count = 0;
    const int SIZE = 4096;

    //open numbers file and read

    FILE* file = fopen("start_numbers.txt","r");
        if (file == NULL){
            //output error
            perror("ERROR: File start_numbers.txt was not opened.");
            return 1;
        }
        while (fscanf(file, "%d", &startnum[count]) == 1){
            count++;
        }

        //close file
        fclose(file);

        for (int i = 0; i < count; i++){
            int num = startnum[i];
            int shm_fd; //Shared memory file descriptor
            pid_t child_pid;

            //Create shared memory object
            shm_fd = shm_open("collatz_sequence", O_CREAT | O_RDWR, 0666);

            //Configure size of shared memory object
            
        }

}