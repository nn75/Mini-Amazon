#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <iostream>

#include "world.h"
#include "world_amazon.pb.h"

using namespace std;

// Constructor for World
World::World(unsigned int n, Warehouse *houses) : port(PORT) {
    sock_fd = -1;
    warehouse_number = n;
    warehouses = houses;
    worldid = -1;
}

// Check whether world is connected
bool World::is_connect() { return sock_fd != -1; }

int World::get_worldid() { return worldid; }

void World::set_worldid(int id) { worldid = id; }

// Connect to target hostname
void World::connect2world(const char *hostname) {
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    int fd;

    // Create socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "\n Socket creation error \n" << endl;
        return;
    }

    memset(&serv_addr, '\x00', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
        close(fd);
        cerr << "\nInvalid address/ Address not supported \n" << endl;
        return;
    }

    // Connect to server
    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        close(fd);
        cerr << "\nConnection Failed \n" << endl;
        return;
    }

    // Update sock_fd
    sock_fd = fd;

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
    AConnect connect_request;
    connect_request.set_isamazon(true);

    // Add warehouses into request
    for (int i = 0; i < warehouse_number; i++) {
        AInitWarehouse *house_request = connect_request.add_initwh();
        house_request->set_id(warehouses[i].get_id());
        house_request->set_x(warehouses[i].get_x());
        house_request->set_y(warehouses[i].get_y());
    }

    // Sepecific worldid if need
    if (worldid != -1) {
        connect_request.set_worldid(worldid);
    }

    // Check connect request
    if (!connect_request.IsInitialized()) {
        cerr << "\n Unknown Error\n" << endl;
        close(sock_fd);
        sock_fd = -1;
        return false;
    }

    // Send request to server
    string *out_string = new string;
    if (!connect_request.SerializeToString(out_string)) {
        cerr << "\n Cannot SerializeToString \n" << endl;
        close(sock_fd);
        sock_fd = -1;
        return false;
    }
    send(sock_fd, out_string->c_str(), out_string->length(), 0);

    // Receive respond
    char *in_cstr = new char[0x1000];
    memset(in_cstr, '\x00', 0x1000);
    recv(sock_fd, in_cstr, 0x1000, 0);
    const string in_string = string(in_cstr);
    cerr << in_string.length() << endl;
    AConnected connect_respond;
    if (!connect_respond.ParseFromString(in_string)) {
        cerr << "\n Cannot ParserFromRecving \n" << endl;
        close(sock_fd);
        sock_fd = -1;
        return false;
    }

    cerr << "\n Connect Success! \n" << endl;

    return true;
}

// This is only used for test class World
int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Warehouse houses[3]{{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};

    World world(3, houses);
    world.connect2world("127.0.0.1");

    google::protobuf::ShutdownProtobufLibrary();
}
