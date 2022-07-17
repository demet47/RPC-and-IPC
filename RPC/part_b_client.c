/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */


// @author = Demet YAYLA
// school id = 2019400105

/*
In this part of the process, we are using RPC instead of IPC like in the previous part. The message RPC sends is more stuctured. RPC sends its message not 
to its child process; instead it sends the message to a daemon (in server) listening to a port on a remote system (client). The message is received by the 
remote system by the server and processed with the requested id'd function of the server. The requested function is executed with the data sent. The output 
is then sent to client with a seperate message. Here, this client code reads input from console and sends it to remote system for which part_b_server.out 
executes a blackbox function and returns a string containing either an error message or an integer ouptut.
Server uses IPC for communication with child process as in part_a
*/

#include "part_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>


// below function takes input values as int i1 and i2 from main function and sends it to server for execution of blackbox. path variable is the path of binary
//file to be executed by the server. file variable is the path for the .txt file to which output will be written. 

void
partb_1(char *host,int i1, int i2,  char path[], char* file)
{
	CLIENT *clnt;
	char **result_1;
	//below struct is for setting the data we want the server function to access.
	inputs  execute_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, PARTB, PARTB_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
//from 41 to 47 we check if we can create a client to connect to the host specified by the host parameter to this funciton.
#endif	/* DEBUG */
	//below from 50 to 60 we set the data we want the host to see. 
	execute_1_arg.a= i1;
	execute_1_arg.b= i2;
	
	int i=0;
	while(i<256 &&  path[i]!='\0'){
		execute_1_arg.path[i]=path[i];
		i++;
	}
	execute_1_arg.path[i] = '\0';
	//below line, we execute the functin "execute" on the remote server.	
	result_1 = execute_1(&execute_1_arg, clnt);
	
	// if result is null some error happened midway; else I open a file with append property and do parsing to see if the output is fail or success. Then I
	// print the output with either "FAIL:" or "SUCCESS:". 
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}else{
		FILE * f;
		f = fopen(file, "a+");
		int len = atoi(*result_1);
		printf("%s\n", *result_1);
		printf("%d\n", len);
		if(len != 0 || (*result_1[0] != '0' && strlen(*result_1) == 1)){
			char initial[10];
			strcpy(initial, "SUCCESS:\0");
			fprintf(f ,"%s\n" , initial);
			fprintf(f, "%s" , *result_1);		
		}
		else{
			char initial[256];
			strcpy(initial, "FAIL:\0");
			fprintf(f,"%s\n" , initial);
			fprintf(f,"%s" , *result_1);
		}

		fclose(f);
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	//below 4 lines I get input from client terminal
	int i1, i2;
	char *host;
	scanf("%d", &i1);
	scanf("%d", &i2);

	//below colum I check if there exists enough arguments
	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	//below line I retrieve the host from argument indexed 3.
	host = argv[3];
	partb_1 (host ,i1, i2, argv[1], argv[2]);
exit (0);
}


