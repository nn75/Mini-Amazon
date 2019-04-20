#ifndef _WORLDRECEIVER_H
#define _WORLDRECEIVER_H

#include <thread>

#include "world_amazon.pb.h"
#include "world_communicator.h"

#include "message_queue.h"

using namespace std;

class WorldReceiver {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // For send and receive from world
    WorldCommunicator* w_communicator;
    // message_queue for sending to world
    message_queue<AResponses>& w_recv_queue;

    thread world_receiver_thread;

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    WorldReceiver();
    WorldReceiver(WorldCommunicator* wc, message_queue<AResponses>& w_r_q);
    // Start receving from world
    void start_send_to_world();
};

#endif
