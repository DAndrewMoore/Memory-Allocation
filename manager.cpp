#include <cstdlib>

#ifndef TIME_H
#include <time.h>
#endif

#include "manager.h"

using namespace std;

int* memoryScope(int maxMem){
    //printf("Memory scoped\n");
    int* space = (int *) malloc(sizeof(int) * maxMem);
    
    for(int i=0; i<maxMem; i++)
        space[i] = 0;
    
    return space;
}

int checkMemory(int *space, int memorySize, int maxMem){
    //printf("Memory check\n");
    int openSize = 0;
    int i=1;
    
    for(; i<maxMem && openSize != memorySize; i++)
        if(space[i] == 0)
            openSize++;
        else
            openSize = 0;
    
    if(openSize == memorySize)
        return i-memorySize;
    else
        return 0;
}

void my_malloc(int *space, int memoryReq, int holeStart, int maxMem, int pid){
    //printf("Other malloc started\n");
    for(int i=0; i<memoryReq; i++)
        space[holeStart+i] =  pid;
}

int my_malloc_and_check(int *space, int memoryReq, int maxMem, int pid){
    //printf("Malloc started");
    int holeStart = checkMemory(space, memoryReq, maxMem);
    
    my_malloc(space, memoryReq, holeStart, maxMem, pid);
    
    return holeStart;
}

void my_free(int *space, int memorySize, int memoryLocale){
    //printf("Free memory\n");
    for(int i=0; i<memorySize; i++)
        space[memoryLocale + i] = 0;
}