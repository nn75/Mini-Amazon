#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <iostream>
#include <pqxx/pqxx>


#include "amazon_ups.pb.h"
#include "message_queue.h"
#include "world_amazon.pb.h"
#include "amazon_ups.pb.h"

#include "ups_communicator.h"
#include "world_communicator.h"

#include "ups_processor.h"
#include "web_processor.h"
#include "world_processor.h"

using namespace std;
using namespace pqxx;


int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Get worldid ups people told us
    if (argc != 2) {
        printf("Syntax Error: ./test <worldid>\n");
        exit(EXIT_FAILURE);
    }
    long worldid_input = atoi(argv[1]);
    cout << "\nworldid_input is:" << worldid_input << endl;

    // Initialize warehouse and world_communicator
    Warehouse houses[3] = {{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};
    WorldCommunicator* world_communicator = new WorldCommunicator(3, houses);

    // Initialize warehouse and world_communicator
    UpsCommunicator* ups_communicator = new UpsCommunicator(0, NULL);

    // Initialize message queue and multithread
    message_queue<pair<long int, ACommands> > s_w_q;   // Send world queue
    message_queue<AResponses> r_w_q;                   // Receive world queue
    message_queue<pair<long int, AUCommands> > s_u_q;  // Send ups queue
    message_queue<UACommands> r_u_q;                   // Receive ups queue

    long int wnum = 0;
    long int unum = 0;
    mutex mt;

    // Test for world connect and create
    cout << "\nTest 1: Connect to 127.0.0.1 without worldid" << endl;
    world_communicator->connect("localhost");

    // Test for connect to worldid told by ups
    // cout << "\nTest: Connect to vcm-6873.vm.duke.edu with input worldid"<<endl; 
    // world_communicator->connect("67.159.94.99", worldid_input);

    // Test for Ups socket
    // UpsCommunicator ups_communicator(0, NULL);
    cout << "\nTest connect ups: Connect to 67.159.94.99" << endl;
    ups_communicator->connect("67.159.94.99", worldid_input);

    /////////////////////////////////
    /// Testing message start here
    /////////////////////////////////
    // Initialize processors
    WorldProcessor* world_processor = new WorldProcessor(s_w_q, r_w_q, s_u_q, world_communicator, wnum, unum, mt);
    UpsProcessor* ups_processor = new UpsProcessor(s_w_q, r_u_q, s_u_q, ups_communicator, wnum, unum, mt); 
    WebProcessor* web_processor = new WebProcessor(s_w_q, s_u_q, wnum, unum, mt); 

    // Test for web connect
    web_processor->connect();

    // send ACommand to world to prepare product in warehouses
    // AUCommands wh_init_1;
    // AWarehouse * wh1 = wh_init_1.add_whinfo();
    // wh1->set_id(1);
    // wh1->set_x(2);
    // wh1->set_y(3);
    // mt.lock();//////lock
    // wh1->set_seqnum(unum);
    // pair<long int, AUCommands> wh_init_1_pair(unum, wh_init_1);
    // unum++;
    // mt.unlock();/////unlock
    // s_u_q.pushback(wh_init_1_pair);
    // cout << "Send ups warehouse1\n";

    // AUCommands wh_init_2;
    // AWarehouse * wh2 = wh_init_2.add_whinfo();
    // wh2->set_id(2);
    // wh2->set_x(4);
    // wh2->set_y(6);
    // mt.lock();//////lock
    // wh2->set_seqnum(unum);
    // pair<long int, AUCommands> wh_init_2_pair(unum, wh_init_2);
    // unum++;
    // mt.unlock();/////unlock
    // s_u_q.pushback(wh_init_2_pair);
    // cout << "Send ups warehouse2\n";

    // AUCommands wh_init_3;
    // AWarehouse * wh3 = wh_init_3.add_whinfo();
    // wh3->set_id(3);
    // wh3->set_x(6);
    // wh3->set_y(9);
    // mt.lock();//////lock
    // wh3->set_seqnum(unum);
    // pair<long int, AUCommands> wh_init_3_pair(unum, wh_init_3);
    // unum++;
    // mt.unlock();/////unlock
    // s_u_q.pushback(wh_init_3_pair);
    // cout << "Send ups warehouse3\n";

    
    // //Initialize an ACommand to buy more, and push into send world queue
    ACommands pm;
    APurchaseMore* apm = pm.add_buy();
    apm->set_whnum(1);
    AProduct* pd = apm->add_things();
    pd->set_id(1);
    pd->set_description("Apple");
    pd->set_count(512);
    mt.lock();  //////lock
    apm->set_seqnum(wnum);
    pair<long int, ACommands> test1(wnum, pm);
    wnum++;
    mt.unlock();  /////unlock
    s_w_q.pushback(test1);


    // usleep(10000);

    // AResponses sim_other_ack;
    // sim_other_ack.add_acks(1);
    // r_w_q.pushback(sim_other_ack);

    // ACommands pk;
    // APack* apk = pk.add_topack();
    // apk->set_whnum(1);
    // AProduct* pd2 = apk->add_things();
    // pd2->set_id(1);
    // pd2->set_description("Apple");
    // pd2->set_count(20);
    // apk->set_shipid(7);
    // apk->set_seqnum(1);
    // wnum++;
    // pair<long int, ACommands> test2(1, pk);
    // s_w_q.pushback(test2);



    // ACommands ld;
    // APutOnTruck* apt = ld.add_load();
    // apt->set_whnum(1);
    // AProduct* pd = apt->add_things();
    // apt->set_seqnum(2);
    // pd->set_id(1);
    // pd->set_description("test");
    // pd->set_count(2);
    // wnum++;

    // AUCommands od;
    // Order* ord = od.add_order();
    // ord->set_whid(1);
    // ord->set_x(2);
    // ord->set_y(3);
    // ord->set_packageid(1);
    // ord->set_upsusername("Stupid");
    // AProduct* pd = ord->add_item();
    // ord->set_seqnum(0);
    // pd->set_id(1);
    // pd->set_description("test");
    // pd->set_amount(2);
    // unum++;

    // AUCommands dv;
    // Deliver* dvr = dv.add_todeliver();
    // dvr->set_packageid(1);
    // dvr->set_seqnum(1);

    // pair<long int, ACommands> test2(1, pk);
    // pair<long int, ACommands> test3(2, ld);
    // pair<long int, AUCommands> test4(0, od);
    // pair<long int, AUCommands> test5(1, dv);
    // s_w_q.pushback(test2);
    // s_w_q.pushback(test3);
    // s_u_q.pushback(test4);
    // s_u_q.pushback(test5);

    // Keep running
    while (1) {
        ;
    }

    world_communicator->disconnect();
    ups_communicator->disconnect();
    web_processor->disconnect();

    // google::protobuf::ShutdownProtobufLibrary();
}
