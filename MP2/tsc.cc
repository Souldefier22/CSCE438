#include <iostream>
#include <string>
#include <unistd.h>
#include <memory>
#include <thread>
#include <grpc++/grpc++.h>
#include "client.h"
#include "sns.grpc.pb.h"

using grpc::ClientContext;
using grpc::Status;
using csce438::SNSService;
using csce438::Request;
using csce438::Reply;
using csce438::Message;

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {}
    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string username;
        std::string port;
        
        // You can have an instance of the client stub
        // as a member variable.
        std::unique_ptr<SNSService::Stub> stub_;
};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "default";
    std::string port = "3010";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
	// ------------------------------------------------------------
	
	stub_ = std::unique_ptr<SNSService::Stub>(SNSService::NewStub(grpc::CreateChannel(hostname + ":" + port, grpc::InsecureChannelCredentials())));
    
    IReply ire;
    Request new_req;
    new_req.set_username(username);
    
    Reply rep;
    ClientContext context;
    
    Status rec_status = stub_->Login(&context, new_req, &rep);
    ire.grpc_status = rec_status;
    
    if(rep.msg() == "Username is invalid"){
        ire.comm_status = FAILURE_ALREADY_EXISTS;
        std::cout << "Bad username" << std::endl;
        return -1;
    }
    else{
        ire.comm_status = SUCCESS;
    }
    
    if(ire.grpc_status.ok() == false){
        std::cout << "The login failed" << std::endl;
        return -1;
    }
    
    return 1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
	//
	// FOLLOW <username>
	// UNFOLLOW <username>
	// LIST
    // TIMELINE
	//
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
	// GUIDE 2:
	// Then, you should create a variable of IReply structure
	// provided by the client.h and initialize it according to
	// the result. Finally you can finish this function by returning
    // the IReply.
	// ------------------------------------------------------------
    
	// ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Follow" service method for FOLLOW command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Follow(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
    // ------------------------------------------------------------
    
    IReply ire;
    Request new_req;
    Reply rep;
    ClientContext context;
    char cinput[input.length() + 1];
    strcpy(cinput, input.c_str());
    if(strncmp(cinput, "UNFOLLOW", 8) == 0){
        std::cout << "Unfollow found" << std::endl;
		new_req.set_username(username);
		std::string user_arg = input.substr(9, input.length()-8); 
		new_req.add_arguments(user_arg);
		
		Status rec_status = stub_->UnFollow(&context, new_req, &rep);
		ire.grpc_status = rec_status;
		
		if(rep.msg() == "Username is invalid"){
		    ire.comm_status = FAILURE_INVALID_USERNAME;
		}
		else if(rep.msg() == "Success"){
		    ire.comm_status = SUCCESS;
		}
		else{
		    ire.comm_status = FAILURE_UNKNOWN;
		}
	}
    else if(strncmp(cinput, "FOLLOW", 6) == 0){
        std::cout << "Follow found" << std::endl;
		new_req.set_username(username);
		std::string user_arg = input.substr(7, input.length()-6); 
		new_req.add_arguments(user_arg);
		
		Status rec_status = stub_->Follow(&context, new_req, &rep);
		ire.grpc_status = rec_status;
		
		if(rep.msg() == "Username is invalid"){
		    ire.comm_status = FAILURE_INVALID_USERNAME;
		}
		else if(rep.msg() == "Already following"){
		    ire.comm_status = FAILURE_ALREADY_EXISTS;
		}
		else if(rep.msg() == "Success"){
		    ire.comm_status = SUCCESS;
		}
		else{
		    ire.comm_status = FAILURE_UNKNOWN;
		}
	}
	else if(strncmp(cinput, "LIST", 4) == 0){
	    std::cout << "List found" << std::endl;
		new_req.set_username(username);
		
		Status rec_status = stub_->List(&context, new_req, &rep);
		ire.grpc_status = rec_status;
		if(rec_status.ok() == true){
		    ire.comm_status = SUCCESS;
		    
		    for(std::string u : rep.all_users()){
		        ire.all_users.push_back(u);
		    }
		    for(std::string u : rep.following_users()){
		        ire.following_users.push_back(u);
		    }
		}
	}
	else if(strncmp(cinput, "TIMELINE", 8) == 0){
		ire.comm_status = SUCCESS;
	}
    
    if(ire.comm_status == SUCCESS){
        return ire;
    }
    else{
        ire.comm_status = FAILURE_UNKNOWN;
        return ire;
    }
}

void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------
	
	while(true){
    	ClientContext context;
    	std::shared_ptr<ClientReaderWriter<Message, Message>> thread_client = stub_->Timeline(&context);
    	
    	std::thread writer([username, thread_client](){
    	    //initial message to connect
    	    Message m;
    	    m.set_username(username);
    	    m.set_msg("connect");
    	    thread_client->Write(m);
    	    
    	    std::string input;
    	    while(1){
    	        std::getline(std::cin, input);
    	        m.set_username = username;
    	        m.set_msg(input);
    	        if(thread_client->Write(m) == false){
    	            break;
    	        }
    	    }
    	    
    	    thread_client->WritesDone();
    	});
	}
}
