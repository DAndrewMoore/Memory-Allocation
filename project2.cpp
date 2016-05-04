/* 
 * File:   project2.cpp
 * Author: Andrew
 *
 * Created on May 1, 2016, 8:59 PM
 */

#include <ctime>
#include <vector>
#include <string>
//#include <iofstream>
#include <fstream>
#include "processes.h"
#include "manager.h"

using namespace std;

void trivialFIFO(vector<processStruct> pVec, std::ofstream& f, int maxMem = 20000001, int processors = 64){
    printf("Maximum Memory Available: %d\n", maxMem);
    printf("Maximum Processors Available: %d\n", processors);
    vector<processStruct> waitQueue;
    vector<processStruct> executing;
    clock_t t;
    clock_t total_malloc_time;
    clock_t total_free_time;
    int waitTime = 0;
    int counter = 0;
    int rem_mem = maxMem;
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
                    if(pS.memoryPrint < rem_mem){
                        t = clock();
                        pS.memorySpot = (int *) malloc(sizeof(int) * pS.memoryPrint);
                        t = clock() - t;
                        total_malloc_time += t;
                        int mem_to_alloc = pS.memoryPrint;
                        rem_mem -= mem_to_alloc;
                    } else {
                        waitQueue.push_back(pS);
                    }
                } else { //If we have no processors in use
                    processStruct pS = pVec.front(); //get the process
                    t = clock();
                    pS.memorySpot = (int *) malloc(sizeof(int) * pS.memoryPrint);
                    t = clock() - t;
                    total_malloc_time += t;
                    int mem_to_alloc = pS.memoryPrint;
                    rem_mem -= mem_to_alloc;
                    executing.push_back(pS); //push on to executing block
                }
            }
            else //otherwise push on to waitqueue
                waitQueue.push_back(pVec.front());
           
            //Either way we'll delete the processes from pseudo-processes
            pVec.erase(pVec.begin());
        }
        
        //If we're not at process income point, but we need something to process
        if(executing.size() <= processors && waitQueue.size() > 0){
            //printf("Checking if we can add a process\n");
            processStruct pS = waitQueue.front(); //Get the process at front of waiting queue
            if(pS.memoryPrint < rem_mem){
                t = clock();
                pS.memorySpot = (int *) malloc(sizeof(int) * pS.memoryPrint);
                t = clock() - t;
                total_malloc_time += t;
                int mem_to_alloc = pS.memoryPrint;
                rem_mem -= mem_to_alloc;
                executing.push_back(pS);
                waitQueue.erase(waitQueue.begin());
            } else {
                checkMem = 0;
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
                int mem_to_alloc = pS.memoryPrint;
                rem_mem += mem_to_alloc;
                t = clock();
                free(executing[i].memorySpot);
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
        
        if(pVec.empty() && executing.empty()&& waitQueue.empty())
            break;
        else if(pVec.empty() && !waitQueue.empty() && executing.empty()){
            rem_mem = maxMem;
            for(int i=0; i<waitQueue.size(); i++){
                if(waitQueue[i].entranceTime <= counter && waitQueue[i].memoryPrint < rem_mem){
                    processStruct pS = waitQueue[i];
                    t = clock();
                    pS.memorySpot = (int *) malloc(sizeof(int) * pS.memoryPrint);
                    t = clock() - t;
                    rem_mem -= pS.memoryPrint;
                    total_malloc_time += t;
                    executing.push_back(pS);
                    waitQueue.erase(waitQueue.begin() + i);
                    i = i - 1;
                }
            }
        }
    }
    printf("\n============== Trivial Manager ==============\n");
    printf("Total count was: %d\n", counter);
    printf("Average count per process: %f\n", (counter / 64.0));
    printf("Total time taken: %f\n", ((float)total_malloc_time + total_free_time)/CLOCKS_PER_SEC);
    f << (((float)total_malloc_time + total_free_time) / CLOCKS_PER_SEC) << ",";
    printf("Total malloc time: %f\n", ((float)total_malloc_time)/CLOCKS_PER_SEC);
    f << (((float)total_malloc_time)/CLOCKS_PER_SEC) << ",";
    printf("Total freeing time: %f\n", ((float)total_free_time)/CLOCKS_PER_SEC);
    f << (((float)total_free_time)/CLOCKS_PER_SEC) << "\n";
    
}

void FIFO(int* space, vector<processStruct> pVec, std::ofstream& f, int maxMem = 20000001, int processors = 64){
    printf("Maximum Memory Available: %d\n", maxMem);
    printf("Maximum Processors Available: %d\n", processors);
    vector<processStruct> waitQueue;
    vector<processStruct> executing;
    clock_t t;
    clock_t total_malloc_time;
    clock_t total_free_time;
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
                        my_malloc(space, pS.memoryPrint, holeStart, maxMem, pS.pid); //assign the space
                        t = clock() - t;
                        total_malloc_time += t;
                        executing.push_back(pS); //add it to execution list
                    } else {
                        waitQueue.push_back(pS); //If we can't, then put it on the waitQueue
                    }
                } else { //If we have no processors in use
                    processStruct pS = pVec.front(); //get the process
                    t = clock();
                    int holeStart = my_malloc(space, pS.memoryPrint, maxMem, pS.pid); //unconditional malloc since 0 processes executing
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
                my_malloc(space, pS.memoryPrint, holeStart, maxMem, pS.pid); //Assign the space
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
    printf("\n============== Custom Manager ==============\n");
    printf("Total count was: %d\n", counter);
    printf("Average count per process: %f\n", (counter / 64.0));
    printf("Total time taken: %f\n", ((float)total_malloc_time + total_free_time)/CLOCKS_PER_SEC);
    f << (((float)total_malloc_time + total_free_time) / CLOCKS_PER_SEC) << ",";
    printf("Total malloc time: %f\n", ((float)total_malloc_time)/CLOCKS_PER_SEC);
    f << (((float)total_malloc_time)/CLOCKS_PER_SEC) << ",";
    printf("Total freeing time: %f\n", ((float)total_free_time)/CLOCKS_PER_SEC);
    f << (((float)total_free_time)/CLOCKS_PER_SEC) << "\n";
}

int main(int argc, char** argv) {
    clock_t t;
    
    
    string seeds[10] = {"seeds", "memories", "not creative", "halp", "GNS3",
                        "James Bond", "ITSS", "Lib-TACO", "who you gonna call",
                        "x3024"};
    
    double memories [3] = {1.0, 0.5, 0.1};
    
    int maxMem = 20000001;
    
    //Get the total Mem requirement
    printf("Total_Time, Total_malloc_time, Total_free_time\n");
    for(string seed: seeds){
        std::ofstream f;
        f.open(".\\seeds\\"+seed+".csv");
        f << "Total_Time,Total_malloc_time,Total_free_time\n";
        //Initiate process vector
        vector<processStruct> pVec = genProcs(64, seed);
        //printf("For seed: %s\n", seed.c_str());
        for(int i=0; i<2; i++){
            for(double memory: memories){
                if(memory == 1.0){
                    t = clock();
                    if(i==1){
                        int* space = memoryScope(maxMem);
                        //printf("Starting scheduler for my_management\n");
                        FIFO(space, pVec, f, maxMem);
                    }
                    else{
                        //printf("Starting scheduler for trivial manager\n");
                        trivialFIFO(pVec, f, maxMem);
                    }
                    t = clock() - t;
                    //printf("It took %d clicks.\n\n", t);
                } else {
                    double totalMem = 0;
                    for(int i=0; i<pVec.size(); i++)
                        totalMem += pVec[i].memoryPrint;
                    t = clock();
                    if(i==1){
                        int* space = memoryScope((int) (totalMem * memory) +1);
                        //printf("Starting scheduler for my_management altered memory size\n");
                        FIFO(space, pVec, f, (int) (totalMem * memory) +1);
                    }
                    else{
                        //printf("Starting scheduler for trivial manager altered memory size\n");
                        trivialFIFO(pVec, f, (int) (totalMem * memory) +1);
                    }
                    t = clock() - t;
                    //printf("It took %d clicks.\n\n", t);
                }
                pVec = genProcs(64, seed);
            }
        }
        printf("\n\n");
    }
    
    return 0;
}

