// #include <arpa/inet.h>
// #include <google/protobuf/io/zero_copy_stream_impl.h>
// #include <google/protobuf/text_format.h>
// #include <netdb.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <cstring>
// #include <iostream>
// #include <string>

// #include "amazon_ups.pb.h"
// #include "ups_sender.h"
// #include "ups_communicator.h"

// using namespace std;

// Upswriter::Upswriter(UpsCommunicator* uc, int ups_fd):ups_communicator(uc){
//     //Other things initialize after add thread
// }

// void Upswriter::start_recv_ups(){
//     AUCommands *au_command = new AUCommands;
//     recvMesgFrom(*au_command, ups_communicator->sock_fd);
// }
