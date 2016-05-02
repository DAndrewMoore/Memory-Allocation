/*
  *Name; Daniel Moore
  *Class: 4600.001
  *Purpose: Predefine necessary strucutres and methods for project.cpp
*/

#include <stdio.h>
#include <random>
#include <math.h>
#include <vector>

typedef struct{
int pid;
int cycleCount;
int memoryPrint;
int entranceTime;
} processStruct;

std::vector<processStruct> genProcs(int, std::string);