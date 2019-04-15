#ifndef _WORLD_H
#define _WORLD_H
#endif

#define PORT 23456

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
    int worldid;

    // Functions
    bool setup_world(void);

   public:
    // Functions
    World(unsigned int n, Warehouse *houses);
    void connect2world(const char *hostname);
    bool is_connect();
    int get_worldid();
    void set_worldid(int id);
};
