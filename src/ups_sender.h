#ifndef _UPSSENDER_H
#define _UPSSENDER_H

#include <thread>

#include "amazon_ups.pb.h"
#include "ups_communicator.h"

#include "message_queue.h"

using namespace std;

class UpsSender {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // For send and receive from ups
    UpsCommunicator* u_communicator;
    // message_queue for sending to world
    message_queue<AUCommands>& u_sender_queue;
    // message_queue<ACommands> w_sender_queue;
    thread ups_sender_thread;

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    UpsSender();
    UpsSender(UpsCommunicator* uc, message_queue<AUCommands>& u_s_q);
    // Start receving from web
    void start_send_to_ups();
};

#endif
