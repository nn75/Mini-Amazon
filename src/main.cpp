#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "mqueue.h"
#include "ups_communicator.h"
#include "web_receiver.h"
#include "world_amazon.pb.h"
#include "world_communicator.h"
#include "world_sender.h"

using namespace std;

message_queue<ACommands> w_sender_queue;

// This is only used for test class World
int main(int argc, char *argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Get worldid ups people told us
    if (argc != 2) {
        printf("Syntax Error: ./test <worldid>\n");
        exit(EXIT_FAILURE);
    }
    long worldid_input = atoi(argv[1]);
    cout << "\nworldid_input is:" << worldid_input << endl;

    Warehouse houses[3]{{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};
    WorldCommunicator world_communicator(3, houses);

    // WorldSender w_sender(&world_communicator);

    // Test for web connect
    // WebReceiver web_receiver;
    // web_receiver.connect();
    // web_receiver.disconnect();

    // Test for Ups socket
    // UpsCommunicator ups_communicator(0, NULL);
    // cout << "\nTest ups 1: Connect to vcm-8888.vcm.duke.edu" << endl;
    // ups_communicator.connect("vcm-8888.vcm.duke.edu");

    // Test for create world
    for (int i = 0; i < 5; i++) {
        cout << "\nTest 1: Connect to 127.0.0.1 without worldid" << endl;
        world_communicator.connect("localhost");
        world_communicator.disconnect();
    }

    // Test for connect to worldid told by ups
    // cout << "\nTest 3: Connect to vcm-6873.vm.duke.edu with input worldid" <<
    // endl; world_communicator.connect("67.159.94.99", worldid_input);
    // world_communicator.disconnect();

    // cout << "\nTest 2: Connect to 127.0.0.1 with invalid worldid" << endl;
    // world_communicator.connect("127.0.0.1", 0xdeadbeef);

    google::protobuf::ShutdownProtobufLibrary();
}
