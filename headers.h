#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include "priority_queue.h"


#define SHKEY 300
struct process
{
    long header;
    int id;
    int arvT;
    int rnT;
    int Prty;
    int remT;
} process;


///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}


void waitTillProcessFinishes(int remainingtime){
    int lastClk = getClk();
    int currentClk = lastClk;
    while (remainingtime > 0)
    {
        currentClk = getClk();
        if(currentClk != lastClk){
            remainingtime -= (currentClk - lastClk);
        }
        lastClk = currentClk;
    }
}


//
// Created by mohamed on 30/04/22.
//



struct queue
{
    struct processEntry *items;     // array to store queue elements
    int maxqueueSize;    // maximum capacity of the queue
    int front;      // front points to the front element in the queue (if any)
    int rear;       // rear points to the last element in the queue
    int queueSize;       // current capacity of the queue
};

struct queue* newQueue(int qs)
{
    struct queue *pt = NULL;
    pt = (struct queue*)malloc(sizeof(struct queue));

    pt->items = (struct processEntry*)malloc( qs * sizeof(struct processEntry));
    pt->maxqueueSize = qs;
    pt->front = 0;
    pt->rear = -1;
    pt->queueSize = 0;

    return pt;
}

int sizeOfQueue(struct queue *pt) {
    return pt->queueSize;
}

int isEmpty(struct queue *pt) {
    return !sizeOfQueue(pt);
}

struct processEntry * front(struct queue *pt)
{
    return &pt->items[pt->front];
}

void enqueue(struct queue *pt, struct processEntry* x)
{
    if (sizeOfQueue(pt) == pt->maxqueueSize)
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    pt->rear = (pt->rear + 1) % pt->maxqueueSize;    // circular queue
    pt->items[pt->rear] = *x;
    pt->queueSize++;

    printf("front = %d, rear = %d\n", pt->front, pt->rear);
}

void dequeue(struct queue *pt)
{
    if (isEmpty(pt))    // front == rear
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    pt->front = (pt->front + 1) % pt->maxqueueSize;  // circular queue
    pt->queueSize--;

    printf("front = %d, rear = %d\n", pt->front, pt->rear);
}