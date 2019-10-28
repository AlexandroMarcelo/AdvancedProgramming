/*
    Alexandro Francisco Marcelo Gonzalez A01021383
    30/03/2019
    
    Program for a simple bank server
    It uses sockets and threads
    The server will process simple transactions on a limited number of accounts

    Based on Gilberto Echeverria`s code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Signals library
#include <errno.h>
#include <signal.h>
// Sockets libraries
#include <netdb.h>
#include <sys/poll.h>
// Posix threads library
#include <pthread.h>

// Custom libraries
#include "bank_codes.h"
#include "sockets.h"
#include "fatal_error.h"

#define MAX_ACCOUNTS 5
#define BUFFER_SIZE 1024
#define MAX_QUEUE 5
#define MAX_THREADS 5

///// Structure definitions

// Data for a single bank account
typedef struct account_struct {
    int id;
    int pin;
    float balance;
} account_t;

// Data for the bank operations
typedef struct bank_struct {
    // Store the total number of operations performed
    int total_transactions;
    // An array of the accounts
    account_t * account_array;
} bank_t;

// Structure for the mutexes to keep the data consistent
typedef struct locks_struct {
    // Mutex for the number of transactions variable
    pthread_mutex_t transactions_mutex;
    // Mutex array for the operations on the accounts
    pthread_mutex_t * account_mutex;
} locks_t;

// Data that will be sent to each structure
typedef struct data_struct {
    // The file descriptor for the socket
    int connection_fd;
    // A pointer to a bank data structure
    bank_t * bank_data;
    // A pointer to a locks structure
    locks_t * data_locks;
} thread_data_t;


// Global variables for signal handlers
int interrupt_exit = 0;


///// FUNCTION DECLARATIONS
void usage(char * program);
void setupHandlers();
void initBank(bank_t * bank_data, locks_t * data_locks);
void readBankFile(bank_t * bank_data);
void waitForConnections(int server_fd, bank_t * bank_data, locks_t * data_locks);
void * attentionThread(void * arg);
void closeBank(bank_t * bank_data, locks_t * data_locks);
int checkValidAccount(int account);

void onInterrupt(int signal); //handling the signals
void storeChanges(bank_t * bank_data); //writte in the file the transactions made
float checkBalance(thread_data_t * connection_data, int source_account);
float depositAccount(thread_data_t * connection_data, int account_to, float amount);
float withdrawFromAccount(thread_data_t * connection_data, int account_from, float amount);
float tranferFunds(thread_data_t * connection_data, int account_from, int account_to, float amount);
void processRequest(thread_data_t * connection_data, int operation, int account_from, int account_to, float amount, response_t * reply, float * balance); //for process the petitions

//flags
int interrupted = 0;

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;
    bank_t bank_data;
    locks_t data_locks;

    printf("\n=== SIMPLE BANK SERVER ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

    // Configure the handler to catch SIGINT
    setupHandlers();

    // Initialize the data structures
    initBank(&bank_data, &data_locks);

	// Show the IPs assigned to this computer
	printLocalIPs();
    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
	// Listen for connections from the clients
    waitForConnections(server_fd, &bank_data, &data_locks);
    // Close the socket
    close(server_fd);

    // Clean the memory used
    closeBank(&bank_data, &data_locks);

    // Finish the main thread
    pthread_exit(NULL);

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
    Handling signals
*/
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
    printf("\n  > You pressed ctr-C.\n");
    interrupted = 1;
}

/*
    Function to initialize all the information necessary
    This will allocate memory for the accounts, and for the mutexes
*/
void initBank(bank_t * bank_data, locks_t * data_locks)
{
    // Set the number of transactions
    bank_data->total_transactions = 0;

    // Allocate the arrays in the structures
    bank_data->account_array = malloc(MAX_ACCOUNTS * sizeof (account_t));
    // Allocate the arrays for the mutexes
    data_locks->account_mutex = malloc(MAX_ACCOUNTS * sizeof (pthread_mutex_t));

    // Initialize the mutexes, using a different method for dynamically created ones
    //data_locks->transactions_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&data_locks->transactions_mutex, NULL);
    for (int i=0; i<MAX_ACCOUNTS; i++)
    {
        //data_locks->account_mutex[i] = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_init(&data_locks->account_mutex[i], NULL);
        // Initialize the account balances too
        bank_data->account_array[i].balance = 0.0;
    }

    // Read the data from the file
    readBankFile(bank_data);
}


/*
    Get the data from the file to initialize the accounts
*/
void readBankFile(bank_t * bank_data)
{
    FILE * file_ptr = NULL;
    char buffer[BUFFER_SIZE];
    int account = 0;
    char * filename = "accounts.txt";

    file_ptr = fopen(filename, "r");
    if (!file_ptr)
    {
        fatalError("ERROR: fopen");
    }

    // Ignore the first line with the headers
    fgets(buffer, BUFFER_SIZE, file_ptr);
    // Read the rest of the account data
    while( fgets(buffer, BUFFER_SIZE, file_ptr) )
    {
        sscanf(buffer, "%d %d %f", &bank_data->account_array[account].id, &bank_data->account_array[account].pin, &bank_data->account_array[account].balance); 
        account++;
    }
    
    fclose(file_ptr);
}


/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd, bank_t * bank_data, locks_t * data_locks)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pthread_t new_tid;
    thread_data_t * connection_data = NULL;
    int number_client = -1; //for help to know if there was connections with clients
    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    int timeout = 500;      // Time in milliseconds (0.5 seconds)
    int status_poll;  //for know if there is a conection

    // Create a structure array to hold the file descriptors to poll
    struct pollfd test_fds[1];

    // Receive the data for the bank, mutexes and socket file descriptor
    // Fill in the structure
    test_fds[0].fd = server_fd;
    test_fds[0].events = POLLIN;    // Check for incomming data

    while(1)
    {
        while (!interrupted)
        {
            status_poll = poll(&test_fds[0], 1, timeout);
            if (status_poll == -1)
            {
                // Something strange happened or ctrl-c
                break;
            }
            if (status_poll == 0) //wait for conection
            {
                printf(".");
                fflush(stdout);
            }
            else
            {
                if (test_fds[0].revents & POLLIN)
                {
                    // ACCEPT
                    // Wait for a client connection
                    client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
                    
                    break;
                }
            }
        }
        if (interrupted == 1)
        {
            printf("Server interrupted. Closing\n");
            break;
        }
        if (client_fd == -1)
        {
            close(client_fd);
            printf("Something strage happened while connecting to the client\n");
            break; 
        }

        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // Prepare the structure to send to the thread
        connection_data = (thread_data_t *)malloc(MAX_ACCOUNTS * sizeof (thread_data_t));
        connection_data->connection_fd = client_fd;
        connection_data->bank_data = bank_data;
        connection_data->data_locks = data_locks;

        number_client++;    
        // CREATE A THREAD
        int created_thread = pthread_create(&new_tid, NULL, attentionThread, connection_data);
        if (created_thread != 0) //error while creating the thread  
        {   
            fatalError("ERROR: accept");
        }
    
    }

    if (number_client != -1) //if there is at least one client connected
    {
        //closing the file descriptors
        close(client_fd);
        close(connection_data->connection_fd);
        
        //for waiting the threads to finish
        pthread_join(new_tid, NULL);
        
        // Show the number of total transactions
        printf("Number of total transactions: %d\n", connection_data->bank_data->total_transactions);

        // Store any changes in the file
        storeChanges(connection_data->bank_data);
        free(connection_data);  
    }
}

/*
    Hear the request from the client and send an answer
*/
void * attentionThread(void * arg)
{
    thread_data_t * connection_data = (thread_data_t *)arg;
    unsigned long int id_thread = (unsigned long int)pthread_self();
    printf("Thread %ld dealing with client: %d.\n", id_thread, connection_data->connection_fd);
    char buffer[BUFFER_SIZE];
    int chars_read = 0; //for know if there is comunication 
    //variables for the comunication channel
    float balance;
    operation_t operation;
    int account_from;
    int account_to;
    float amount;
    response_t reply = 0; //OK, INSUFFICIENT, NO_ACCOUNT, BYE, ERROR

	int timeout = 500;		// Time in milliseconds (0.5 seconds)
    int status_poll;

    // Create a structure array to hold the file descriptors to poll
    struct pollfd test_fds[1];

    // Receive the data for the bank, mutexes and socket file descriptor
    // Fill in the structure
    test_fds[0].fd = connection_data->connection_fd;
    test_fds[0].events = POLLIN;    // Check for incomming data

    // Loop to listen for messages from the client
    while(reply != BYE)   
    {
        while (!interrupted)
        {
            status_poll = poll(&test_fds[0], 1, timeout);
            if (status_poll == -1)
            {
                // Something strange happened or interrupted
                break;
            }
            if (status_poll == 0) //wait for conection
            {
                printf("_");
                fflush(stdout);
            }
            else
            {
                if (test_fds[0].revents & POLLIN)
                {
                    chars_read = recvString(connection_data->connection_fd, buffer, BUFFER_SIZE);
                    break;                    
                }
            }
        }
        
        if (interrupted == 1 )
        {
            printf("Thread %ld interrupted.\n", id_thread);
            break;
        }
        if (chars_read <= 0 )
        {
            printf("Thread: %ld. The connection with the client: %d was lost\n", id_thread, connection_data->connection_fd);
            break;
        }
        // Extract the data
        sscanf(buffer, "%u %d %d %f", &operation, &account_from, &account_to, &amount);
        //printf("Buffer: %s\n", buffer);
        // Process the request being careful of data consistency
        processRequest(connection_data,operation, account_from, account_to, amount, &reply, &balance);
        // Update the number of transactions. excepting check y exit
        if ((int)reply != (int)EXIT && (int)reply != (int)CHECK) 
        {
            pthread_mutex_lock(&connection_data->data_locks->transactions_mutex);
            connection_data->bank_data->total_transactions++;
            pthread_mutex_unlock(&connection_data->data_locks->transactions_mutex);
        }
        // Send a reply: response_t (bank_codes.h) and the new balance of the accont (0 if the response does not use a balance)
        // Prepare the message to the client
        sprintf(buffer, "%d %f", reply, balance);
        //printf("SENDING TO CLIENT: %s\n", buffer);
        // Send the reply
        sendString(connection_data->connection_fd, buffer, strlen(buffer)+1);  
    }
    pthread_exit(NULL);
}

//Process the client request
void processRequest(thread_data_t * connection_data, int operation, int account_from, int account_to, float amount, response_t * reply_, float * balance_)
{
    response_t reply;
    float balance;
    switch(operation)
    {
        // Check balance
        case CHECK:
            printf("CHECK from client\n");
            balance = checkBalance(connection_data, account_from);
            if (balance == -1)
            {
                reply = NO_ACCOUNT;
                balance = 0;
            }
            else
            {
                reply = OK;
            }
            break;
        // Deposit into account
        case DEPOSIT:
            //enter to critical section
            pthread_mutex_lock(&connection_data->data_locks->account_mutex[account_to]);
            printf("DEPOSIT from client\n");
            balance = depositAccount(connection_data, account_to, amount);

            if (balance == -1)
            {
                reply = NO_ACCOUNT;
                balance = 0;
            }
            else
            {
                reply = DEPOSIT_OK;
            }
            pthread_mutex_unlock(&connection_data->data_locks->account_mutex[account_to]);
            //Exit the critical section
            break;
        // Withdraw from account
        case WITHDRAW:
            printf("WITHDRAW from client\n");
            pthread_mutex_lock(&connection_data->data_locks->account_mutex[account_from]);
            balance = withdrawFromAccount(connection_data, account_from, amount);
            if (balance == -1)
            {
                reply = NO_ACCOUNT;
                balance = 0;
            }
            else if (balance == -11)
            {
                reply = INSUFFICIENT;
                balance = 0;
            }
            else
            {
                reply = OK;
            }
            pthread_mutex_unlock(&connection_data->data_locks->account_mutex[account_from]);
            break;
        // Transfer between accounts
        case TRANSFER:
            pthread_mutex_lock(&connection_data->data_locks->account_mutex[account_from]);
            pthread_mutex_lock(&connection_data->data_locks->account_mutex[account_to]);
            printf("TRANSFER from client\n");
            balance = tranferFunds(connection_data, account_from, account_to, amount);
            if (balance == -1)
            {
                reply = NO_ACCOUNT;
                balance = 0;
            }
            else if (balance == -11)
            {
                reply = INSUFFICIENT;
                balance = 0;
            }
            else
            {
                reply = TRANSFER_OK;
            }
            pthread_mutex_unlock(&connection_data->data_locks->account_mutex[account_to]);
            pthread_mutex_unlock(&connection_data->data_locks->account_mutex[account_from]);
            break;
        // Exit the bank
        case EXIT:
            printf("EXIT from client\n");
            reply = BYE;
            balance = 0;
            break;
    } //switch case
    *reply_ = reply;
    *balance_ = balance;
}

//Check the balance from an acoount (return -1 if there is not a source account, otherwise the balance of the account)
float checkBalance(thread_data_t * connection_data, int source_account)
{
    float balance;
    if (!checkValidAccount(source_account)) 
    {
        balance = -1;
    }
    else
    {
        balance = connection_data->bank_data->account_array[source_account].balance;
    }
    return balance;
}

//Deposit to an account (return -1 it there is an invalid account destiny, otherwise returns the modified balance )
float depositAccount(thread_data_t * connection_data, int account_to, float amount)
{
    float balance;
    if (!checkValidAccount(account_to)) 
    {
        balance = -1;
    }
    else
    {
        connection_data->bank_data->account_array[account_to].balance += amount; //adding the amount
        balance = connection_data->bank_data->account_array[account_to].balance;
    }
    return balance;
}

//Withdraw from an account (return -1 if there is not a source account, otherwise returns the balance of the account)
float withdrawFromAccount(thread_data_t * connection_data, int account_from, float amount)
{
    float balance;
    float current_balance;
    if (!checkValidAccount(account_from) ) 
    {
        balance = -1;
    }
    else
    {
        current_balance = checkBalance(connection_data, account_from); //For checking the balance is according with his petition
        if (current_balance >= amount) 
        {
            connection_data->bank_data->account_array[account_from].balance -= amount;
            balance = connection_data->bank_data->account_array[account_from].balance;
        }
        else
        {
            balance = -11; //no required funds in account
        }   
    }
    return balance;
}

//Transfer founds from an account to a destiny account (return -1 if there is an invalid account, -11 if the source account doesn not have the necessary funds, otherwise returns )
float tranferFunds(thread_data_t * connection_data, int account_from, int account_to, float amount)
{
    float balance;
    float account_from_balance;
    if (!checkValidAccount(account_from) || !checkValidAccount(account_to)) 
    {
        balance = -1;
    }
    else
    {
        account_from_balance = checkBalance(connection_data, account_from);
        if (account_from_balance >= amount) 
        {
            connection_data->bank_data->account_array[account_from].balance -= amount;
            connection_data->bank_data->account_array[account_to].balance += amount;
            balance = connection_data->bank_data->account_array[account_to].balance;
        }
        else
        {
            balance = -11; //no required funds in source account
        }   
    }
    return balance;
}

//writing the changes in the accounts
void storeChanges(bank_t * bank_data)
{
    FILE * file = NULL;
    char * filename = "accounts.txt";

    file = fopen(filename, "w");
    if (!file)
    {
        fatalError("ERROR: fopen");
    }

    fprintf(file, "Account_number PIN Balance\n");

    // Read the rest of the account data
    for(int i = 0; i < MAX_ACCOUNTS; i++)
    {
        fprintf(file, "%d %d %.2f\n", bank_data->account_array[i].id, bank_data->account_array[i].pin, bank_data->account_array[i].balance);
    }

    fclose(file);
}
/*
    Free all the memory used for the bank data
*/
void closeBank(bank_t * bank_data, locks_t * data_locks)
{
    printf("DEBUG: Clearing the memory for the thread\n");
    free(bank_data->account_array);
    free(data_locks->account_mutex);
}


/*
    Return true if the account provided is within the valid range,
    return false otherwise
*/
int checkValidAccount(int account)
{
    return (account >= 0 && account < MAX_ACCOUNTS);
}
