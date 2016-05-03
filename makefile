CXX = g++
CXXFLAGS = -std=c++0x -w -pthread

mainMerge: fcfsmem.o manager.o
	$(CXX) $(CXXFLAGS) -o mainMerge fcfsmem.o manager.o

mainp2: project2.o processes.o manager.o
	$(CXX) $(CXXFLAGS) -o main project2.o processes.o manager.o

fcfsmem.o: fcfsmem.cpp manager.h
	$(CXX) $(CXXFLAGS) -c fcfsmem.cpp

project2.o: project2.cpp processes.h manager.h
	$(CXX) $(CXXFLAGS) -c project2.cpp

processes.o: processes.h

manager.o: manager.h

cleanp2:
	rm project2.o processes.o manager.o

cleanMerge:
	rm fcfsmem.o manager.o
