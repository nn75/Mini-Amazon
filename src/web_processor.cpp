#include <arpa/inet.h>
#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <thread>

#include "web_processor.h"

using namespace std;
using namespace pqxx;

// Constructor for Communicator
WebProcessor::WebProcessor(message_queue<pair<long int, ACommands> > &s_w_q,
                           message_queue<pair<long int, AUCommands> > &s_u_q,
                           long int &wnum, long int &unum, mutex &mt)
    : send_world_queue(s_w_q),
      send_ups_queue(s_u_q),
      world_seqnum(wnum),
      ups_seqnum(unum),
      mtx(mt) {
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
    // string ups_account = tokens[1];

    cout << "\nTracking number: " << tracking_number;

    // cout << "\nTracking number: " << tracking_number
    //      << "\nUPS account: " << ups_account << endl;

    // 1) Take out pending order and add to order table
    // 2) Send AUCommands to ups to send truck
    // 3) Send ACommands to world to pack product (If the stock is not enough,
    // send ToPurchaseMore)

    // 1.1) Get info from orders_pendingorder
    connection C(
        "dbname = mini_amazon user = postgres password = passw0rd hostaddr = "
        "67.159.95.41 port = 5432");
    if (C.is_open()) {
    } else {
        cout << "ready = Can't open database" << endl;
    }
    string get_pending_order =
        "SELECT * FROM orders_pendingorder WHERE tracking_number = " +
        tracking_number + ";";
    nontransaction N(C);
    result R1(N.exec(get_pending_order));
    long int tracking_number_int = atoi(tracking_number.c_str());
    int user_id;
    string product_name;
    int amount;
    int address_x;
    int address_y;
    string ups_account;
    if (R1.size() == 1) {
        result::const_iterator it1 = R1.begin();
        // tracking number,0;
        // creditcard, 1;
        user_id = it1[2].as<int>();
        product_name = it1[3].as<string>();
        amount = it1[4].as<int>();
        address_x = it1[5].as<int>();
        address_y = it1[6].as<int>();
        ups_account = it1[7].as<string>();
    } else {
        cout << "Use tracking number to get pending order failed\n";
    }

    // 1.2) Decide which warehouse to buy more stock according to dist
    string decide_warehouse = "SELECT * FROM wharehouse;";
    result R2(N.exec(decide_warehouse));
    int min_dist = INT_MAX;
    int closest_wh_id = 0;
    int closest_wh_x = 0;
    int closest_wh_y = 0;
    for (result::const_iterator it2 = R2.begin(); it2 != R2.end(); it2++) {
        int wh_id = it2[0].as<int>();
        int wh_x = it2[1].as<int>();
        int wh_y = it2[2].as<int>();
        int curr_dist = sqrt((address_x - wh_x) * (address_x - wh_x) +
                             (address_y - wh_y) * (address_y - wh_y));
        if (curr_dist < min_dist) {
            min_dist = curr_dist;
            closest_wh_id = wh_id;
            closest_wh_x = wh_x;
            closest_wh_y = wh_y;
        }
    }

    // 1.3) Get the product_id from the closest warehouse
    string get_product_id = "SELECT * FROM orders_product WHERE wh_id = " +
                            to_string(closest_wh_id) +
                            "AND product_name = " + product_name + ";";
    result R3(N.exec(get_product_id));
    if (R3.size() == 1) {
        // There exists such product in this warehouse
        result::const_iterator it3 = R3.begin();
        int product_id = it3[1].as<int>();
        int stock = it3[4].as<int>();

        if (stock - amount <
            100) {  // If the (stock - amount) < 100, send purchase more message
                    // to world, leave order in pending order
            // Add to real order list, set status stocking
            std::string add_stocking_order =
                "INSERT INTO orders_order(tracking_number, user_id, "
                "ups_account, "
                "product_id, wh_id, truck_id,status,adr_x,adr_y) VALUES ( " +
                to_string(tracking_number) + ", " + to_string(user_id) + ", " +
                ups_account + ", " + to_string(product_id) + ", " +
                to_string(closest_wh_id) + ", " + to_string(-1) + ", " +
                "stocking" + ", " + to_string(address_x) + ", " +
                to_string(address_y) + ");";
            work W(C);
            W.exec(add_stocking_order);
            W.commit();
            ////Send purchase more message to world
            int count = 500 + amount;
            ACommands buy;
            APurchaseMore *apm_buy = buy.add_buy();
            apm_buy->set_whnum(closest_wh_id);
            AProduct *pd_buy = apm_buy->add_things();
            pd_buy->set_id(product_id);
            pd_buy->set_description(product_name);
            pd_buy->set_count(amount);
            mtx.lock();  //////lock
            apm_buy->set_seqnum(world_seqnum);
            pair<long int, ACommands> buy_pair(world_seqnum, buy);
            world_seqnum++;
            mtx.unlock();  /////unlock
            send_world_queue.pushback(buy_pair);
        } else if (stock - amount >=
                   100) {  // If stock enough send pack message to world and
                           // send truck message to ups, add it to real order
                           // table
            // Add to real order list, set status packing

            // Send pack message to world
            ACommands topack;
            APack *ap_topack = topack.add_topack();
            ap_topack->set_whnum(closest_wh_id);
            AProduct *pd_topack = ap_topack->add_things();
            pd_topack->set_id(product_id);
            pd_topack->set_description(product_name);
            pd_topack->set_count(amount);
            ap_topack->set_shipid(tracking_number_int);
            mtx.lock();  //////lock
            ap_topack->set_seqnum(world_seqnum);
            pair<long int, ACommands> topack_pair(world_seqnum, topack);
            world_seqnum++;
            mtx.unlock();  /////unlock
            send_world_queue.pushback(topack_pair);

            // Send truck message to ups
            // AUCommands od;
            // Order* ord = od.add_order();
            // ord->set_whid(closest_wh_id);
            // ord->set_x(closest_wh_x);
            // ord->set_y(closest_wh_y);
            // ord->set_packageid(tracking_number_int);
            // ord->set_upsusername(ups_account);
            // Product* pd = ord->add_item();
            // pd->set_id(product_id);
            // pd->set_description(product_name);
            // pd->set_amount(amount);
            // mtx.lock();//////lock
            // ord->set_seqnum(ups_seqnum);
            // pair<long int, AUCommands> order_pair(ups_seqnum, od);
            // ups_seqnum++;
            // mtx.unlock();/////unlock
            // send_ups_queue.pushback(order_pair);
        }
    } else {
        cout << "Use wh_id and product_name to get product failed\n";
    }
}
