#ifndef _WORLD_H
#define _WORLD_H
#endif

#define PORT 23456
#define INVALID_FD -1
#define INVALID_ID -1

#include "warehouse.h"

class World {
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

    // Hand shake with server
    bool setup_world(void);
    // Change state into failed connection
    void fail_connect(const char *err_msg);

   public:
    // Constructor
    World(unsigned int n, Warehouse *houses);
    // Connect to server
    void connect2world(const char *hostname);
    // Check whether world is connect
    bool is_connect();
    // Get worldid
    long get_worldid();
    // Set worldid
    void set_worldid(long id);
};
