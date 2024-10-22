#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>

void writeOutput(char *command, char *output){
    FILE *fp;
    fp = fopen("output.txt", "a");
    fprintf(fp, "The output of: %s : is\n",command);
    fprintf(fp, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n",output);
    fclose(fp);
}

void executeCommand(char *command){
    int pipefd[2];
    if (pipe(pipefd) == -1){
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t pid1 = fork();
    if (pipe(pipefd) == -1){
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t cmd_pid = fork();
    if (cmd_pid == -1){
        perror("Command fork failed");
        exit(1);
    }
    if (cmd_pid == 0){ //Child process for ocmmand execution
        close(pipefd[0]); //Close read end of the pipe

        //Redirect the output to the pipe
        dup2(pipefd[1], STDOUT_FILENO); //
        close(pipefd[1]);

        //Execute the commansd using execvp
        char *args[4];
        args[0] = "/bin/sh";
        args[1] = "-c";
        args[2] = command;
        args[3] = NULL;
        //If execvp fails, report the error
        perror("Command excution failed");
        exit(1);

    }
//Parent process
    else{
        close(pipefd[1]); //Close write end of the pipe

        char output[4096]; //Adjust the size as needed
        int bytes_read = read(pipefd[0], output, sizeof(output));
        if (bytes_read > 0){
            output[bytes_read] = '\0';
            writeOutput(command, output);

        }
        close(pipefd[0]);

        //Wait for the child process to finish
        wait(NULL);

    }
}

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;

    }
    const char *input_file = argv[1];
    FILE *file = fopen(input_file,"r");
    if (!file){
        perror("ERROR: Input file not opened.");
        return 1;

    }

    char command[256]; //Adjust the size as needed
    while (fgets(command, sizeof(command), file)){
        //Remove newline charcter at the end
        size_t len = strlen(command);
        if (len > 9 && command[len - 1] == '\n'){
            command[len - 1] = '\0';
        }
        executeCommand(command);
    }
    fclose(file);
    //Execute 'uname -a'
    executeCommand("uname -a");

    printf("SUCCESS: Check 'output.txt' for results.\n");
    return 0;
    
}
