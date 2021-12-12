#ifndef QUEUE
#define QUEUE

#include<iostream>
#include<iomanip>
#include <cstdlib>
#include <time.h>
#include <math.h>

using namespace std;

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;
} Queue ;

Queue * createQueue(int maxElements)
{

        //Create a Queue
        Queue *Q;
        Q = (Queue*)malloc(sizeof(Queue));

        //Initialise its properties
        Q->elements = (int *)malloc(sizeof(int)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = -1;
        Q->rear = 0;
        return Q;
}

void printQ(Queue * Q)
{
    cout << "Queue:  ";
    for (int i = 0; i < Q->size; i++)
    {
        cout << Q->elements[i] << "  ";
    }
    cout << endl;
}

void Enqueue(Queue *Q,int element)
{
        //If the Queue is full, we cannot push an element into it as there is no space for it.
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                for (int i = Q->size; i > 0; i--) {
                    Q->elements[i] = Q->elements[i - 1];
                }
                Q->size++;
                Q->front = Q->front + 1;
                Q->elements[Q->rear] = element;
        }
        return;
}

int  Dequeue(Queue *Q)
{
    int  tempElem = -1;
    int element;

    //If Queue size is zero then it is empty. So we cannot pop
    if(Q->size==0) {
        cout << "Queue is Empty" << endl;
        return tempElem;
        }
    else {
        element = Q->elements[Q->front];
        Q->size--;
        Q->front--;
        return element;
    }
}

#endif

