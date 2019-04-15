sampleobjects = buffer_manager.o file_manager.o sample_run.o
managerfiles = buffer_manager.o file_manager.o
all: sample_run merge_sort insertion binary_search

sample_run : $(sampleobjects)
	g++ -g -std=c++11 -o sample_run $(sampleobjects)

insertion: $(managerfiles) insertion.o
	g++ -g -std=c++11 -o insertion $(managerfiles) insertion.o

binary_search: $(managerfiles) binary_search.o
	g++ -g -std=c++11 -o binary_search $(managerfiles) binary_search.o

merge_sort: $(managerfiles) merge_sort.o
	g++ -g -std=c++11 -o merge_sort $(managerfiles) merge_sort.o

sample_run.o : sample_run.cpp buffer_manager.cpp file_manager.cpp
	g++ -g -std=c++11 -c sample_run.cpp

merge_sort.o: merge_sort.cpp file_manager.cpp buffer_manager.cpp
	g++ -g -std=c++11 -c merge_sort.cpp

insertion.o: insertion.cpp file_manager.cpp buffer_manager.cpp 
	g++ -g -std=c++11 -c insertion.cpp

binary_search.o: binary_search.cpp file_manager.cpp buffer_manager.cpp binary_search.h
	g++ -g -std=c++11 -c binary_search.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -g -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -g -std=c++11 -c file_manager.cpp


clean :
	rm -f *.o
	rm -f sample_run insertion merge_sort binary_search
