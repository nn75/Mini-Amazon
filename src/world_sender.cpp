#include <arpa/inet.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

#include "world_amazon.pb.h"
#include "world_communicator.h"
#include "message_queue.h"

#include "world_sender.h"

using namespace std;

WorldSender::WorldSender(WorldCommunicator* wc,
                         message_queue<pair<long int, ACommands> >& w_s_q)
    : w_communicator(wc), w_sender_queue(w_s_q), world_sender_thread() {
    // Other things initialize after add thread
    // world_sender_thread;
    cout << "start world sender thread:\n" << endl;
    this->world_sender_thread = thread(&WorldSender::start_send_to_world, this);
}

void WorldSender::start_send_to_world() {
    while (1) {
        ACommands message_to_world = w_sender_queue.send_next().second;
        if(!w_communicator->send_msg( message_to_world)){
            cout << "Send to world failed\n";
            break;
        }
    }
}
