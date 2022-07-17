// @author: Demet YAYLA 
// school id: 2019400105

/*
Main idea of this part of the project is to create a child process that executes the binary program given with the arguments
and provide the communication with the main code and child process through pipes. We examine interprocess communication
(more specifically parent-child process comunication). The data flow from and to the outside is within parent. Also fail-success
states are parsed in parent process. The child process arranges the pipes for communication and executes the binary file given
with the arguments. Its output is connected to a pipe directly so the output will be received by the parent.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <math.h>

// below are flags setting the output file to be write-only, if not existing create and appending mode
#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_APPEND)
// below are modes 
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


//PROGRAM ASSUMES THAT ERROR MESSAGE FROM BLACKBOX ISN'T STARTING WITH NUMBERS
//argv[1] is the blackbox path, argv[2] is the .txt file's path for the output
int main(int argc, char *argv[]){
    pid_t pid;
    // c2p is the pipe to which child process writes its error messages and non-erroneous outputs (childs process's stdout and 
    // stderr is connected to pipe's write end) and from which parent process reads
    
    //p2c is the pipe to which parent process writes the data it gets by using scanf from the console and from which 
    //child process reads the message as its stdin input.

    //fd_out is the file descriptor for the output file. 
    int c2p[2], p2c[2], fd_out;
    int PARENT_READ = 0; //read end of c2p
    int CHILD_WRITE = 1; //write end of c2p
    int CHILD_READ = 0; //read end of p2c
    int PARENT_WRITE = 1; //write end of p2c
    char buffer[1000000]; //buffer to which parent reads the output of blackbox

    //p2c ==> 0 -> parent_write, 1->parent read
    //c2p ==> 0-> child write, 1->child read

    //below I open the pipes
    if( (pipe(c2p) == -1) || (pipe(p2c) == -1)) {
        perror("FAIL:\nFailed to setup pipeline\n");
        return 1;
    }

    //below I fork
    if((pid = fork()) == -1){
        perror("FAIL:\nFailed to fork.\n");
        return 1;
    }
    
    //below is code for child process. It first connects its stdin & stderr to c2p write end and its stdin to p2c read end.
    //Then closes the unnecessary pipe ends. Then it calls the blackbox code for execution.
    if(pid == 0){
        if((dup2(c2p[CHILD_WRITE], STDOUT_FILENO) == -1) || (dup2(c2p[CHILD_WRITE],STDERR_FILENO) == -1)|| (dup2(p2c[CHILD_READ], STDIN_FILENO) == -1)){
            perror("FAIL:\nFailed to redirect console input.\n");
            exit(-1);
        }else if((close(c2p[PARENT_READ]) == -1) || (close(c2p[CHILD_WRITE]) == -1) || (close(p2c[PARENT_WRITE]) == -1)){
            perror("FAIL:\nFailed to close unused pipe descriptors.\n");
            exit(-1); //NOT SURE!!
        }
        execl(argv[1], argv[1], NULL);
        perror("FAIL:\nFailed to execute blackbox.\n");
        return 1; //NOT SURE!!
    }
    
    // below is the code for parent process after forking. First I open a file descriptor for part_a_output.txt. Then
    // Then I put this file descriptor in place of stdout and stderr. Then I close the unnecessary pipes. I scan two integers as
    // input from the console. I merge these inputs as a single string and write it to p2c pipe. I wait for a response from c2p pipe.
    // When I child terminates, I read the result of blackbox from c2p pipe to buffer. len is the byte length of the message.
    // I append the terminator character for strings to the message buffer. I utilize atoi function to identify whether the 
    // message is success or failure. If demet is other than zero, the answer is success and return value for blackbox is 
    // demet. If demet is zero, it is fail if the return value isn't int zero by itself (I check the string for it). I added an
    // additional check with flag to see if the error message starts with integer value because atoi returns the initial integer part of a string
    // it has any. 
    if((fd_out = open(argv[2], CREATE_FLAGS, CREATE_MODE)) == -1){
        perror("FAIL:\nFailed to open file.\n");
        return 1;
    }else if((dup2(fd_out, STDOUT_FILENO) == -1) || (dup2(fd_out, STDERR_FILENO) == -1)){
        perror("FAIL:\nFailed to redirect program output.\n");
        return 1;
    }else if((close(c2p[CHILD_WRITE]) == -1) || (close(p2c[CHILD_READ]) == -1) ){
        perror("FAIL:\nFailed to close unused pipe descriptors.\n");
        exit(-1); //NOT SURE!!
    }


    int var1;
    int var2;
    scanf("%d", &var1);
    scanf("%d", &var2);
    
    char varA[1000];
    sprintf(varA, "%d %d", var1, var2);
   
    if((write(p2c[PARENT_WRITE], varA, strlen(varA)+1)) == -1){
        perror("FAIL:\nFailed to redirect stdin to blackbox.");
    }

    wait(NULL);


    int len = read(c2p[PARENT_READ], buffer, 1000000);
    buffer[len] = '\0';
    int demet = atoi(buffer);

    //below till line 126 I check if the error message initiates with an integer.
    if(((demet == 0) && (!((buffer[0] == '0') && (len == 2))))){
        printf("FAIL:\n%s", buffer);
        return 1;
    }else{
        printf("SUCCESS:\n%s", buffer);
        return 0;
    }

}
