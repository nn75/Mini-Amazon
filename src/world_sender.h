#ifndef _WORLDSENDER_H
#define _WORLDSENDER_H

#include <thread>

#include "world_communicator.h"

using namespace std;

class WorldSender {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // For send and receive from ups
    WorldCommunicator* w_communicator;
    // message_queue for sending to world
    // message_queue<ACommands> w_sender_queue;
    thread world_sender_thread;

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    WorldSender(WorldCommunicator& wc);
    // Start receving from web
    void start_send_to_world();
};

#endif
