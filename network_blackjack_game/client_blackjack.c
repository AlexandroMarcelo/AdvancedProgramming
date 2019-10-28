/*
Alexandro Francisco Marcelo Gonzalez A01021383
Credits to Gilberto Echeverria

01/03/2019
*/

#include "blackjack_game_tools.h"
#include "sockets_tools.h"
#include "game_controller.h" //the media where the client and server comunicates

// Constant values
#define BUFFER_SIZE 200

// Function declarations
void usage(char * program); //if the client doesn not pass correctly the arguments
void communicationLoop(); //the game for the client

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
    }
    
    int connection_fd;
    
    connection_fd = connectToServer(argv[1], argv[2]); //stablish the comunication
    
    communicationLoop(connection_fd); //start the comunication for the game
    
    return 0;
}

void usage(char * program)
{
    printf("Usage: %s {server_address} {port_number}\n", program);
    exit(EXIT_SUCCESS);
}

void communicationLoop(int connection_fd)
{
    char buffer[BUFFER_SIZE]; //for the handshake
    int chars_read = 0; //for know if there is comunication 
    Game_controller game; //creating the controller which going to manage the  comunication, petitions and replies between server and client
    float original_bet; //for knowing the firs bet to enter to the table
    int aux = 0; //just an auxiliary variable
    int insurance = 0; //to know if the player wants to have insurance

    // Handshake
    send(connection_fd, "START", strlen("START")+1, 0); //say the client is ready
    chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE); //reply that the server also is ready
        
    printf("\n*******Welcome to the Blackjack game.*******\n");
    //printf("Take 50 chips and let the fun begin!\n\n");
    //game.player_wallet = 50;
    printf("How much chips/money do you have to bet?\n");
    scanf("%f", &game.player_wallet);//keeping the players wallet
    while(1)//loop for the game
    {
        if (game.player_wallet < 5) { 
            printf("Sorry you can not enter to any table, you do not have enough chips to bet.\n");
            break;
        }
        game.player_status = 0;//telling the server the player is ready for the first game each time he enter to the table
        aux = 0;
        printf("\nYour current wallet: %.2f chips.\n\n", game.player_wallet);
        printf("To enter this table you must bet between 5-500 chips\nHow much would you like to bet? (Type -1 to Exit the game)\nType here: ");
        scanf("%f", &original_bet);
        
        if (original_bet > game.player_wallet) //if the bet is higher the his wallet
        {
            printf("\nYou have not the enough money to bet. Try to reduce your bet.\n\n");
        }
        else //if the bet is correct according to his wallet
        {
            game.bet = original_bet;//saves the bet to tell it to the server
            printf("\n");
        
            if(original_bet == -1)// if the player wants to exit
            {
                break;
            }
            else if (original_bet < 5 || original_bet > 500) //the table only accepts between 5 - 500
            {
                printf("You shuld bet only chips between 5 - 500\n\n\n");
            }

            while(original_bet >= 5 && original_bet <= 500) //while the bet is between 5 - 500 the player can enter to the table
            {
                send(connection_fd, &game, sizeof game, 0);//Send the initial bet to the server
                
                chars_read = recvMessage(connection_fd, &game, sizeof game);//the reply from the server containing the first two cards and the first card for the dealer
                if (chars_read <= 0)//if an error ocurrs
                {
                    break;
                }

                printf("\nHere are your cards\n");
                printCards(game.player_cards, game.player_num_cards);//displaying the players cards

                printf("\nHere are the dealer`s cards\n");
                printCards(game.dealer_cards, game.dealer_num_cards);

                if (game.player_status == 1) //if the player wins
                {
                    if (game.dealer_hand > 21) 
                    {
                        printf("Congratulations you have won %.2f chips!, because the dealer`s hand (%d) overpass 21 (BUST).\n\n", game.bet, game.dealer_hand);
                    }
                    else
                    {
                        printf("Congratulations, you have won %.2f chips!, because your hand (%d) is higher that dealer`s hand (%d).\n\n", game.bet, game.player_hand, game.dealer_hand);
                    }
                    //game.player_wallet += game.bet;
                    break;
                }
                else if (game.player_status == -1) //if the player loses
                {
                    if (game.player_hand > 21)
                    {
                        printf("I`m sorry, you have lost %.2f chips, because your hand (%d) overpass 21 (BUST).\n\n", game.bet, game.player_hand);
                    }
                    else
                    {
                        printf("I`m sorry, you have lost %.2f chips, because your hand (%d) is lower that dealer`s hand (%d).\n\n", game.bet, game.player_hand, game.dealer_hand);
                    }
                    break;
                }
                else if (game.player_status == 2) //Draw
                {
                    printf("There was a draw. Nobody wins. You did not lose your bet.\n\n");
                    break;
                }
                else if (game.player_status == 5)//the player wins his Insurance bet
                {
                    printf("Very nice! you hit that dealer`s hand has Blackjack. You have won %.2f chips.\n", game.bet/2);
                    game.player_option = 0;
                }
                else if (game.player_status == -5) //lose his Insurance bet 
                {
                    printf("Sorry, you did not hit that dealer`s hand has Blackjack. You have lost %.2f chips.\n\n", game.bet/2);
                    game.player_option = 0;
                }
                else if (game.player_status == 4) //if the player surrended
                {
                    printf("You have surrendered, you have lost %.2f chips.\n", game.bet);
                    printf("Come back soon.\n\n");
                    break;
                }
                else if (game.player_status == -55) //if the player`s wallet can not manage with his insurance bet, the player can not get insured
                {
                    printf("You can not be insured, you do not have enough chips for do that.\n");
                    game.player_option = 0;
                }
                else if (game.player_status == -3) //The player can not double down
                {
                    printf("You can not double down, you do not have enough chips for do that.\n");
                    game.player_option = 0;
                }

                if ((game.dealer_hand == 10 || game.dealer_hand == 11) && game.dealer_num_cards == 1 && aux == 0) //if the dealer hand contains 10 or an Ace, the player can take his insurance
                {
                    printf("It looks that the dealer`s hand could has Blackjack. Would you like to Insurance yourself? (bet the half of the original bet if the dealer has Blackjack)\n1. Yes, I want my Insurance,\n2. No, I`ll continue\nOption: ");
                    scanf("%d", &insurance);
                    if (insurance == 1) //Yes
                    {
                        printf("All right! You are insured!\n");
                        game.player_option = 5;//Tell to the server the player wants to have insurance
                    }
                    else if(insurance == 2) //NO
                    {
                        printf("It`s ok. Good Luck!\n");
                        game.player_option = 0;
                    }
                    else
                    {
                        printf("You type an incorrect option. I guess you do not want the insurance. Good Luck!\n");
                        game.player_option = 0;
                    }
                    aux++;
                }
                else if (game.player_status == 3) //if the player Double Down
                {                    
                    game.player_option = 1;//To comunicate to the server that player have finished because the dealer gives him his last card due Double Down, therefore, checks in the server if overpass 21 the player`s hand or wins 
                }
                else //The player`s turn
                {   
                    printf("\nYour current wallet: %.2f chips.\n\n", game.player_wallet);
                    printf("What would you like to do? Type the option\n");
                    printf("1. Stand (stay with your current hand)\n2. Hit (take another card)\n3. Double Down (put a second wager equal to your first bet)\n4. Surrender (give up)\nOption: ");
                    scanf("%d", &game.player_option);

                    switch (game.player_option)
                    {
                        case 1:
                            //Finish the game and see who wins, the dealer get his cards (informs the server)
                            printf("You type Stand\n");
                            game.player_option = 1;
                            break;
                        case 2:
                            //Take another card from the dealer
                            printf("You type Hit\n");
                            game.player_option = 2;
                            break;
                        case 3:
                            //Double down the bet
                            if (game.player_wallet >= (2*original_bet)) 
                            {
                                printf("You have Double Down. Your current wager is: %.2f\n", game.bet*2);
                            }
                            game.player_option = 3;
                            break;
                        case 4:
                            //Surrender, the player send to the server he surrends
                            game.player_option = 4;
                            break;
                        default:
                            game.player_option = 0;
                            printf("**Type a correct option.**\n");
                    }
                }
            }//end while loop for an specific table game
        }//end of the else for check if the bet is correct according to the table policies
    }//end while loop for the entire game
    printf("***Thak you for playing Blackjack here! Have a nice day!***\n\n");
    close(connection_fd);// Close the socket to the client
}

void printCards(int cards[], int num_cards) //just for printing the cards to the player
{
    for(int i = 0; i < num_cards; i++)
    {
        if (cards[i] == 10) //if there is a 10 card, just print it as integer
        {
            printf("%d ", cards[i]);
        }
        else //if there is another card, to treat them as chars because of J Q K or A
        {
            printf("%c ", convertToCard(cards[i]));
        }
    }
    printf("\n\n");
}

char convertToCard(int value) //convert the card from integer value to a blackjack card
{
    char card;
    switch (value)
    {
        case 1:
            return 'A';
            break;
        case 11:
            return 'J';
            break;
        case 12:
            return 'Q';
            break;
        case 13:
            return 'K';
            break;
        default:
            card = value + '0'; //transforming to char whatever card is (2-9)
            return card;
            break;
    }
}
