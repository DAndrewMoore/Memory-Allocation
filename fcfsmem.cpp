//Project 2 
//Memory Management
//Emory King
//Daniel Moore


//#include <stdlib>
//#include <unistd>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <time.h>
#include <random>
#include "manager.h"

//using namespace std;

class tuple {
public:
        int pid, arrival_t, start_flag, started, waited, 
			stalled, cycles, memory, tlip, fin, switching,
			switch_time, memPos;
		int* memSpot;
	tuple(int, int,int,int);
};

tuple::tuple (int count, int at, int cyc, int mem) {
    pid = count;
    arrival_t = at;
    cycles = cyc; //cycles remaining
    memory = mem;
    waited = 0;	
    stalled = 0;
    start_flag = 0;
    fin = 0;
    tlip = cyc;
    switching = 0;
}


void processProcs (double mem_change){

int k = 64; 		//number of processes
int q = 50; 		//RoundRobin quantum
int cts = 10; 		//context switch
int i = 0;			//tuples index
int j = 0;			//arrival t
int c = 0;			//completed processes index
int current_time = 0;//time elapsed in scheduler
int finished = 0;	//# processes finished
int lowest;			//processing time interval
int l = 0;
int a = 0;			//context switch count
int p = 0; 
int next = 0;		//index for arriving processes
clock_t cusTime;
clock_t cusTotalMalTime = 0;
clock_t cusTotalFreeTime = 0;
clock_t regTime;
clock_t regTotalMalTime = 0;
clock_t regTotalFreeTime = 0;

int rem_mem, max_mem, tot_mem, total_processes_mem;
max_mem = 20;
tot_mem = 0;
total_processes_mem = 0;

tuple *tuples[k];	//initial processes
tuple *completed[k];//completed processed
tuple *proc[64];		//4 cores
std::queue<tuple*> fifo;	//FIFO queue

int total_free_time = 0;
int total_malloc_time = 0;
int total_my_malloc_time = 0;
int total_my_free_time = 0;

//Initiate the memory generator with poisson distrubtion
std::string aSeed = "Test";
std::seed_seq seed (aSeed.begin(), aSeed.end());
	
std::default_random_engine generator (seed);
std::normal_distribution<double> cycles(6000.0, 1200.0);
std::poisson_distribution<int> memory(20);

///////////////////////////////////////////////////////////////////////////////////////
srand(time(NULL));
std::cout << "spawning " << k << " processes" << std::endl;
//create processes with arrival timess at intervals of 50
for (j=0; j<3200; j+=50){
    int cycleCount = cycles(generator);
    int mem = memory(generator);
	tuples[i] = new tuple(i, j, cycleCount, mem);	// j = arrival t
	total_processes_mem += mem;
	i++;
}
std::cout << "total_processes_mem: " << total_processes_mem << std::endl; 

//print process info
i =0;
std::cout << "pid cycles arival  memory" << std::endl;
for(i;i<k;i++){
	std::cout << tuples[i]->pid <<"   " << tuples[i]->cycles <<"   "<< tuples[i]->arrival_t << "       " << tuples[i]->memory << std::endl;
}


//initialize cores
for (i=0; i<k; i++){
	proc[i] = NULL;
}

//change according to total need
if(mem_change != 1.0)
	max_mem = (int) (total_processes_mem * mem_change);
else
	max_mem = 20000001; //default memory size

//initialize custom manager space
cusTime = clock();
int* space = memoryScope(max_mem);
cusTime = clock() - cusTime;
cusTotalMalTime += cusTime; //since it is a malloc we add it to total malloc time

//until not finished
for (current_time=0; finished<64; current_time+=lowest){ // process in lowest current_time increments
	lowest = 50;
	rem_mem = max_mem - tot_mem;
	////////////////////////// FILL //////////////////////////////
	// add waiting (fifo) to running if memory is available
	//std::cout << std::endl << "---------------------------------------curent time:  " << current_time << std::endl;
	for (i=0; i<k; i++){
		if (fifo.empty() != 1 && (fifo.front()->memory <= rem_mem) && proc[i] == 0){
				proc[i] = fifo.front();
				tot_mem += proc[i]->memory;
				///////////////////////////////////////////
				// TRIVIAL malloc() TIMING
				//start trivial time
				regTime = clock();
				//malloc() memory
				proc[i]->memSpot = (int *) malloc(sizeof(int) * proc[i]->memory);
				//end trivial time
				regTime = clock() - regTime;
				//total_malloc_time += ?;
				regTotalMalTime += regTime;
				///////////////////////////////////////////
                                
                                for(int m=0; m<proc[i]->memory; m++)
                                    proc[i]->memSpot[m] = rand() % 1000 + 1;
                                
				///////////////////////////////////////////
				// CUSTOM my_malloc() TIMING
				//start custom time
				cusTime = clock();
				//my_malloc() memory
				proc[i]->memPos = my_malloc(space, proc[i]->memory, max_mem);
				//end custom time
				cusTime = clock() - cusTime;
				//total_my_malloc_time += ?;
				cusTotalMalTime += cusTime;
				///////////////////////////////////////////
				fifo.pop();
				std::cout << "Process " << proc[i]->pid << " allocated " <<  proc[i]->memory << " memory at time " << current_time << std::endl;
		}
	}

	// add arriving to waiting (fifo) if time is right
	if (next < k && tuples[next]->arrival_t <= current_time){
			fifo.push(tuples[next]);
			next++;
	}

	// add waiting (fifo) to running if memory is available
	for (i=0; i<k-1; i++){
		if (fifo.empty() != 1 && (fifo.front()->memory <= rem_mem) && proc[i] == 0){
				proc[i] = fifo.front();
				tot_mem += proc[i]->memory;
				///////////////////////////////////////////
				// TRIVIAL malloc() TIMING
				//start trivial time
				regTime = clock();
				//malloc() memory
				int* regMem = (int *) malloc(sizeof(int) * proc[i]->memory);
				//end trivial time
				regTime = clock() - regTime;
				//total_malloc_time += ?;
				regTotalMalTime += regTime;
				///////////////////////////////////////////

				///////////////////////////////////////////
				// CUSTOM my_malloc() TIMING
				//start custom time
				cusTime = clock();
				//my_malloc() memory
				proc[i]->memPos = my_malloc(space, proc[i]->memory, max_mem);
				//end custom time
				cusTime = clock() - cusTime;
				//total_my_malloc_time += ?;
				cusTotalMalTime += cusTime;
				///////////////////////////////////////////
				fifo.pop();
				std::cout << "Process " << proc[i]->pid << " allocated " <<  proc[i]->memory << " memory at time " << current_time << std::endl;
		}
	}
		////////////////////////////////////////////////////////////////////////////////////
		//set processing interval "lowest"
		for (i=0; i<k; i++){
			if (proc[i] != 0 && proc[i]->tlip < lowest){
				lowest = proc[i]->tlip;
			}
		}
		// if interval overruns next arrival time, lower interval 
		if (next < k && current_time+lowest > tuples[next]->arrival_t){
			lowest = tuples[next]->arrival_t - current_time;
		}
		////////////////////////////////////////////////////////////////////////////////////
				
	////////////////////////////////////////////////////////////////////////////////////
	//release memory from completed processes
	for (i=0; i<k; i++){
		if (proc[i]!=NULL && proc[i]->tlip == lowest){//time to free this procs memory 
			
			proc[i]->tlip = 0;
                        std::cout << finished << std::endl;
			finished++;
			tot_mem -= proc[i]->memory;
			std::cout << "Process " << proc[i]->pid << " released " <<  proc[i]->memory << " memory at time " << current_time << std::endl;

			///////////////////////////////////////////
			// TRIVIAL free() TIMING
			//start trivial time
			regTime = clock();
			//free() memory
			free(proc[i]->memSpot);
			//end trivial time
			regTime = clock() - regTime;
			//total_free_time += ?;
			regTotalFreeTime += regTime;
			///////////////////////////////////////////

			///////////////////////////////////////////
			// CUSTOM my_free() TIMING
			//start custom time
			cusTime = clock();
			//my_free() memory
			my_free(space, proc[i]->memory, proc[i]->memPos);
			//end custom time
			cusTime = clock() - cusTime;
			//total_my_free_time += ?;
			cusTotalFreeTime += cusTime;
			///////////////////////////////////////////
			proc[i] = NULL;
		}
		else if (proc[i]!=NULL && proc[i]->tlip != lowest){//memory still in use. continue...
			proc[i]->tlip -= lowest;
		}
		else {}//cout << i << " proc[i] empty" << endl;
	}
	////////////////////////////////////////////////////////////////////////////////////
}//finished 64 processes

/////////////////////////////////////////////////////
//   			DISPLAY DATA
clock_t total_trivial = regTotalMalTime + regTotalFreeTime;
clock_t total_custom = cusTotalMalTime + cusTotalFreeTime; 

std::cout << "-----------------------------------------------------------" << std::endl;
std::cout << "Processes Finished." << std::endl;

std::cout  << std::endl<<std::endl<< "Total time for trivial management system" << std::endl << "--------------------------------------" << std::endl;
std::cout << "free(): "<< total_free_time << "     malloc(): "<< regTotalMalTime << "     total trivial: "<< total_trivial  << std::endl;

std::cout << std::endl << std::endl << "Total time for custom management system" << std::endl << "--------------------------------------" << std::endl;
std::cout << "my_malloc(): "<< cusTotalFreeTime << "     my_free(): "<< cusTotalMalTime << "     total custom: "<< total_custom <<  std::endl;
//
/////////////////////////////////////////////////////
}

int main (){
	double mem_sizes [3] = {1.0, 0.5, 0.1};
	for(double mem_size: mem_sizes)
		processProcs(mem_size);
        
        return 0;
}
