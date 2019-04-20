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
// #include "ups_communicator.h"
// #include "message_queue.h"

// #include "ups_sender.h"

// using namespace std;

// UpsSender::UpsSender(UpsCommunicator* uc,
//                          message_queue<pair<long int, AUCommands> >& u_s_q)
//     : u_communicator(uc), u_sender_queue(u_s_q), ups_sender_thread() {
//     // Other things initialize after add thread
//     // world_sender_thread;
//     cout << "start world sender thread:\n" << endl;
//     ups_sender_thread = thread(&UpsSender::start_send_to_ups, this);
// }

// void UpsSender::start_send_to_ups() {
//     while (1) {
//         AUCommands message_to_ups = u_sender_queue.popfront().second;
//         u_communicator->send_msg(message_to_ups);
//     }
// }