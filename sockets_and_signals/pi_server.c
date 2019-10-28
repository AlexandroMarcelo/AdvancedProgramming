/*
Alexandro Fracisco Marcelo Gonzalez A01021383
17/03/2019
Based on Gilberto Echeverria code

    Server program to compute the value of PI
    This program calls the library function 'get_pi'
    It will create child processes to attend requests
    It receives connections using sockets

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//Error library
#include <errno.h>
//Signal libraries
#include <signal.h>
#include <sys/poll.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
// Custom libraries
#include "get_pi.h"
#include "sockets.h"
#include "fatal_error.h"

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

///// FUNCTION DECLARATIONS
void usage(char * program);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void setupHandlers();
void onInterrupt(int signal);

//flags
int interrupted = 0;

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;

    printf("\n=== SERVER FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

	// Show the IPs assigned to this computer
	printLocalIPs();

    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);

	// Listen for connections from the clients
    waitForConnections(server_fd);

    // Close the socket
    close(server_fd);

    return 0;
}

//Explanation to the user of the parameters required to run the program
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}

//Main loop to wait for incomming connections
void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address; //the socker struct to get the address for each client
    socklen_t client_address_size; //to manage the client socket
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd; //the client file descriptor
    pid_t new_pid; //for the fork to handle each client

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
        // Wait for a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
        
        if (client_fd == -1)
        {
            fatalError("accept");
        }
         
        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // Create a new child process to deal with the client
        new_pid = fork();
        
        if (new_pid > 0) // Parent
        {
            // Close the new socket
            close(client_fd);
        }

        else if (new_pid == 0) // Child
        {
            // Close the main server socket to avoid interfering with the parent
            close(server_fd);

            printf("Child process %d dealing with client\n", getpid());

            //to handle the signlas
            setupHandlers();

            // Deal with the client
            attendRequest(client_fd);

            // Close the new socket
            close(client_fd);

            // Terminate the child process
            exit(EXIT_SUCCESS);
        }

        else //error when fork
        {
            fatalError("fork");
        }
    }
}

//To handle the signals
void setupHandlers()
{
   struct sigaction new_action;
   struct sigaction old_action;
   
   // Prepare the structure to handle a signal
   new_action.sa_handler = onInterrupt;
   new_action.sa_flags = 0;
   sigfillset(&new_action.sa_mask);
   
   // Catch the signal for Ctrl-C
   sigaction(SIGINT, &new_action, &old_action);
}

//If there is a signal to handle
void onInterrupt(int signal)
{
    printf("You pressed ctr-C\n");
    interrupted = 1;
}

//Hear the request from the client and send an answer
void attendRequest(int client_fd)
{
    char buffer[BUFFER_SIZE]; //The channel for comunicate client and server
    unsigned long int iterations; 

    // Receive the number of iterations from the client
    recvString(client_fd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%lu", &iterations);

    printf(" > Got request from client with iterations=%lu\n", iterations);

    //Variables to the pi result
    double pi_result = 4;
    int sign = -1;
    unsigned long int divisor = 3;
    unsigned long int counter = 0;

    int timeout = 0;//Poll timing
    int status; //status for the poll
    struct pollfd test_poll; //Creating the structure for the poll
    test_poll.fd = client_fd; // File descriptor server
    test_poll.events = POLLIN; //for the events coming on
    int client_interrupted_signal = 0;//the server is correct (1 if it was interrupted)

    while ((counter < iterations) && !interrupted) //while it is not interrupted
    {
        pi_result += sign * (4.0/divisor);
        sign *= -1;
        divisor += 2;
        counter++;

        status = poll(&test_poll, 1, timeout);
        if (status == -1) //if there was an error
        {
            break;
        }
        if (status == 0) //timeout
        {
            //printf(".");
            fflush(stdout);
        }
        else //if there is a signal sent from the client
        {
            if (test_poll.revents & POLLIN)
            {
                recvString(client_fd, buffer, BUFFER_SIZE); //recieve the signal from the server
                sscanf(buffer, "%d", &client_interrupted_signal);
                printf("\n\n > The client sent a signal.\n");
                break;
            }
        }
    }
    
    // Prepare the response to the client
    sprintf(buffer, "%d %lu %.20lf", client_interrupted_signal, counter, pi_result);

    if (interrupted) //if it was interrupted send the signal to the client
    {
        printf(" > The server was interrupted by a signal, Iam sending this to the clients\n");
        sprintf(buffer, "%d %lu %.20lf", interrupted, counter, pi_result);
        sendString(client_fd, buffer); //sending the response for interruption
    }

    printf(" > The number of iterations made: %lu\n", counter);
    printf(" > Sending PI=%.20lf\n", pi_result);

    // Send the response
    sendString(client_fd, buffer);
}
