#include <cstdlib>
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
    
    for(; i<20000001 && openSize != memorySize; i++)
        if(space[i] == 0)
            openSize++;
        else
            openSize = 0;
    
    if(openSize == memorySize)
        return i-memorySize+1;
    else
        return 0;
}

int my_malloc(int *space, int memoryReq, int maxMem){
    //printf("Malloc started");
    int holeStart = checkMemory(space, memoryReq, maxMem);
    
    if(holeStart){
        for(int i=0; i<memoryReq; i++)
            space[holeStart+i] = 1;
    }
    
    return holeStart;
}

void my_malloc(int *space, int memoryReq, int holeStart, int maxMem){
    //printf("Other malloc started\n");
    for(int i=0; i<memoryReq; i++)
        space[holeStart+i] = 1;
}

void my_free(int *space, int memorySize, int memoryLocale){
    //printf("Free memory\n");
    for(int i=0; i<memorySize; i++)
        space[memoryLocale + i] = 0;
}