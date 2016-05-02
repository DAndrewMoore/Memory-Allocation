CXX = g++
CXXFLAGS = -std=c++0x

main: fcfsmem.o manager.o
	$(CXX) $(CXXFLAGS) -o main fcfsmem.o manager.o

fcfsmem.o: fcfsmem.cpp manager.h
	$(CXX) $(CXXFLAGS) -c fcfsmem.cpp

manager.o: manager.h

clean:
	rm fcfsmem.o manager.o
