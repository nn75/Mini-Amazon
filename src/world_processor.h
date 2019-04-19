#ifndef __WORLDPROCESSOR__H_
#define __WORLDPROCESSOR__H_

#include "communicator.h"

// worldworker(w_send_queue, w_recv_queue, ups_send_queue, world_sender,
// world_receiver, world_communicator,seq)
class WorldProcessor {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // target's port number
    message_queue<ACommands>* send_world_queue;
    message_queue<ACommands>* recv_world_queue;
    message_queue<AUCommands>* send_ups_queue;
    WorldSender world_sender;
    WorldReceiver world_receiver;
    WorldCommunicator world_communicator;
    long seqnum;
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   public:
    WorldProcessor();
    ~WorldProcessor();
};

#endif
