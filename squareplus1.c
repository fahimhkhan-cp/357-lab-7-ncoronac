#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int  main(){
    int pipe1[2], pipe2[2], pipe3[2];
    pid_t pid1, pid2;
    int num = 0;
    int squared_val;
    int result;

    while (scanf("%d", &num) != EOF){

        // Create the pipes
        if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
            perror("pipe failed");
            return 1;
        }

        // Fork the first child process
        if ((pid1 = fork()) < 0) {
            perror("fork failed");
            return 1;
        }
        if (pid1 == 0){
            // square the received value and forward the result to the other child
            close(pipe1[1]); // Close unused write end of the pipe
            close(pipe2[0]); // 
            close(pipe3[0]); // 
            close(pipe3[1]); //

            // Read number from parent, square and send recieved value to second child
            read(pipe1[0], &num, sizeof(int));
            squared_val = num * num;
            write(pipe2[1], &squared_val, sizeof(int));

            close(pipe1[0]);
            close(pipe2[1]);
            exit(0);
        }

        // Fork the second child process
        if ((pid2 = fork()) < 0) {
            perror("fork failed");
            return 1;
        } 
        if (pid2 == 0){
            // square the received value and forward the result to the other child
            close(pipe1[0]);
            close(pipe1[1]); 
            close(pipe2[1]); 
            close(pipe3[0]);

            // Read number from child 1, square and send recieved value to parent
            read(pipe2[0], &num, sizeof(int));
            result = num + 1;
            write(pipe3[1], &result, sizeof(int));

            close(pipe2[0]);
            close(pipe3[1]);
            exit(0);
        }

        // Parent process
        close(pipe1[0]); // Close unused read end of the pipe
        close(pipe2[1]); //
        close(pipe3[1]); // Close the write end of the pipe after writing

        write(pipe1[1], &num, sizeof(int));     // Send number to first child
        read(pipe3[0], &result, sizeof(int));   // Read result from second child

        printf("Result: %d\n\n", result);

        wait(NULL);
        wait(NULL);

        // Close pipes
        close(pipe1[1]);
        close(pipe2[1]);
        close(pipe3[0]);

    }

    return 0;
}