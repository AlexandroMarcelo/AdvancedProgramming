/*
    Server program

    by 
    Luis Fernando Carrasco A01021172
    Alexandro Francisco Marcelo A01021383
    Daniel Pelagio Vazquez A01227873
    10/05/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>

//Signal and polling libraries
#include <signal.h>
#include <errno.h>
#include <sys/poll.h>

// Custom libraries
#include "sockets.h"
#include "menu.hpp"

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5
#define INT_SERVER 2

///// FUNCTION DECLARATIONS
void usage(char * program);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void onInterrupt(int signal);
void setupHandlers();

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
    
    setupHandlers();
	// Listen for connections from the clients
    waitForConnections(server_fd);
    // Close the socket
    close(server_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}


/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid;

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
        // ACCEPT
        // Wait for a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
        if (client_fd == -1)
        {
            fatalError("accept");
        }
         
        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // FORK
        // Create a new child process to deal with the client
        new_pid = fork();
        // Parent
        if (new_pid > 0)
        {
            // Close the new socket
            close(client_fd);
        }
        else if (new_pid == 0)
        {
            // Close the main server socket to avoid interfering with the parent
            close(server_fd);
            printf("Child process %d dealing with client\n", getpid());
            // Deal with the client
            attendRequest(client_fd);
            // Close the new socket
            close(client_fd);
            // Terminate the child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            fatalError("fork");
        }

    }
}

/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd)
{
    char buffer[BUFFER_SIZE];

    int timeout = 0; // check the file descriptor as often as possible, to not slow down the calculations as much
    int status;
    struct pollfd test_poll;
    test_poll.fd = client_fd;
    test_poll.events = POLLIN;
    int sig = 0;
    
    recvString(client_fd, buffer, BUFFER_SIZE);
    //sscanf(buffer, "%lu", &iterations);
    
    //printf(" > Got request from client with iterations=%lu\n", iterations);

    // Compute the value of PI
    while(!interrupted)
    {
        status = poll(&test_poll, 1, timeout);
        /*if (status == 0)
        {
            printf(".");
            fflush(stdout);
        }*/
        if (status > 0)
        {
            if (test_poll.revents & POLLIN)
            {
                // RECV
                // Receive the request
                recvString(client_fd, buffer, BUFFER_SIZE);
                sscanf(buffer, "%d", &sig);
                printf("Got request from the client to stop, sending the current value of Pi\n");
                break;
            }
        }
        else if (status < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("ERROR: poll");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    if (interrupted)
        sig = INT_SERVER;

    // Prepare the response to the client
    sprintf(buffer, "%d", sig);
    // SEND
    // Send the response
    sendString(client_fd, buffer);
}


void onInterrupt(int signal)
{
    printf("I was interrupted\n");
    interrupted = 1;
}

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
