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
                processStruct pS = pVec.front();
                int holeStart = my_malloc(space, pS.memoryPrint);
                pS.memoryOffset(holeStart);
                executing.push_back(pS);
            }
            else
                waitQueue.push_back(pVec.front());
           
            //Either way we'll delete the processes from pseudo-processes
            pVec.erase(pVec.begin());
        }
        
        //If we're not at process income point, but we need something to process
        if(executing.size() == 0 && checkMemory(pVec.front().memoryPrint)){
            executing.push_back(pVec.front());
            pVec.erase(pVec.begin());
        }
        
        //If we have processes in execution
        if(executing.size() > 0){
            for(int i=0; i<executing.size(); i++)
                executing[i].cycleCount--;
        }
        
        //Check if any process is done
        for(int i=0; i<executing.size(); i++)
            if(executing[i].cycleCount <= 0)
                executing.erase(executing.begin());
        
        waitTime += waitQueue.size();
        
        if(pVec.size() == 0 && executing.size() == 0)
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

