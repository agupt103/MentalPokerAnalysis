#ifndef DEALCARD_H
#define DEALCARD_H
#include <stdio.h>
#include <iostream>
#include <string>

void print_cards(int *cards,int count);

void get_card_deck(int* cards,int count);

bool encrypt_cards(int* hand,int encryption_key,int count, int known_num);

bool decrypt_cards(int* hand,int decryption_key,int count,int known_num);

bool select_hands(int* cards,int* player_hand,int* other_player_hand,int count);

bool shuffle(int* cards, int count);

bool deal_cards(int* hand,int* other_player_hand, std::string player_status, int player_port, std::string opponent_ip, int opponent_port, int encryption_key, int decryption_key, int known_num);

#endif // DEALCARD_H
