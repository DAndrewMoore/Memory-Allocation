CXX = g++
CXXFLAGS = -std=c++0x -g

main: project2.o processes.o manager.o
	$(CXX) $(CXXFLAGS) -o main project2.o processes.o manager.o

fcfsmem.o: fcfsmem.cpp processes.h manager.h
	$(CXX) $(CXXFLAGS) -c fcfsmem.cpp

processes.o: processes.h

manager.o: manager.h

clean:
	rm project2.o processes.o manager.o
