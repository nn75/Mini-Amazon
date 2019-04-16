#include <arpa/inet.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

#include "communicate.h"
#include "world.h"
#include "world_amazon.pb.h"

using namespace std;

// Constructor for World
World::World(unsigned int n, Warehouse *houses) : port(PORT) {
    sock_fd = INVALID_FD;
    warehouse_number = n;
    warehouses = houses;
    worldid = INVALID_ID;
}

// Check whether world is connected
bool World::is_connect() { return sock_fd != -1; }

long World::get_worldid() { return worldid; }

void World::set_worldid(long id) { worldid = id; }

void World::fail_connect(const char *err_msg) {
    cerr << "\n " << err_msg << " \n" << endl;
    if (sock_fd != INVALID_FD) {
        close(sock_fd);
    }
    sock_fd = INVALID_FD;
    worldid = INVALID_ID;
}

// Connect to target hostname
void World::connect2world(const char *hostname) {
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        this->fail_connect("Socket creation error");
        return;
    }

    memset(&serv_addr, '\x00', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
        this->fail_connect("Invalid address/ Address not supported");
        return;
    }

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
        0) {
        this->fail_connect("Connect failed");
        return;
    }

    // Connect to world with shackhands
    this->setup_world();

    return;
}

bool World::setup_world(void) {
    // Check sock connect statue
    if (!this->is_connect()) {
        return false;
    }

    // New a connect retuest
    AConnect *connect_request = new AConnect;
    connect_request->set_isamazon(true);

    // Add warehouses into request
    for (int i = 0; i < warehouse_number; i++) {
        AInitWarehouse *house_request = connect_request->add_initwh();
        house_request->set_id(warehouses[i].get_id());
        house_request->set_x(warehouses[i].get_x());
        house_request->set_y(warehouses[i].get_y());
    }

    // Sepecific worldid if need
    if (worldid != INVALID_ID) {
        connect_request->set_worldid(worldid);
    }

    // Check whether connect_request is valid
    if (!connect_request->IsInitialized()) {
        this->fail_connect("Unknown Error");
        return false;
    }
#ifdef DEBUG
    cerr << "Send WorldID: " << connect_request->worldid() << endl;
    cerr << "Send Initwh size: " << connect_request->initwh_size() << endl;
    cerr << "Send IsAmazon: " << connect_request->isamazon() << endl;
#endif

    // Send request to server
    if (!sendMesgTo<AConnect>(*connect_request, sock_fd)) {
        this->fail_connect("Cannot send AConnect request");
        return false;
    }

    // Receive respond
    AConnected *connect_response = new AConnected;
    if (!recvMesgFrom(*connect_response, sock_fd)) {
        this->fail_connect("Cannot recv AConnected response");
        return false;
    }

    if (strncmp("connected!", connect_response->result().c_str(),
                strlen("connected!"))) {
        this->fail_connect("Recv failed response");
        return false;
    }

    if (worldid == INVALID_ID) {
        worldid = connect_response->worldid();
    } else {
        if (worldid != connect_response->worldid()) {
            this->fail_connect("Inconstant worldid");
            return false;
        }
    }

#ifdef DEBUG
    cerr << "Recv WorldID: " << worldid << endl;
#endif

    cerr << "\n Connect Success! \n" << endl;

    return true;
}
