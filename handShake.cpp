#include "dealCard.h"
#include "communication.h"
#include "encryption.h"
#include "handShake.h"
#include "unistd.h"
#define HANDSHAKE_ACK "@RECIEVED@"
#define HANDSHAKE_FAIL "@FAILED@"
#define PING_RECIEVED_MSG "@@@PING_RECIEVED@@@"

using namespace std;

bool wait_for_opponent(int player_port, string opponent_ip, int opponent_port){
    // Waits till the opponent sends you a ping reply message, we use this to ensure that the other player is active
    // @param player_port - The player port
    // @param opponent_ip - The ip of opponent
    // @param opponent_port - The port of opponent
    // @return status of the operation
    //
    string ping_message = create_ping_message(), tmp_msg;
    time_t time1,time2;
    double seconds_elasped;
    send_message(opponent_ip, opponent_port, ping_message);
    time(&time1);
        tmp_msg = wait_for_message();
        if(tmp_msg != ""&&tmp_msg.find(PING_RECIEVED_MSG) >= 0){
            send_message(opponent_ip, opponent_port, create_ping_recieved_message());
            return true;
        }
    return false;
}

bool initial_handshake(int player_port, string player_status, string opponent_ip, int opponent_port){
    // A sequence of steps to establish the initial conditions for the game
    // First we wait till the opponent is active
    // Next we exchange the player status and the prime number chosen by the opponent
    // @param player_port - The player port
    // @param player_status - The player status
    // @param opponent_ip - The ip of opponent
    // @param opponent_port - The port of opponent
    // @return status of the operation
    //

    string tmp_msg, tmp_msg2;
    time_t time1,time2;
    string opponent_status;
    int tmp_arr[100], tmp_count;
    if(wait_for_opponent(player_port,opponent_ip,opponent_port)){
        cout<<"Opponent is active"<<endl;
        //if(player_status != "dealer"){
        sleep(1);
        clear_ping_msgs();
    }
    else{
        exit(0);
    }
    tmp_msg = prepare_message(player_status);
    send_message(opponent_ip, opponent_port, tmp_msg);
    opponent_status = wait_for_message();
    cout<<"Opponent status is "<<opponent_status<<endl;
    if(opponent_status == player_status){
        cout<<"Both players cant have the same status"<<endl;
        exit(0);
    }
    if(opponent_status != "dealer" && player_status != "dealer"){
        cout<<"There should be atleast one dealer"<<endl;
        exit(0);
    }
   return true;

}

int total_hand_value(int* hand,int count){
    // Funtion to calculate the total value of a given hand
    // @param hand - an integer array containing the cards
    // @param count - Size of the hand
    // @return the status of the operation
    //
    int total = 0;
    int g;
    for(int i=0;i<count;i++)
    {
        total+= hand[i];
    }
    return total;
}

bool compare_hands(int* hand1,int* hand2, int count){
    // Check if two hands are equivalent or not
    // This function is used in the verfication stage to compare the hand
    // passed by the opponent and the encrypted one we had before
    // @param hand1 - The first hand
    // @param hand2 - The second hand
    // @param count - hand size
    // @return status of the operation
    //
    bool result=true;
    for(int i = 0 ; i < count; i++)
        {
            if(hand1[i]!=hand2[i])
            {
                result=false;
                break;
            }
        }
    return result;
}

