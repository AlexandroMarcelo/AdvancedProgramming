/*
Alexandro Fracisco Marcelo Gonzalez A01021383
17/03/2019
Based on Gilberto Echeverria code

    Client program to get the value of PI
    This program connects to the server using sockets
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
#include "sockets.h"
#include "fatal_error.h"


#define BUFFER_SIZE 1024

///// FUNCTION DECLARATIONS
void usage(char * program);
void requestPI(int connection_fd);
void setupHandlers();
void onInterrupt(int signal);

//FLAGS
int interrupted = 0;

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd; //for the file descriptor to manage the connections

    printf("\n=== CLIENT FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }
    
    //Setting up the signal handler
    setupHandlers();

    // Start the server
    connection_fd = connectSocket(argv[1], argv[2]);
    
	// Listen for connections from the clients
    requestPI(connection_fd);

    // Close the socket
    close(connection_fd);

    return 0;
}

//For the parameters the user needs to include
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
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
    printf("\n\n > You pressed ctr-C\n\n");
    interrupted = 1;
}

//The request function for the client
void requestPI(int connection_fd)
{
    int status;
    struct pollfd test_poll; //Creating the structure for the poll
    test_poll.fd = connection_fd; // File descriptor client
    test_poll.events = POLLIN; //for the events coming on
    int timeout = 0; //The timing for the poll

    char buffer[BUFFER_SIZE]; //The channel for comunicate client and server
    unsigned long int iterations; //The amount of iterations for the pi computation
    double pi_result; 
    unsigned long int counter_result;
    int server_interrupted_signal = 0; //If there server sends a message to the client that he was interrupted (1 if it was interrupted, 0 if not)

    printf("Enter the number of iterations for PI: ");
    scanf("%lu", &iterations);

    // Prepare the response to the server
    sprintf(buffer, "%lu", iterations);

    // Send the response
    sendString(connection_fd, buffer); //sending the number of itarations
    
    while (!interrupted) //while it is not interrupted
    {
        status = poll(&test_poll, 1, timeout);
        if (status == -1) //if there was an error
        {
            break;
        }
        if (status == 0) //timeout for the poll
        {
            //printf(".");
            fflush(stdout);
        }
        else //if there is a signal sent by the server
        {
            if (test_poll.revents & POLLIN)
            {
                recvString(connection_fd, buffer, BUFFER_SIZE); //recieve the signal from the server
                sscanf(buffer, "%d", &server_interrupted_signal);
                break; //break because a signal
            }
        }
    }
        
    if (interrupted) //if the client was interrupted send a signal to the server
    {

        printf(" > Interrupted by a signal, Sending this to the server\n");
        sprintf(buffer, "%d", interrupted);
        sendString(connection_fd, buffer); //send the signal that the client was interrupted
        recvString(connection_fd, buffer, BUFFER_SIZE); //receive the result sent by the server
    }

    // Receive the request
    if (server_interrupted_signal) {
        printf("\n > The server was interrupted, Here are the results.\n");
    }

    sscanf(buffer, "%d %lu %lf", &server_interrupted_signal, &counter_result, &pi_result);
        
    // Print the result
    printf("\nNumber of iterations made: %lu\n", counter_result);
    printf("The value for PI is: %.20lf\n", pi_result);
}
