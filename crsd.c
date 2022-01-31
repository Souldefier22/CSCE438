#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include "interface.h"

std::vector<struct chatroom>* chatrooms = new std::vector<struct chatroom>;
int start_port = 3006;

struct chatroom{
    int num_members;
    int slave;
    int port;
    int members[256];
    std::string name;
    bool active = false;
    pthread_t id;
};

void * chat_handler(void * input){
    printf("Made it to chat_handler\n");
    exit(1);
}

//what to do for different requests given by client
void * client_request(void * master_sock){
    int master_socket = *(int *) master_sock;
    printf("Made it to client_request\n");
    
    
    char request[256];
    int data_rec = 0;
    std::string name;
    std::string response;
    
    while(1){
        memset(request, 0, 256);
        data_rec = recv(master_socket, request, 256, 0);
        //check if request has data
        if(data_rec > 0){
            if(strncmp(request, "CREATE", 6) == 0){
                printf("Create found in server\n");
                
                struct chatroom room;
                room.num_members = 0;
                room.active = true;
                //get a new port for the new room based on previously used ports
                if(chatrooms->empty()){
                    room.port = start_port + 1;
                }
                else{
                    chatroom prev_room = chatrooms->at(chatrooms->size() - 1);
                    room.port = prev_room.port + 1;
                }
                
                //find the space is the request given and then save the name
                for(int i = 0; i < strlen(request); i++){
                    if(isspace(request[i])){
                        name = "";
                    }
                    else{
                        name += request[i];
                    }
                }
                printf("Name of room is: \n", name);
                room.name = name;
                chatrooms->push_back(room);
                
                pthread_t room_thread;
                pthread_attr_t room_attr;
                pthread_create(&room_thread, &room_attr, chat_handler, (void*) &room);
                response = "0\n";
                
                send(master_socket, response.c_str(), response.length(), 0);
                
            }
            else if(strncmp(request, "DELETE", 6) == 0){
                printf("Delete found in server\n");
            }
            else if(strncmp(request, "JOIN", 4) == 0){
                printf("Join found in server\n");
            }
            else if(strncmp(request, "LIST", 4) == 0){
                printf("List found in server\n");
            }
        }
    }
    //pthread_exit(NULL);
}

int main(int argc, char const *argv[]){
    printf("Beginning the main\n");
    //get preliminary information such as port number or name
    int slave_socket; //server
    int master_socket; //client
    int rc;
    char buffer[256];
    int num_rooms = 0;
    fd_set read_fd;
    
    //memset(chatrooms, 0, sizeof(struct chatroom) * 256);
    
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    
    //getting a socket descriptor 
    slave_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(slave_socket < 0){
        perror("Create Socket Failed");
        return -1;
    }
    
    printf("Socket has been created\n");
    
    //associate port with socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));
    
    start_port = atoi(argv[1]);
    
    rc = bind(slave_socket, (struct sockaddr*)&server, sizeof(server));
    if(rc < 0){
        perror("Binding Failed");
        return -1;
    }
    
    printf("Socket is binded\n");
    
    rc = listen(slave_socket, 10);
    if(rc < 0){
        perror("Listen Failed");
        return -1;
    }
    
    printf("Socket is listening\n");
    
    struct sockaddr_in master_data;
    int info_length = sizeof(master_data);
    memset(&master_data, 0, info_length);
    
    //master socket is the socket for the client while slave socket is the socket for the server
    while(1){
        master_socket = accept(slave_socket, (struct sockaddr*) &master_data, (socklen_t*) &info_length);
        
        pthread_t cur_thread;
        
        if(pthread_create(&cur_thread, NULL, &client_request, (void*) &master_socket) != 0) {
            printf("Thread creation failed\n");
            break;
        }
    }
    
    return 0;
}