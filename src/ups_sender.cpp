#include <arpa/inet.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

#include "amazon_ups.pb.h"
#include "message_queue.h"
#include "ups_communicator.h"

#include "ups_sender.h"

using namespace std;

UpsSender::UpsSender(UpsCommunicator* uc, message_queue<AUCommands>& w_s_q)
    : u_communicator(uc), u_sender_queue(w_s_q), ups_sender_thread() {
    // Other things initialize after add thread
    // ups_sender_thread;
    cout << "start ups sender thread:\n" << endl;
    this->ups_sender_thread = thread(&UpsSender::start_send_to_ups, this);
}

void UpsSender::start_send_to_ups() {
    while (1) {
        AUCommands message_to_ups = u_sender_queue.send_next();
        if (!u_communicator->send_msg(message_to_ups)) {
            cout << "Send to ups failed\n";
            break;
        }
    }
}
