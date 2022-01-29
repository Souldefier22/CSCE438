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
#include "interface.h"

struct chatroom{
    int num_members;
    int slave;
    int port;
    int members[256];
    char name[256];
    pthread_t id;
};

void * client_request(void * master_sock){
    int master_socket = *(int *) master_sock;
    printf("Made it to client_request\n");
    
    char request[256];
    int data_rec = 0;
    
    while(1){
        memset(request, 0, 256);
        data_rec = recv(master_socket, request, 256, 0);
        printf("Inside loop\n");
        //check if request has data
        if(data_rec > 0){
            if(strncmp(request, "CREATE", 6) == 0){
                printf("Create found in server\n");
                return -1;
            }
            else if(strncmp(request, "DELETE", 6) == 0){
                printf("Delete found in server\n");
                return -1;
            }
            else if(strncmp(request, "JOIN", 4) == 0){
                printf("Join found in server\n");
                return -1;
            }
            else if(strncmp(request, "LIST", 4) == 0){
                printf("List found in server\n");
                return -1;
            }
        }
    }
    return -1;
}

int main(int argc, char const *argv[]){
    printf("Beginning the main\n");
    //get preliminary information such as port number or name
    int slave_socket;
    int master_socket;
    int rc;
    struct chatroom rooms[256];
    char buffer[256];
    int num_rooms = 0;
    fd_set read_fd;
    
    memset(rooms, 0, sizeof(struct chatroom) * 256);
    
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
    
    /*
    master_socket = accept(slave_socket, NULL, NULL);
    if(master_socket < 0){
        perror("Socket for master failed");
        return -1;
    }
    
    printf("Socket is accepting\n");
    
    FD_ZERO(&read_fd);
    FD_SET(master_socket, &read_fd);
    
    rc = select(master_socket+1, &read_fd, NULL, NULL, NULL);
    if(rc < 0){
        perror("Select Failed");
        return -1;
    }
    
    printf("System selected\n");
    
    int length = 256;
    
    rc = recv(master_socket, buffer, sizeof(buffer), 0);
    
    rc = send(master_socket, buffer, sizeof(buffer), 0);
    
    */
    struct sockaddr_in master_data;
    int info_length = sizeof(master_data);
    memset(&master_data, 0, info_length);
    
    //master socket is the socket for the client while slave socket is the socket for the server
    while(1){
        master_socket = accept(slave_socket, (struct sockaddr*) &master_data, (socklen_t*) &info_length);
        
        pthread_t cur_thread;
        pthread_attr_t cur_attr;
        
        if(pthread_create(&cur_thread, &cur_attr, &client_request, (void*) &master_socket) != 0) {
            printf("Thread creation failed\n");
            break;
        }
    }
    
    return 0;
}
