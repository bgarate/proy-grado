//
// Created by bruno on 13/05/17.
//

#include "MessageQueue.h"

void MessageQueue::push(Message msg) {

    Message* msgPtr = new Message(msg);

    queue.push(msgPtr);

}

MessageQueue::~MessageQueue() {

    while(!queue.empty()) {
        Message *msg;
        queue.pop(msg);
        delete msg;
    }

}

Message MessageQueue::pop() {
    Message* msg;
    queue.pop(msg);
    Message msgCopy = *msg;

    delete msg;

    return msgCopy;
}

bool MessageQueue::empty() {
    return queue.empty();
}
