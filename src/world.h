#ifndef _WORLD_H
#define _WORLD_H
#endif

#define PORT 23456
#define INVALID_FD -1
#define INVALID_ID -1

#include "warehouse.h"

class World {
    /////////////////////////////////
    /// Private members start here
    /////////////////////////////////
   private:
    // World's port number
    const int port;
    // Connected sock_fd, -1 means failed
    int sock_fd;
    // The number of warehouses
    unsigned int warehouse_number;
    Warehouse *warehouses;
    // world id, for test
    long worldid;

    // Connect sock with server
    bool setup_sock(const char *hostname);
    // Hand shake with server
    bool setup_world(long id);
    // Change state into failed connection
    void fail_connect(const char *err_msg);

    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    World(unsigned int n, Warehouse *houses);
    // Disconnect from server
    void disconnect();
    // Connect to server
    bool connect(const char *hostname, long id = INVALID_ID);
    // Check whether world is connect
    bool is_connect();
    // Get worldid
    long get_worldid();

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
