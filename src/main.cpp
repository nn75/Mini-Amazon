#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "world.h"
#include "ups.h"

using namespace std;

// This is only used for test class World
int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Warehouse houses[3]{{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};

    World world(1, houses);

    cout << "\nTest world 1: Connect to 127.0.0.1 without worldid" << endl;
    world.connect("127.0.0.1");
    world.disconnect();

    cout << "\nTest world 2: Connect to 127.0.0.1 with invalid worldid" << endl;
    world.connect("127.0.0.1", 0xdeadbeef);

    Ups ups;

    cout << "\nTest ups 1: Connect to vcm-8888.vcm.duke.edu" << endl;
    ups.connect("vcm-8888.vcm.duke.edu");

    google::protobuf::ShutdownProtobufLibrary();
}
