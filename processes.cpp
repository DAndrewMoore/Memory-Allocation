#include "processes.h"

using namespace std;

vector<processStruct> genProcs(int numP, string aSeed){
	string::size_type sz;
	int k = numP;
	
	seed_seq seed (aSeed.begin(), aSeed.end());
	
	default_random_engine generator (seed);
	normal_distribution<double> cycles(6000.0, 1200.0);
	//uniform_int_distribution<int> cycles(1,100);
	poisson_distribution<int> memory(20);

	vector<processStruct> pVector;

	for(int i=0; i<k; i++){
		processStruct temp;
		temp.pid = 5000+i;
		temp.cycleCount = cycles(generator);
		//temp.cycleCount = rand() % 19 + 1;
		temp.memoryPrint = memory(generator);
		temp.entranceTime = 50*i;
		pVector.push_back(temp);
	}

	return pVector;
}
