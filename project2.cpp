/* 
 * File:   project2.cpp
 * Author: Andrew
 *
 * Created on May 1, 2016, 8:59 PM
 */

#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include "processes.h"

using namespace std;

int* memoryScope(){
    int* space = (int *) malloc(sizeof(int) * 2000001);
    return space;
}

int checkMemory(int* space, int memorySize){
    int openSize = 0;
    int i=1;
    
    for(; i<20000001 && openSize == memorySize; i++)
        if(space[i] == 0)
            openSize++;
        else
            openSize = 0;
    
    if(openSize == memorySize)
        return i-memorySize+1;
    else
        return 0;
}

int my_malloc(int* space, int memoryReq){
    int holeStart = checkMemory(memoryReq);
    
    if(holeStart){
        for(int i=0; i<memoryReq; i++)
            space[holeStart+i] = 1;
    }
    
    return holeStart;
}

void my_free(int memoryLocale, int memorySize, int* space){
    for(int i=0; i<memorySize; i++)
        space[memoryLocale + i] = 0;
}

void FIFO(vector<processStruct> pVec, int processors = 1){
    vector<processStruct> waitQueue;
    vector<processStruct> executing;
    int waitTime = 0;
    int* space = memoryScope();
     
    while(1){
        //If we need to enqueue a new process
        if(waitTime % 50 == 0 && pVec.size() != 0){
            //If we should put it straight on the processor
            if(executing.size() == 0 && executing.size() < processors){
                processStruct pS = pVec.front(); //get the process
                int holeStart = my_malloc(space, pS.memoryPrint); //unconditional malloc since 0 processes executing
                pS.memoryOffset = holeStart; //assign the holeStart position for faster deletion
                executing.push_back(pS); //push on to executing block
            }
            else //otherwise push on to waitqueue
                waitQueue.push_back(pVec.front());
           
            //Either way we'll delete the processes from pseudo-processes
            pVec.erase(pVec.begin());
        }
        
        //If we're not at process income point, but we need something to process
        if(executing.size() == 0 && waitQueue.size() > 0){
            processStruct pS = waitQueue.front(); //Get the process at front of waiting queue
            pS.memoryOffset = my_malloc(space, pS.memoryPrint); //get the memory start position
            executing.push_back(pS); //push on to executing block
            waitQueue.erase(waitQueue.begin()); //delete from waiting queue
        }
        
        //If we have processes in execution
        if(executing.size() > 0){
            for(int i=0; i<executing.size(); i++)
                executing[i].cycleCount--;
        }
        
        //Check if any process is done
        for(int i=0; i<executing.size(); i++)
            if(executing[i].cycleCount <= 0){
                processStruct pS = executing[i];
                my_free(pS.memoryOffset, pS.memoryPrint, space);
                executing.erase(executing.begin());
            }
        
        waitTime += waitQueue.size();
        
        if(pVec.size() == 0 && executing.size() == 0 && waitQueue.size() == 0)
            break;
    }
    
    printf("Total wait time: %d\n", waitTime);
    waitTime = waitTime / 50.0;
    printf("Average wait time: %d\n", waitTime);
}

int main(int argc, char** argv) {
    clock_t t;
    
    vector<processStruct> pVec = genProcs(50, "Test");
    
    t = clock();
    FIFO(pVec);
    t = clock() - t;
    
    printf("It took %d clicks.\n", t);
    
    return 0;
}

