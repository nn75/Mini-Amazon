#ifndef _UPSECEIVER_H
#define _UPSECEIVER_H

#include <thread>

#include "world_amazon.pb.h"
#include "ups_communicator.h"

#include "message_queue.h"

using namespace std;

class UpsReceiver {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // For send and receive from ups
    UpsCommunicator* u_communicator;
    // message_queue for sending to ups
    message_queue<UACommands>& u_recv_queue;

    thread ups_receiver_thread;

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    UpsReceiver();
    UpsReceiver(UpsCommunicator* uc, message_queue<UACommands>& u_r_q);
    // Start receving from ups
    void start_send_to_ups();
};

#endif
