#ifndef _WORLDSENDER_H
#define _WORLDSENDER_H

#include <thread>

#include "world_communicator.h"
#include "world_amazon.pb.h"

#include "message_queue.h"

using namespace std;

class WorldSender {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // For send and receive from ups
    WorldCommunicator* w_communicator;
    // message_queue for sending to world
    message_queue<pair<int, ACommands> >& w_sender_queue;
    // message_queue<ACommands> w_sender_queue;
    thread world_sender_thread;

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    WorldSender(WorldCommunicator* wc, message_queue<pair<int, ACommands> >& w_s_q);
    // Start receving from web
    void start_send_to_world();
};

#endif
