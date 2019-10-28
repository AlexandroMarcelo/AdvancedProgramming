/*
Alexandro Francisco Marcelo Gonzalez A01021383

01/03/2019
*/

#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

typedef struct player_struct {
    int player_cards[30];//store the player`s cards
    int player_num_cards;//store the amount of cards in player`s hand
    int player_hand;//the sumatory of the player`s hand
    int player_option;//get the input for each case of blackjack
    int player_status;//check status player: -1. Loses, 0. new, 1. Wins, 2. Draw, 10. in game
    float player_wallet;//the amount of tokens left to the player
    float bet;//the current bet
    int dealer_cards[30];//store the dealer`s cards
    int dealer_num_cards;//the amount of cards in dealer´s hand
    int dealer_hand;//get the sumatory of the dealer`s hand
} Game_controller;

#endif
