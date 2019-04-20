#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "amazon_ups.pb.h"
#include "message_queue.h"
#include "world_amazon.pb.h"

#include "ups_communicator.h"
#include "world_communicator.h"

#include "web_processor.h"
#include "world_processor.h"

using namespace std;

// This is only used for test class World
int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Get worldid ups people told us
    if (argc != 2) {
        printf("Syntax Error: ./test <worldid>\n");
        exit(EXIT_FAILURE);
    }
    long worldid_input = atoi(argv[1]);
    cout << "\nworldid_input is:" << worldid_input << endl;

    // Initialize
    Warehouse houses[3] = {{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};
    WorldCommunicator* world_communicator = new WorldCommunicator(3, houses);

    // Test for create world
    // for (int i = 0; i < 5; i++) {
    cout << "\nTest 1: Connect to 127.0.0.1 without worldid" << endl;
    world_communicator->connect("localhost");
    // world_communicator->disconnect();
    //}

    // Initialize message queue and multithread
    message_queue<pair<long int, ACommands> > s_w_q;   // Send world queue
    message_queue<AResponses> r_w_q;                   // Receive world queue
    message_queue<pair<long int, AUCommands> > s_u_q;  // Send ups queue
    long int wnum = 0;
    long int unum = 0;
    mutex mt;

    // Test for web connect
    WebProcessor* web_processor = new WebProcessor(s_u_q, unum, mt);
    web_processor->connect();

    // Test for Ups socket
    // UpsCommunicator ups_communicator(0, NULL);
    // cout << "\nTest ups 1: Connect to vcm-8888.vcm.duke.edu" << endl;
    // ups_communicator.connect("vcm-8888.vcm.duke.edu");

    // Test for connect to worldid told by ups
    // cout << "\nTest 3: Connect to vcm-6873.vm.duke.edu with input worldid" <<
    // endl; world_communicator.connect("67.159.94.99", worldid_input);
    // world_communicator.disconnect();

    // cout << "\nTest 2: Connect to 127.0.0.1 with invalid worldid" << endl;
    // world_communicator.connect("127.0.0.1", 0xdeadbeef);

    // New a connect retuest
    ACommands pm;

    // Initialize an ACommand to buy more, and push into send world queue
    APurchaseMore* apm = pm.add_buy();
    apm->set_whnum(1);
    AProduct* pd = apm->add_things();
    apm->set_seqnum(0);
    pd->set_id(1);
    pd->set_description("test");
    pd->set_count(100);
    wnum++;

    pair<long int, ACommands> test1(0, pm);
    s_w_q.pushback(test1);

    WorldProcessor* world_processor = new WorldProcessor(
        s_w_q, r_w_q, s_u_q, world_communicator, wnum, unum, mt);

    // Keep running
    while (1) {
        ;
    }

    // world_communicator->disconnect();
    // web_processor->disconnect();

    // google::protobuf::ShutdownProtobufLibrary();
}
