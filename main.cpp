#include <iostream>
#include "communication.h"
#include "dealCard.h"
#include "encryption.h"
#include "handShake.h"
#include <unistd.h>
#include <time.h>

#define TOTAL_CARD_COUNT 52
#define HAND_COUNT 5

using namespace std;
string opponent_ip, player_status;
int player_port, opponent_port, server_socket;
struct sockaddr_in server;

int main()
{
    string tmp_msg;
    int num1, num2, num, encryption_key, decryption_key, my_hand_value, opponent_hand_value, opponent_decryption_key;
    int hand[HAND_COUNT], cards[TOTAL_CARD_COUNT],opponent_hand_encr[HAND_COUNT],opponent_hand_recv[HAND_COUNT], tmp_arr[TOTAL_CARD_COUNT];
    bool handshake_status;
    int dealer_port = 9999,non_dealer_port = 9998, dealer_prime = 7, non_dealer_prime = 13;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server_socket =  socket(AF_INET, SOCK_DGRAM, 0);
    opponent_ip = "127.0.0.1";
    cout<<"MENTAL POKER "<<endl;
    cout<<"First Selecting the port and player status based on availability "<<endl;
    if (server_socket < 0)
    {
         std::cout<<"UDP_socket error"<<std::endl;
         exit(1);
    }

    //first test dealer port
    server.sin_port = htons(dealer_port);
    if ((bind(server_socket, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
         server.sin_port = htons(non_dealer_port);
         if ((bind(server_socket, (struct sockaddr *)&server, sizeof(server))) < 0)
         {
             cout<<"Unable to bind either of the ports"<<endl;
             exit(0);
         }
         player_status = "non-dealer";
         player_port = non_dealer_port;
         opponent_port = dealer_port;
         num1 = non_dealer_prime;
         num2 = dealer_prime;
    }
    else{

        player_status = "dealer";
        player_port = dealer_port;
        opponent_port = non_dealer_port;
        num1 = dealer_prime;
        num2 = non_dealer_prime;
    }
    cout<<"My Status "<<player_status<<endl;
    cout<<"My port "<<player_port<<endl;
    cout<<"Chosen Prime Number "<<num1<<endl;
    cout<<"Opponent IP "<<opponent_ip<<endl;
    cout<<"Opponent port "<<opponent_port<<endl;
    cout<<"Prime No chose by Opponent "<<num2<<endl;
    cout<<"Starting the Server"<<endl;
    start_server(player_port);
    handshake_status= initial_handshake(player_port, player_status, opponent_ip, opponent_port);
    //out<<"Reached here"<<endl;
    int phi;
    int Binv,Ainv;
    num = num1 * num2;
    phi = (num1 - 1) * (num2 - 1);
    encryption_key = get_enc_key(phi);
    decryption_key = get_dec_key(phi, encryption_key);
    cout<<"Dealing cards "<<endl;
    deal_cards(hand,opponent_hand_encr,player_status,player_port,opponent_ip,opponent_port,encryption_key,decryption_key,num);
    cout<<"Cards dealing Completed"<<endl;
    cout<<"Starting actual Game"<<endl;
    cout<<"1. Calculate your Hand value"<<endl;
    my_hand_value = total_hand_value(hand,HAND_COUNT);
    cout<<"My hand value is "<<my_hand_value<<endl;
    tmp_arr[0] = my_hand_value;
    tmp_msg = prepare_cards(tmp_arr,1);
    cout<<"2. Send your hand value to opponent"<<endl;
    send_message(opponent_ip,opponent_port,tmp_msg);
    cout<<"Value send"<<endl;
    cout<<"3. Wait for hand value from opponent"<<endl;
    wait_for_cards(tmp_arr,1);
    opponent_hand_value = tmp_arr[0];
    cout<<"Opponent hand value is "<<opponent_hand_value<<endl;
    cout<<"4. Compare Hand values"<<endl;
    if(my_hand_value > opponent_hand_value){
        cout<<"CONGRATULATIONS!!!!! You won"<<endl;
    }
    else{
        cout<<"SORRY!!! YOU LOST :( "<<endl;
    }
    /**** VERIFICATION ******/
    cout<<"Starting Verification"<<endl;
    tmp_arr[0] = decryption_key;
    tmp_msg = prepare_cards(tmp_arr,1);
    send_message(opponent_ip,opponent_port,tmp_msg);
    wait_for_cards(tmp_arr,1);
    opponent_decryption_key = tmp_arr[0];
    tmp_msg = prepare_cards(hand,HAND_COUNT);
    send_message(opponent_ip,opponent_port,tmp_msg);
    wait_for_cards(opponent_hand_recv,HAND_COUNT);
    decrypt_cards(opponent_hand_encr,decryption_key,HAND_COUNT,num);

    /*
     * check if the opponent hand value is same as what you were send before
     */
    if(total_hand_value(opponent_hand_recv,HAND_COUNT) != opponent_hand_value){
        cout<<"Your Opponent was a cheater !!!!!!!"<<endl;
    }

    else if(!compare_hands(opponent_hand_encr,opponent_hand_recv,HAND_COUNT)){
        cout<<"Your Opponent was a cheater !!!!!!!"<<endl;
    }
    cout<<"Verification cleared, No players has cheated"<<endl;
    cout<<"Your hand was"<<endl;
    print_cards(hand,HAND_COUNT);
    cout<<"Your Opponent hand was"<<endl;
    print_cards(opponent_hand_encr,HAND_COUNT);
    stop_server();
    cout<<"MENTAL POKER FINISHED THANKS FOR PLAYING"<<endl;
}

