#ifndef HANDSHAKE_H
#define HANDSHAKE_H

bool initial_handshake(int player_port, std::string player_status, std::string opponent_ip, int opponent_port);

int total_hand_value(int* hand,int count);

bool compare_hands(int* hand1,int* hand2, int count);

#endif // HANDSHAKE_H

