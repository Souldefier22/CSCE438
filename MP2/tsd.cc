#include <ctime>

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>

#include "sns.grpc.pb.h"

using google::protobuf::Timestamp;
using google::protobuf::Duration;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using csce438::Message;
using csce438::Request;
using csce438::Reply;
using csce438::SNSService;

std::vector<struct user>* users = new std::vector<struct user>;

struct user{
    std::string username;
    std::vector<std::string> followers;
    std::vector<std::string> following;
};

class SNSServiceImpl final : public SNSService::Service {
  
  Status List(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // LIST request from the user. Ensure that both the fields
    // all_users & following_users are populated
    // ------------------------------------------------------------
    std::string name = request->username();
    
    user cur_user;
    if(users->empty() != true){
      //add the users
      for(auto i = users->begin(); i != users->end(); ++i){
          cur_user = *i;
          reply->add_all_users(cur_user.username);
          std::cout << cur_user.username << std::endl;
          
          //add the followers for that username
          std::vector<std::string>::const_iterator it;
          if(cur_user.username == name && !cur_user.followers.empty()){
            for(it = cur_user.followers.begin(); it != cur_user.followers.end(); it++){
              reply->add_following_users(*it);
            }
          }
      }
    }
    return Status::OK;
  }

  Status Follow(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // request from a user to follow one of the existing
    // users
    // ------------------------------------------------------------
    return Status::OK; 
  }

  Status UnFollow(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // request from a user to unfollow one of his/her existing
    // followers
    // ------------------------------------------------------------
    return Status::OK;
  }
  
  Status Login(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // a new user and verify if the username is available
    // or already taken
    // ------------------------------------------------------------
    std::string name = request->username();
    
    user cur_user;
    bool used = false;
    //check if name already exists
    if(users->empty() != true){
      for(auto i = users->begin(); i != users->end(); ++i){
          cur_user = *i;
          if(cur_user.username == name){
              used = true;
              break;
          }
      }
    }
    
    if(used == false){
      struct user new_user;
      new_user.username = name;
      
      users->push_back(new_user);
      reply->set_msg("Successful Login");
      return Status::OK;
    }
    else{
      reply->set_msg("Username is invalid");
      return Status::OK;
    }
  }

  Status Timeline(ServerContext* context, ServerReaderWriter<Message, Message>* stream) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // receiving a message/post from a user, recording it in a file
    // and then making it available on his/her follower's streams
    // ------------------------------------------------------------
    return Status::OK;
  }

};

void RunServer(std::string port_no) {
  // ------------------------------------------------------------
  // In this function, you are to write code 
  // which would start the server, make it listen on a particular
  // port number.
  // ------------------------------------------------------------
  
  std::string server_addr("127.0.0.1:");
  server_addr = server_addr + port_no;
  SNSServiceImpl service;
  
  //build and start the server
  ServerBuilder builder;
  builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "The server is now listening on: " << server_addr << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  
  std::string port = "3010";
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:")) != -1){
    switch(opt) {
      case 'p':
          port = optarg;
          break;
      default:
	         std::cerr << "Invalid Command Line Argument\n";
    }
  }
  RunServer(port);
  return 0;
}
