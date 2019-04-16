#ifndef _COMMUNICATE_H
#define _COMMUNICATE_H
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

// this is adpated from code that a Google engineer posted online
template <typename T>
bool sendMesgTo(const T& message, int out_fd) {
    google::protobuf::io::FileOutputStream out(out_fd);
    {  // extra scope: make output go away before out->Flush()
        // We create a new coded stream for each message.
        // Don’t worry, this is fast.
        google::protobuf::io::CodedOutputStream output(
            &out);  // Write the size.
        const int size = message.ByteSize();
        output.WriteVarint32(size);
        uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
        if (buffer != NULL) {
            // Optimization: The message fits in one buffer, so use the faster
            // direct-to-array serialization path.
            message.SerializeWithCachedSizesToArray(buffer);
        } else {
            // Slightly-slower path when the message is multiple buffers.
            message.SerializeWithCachedSizes(&output);
            if (output.HadError()) {
                return false;
            }
        }
    }
    out.Flush();
    return true;
}

// this is adpated from code that a Google engineer posted online
template <typename T>
bool recvMesgFrom(T& message, int in_fd) {
    google::protobuf::io::FileInputStream in(in_fd);
    google::protobuf::io::CodedInputStream input(&in);
    uint32_t size;
    if (!input.ReadVarint32(&size)) {
        return false;
    }
    // Tell the stream not to read beyond that size.
    google::protobuf::io::CodedInputStream::Limit limit =
        input.PushLimit(size);  // Parse the message.
    if (!message.MergeFromCodedStream(&input)) {
        return false;
    }
    if (!input.ConsumedEntireMessage()) {
        return false;
    }
    // Release the limit.
    input.PopLimit(limit);
    return true;
}
