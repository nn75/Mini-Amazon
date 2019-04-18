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
#include "ups.h"
#include "world_amazon.pb.h"

using namespace std;

/////////////////////////////////
/// Public methods start here
/////////////////////////////////

// Constructor for Ups
Ups::Ups() : port(UPS_PORT) {
}

// Check whether ups is connected
bool Ups::is_connect() {
    return (sock_fd != INVALID_FD);
}

void Ups::disconnect() {
    if (sock_fd != INVALID_FD) {
        close(sock_fd);
    }
    sock_fd = INVALID_FD;
}

// Connect to ups
bool Ups::connect(const char *hostname) {
    // Connect socket with server
    if (!this->setup_sock(hostname)) {
        this->fail_connect("Ups: Setup socket failed");
        return false;
    }

    cerr << "Ups: Connect Success!" << endl;

    return true;
}

/////////////////////////////////
/// Private methods start here
/////////////////////////////////

void Ups::fail_connect(const char *err_msg) {
    cerr << "Ups Err:  " << err_msg << endl;
    this->disconnect();
}

bool Ups::setup_sock(const char *hostname) {
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        this->fail_connect("Ups: Socket creation error");
        return false;
    }

    memset(&serv_addr, '\x00', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
        this->fail_connect("Ups: Invalid address/ Address not supported");
        return false;
    }

    // Connect to server
    if (::connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        this->fail_connect("Ups: Connect failed");
        return false;
    }

    return true;
}


