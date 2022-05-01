#include "headers.h"
#include "queue.h" // queue and commons

/* Modify this file as needed*/
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
int runRoundRobin(struct queue * currentQueue, int quantum){ // returns time spent
    if(isEmpty(currentQueue))
        return 0;
    struct processBlock* pr = front(currentQueue);
    int remainingTime;
    if(pr->remainingTime <= 0){
        dequeue(currentQueue); // finished
        return 0;
    }
    if(pr->remainingTime <= quantum)
        remainingTime = pr->remainingTime;
    else
        remainingTime = quantum;
    int pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        char remainingTimePassed[10]; // max int has 10 digits
        sprintf(remainingTimePassed, "%d", remainingTime); // converting the int to string
        char *data[] = {"process.out", remainingTimePassed, NULL};
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
    }
    else{
        dequeue(currentQueue);
        waitTillProcessFinishes(remainingTime);
        pr->remainingTime -= remainingTime;
        if(pr->remainingTime > 0)
            enqueue(currentQueue, pr);
        else
            printf("Process %d finishes...", pr->id);
    }
}


int main(int agrc, char * argv[])
{
    initClk();
    struct processBlock* pr[4];
    for(int i = 0; i<4; i++){
        pr[i] = malloc(sizeof(struct processBlock));
        pr[i]->remainingTime = (i + 1) * 2;
        pr[i]->id = i+1;
    }
    struct queue* currentQueue = newQueue(4);
    int lastClk = getClk();
    int currentClk = lastClk;
    int index = 0;
    enqueue(currentQueue, pr[index]);
    index++;
    while(!isEmpty(currentQueue) || index <= 3){
        currentClk = getClk();
        if(index < 4 && index > 0 && currentClk - 2 >= lastClk){
            enqueue(currentQueue, pr[index]);
            index++;
            lastClk = currentClk;
        }
        runRoundRobin(currentQueue, 3);
    }
    destroyClk(false);

    return 0;
}
