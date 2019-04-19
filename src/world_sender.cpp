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
#include "world_sender.h"

using namespace std;

WorldSender::WorldSender(WorldCommunicator* wc) : w_communicator(wc) {
    // Other things initialize after add thread
    // world_sender_thread;
    this->world_sender_thread = thread(&WorldSender::start_send_to_world, this);
}

void WorldSender::start_send_to_world() {
    while (1) {
        if (shutdown) {
            break;
        }
        ACommands message_to_world = w_sender_queue.popfront();
        w_communicator->send_msg(message_to_world);
    }
}
