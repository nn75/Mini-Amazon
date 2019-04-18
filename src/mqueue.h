#ifndef __M_QUEUE__
#define __M_QUEUE__

#include <deque>
#include <mutex>
#include <iostream>



using namespace std;

template<class T>

class message_queue{
    /////////////////////////////////
    /// Private methods start here
    /////////////////////////////////
  private:
    deque<T> dq;
    mutable mutex m;
    //The postion of 
    int pos;
    /////////////////////////////////
    /// Public members start here
    /////////////////////////////////
  public:
    // Constructor
    message_queue(): dq(), m(), pos(0){};
    // push to back of queue
    void pushback(T value);
    // pop front and then pushback
    bool popfront(T& value);
    //Check existence of message and its position 
    bool contain(T value);
    //Remove acked message
    bool front(T& value);
    //Destructor
    ~message_queue(){};
};
template<class T>
void message_queue<T>::pushback(T value){
    lock_guard<mutex> lock(m);
    dq.push_back(value);    
}
template<class T>
bool message_queue<T>::popfront(T & value){
    lock_guard<mutex> lock(m);
    if(dq.empty()){
        return false;
        cout << "message queue is emtpy" << endl;
    }else{
        value = dq.front();
        dq.pop_front();
        return true;
    }
}
template<class T>
bool message_queue<T>::contain(T value){
    lock_guard<mutex> lock(m);
    for(int i = 0 ; i < dq.size(); i++) {
        if(dq[i] == value){
            pos = i;
            return true;
        }
    }
    return false;
}
template<class T>
bool message_queue<T>::front(T& value){
    lock_guard<mutex> lock(m);
    if(dq.empty()){
        return false;
        cout << "message queue is emtpy" << endl;
    }else{
        value = dq.front();
        return true;
    }
}



#endif
