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

#include "ups_receiver.h"

using namespace std;

UpsReceiver::UpsReceiver(UpsCommunicator* uc, message_queue<UACommands>& u_r_q)
    : u_communicator(uc), u_recv_queue(u_r_q), ups_receiver_thread() {
    // Other things initialize after add thread
    // ups_receiver_thread;
    cout << "start ups receiver thread:\n" << endl;
    ups_receiver_thread = thread(&UpsReceiver::start_send_to_ups, this);
}

void UpsReceiver::start_send_to_ups() {
    while (1) {
        UACommands message_from_ups;
        if (!u_communicator->recv_msg(message_from_ups)) {
            cout << "Receive from ups failed\n";
            break;
        }
        u_recv_queue.pushback(message_from_ups);
    }
}
