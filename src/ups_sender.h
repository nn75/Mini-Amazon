#ifndef _WEBRECEIVER_H
#define _WEBRECEIVER_H


class UpsReceiver {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    //For send and receive from ups
    UpsCommunicator* ups_communicator;

    //std::thread thread;

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    UpsReceiver();
    //Start receving from web
    void start_recv_ups();

};

#endif
