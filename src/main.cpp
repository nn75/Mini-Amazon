#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "ups_communicator.h"
#include "world_communicator.h"

using namespace std;

// This is only used for test class World
int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Warehouse houses[3]{{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};

    WorldCommunicator world_communicator(3, houses);

    cout << "\nTest 1: Connect to 127.0.0.1 without worldid" << endl;
    world_communicator.connect("127.0.0.1");
    world_communicator.disconnect();

    cout << "\nTest 2: Connect to 127.0.0.1 with invalid worldid" << endl;
    world_communicator.connect("127.0.0.1", 0xdeadbeef);

    UpsCommunicator ups_communicator(0, NULL);

    cout << "\nTest ups 1: Connect to vcm-8888.vcm.duke.edu" << endl;
    ups_communicator.connect("vcm-8888.vcm.duke.edu");

    google::protobuf::ShutdownProtobufLibrary();
}
