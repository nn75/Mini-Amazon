#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include "web_processor.h"

using namespace std;

// Constructor for Communicator
WebProcessor::WebProcessor(message_queue<pair<long int, AUCommands> > &s_u_q,
                           long int &unum, mutex &mt)
    : send_ups_queue(s_u_q), ups_seqnum(unum), mtx(mt) {
    sock_fd = INVALID_FD;
    web_client_fd = INVALID_WEB_FD;
    type = "Web";
}

// Connect and start receiving
bool WebProcessor::connect() {
    // Connect socket with server
    if (!this->setup_sock()) {
        this->fail_connect("Setup web socket failed");
        return false;
    }

    // For testing
    // this->start_recv();

    // For thread
    web_recevier_thread = thread(&WebProcessor::start_recv, this);
    cout << "Start web recevier thread\n";

    return true;
}

void WebProcessor::fail_connect(const char *err_msg) {
    cerr << type << " Err:  " << err_msg << endl;
    this->disconnect();
}

void WebProcessor::disconnect() {
    if (sock_fd != INVALID_FD) {
        close(sock_fd);
    }
    sock_fd = INVALID_FD;
}

// Set up socket with web
bool WebProcessor::setup_sock() {
    int status;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    const char *port = "45678";

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        this->fail_connect("Cannot get address info for Amazon");
        return false;
    }

    sock_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
    if (sock_fd == -1) {
        this->fail_connect("Cannot create web socket");
        return false;
    }

    int yes = 1;
    status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status =
        ::bind(sock_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        this->fail_connect("Cannot bind web socket");
        return false;
    }
    status = listen(sock_fd, 100);
    if (status == -1) {
        this->fail_connect("Cannot listen on web socket");
        return false;
    }
    cout << "WebReceiver listening for connection\n";

    return true;
}

// Start receving from web
void WebProcessor::start_recv() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    while (1) {
        web_client_fd =
            accept(sock_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (web_client_fd != -1) {
            this->get_buy_info();
            web_client_fd = INVALID_WEB_FD;
        } else {
            cout << "One user: Connect Success!" << endl;
        }
    }
}

vector<string> WebProcessor::split(const string &str, const string &sep) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(sep, prev);
        if (pos == string::npos) {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) {
            tokens.push_back(token);
        }
        prev = pos + sep.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

// Parse info received from web
void WebProcessor::get_buy_info() {
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));

    recv(web_client_fd, buffer, 2048, 0);

    string info(buffer);
    vector<string> tokens = split(info, "/");
    string tracking_number = tokens[0];
    string ups_account = tokens[1];

    cout << "\nTracking number: " << tracking_number
         << "\nUPS account: " << ups_account << endl;



    //Insert new order to order table

    
}
