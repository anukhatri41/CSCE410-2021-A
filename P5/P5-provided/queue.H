/* Queue Implementation from https://www.geeksforgeeks.org/queue-linked-list-implementation/ */

#include "thread.H"
#include "utils.H"

struct QNode{
    Thread* thread;
    QNode* next;

    QNode(Thread* _thread){
        thread = _thread;
        next = NULL;
    }
};

struct RQueue{
    QNode *front;
    QNode *rear;

    RQueue(){
        front = NULL;
        rear = NULL;
    }

    void enqueue(Thread* t){
        QNode* temp = new QNode(t);
        if(rear == NULL){
            front = rear = temp;
            return;
        }

        rear->next = temp;
        rear = temp;
    }

    // Modified to return thread instead of just deleting it
    Thread* dequeue(){
        if(front == NULL){
            return;
        }

        QNode* dead = front;
        Thread* temp = dead->front;
        front = front->next;

        if(front == NULL){
            rear = NULL;
        }

        delete dead;
        return temp;
    }
};