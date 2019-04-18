
#ifndef _UPS_H
#define _UPS_H

#define UPS_PORT 34567
#define INVALID_FD -1


class Ups {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // Ups's port number
    const int port;
    // Connected sock_fd, -1 means failed
    int sock_fd;

    // Connect sock with server
    bool setup_sock(const char *hostname);
    // Change state into failed connection
    void fail_connect(const char *err_msg);

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    Ups();
    // Disconnect from server
    void disconnect();
    // Connect to server
    bool connect(const char *hostname);
    // Check whether world is connect
    bool is_connect();

    // Method for send protobuf message
    template <typename T>
    bool send_msg(const T &message) {
        if (!this->is_connect()) {
            return false;
        }
        return sendMesgTo(message, sock_fd);
    }

    // Method for recv protobuf message
    template <typename T>
    bool recv_msg(T &message) {
        if (!this->is_connect()) {
            return false;
        }
        return recvMesgFrom(message, sock_fd);
    }
};

#endif