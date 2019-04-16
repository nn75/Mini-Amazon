#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "world.h"

using namespace std;

// This is only used for test class World
int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Warehouse houses[3]{{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};

    World world(0, houses);
    world.connect2world("127.0.0.1");

    google::protobuf::ShutdownProtobufLibrary();
}
