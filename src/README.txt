To make the file：

make -f Makefile.nn
make -f Makefile.nn debug

To run：

LD_PRELOAD=/home/nn75/protobuf-3.7.1/lib/libprotobuf.so.18 ./test
LD_PRELOAD=/home/nn75/protobuf-3.7.1/lib/libprotobuf.so.18 ./debug