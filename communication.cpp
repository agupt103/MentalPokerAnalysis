/*
 * Containts functions to allow the players to communicate over
 * the network
 *
 */

#define MAX_LENGTH 1000
#define MSG_PREFIX "START"
#define MSG_SUFFIX "END"
#define MSG_PREFIX_LENGTH 5
#define PING_MSG "@@@PING@@@"
#define PING_RECIEVED_MSG "@@@PING_RECIEVED@@@"

#include "queue"
#include "pthread.h"
#include "communication.h"

int server_status = 0;
pthread_t thrd1;
std::queue<std::string> message_queue;

std::string create_ping_message(){
    // Used to create a ping message
    return std::string(MSG_PREFIX)+"\n"+std::string(PING_MSG)+"\n"+std::string(MSG_SUFFIX);
}
std::string create_ping_recieved_message(){
    // Used to create a ping reply message
    return std::string(MSG_PREFIX)+"\n"+std::string(PING_RECIEVED_MSG)+"\n"+std::string(MSG_SUFFIX);
}

void *server_thread(void* server_port){
    // The main server thread, here we keep listening for incoming messages
    // if the recieved message was a ping message we send a reply back else we store
    // the message in the message queue
    // @param server_port - The port of the server

    double seconds_elasped;
    struct sockaddr_in sender;
    int client_socket;
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    std::string ping_reply = create_ping_recieved_message(),msg_str;
    int address_length, message_length;
    char message[MAX_LENGTH],client_ip[INET_ADDRSTRLEN];
    while(server_status == 1){
        address_length = sizeof(sender);
        message_length = recvfrom(server_socket,message,MAX_LENGTH,0,(struct sockaddr *)&sender,(socklen_t*)&address_length);
        msg_str = std::string(message);
        if(message_length > 0){
            if (msg_str.find(PING_MSG) >= 0 && msg_str.find(PING_MSG) < message_length){\
                //ping_queue.push(PING_MSG);
                //inet_ntop(AF_INET, &sender.sin_addr,client_ip,INET_ADDRSTRLEN);
                send_message(opponent_ip,opponent_port,ping_reply);
                //sendto(client_socket,ping_mesage.c_str(),ping_mesage.length(),0,(struct sockaddr *)&sender,sizeof(sender));
            }
            else{
                //std::cout<<msg_str<<std::endl;
                message_queue.push(msg_str);
            }
        }
    }
}


bool start_server(int server_port){
    // Function to start the server thread
    // @param server_port - The port of the server

    int *server_port_ptr = (int*)malloc(sizeof(int));
    *server_port_ptr = server_port;
    server_status = 1;

    pthread_create( &thrd1, NULL, server_thread, (void*)server_port_ptr);

}

bool stop_server(){
    // Function to stop the server thread
    server_status = 0;
    close(server_socket);
    //pthread_join(thrd1, NULL);
}

std::string recieve_message(){
    // Check if there are any new messages in the message queue
    // @return the recieved encoded message
    std::string message;
    //std::cout<<message_queue.size()<<std::endl;
   if(message_queue.size() > 0){
        message = message_queue.front();
        message_queue.pop();
        return message;
    }
    return "";
}



void send_message(std::string server_ip, int server_port, std::string mesg){
    // Send a message to your opponent
    struct sockaddr_in destination;
    int client_socket;
    destination.sin_family = AF_INET;
    destination.sin_addr.s_addr = inet_addr(server_ip.c_str());
    destination.sin_port = htons(server_port);
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    sendto(client_socket,mesg.c_str(),mesg.length(),0,(struct sockaddr *)&destination,sizeof(destination));

}

std::string prepare_cards(int *msg_array,int count){
    // Create a string encoding for your set of cards
    // Encoding information - Here each number representing the card is encoded as a string
    // and then concatenated with '\n' as the separator and then we add MSG_PREFIX at the beggining and
    // MSG_SUFFIX at the end
    // @param msg_array an integer array whose contents needs to be encoded in a message
    // @param count the size of msg_array
    // @return the encoded string
    //
    std::string message = MSG_PREFIX;
    std::stringstream ss;
    for(int i=0;i < count;i++){
        ss.str("");
        ss << msg_array[i];
        message = message + "\n" + ss.str();
    }
    message = message + "\n" + MSG_SUFFIX;

    return message;
}

int extract_cards(int *msg_array, std::string msg_string){
    // Extract the cards that were encoded in the given string
    // @param msg_array The array that is used to hold the extracted integers
    // @param msg_string string containing the encoded string
    // @return execution status (returns -1 if it fails)
    std::string tmp_string = "";
    int msg_index = 0;
    char tmp_char;
    if (msg_string.find(MSG_PREFIX) != 0 || msg_string.length() <= MSG_PREFIX_LENGTH){
        std::cout<<"Recieved message doesnt contain Header"<<std::endl;
        return -1;
    }
    for (int i = 6; i < msg_string.length();i++){
        tmp_char = msg_string[i];
        if (tmp_char == '\n'){
            if (tmp_string != ""){
                std::istringstream (tmp_string) >> msg_array[msg_index];
                msg_index++;
                tmp_string = "";
            }
            if (msg_string.substr(i+1,(msg_string.length() - i - 1)).find(MSG_SUFFIX) == 0 ){
                break;
            }
         }
        else{
                tmp_char = msg_string[i];
                if (!isdigit(tmp_char)){
                    std::cout<<"Here "<<tmp_char<<"+"<<std::endl;
                    return -1;
                }
                tmp_string = tmp_string + tmp_char;
        }
     }
     if(msg_index == 0){
         return -1;
     }
     return msg_index;
    }


bool wait_for_cards(int* cards, int count){
    // We wait for the opponent to send us card information
    // @param cards - The array used to hold the recieved array
    // @param count - The size of the array
    // @return execution status
    std::string tmp_msg;
    time_t time1,time2;
    double seconds_elasped;
    int msg_flag = -1,tmp_count;
    time(&time1);
    while(1){

        tmp_msg = recieve_message();
        if(tmp_msg ==""){
            continue;
        }
        else{
            msg_flag = 0;
            tmp_count = extract_cards(cards,tmp_msg);
            if (tmp_count != count){
                std::cout<<"Didnt recieve the expected number of cards"<<std::endl;
                exit(0);
            }
            break;
        }
        time(&time2);
        seconds_elasped = difftime(time2,time1);
        if(seconds_elasped > 60.0){
            break;
        }

    }
    if (msg_flag == -1){
        std::cout<<"No message recieved in the last 10 minutes"<<std::endl;
        exit(0);
    }
    return true;

}

std::string prepare_message(std::string msg){
    // Prepare the messages for transmission
    // @param msg the message to be encoded
    // @return the encoded message
    //
    std::string message = std::string(MSG_PREFIX) + "\n" + msg + "\n" + std::string(MSG_SUFFIX);
    return message;
}

std::string extract_message(std::string msg_string){
    // Extract the message from the encoded string
    // @param msg_string encoded message
    // return decoded message
    //
    std::string tmp_string = "";
    char tmp_char;
    if (msg_string.find(MSG_PREFIX) != 0 || msg_string.length() <= MSG_PREFIX_LENGTH){
        std::cout<<"Recieved message doesnt contain Header"<<std::endl;
        return "";
    }
    for (int i = 6; i < msg_string.length();i++){
        tmp_char = msg_string[i];
        if (tmp_char == '\n'){
            if (tmp_string != ""){
                return (tmp_string);
            }
        }
        else{
            tmp_char = msg_string[i];
            tmp_string = tmp_string + tmp_char;
        }

    }
    return "";
}

std::string wait_for_message(){
    // Wait for the opponent to send you a message, Here we have a time out of 60 seconds
    // @return recieved message
    time_t time1,time2;
    std::string tmp_msg,tmp_msg2="";
    int msg_flag = -1,tmp_count;
    double seconds_elasped;
    time(&time1);
    while(1){
        time(&time2);
        seconds_elasped = difftime(time2,time1);
        if(seconds_elasped > 60.0){
            break;
        }
        tmp_msg = recieve_message();
        if(tmp_msg ==""){
            continue;
        }
        else{
            //std::cout<<tmp_msg<<std::endl;

            msg_flag = 0;
            tmp_msg2 = extract_message(tmp_msg);
            if (tmp_msg2 == ""){
                std::cout<<"Didnt recieve the expected number of cards"<<std::endl;
                exit(0);
            }
            break;
        }
    }
    if (msg_flag == -1){
        std::cout<<"No message recieved in the last 10 minutes"<<std::endl;
        exit(0);
    }
    return tmp_msg2;

}
void clear_msg_queues(){
    // Clear contents from message queue
    while(!message_queue.empty()){
        message_queue.pop();
    }

}

void clear_ping_msgs(){
    // Only remove ping related messages from the message queue
    std::string tmp_string;
    std::queue<std::string> tmp_queue;
    while(!message_queue.empty()){
        tmp_string = message_queue.front();
        //std::cout<<tmp_string<<std::endl;
        message_queue.pop();
        if(tmp_string.find(PING_MSG) >= 0 && tmp_string.find(PING_MSG) < tmp_string.length()){
            //std::cout<<tmp_string<<std::endl;

            continue;
        }
        else if(tmp_string.find(PING_RECIEVED_MSG) >= 0 && tmp_string.find(PING_RECIEVED_MSG) < tmp_string.length()){
            //std::cout<<tmp_string<<std::endl;

            continue;
        }
        else{
            //std::cout<<tmp_string<<std::endl;
            tmp_queue.push(tmp_string);
        }
    }
    //std::cout<<"Size of tmp queue "<<tmp_queue.size()<<std::endl;

    message_queue.swap(tmp_queue);
    //std::cout<<"Size of message queue "<<message_queue.size()<<std::endl;
}
