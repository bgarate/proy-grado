//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_MESSAGEQUEUE_H
#define PROY_GRADO_MESSAGEQUEUE_H


#include <src/proto/message.pb.h>
#include <boost/lockfree/queue.hpp>

class MessageQueue {

public:
    ~MessageQueue();
    void push(Message msg);
    Message pop();
    bool empty();
private:

    static const int QUEUE_CAPACITY = 100;

    typedef boost::lockfree::queue<Message*, boost::lockfree::capacity<QUEUE_CAPACITY>> InternalQueue;

    InternalQueue queue;

};


#endif //PROY_GRADO_MESSAGEQUEUE_H
