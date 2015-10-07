#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <pthread.h>
#include <queue>
#include <stdlib.h>
#include <unistd.h>

//External Variables to be used in communication functions
extern std::string opponent_ip, player_status;
extern int player_port, opponent_port, server_socket;


std::string create_ping_message();

std::string check_for_pings();

std::string create_ping_recieved_message();

void clear_msg_queues();

void *server_thread(void* server_port);

bool start_server(int server_port);

bool stop_server();

std::string recieve_message();

void send_message(std::string server_ip, int server_port, std::string mesg);

std::string prepare_cards(int *msg_array,int count);

int extract_cards(int *msg_array, std::string msg_string);

bool wait_for_cards(int* cards,int count);

std::string prepare_message(std::string msg);

std::string extract_message(std::string msg);

std::string wait_for_message();

void clear_ping_msgs();

#endif // COMMUNICATION_H
