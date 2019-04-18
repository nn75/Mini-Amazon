#include "mqueue.h"

int main(){
    message_queue<int> queue;
    queue.pushback(1);
    queue.pushback(2);
    queue.pushback(3);
    int test;
    queue.popfront(test);
    cout<<test<<endl;
    queue.front(test);
    cout<<test<<endl;
    cout<<queue.contain(3)<<endl;

}