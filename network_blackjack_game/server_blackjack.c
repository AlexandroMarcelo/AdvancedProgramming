/*
Alexandro Francisco Marcelo Gonzalez A01021383
Credits to Gilberto Echeverria

01/03/2019
*/

#include <sys/poll.h>

#include "blackjack_game_tools.h"
#include "sockets_tools.h"
#include "game_controller.h" //the media where the client and server comunicates

// Constant values
#define MAX_QUEUE 5
#define BUFFER_SIZE 200

// Function declarations
void usage(char * program); //if the client doesn not pass correctly the arguments
void waitForConnections(int server_fd); //wait until a client get`s in
void communicationLoop(); //the game on the server

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
    }
    
    int server_fd;

    server_fd = prepareServer(argv[1], MAX_QUEUE); //set up the server
    
    // Start waiting for incoming connections
    waitForConnections(server_fd); 
    
    return 0;
}

void usage(char * program)
{
    printf("Usage: %s {port_number}\n", program);
    exit(EXIT_SUCCESS);
}

void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof client_address;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid; //create several child process for manage each client
    
    while(1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
        if (client_fd == -1) //if there is a problem while comunicating
        {
            close(server_fd);
            close(client_fd);
            perror("ERROR: listen");
        }
        
        new_pid = fork(); //fork to child process
        
        if (new_pid == 0)   // Child process
        {
            // Close the main port
            close(server_fd);
            srand(time(NULL)); // Initialize the random seed for each child (client)
            inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, INET_ADDRSTRLEN); 
            printf("Connection from: %s, port %i\n", client_presentation, client_address.sin_port);
            
            // Start the communication loop for the Blackjack game
            communicationLoop(client_fd);
            
            // Terminate the child process for each client
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        else if (new_pid > 0)   // Parent process
        {
            close(client_fd);
        }
        else
        {
            perror("ERROR: fork");
            exit(EXIT_FAILURE);
        }
    }
    // Close the server port
    close(server_fd);
}

void communicationLoop(int connection_fd)
{
    char buffer[BUFFER_SIZE]; //for the handshake
    int chars_read = 0; //for know if there is comunication 
    Game_controller game; //creating the controller which going to manage the  comunication, petitions and replies between server and client
    struct pollfd polling; //creating the polling for manage the incoming data
    int status; //to get the status of the polling

    polling.fd = connection_fd;
    srand(time(NULL)); // Initialize the random seed
    polling.events = POLLIN; // Check for data comming in
    
    // Handshake
    chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
    send(connection_fd, "READY", strlen("READY")+1, 0);
    
    // Game loop
    while(1)
    {
        while(1)
        {
            status = poll(&polling, 1, 1000);
            if (status == -1)
            {
                // Something strange happened
                break;
            }
            if (status == 0) //wait for conection
            {
                printf(".");
                fflush(stdout);
            }
            else
            {
                if (polling.revents & POLLIN)
                {
                    // Get the option for the client
                    chars_read = recvMessage(connection_fd, &game, sizeof game);
                    break;
                }
            }
        }
        if (chars_read <= 0)
        {
            break;
        }

        if (game.player_status == 0)//the first time the player plays in the table, dealer gives two cards for him and one for dealer
        {
            game.player_cards[0] = getRandomCard(); //get the random card for the player
            game.dealer_cards[0] = getRandomCard(); //get the random card for the delaer
            game.player_cards[1] = getRandomCard();
            game.player_num_cards = 2; //setting the number of cards for the player
            game.dealer_num_cards = 1; //setting the number of cards for the player
            game.dealer_hand = checkHand(game.dealer_cards, game.dealer_num_cards); //setting the sumatory of the dealer
            game.player_hand = checkHand(game.player_cards, game.player_num_cards); //setting the sumatory of the player
            game.player_status = 10;//set player status to 'in game'
        }
        else
        {
            switch (game.player_option)
            {
                case 1:
                    //Finish the game and see who wins
                    printf("The player type stand\n");
                    game.player_hand = checkHand(game.player_cards, game.player_num_cards);//setting the sumatory of the player
                    game.dealer_num_cards = dealersTurn(game.dealer_cards, game.dealer_num_cards); //return the number of cards and the hand with 17 or more 
                    game.dealer_hand = checkHand(game.dealer_cards, game.dealer_num_cards); //setting the sumatory of the dealer
                    if (game.dealer_hand > 21) 
                    {
                        game.player_status = 1;//the player win if the dealers overpass 21
                        game.player_wallet += game.bet; //add the bet to the wallet of the player
                    }
                    else
                    {
                        if (game.dealer_hand > game.player_hand) //dealer wins and player loses
                        {
                            game.player_wallet -= game.bet; //substracting the bet from the wallet of the player
                            game.player_status = -1; //tell the player that he loses
                        }
                        else if (game.dealer_hand < game.player_hand) //dealer loses and player wins
                        {
                            game.player_wallet += game.bet; //add the bet to the wallet of the player
                            game.player_status = 1; //tell the player that he wins
                        }
                        else if (game.dealer_hand == game.player_hand) //Draw
                        {
                            game.player_status = 2; //tell the player that he draw
                        }
                    }
                    break;
                case 2:
                    //Take another card from the dealer
                    printf("The player type Hit\n");
                    game.player_cards[game.player_num_cards] = getRandomCard();
                    game.player_num_cards++;
                    game.player_hand = checkHand(game.player_cards, game.player_num_cards);
                    if (game.player_hand > 21) {
                        game.player_wallet -= game.bet; //substracting the bet from the wallet of the player
                        game.player_status = -1;//the player lose because he overpass 21, got busted
                    }
                    break;
                case 3:
                    printf("The player type Double down\n");
                    if (game.player_wallet >= (2*game.bet)) //if the player has the amount of doubling down, it continues
                    {
                        game.bet += (game.bet); //doubling his original bet
                        game.player_cards[game.player_num_cards] = getRandomCard(); //get the rendom and last card to the player`s card
                        game.player_num_cards++; //adding to the number of cards from the player
                        game.player_hand = checkHand(game.player_cards, game.player_num_cards); //get the sumatory of his hand
                        if (game.player_hand > 21) //if get busted
                        {  
                            game.player_wallet -= game.bet; //substract from the player`s wallet
                            game.player_status = -1; //telling the client he loses
                        }
                        else //if he does not over pass 21
                        {
                            game.player_status = 3; //setting the status of the player to Double down correctly
                        }
                    }
                    else//otherwise, the player can not double down
                    {
                        game.player_status = -3; //telling to the player he could not double down
                    }
                    break;
                case 4:
                    game.player_wallet -= game.bet; //substracting from the player`s wallet the bet if the surrended
                    game.player_status = 4;//for telling the client that he surrended
                    break;
                case 5:
                    printf("The player bet for an Insurance\n");
                    if (game.player_wallet >= (game.bet + (game.bet/2))) //if his wallet can manage with his insurance bet
                    {
                        game.dealer_cards[game.dealer_num_cards] = getRandomCard(); //getting a card to the dealer`s hand
                        game.dealer_num_cards++;
                        game.dealer_hand = checkHand(game.dealer_cards, game.dealer_num_cards);
                        if (game.dealer_hand != 21) 
                        {
                            game.player_wallet -= game.bet/2; //substracting the insurance bet to the player`s wallet
                            game.player_status = -5;//the player lose his Insurance bet because the dealer does not have 21, so the server tells him
                        }
                        else //else, the player wins his Insurance bet if...
                        {
                            if (game.player_hand == 21) //if the player has 21 also as the dealer, they draw
                            {
                                game.player_status = 2; //Draw
                            }
                            else if (game.player_hand != 21) //if the player does not have Blackjack, then he wins his insurance bet, and continues the game
                            {
                                game.player_wallet += game.bet/2; //wins his insurance bet
                                game.player_status = 5; //telling the player he wins the insurance bet
                            }
                        }
                    }
                    else//otherwise, the player can not get his insurance
                    {
                        game.player_status = -55; //telling the player he could not get his insurance
                    }                    
                    break;
            }
        }
        send(connection_fd, &game, sizeof game, 0); //sending the status to the player
    }
    // Close the socket to the client
    close(connection_fd);
}

int getRandomCard() //get a random card
{
    int range = 13; //the number of cards in blackjack
    int random_card; //the random card
    random_card = rand() % range + 1; //getting the random card between 1-13
    return random_card;
}

int dealersTurn(int * cards, int num_cards) //when the player stand, is the turn of the dealer to get 17 or more
{
    int sum_hand = checkHand(cards,num_cards);// getting the sumatory of his hand for the dealer
    while (sum_hand < 17)//check if his hand is 17 or less
    {
        cards[num_cards] = getRandomCard();//gets the random card
        num_cards++; //adding the number of cards
        sum_hand = checkHand(cards,num_cards);//cheching the sumatory to know if he continues getting cards of if he stops
    } 
    return num_cards;//return the number of dealer`s cards
}

int checkHand(int cards[], int num_cards) //return the sumatory of the hand
{
    int count = 0;
    int ace_value = 11; //for setting the value of the Ace
    int aux_count = 0;
    for(int i = 0; i < num_cards; i++)
    {
        if (cards[i] > 10) //If there is an J Q or K -> take it as 10
        {
            count += 10;
        }
        else if (cards[i] == 1)//if there is an Ace -> take it as 1 or 11 depend of the sumatory
        {
            ace_value = 11;
            aux_count = count;
            aux_count += ace_value;
            if (aux_count > 21) //take the Ace as 1
            {
                ace_value = 1;
                count += ace_value;
            }
            else if (aux_count <= 21)//take the Ace as 11
            {
                count = aux_count;
            }
        }
        else
        {
            count += cards[i];
        }
    }
    return count;//returns the amount of cards
}
