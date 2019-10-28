/*
Alexandro Francisco Marcelo Gonzalez A01021383

01/03/2019
*/

#ifndef BLACKJACK_GAME_TOOLS_H
#define BLACKJACK_GAME_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h> 

#include "game_controller.h"

char convertToCard(int value); //converts the cards to a printable form A 2 ... 10 J Q K
int checkHand(int cards[], int num_cards);//returns the sumatory of the hand
void printCards(int cards[], int num_cards); //print the cards to the player
int getRandomCard(); //get the random card
int dealersTurn(int * cards, int num_cards); //when the player stand, is the turn of the dealer to get 17 or more

#endif
