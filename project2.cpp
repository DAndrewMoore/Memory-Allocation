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
    int* space = (int *) malloc(sizeof(int) * (20) *1000000);
    return space;
}

int checkMemory(int memory){
    
}

int my_malloc(int memoryReq){
    if(checkMemory(memoryReq)){
        
    }
}

void my_free(int* memoryLocale, int memorySize){
    for(int i=0; i<memorySize; i++)
        memoryLocale[i] = 0;
}

void FIFO(vector<processStruct> pVec, int processors = 1){
    vector<processStruct> waitQueue;
    vector<processStruct> executing;
    int waitTime = 0;
     
    while(1){
        //If we need to enqueue a new process
        if(waitTime % 50 == 0 && pVec.size() != 0){
            //If we should put it straight on the processor
            if(executing.size() == 0 && executing.size() < processors)
                executing.push_back(pVec.front());
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

