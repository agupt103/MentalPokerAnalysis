#include "dealCard.h"
#include "communication.h"
#include "encryption.h"
#define TOTAL_CARD_COUNT 52
#define HAND_COUNT 5

using namespace std;

void print_cards(int *cards,int count){
    // Prints the given set of cards
    // @param cards - The array to be printed
    // @param count - The size of that array
    //
    for(int i=0;i<count;i++){
        cout<<cards[i]<<endl;
    }
}

void get_card_deck(int* cards,int count){
    // Creates a set of cards
    // @param cards - The array to hold the cards
    // @param count - The size of that array
    //
    for(int i=0;i<count;i++){
        cards[i] = i+1;
    }
}

bool encrypt_cards(int* hand,int encryption_key,int count,int known_num){
    // Here the set of unencrypted cards are encrypted using the given encryption key
    // @param hand - an integer array containing the cards to be encrypted, will also contain the encrypted cards
    // @param encryption_key - the encryption key used for encryption
    // @param count - size of the hand
    // @param known_num - A number decided between the two players
    // @return the status of the operation
    //
    int tmp;
    for(int i =0;i<count;i++){
        tmp = encrypt_number(hand[i],encryption_key,  known_num);
        hand[i] = tmp;
    }
}

bool decrypt_cards(int* hand,int decryption_key,int count,int known_num){
    // Here the set of encrypted cards are decrypted using the given decryption key
    // @param hand - an integer array containing the cards to be decrypted, will also contain the decrypted cards
    // @param decryption_key - the decryption key used for decryption
    // @param count - size of the hand
    // @param known_num - A number decided between the two players
    // @return the status of the operation
    //
    int tmp;
    for(int i =0;i<count;i++){
        tmp = encrypt_number(hand[i],decryption_key,  known_num);
        hand[i] = tmp;
    }
}


bool select_hands(int* cards,int* player_hand,int* other_player_hand,int count){
    // Select two sets of hands from a given set of cards
    // Each hand will have the same number of cards, which is given by the argument count
    // @param cards - the full deck of cards
    // @param player_hand - The hand for the dealer
    // @param other_player_hand - The hand for the other player
    // @param count - The count of the hands
    // @return status of the operation
    //
    srand ( time(NULL) );
    int random_pos[count], index = 0,hand_index = 0, rpos, found = 0;
    while (index < count){
        rpos = rand() % count;
        found = -1;
        for(int i = 0;i <= index;i++){
            if(random_pos[i] == rpos){
                found = 0;
                break;
            }
        }
        if ( found == -1){
            random_pos[index] = rpos;
            index++;
        }
    }
    for(int i = 0;i< count;i++){
        if(i < count/2){
            player_hand[i] = cards[random_pos[i]];
        }
        else{
            other_player_hand[i - count/2] = cards[random_pos[i]];
        }
    }
    return true;
}


bool shuffle(int* cards, int count){
    // Randomly shuffle the set of cards
    // @param cards - card set to be shuffled
    // @param count - size of the card set
    int tmp1,tmp2;
    srand ( time(NULL) );
    for(int i=0;i<count;i++){
        tmp1= cards[i];
        tmp2 = rand()%count;
        cards[i] = cards[tmp2];
        cards[tmp2] = tmp1;
    }
}

bool deal_cards(int* hand, int* other_player_hand, string player_status, int player_port, string opponent_ip, int opponent_port, int encryption_key, int decryption_key, int known_num){
    // This function is used to distribute the cards between the two users using the mental poker protocol
    // Here the player who is the dealer selects the two hands which will be used
    // The non dealer created the initial 52 card set
    // @param player_hand - The hand for the dealer
    // @param other_player_hand - The hand for the other player
    // @param player_status - The status of the player
    // @param player_port - the port of the player
    // @param opponent_ip - The ip of the opponent
    // @param opponent_port - The port of the opponent
    // @param encryption_key - The encryption key
    // @param decryption_key - The decryption key
    // @param known_num - The number decided upon by both players
    // @return status of the operation
    //
    int  tmp_count, msg_flag = 0, cards[TOTAL_CARD_COUNT];
    string tmp_msg,tmp_msg2;

    if(player_status == "dealer"){
        wait_for_cards(cards,TOTAL_CARD_COUNT);
        select_hands(cards,hand,other_player_hand,HAND_COUNT*2);
        tmp_msg = prepare_cards(other_player_hand,HAND_COUNT);
        send_message(opponent_ip,opponent_port,tmp_msg);
        encrypt_cards(hand,encryption_key,HAND_COUNT, known_num);
        tmp_msg = prepare_cards(hand,HAND_COUNT);
        send_message(opponent_ip,opponent_port,tmp_msg);
        wait_for_cards(hand,HAND_COUNT);
        decrypt_cards(hand,decryption_key,HAND_COUNT, known_num);

    }
    else{
            get_card_deck(cards,TOTAL_CARD_COUNT);
            shuffle(cards,TOTAL_CARD_COUNT);
            encrypt_cards(cards,encryption_key,TOTAL_CARD_COUNT, known_num);
            tmp_msg = prepare_cards(cards,TOTAL_CARD_COUNT);
            send_message(opponent_ip,opponent_port,tmp_msg);
            wait_for_cards(hand,HAND_COUNT);
            wait_for_cards(other_player_hand,HAND_COUNT);
            decrypt_cards(other_player_hand,decryption_key,HAND_COUNT, known_num);
            tmp_msg = prepare_cards(other_player_hand,HAND_COUNT);
            send_message(opponent_ip,opponent_port,tmp_msg);
            decrypt_cards(hand,decryption_key,HAND_COUNT, known_num);
     }
    return true;
}

