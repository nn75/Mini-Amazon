#ifndef __M_QUEUE__
#define __M_QUEUE__

#include <deque>
#include <iostream>
#include <mutex>

using namespace std;

template <class T>
class message_queue {
    /////////////////////////////////
    /// Private methods start here
    /////////////////////////////////
   private:
    deque<T> dq;
    mutable mutex m;
    // The postion of message
    int pos;
    // The size of message_queue
    int dq_size;
    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
   public:
    // Constructor
    message_queue() : dq(), m(), pos(0), dq_size(0){};
    // Check existence of message and its position
    bool contain(T value);
    // Check if message_queue is emoty
    bool whether_empty();
    // push to back of queue
    void pushback(T value);
    // pop front and then pushback
    T popfront();
    // Remove acked message
    T front();
    // Destructor
    ~message_queue(){};
};

template <class T>
bool message_queue<T>::contain(T value) {
    lock_guard<mutex> lock(m);
    for (int i = 0; i < dq.size(); i++) {
        if (dq[i] == value) {
            pos = i;
            return true;
        }
    }
    return false;
}

template <class T>
bool message_queue<T>::whether_empty() {
    lock_guard<mutex> lock(m);
    return (dq_size != 0);
}

template <class T>
void message_queue<T>::pushback(T value) {
    lock_guard<mutex> lock(m);
    dq.push_back(value);
    dq_size++;
}

template <class T>
T message_queue<T>::popfront() {
    lock_guard<mutex> lock(m);
    T deque_head = dq.front();
    dq.pop();
    dq_size--;
    return deque_head;
}

template <class T>
T message_queue<T>::front() {
    lock_guard<mutex> lock(m);
    return dq.front();
}

#endif
