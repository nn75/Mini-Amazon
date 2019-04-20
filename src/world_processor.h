#ifndef __WORLDPROCESSOR__H_
#define __WORLDPROCESSOR__H_

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <iostream>
#include <thread>
#include "amazon_ups.pb.h"
#include "communicator.h"
#include "world_amazon.pb.h"
#include "world_receiver.h"
#include "world_sender.h"

using namespace std;

// worldworker(w_send_queue, w_recv_queue, ups_send_queue, world_sender,
// world_receiver, world_communicator,seq)
class WorldProcessor {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // target's port number
    message_queue<pair<long int, ACommands> >& send_world_queue;
    message_queue<AResponses>& recv_world_queue;
    message_queue<AUCommands>& send_ups_queue;
    WorldSender world_sender;
    WorldReceiver world_receiver;
    long int& world_seqnum;
    long int& ups_seqnum;
    mutex& mtx;
    thread world_thread;
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   public:
    WorldProcessor(message_queue<pair<long int, ACommands> >& mq1,
                   message_queue<AResponses>& mq2,
                   message_queue<AUCommands>& mq3,
                   WorldCommunicator* world_communicator, long int& wnum,
                   long int& unum, mutex& mt);
    ~WorldProcessor(){};
    void world_command_process();
};

#endif
