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
    
    while(1){
        if(slave_socket != -1){
            close(slave_socket);
        }
        if(master_socket != -1){
            close(master_socket);
        }
    }
    
    return 0;
}
