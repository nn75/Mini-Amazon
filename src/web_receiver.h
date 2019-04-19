#ifndef _WEBRECEIVER_H
#define _WEBRECEIVER_H

#define INVALID_FD -1
#define INVALID_WEB_FD -1

class WebReceiver {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    int sock_fd;

    int web_client_fd;
    // communicator type
    const char *type;
    // Connect sock with web client
    bool setup_sock();
    // Change state into failed connection
    void fail_connect(const char *err_msg);

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    WebReceiver();
    // Disconnect from web
    void disconnect();
    // Connect web
    bool connect();
    // Start receving from web
    void start_recv();
    // Parse info received from web
    void get_buy_info();
};

#endif
