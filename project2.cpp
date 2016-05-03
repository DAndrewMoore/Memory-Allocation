/* 
 * File:   project2.cpp
 * Author: Andrew
 *
 * Created on May 1, 2016, 8:59 PM
 */

#include <ctime>
#include <vector>
#include <string>
#include "processes.h"
#include "manager.h"

using namespace std;

void FIFO(int* space, vector<processStruct> pVec, int maxMem = 20000001, int processors = 64){
    printf("Maximum Memory Available: %d\n", maxMem);
    printf("Maximum Processors Available: %d\n", processors);
    vector<processStruct> waitQueue;
    vector<processStruct> executing;
    clock_t t;
    clock_t total_malloc_time = 0;
    clock_t total_free_time = 0;
    int waitTime = 0;
    int counter = 0;
    int checkMem = 1;
    
    //printf("Enter the while(d)\n");
    while(1){
        //If we need to enqueue a new process
        if(counter % 50 == 0 && pVec.size() != 0){
            //If we should put it straight on the processor
            if(executing.size() < processors){
                //If we have more than one processor in execution
                if(executing.size() != 0){
                    processStruct pS = pVec.front(); //Get the process
                    t = clock();
                    int holeStart = checkMemory(space, pS.memoryPrint, maxMem); //Check if we can add it to the execution list
                    t = clock() - t;
                    total_malloc_time += t;
                    //If we can add it to the execution list
                    if(holeStart > 0){
                        pS.memoryOffset = holeStart; //set the offset
                        t = clock();
                        my_malloc(space, pS.memoryPrint, holeStart, maxMem); //assign the space
                        t = clock() - t;
                        total_malloc_time += t;
                        executing.push_back(pS); //add it to execution list
                    } else {
                        waitQueue.push_back(pS); //If we can't, then put it on the waitQueue
                    }
                } else { //If we have no processors in use
                    processStruct pS = pVec.front(); //get the process
                    t = clock();
                    int holeStart = my_malloc(space, pS.memoryPrint, maxMem); //unconditional malloc since 0 processes executing
                    t = clock() - t;
                    total_malloc_time += t;
                    pS.memoryOffset = holeStart; //assign the holeStart position for faster deletion
                    executing.push_back(pS); //push on to executing block
                }
            }
            else //otherwise push on to waitqueue
                waitQueue.push_back(pVec.front());
           
            //Either way we'll delete the processes from pseudo-processes
            pVec.erase(pVec.begin());
        }
        
        //If we're not at process income point, but we need something to process
        if(checkMem && executing.size() < processors && waitQueue.size() > 0){
            //printf("Checking if we can add a process\n");
            processStruct pS = waitQueue.front(); //Get the process at front of waiting queue
            t = clock();
            int holeStart = checkMemory(space, pS.memoryPrint, maxMem); //Check if we have enough memory available
            t = clock() - t;
            total_malloc_time += t;
            
            //If we have enough memory
            if(holeStart > 0){
                //printf("We can add a process to the executioner\n");
                pS.memoryOffset = holeStart; //Set the memory start position
                t = clock();
                my_malloc(space, pS.memoryPrint, holeStart, maxMem); //Assign the space
                t = clock() - t;
                total_malloc_time += t;
                executing.push_back(pS); //push on to executing block
                waitQueue.erase(waitQueue.begin()); //delete from waiting queue
            } else { //If we don't have enough memory
                checkMem = 0; //Don't check again until this bit is set (like if a process finishes execution)
            }
        }
        
        //If we have processes in execution
        if(executing.size() > 0){
            for(int i=0; i<executing.size(); i++)
                executing[i].cycleCount--;
        }
        
        //Check if any process is done
        for(int i=0; i<executing.size(); i++)
            if(executing[i].cycleCount <= 0){
                processStruct pS = executing[i]; //get the process info
                t = clock();
                my_free(space, pS.memoryPrint, pS.memoryOffset); //free the space
                t = clock() - t;
                total_free_time += t;
                executing.erase(executing.begin()+i); //erase the process
                i = i-1;
                checkMem = 1; //initiate a check if waitQueue processes are waiting for memory
                //printf("Process unloaded\n");
            }
        
        //Increase waitTime by amount of processes waiting
        waitTime += waitQueue.size();
        
        counter++;
        
        if(pVec.size() == 0 && executing.size() == 0 && waitQueue.size() == 0)
            break;
    }
    printf("Total count was: %d\n", counter);
    printf("Average count per process: %f\n", (counter / 64.0));
    printf("Total time taken: %f\n", ((float)total_malloc_time + total_free_time)/CLOCKS_PER_SEC);
    printf("Total malloc time: %f\n", ((float)total_malloc_time)/CLOCKS_PER_SEC);
    printf("Total freeing time: %f\n", ((float)total_free_time)/CLOCKS_PER_SEC);
}

int main(int argc, char** argv) {
    clock_t t;
    
    //Initiate process vector
    vector<processStruct> pVec = genProcs(64, "Test");
    
    double memories [3] = {1.0, 0.5, 0.1};
    
    int maxMem = 20000001;
    
    //Get the total Mem requirement
    double totalMem = 0;
    for(int i=0; i<pVec.size(); i++)
        totalMem += pVec[i].memoryPrint;
    
    
    for(double memory: memories){
        if(memory == 1.0){
            t = clock();
            int* space = memoryScope(maxMem);
            FIFO(space, pVec, maxMem);
            t = clock() - t;
            printf("It took %d clicks.\n\n", t);
        } else {
            t = clock();
            int* space = memoryScope((int) (totalMem * memory) +1);
            FIFO(space, pVec, (int) (totalMem * memory) +1);
            t = clock() - t;
            printf("It took %d clicks.\n\n", t);
        }
        pVec = genProcs(64, "Test");
    }
    
//    int maxNeeded = 0;
//    int maxPos = 0;
//    for(int i=0; i<pVec.size(); i++)
//        if(pVec[i].memoryPrint > maxNeeded){
//            maxNeeded = pVec[i].memoryPrint;
//            maxPos = i;
//        }
//    
//    t = clock();
//    int* space = memoryScope(maxNeeded);
//    FIFO(space, pVec, maxNeeded);
//    t = clock() - t;
//    printf("It took %d clicks.\n", t);
    
    return 0;
}

