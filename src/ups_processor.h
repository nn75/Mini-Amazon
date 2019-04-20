#ifndef __UPSPROCESSOR__H_
#define __UPSPROCESSOR__H_

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <iostream>
#include <thread>

#include "amazon_ups.pb.h"
#include "communicator.h"
#include "ups_receiver.h"
#include "ups_sender.h"
#include "world_amazon.pb.h"

using namespace std;

class UpsProcessor {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // target's port number
    message_queue<pair<long int, ACommands> >& send_world_queue;
    message_queue<AResponses>& recv_world_queue;
    message_queue<AUCommands>& send_ups_queue;
    UpsSender ups_sender;
    UpsReceiver ups_receiver;
    long int& world_seqnum;
    long int& ups_seqnum;
    mutex& mtx;
    thread ups_thread;
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   public:
    UpsProcessor(message_queue<pair<long int, ACommands> >& mq1,
                 message_queue<AResponses>& mq2, message_queue<AUCommands>& mq3,
                 UpsCommunicator* ups_communicator, long int& wnum,
                 long int& unum, mutex& mt);
    ~UpsProcessor(){};
    void ups_command_process();
};

#endif
