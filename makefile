CXX = g++
CXXFLAGS = -std=c++0x

main: project2.o processes.o
	$(CXX) $(CXXFLAGS) -o main project2.o processes.o

project.o: project2.cpp processes.h
	$(CXX) $(CXXFLAGS) -c project2.cpp

processes.o: processes.h

clean:
	rm processes.o project2.o