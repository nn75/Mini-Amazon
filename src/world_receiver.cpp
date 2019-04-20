#include <arpa/inet.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

#include "message_queue.h"
#include "world_amazon.pb.h"
#include "world_communicator.h"

#include "world_receiver.h"

using namespace std;

WorldReceiver::WorldReceiver(WorldCommunicator* wc,
                             message_queue<AResponses>& w_r_q)
    : w_communicator(wc), w_recv_queue(w_r_q), world_receiver_thread() {
    // Other things initialize after add thread
    // world_sender_thread;
    cout << "start world receiver thread:\n" << endl;
    world_receiver_thread = thread(&WorldReceiver::start_send_to_world, this);
}

void WorldReceiver::start_send_to_world() {
    while (1) {
        AResponses message_from_world;
        if (!w_communicator->recv_msg(message_from_world)) {
            cout << "Receive from world failed\n";
            break;
        }
        w_recv_queue.pushback(message_from_world);
    }
}
