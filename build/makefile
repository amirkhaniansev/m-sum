all: manager processor test

test: ../include/shm_constants.hpp ../test/test_main.cpp
		g++ ../test/test_main.cpp -o test -pthread

manager: mem_manager.o sync.o ../src/manager_main.cpp
		g++ mem_manager.o sync.o ../src/manager_main.cpp -o manager -pthread

processor: mem_manager.o ../include/mem_manager.hpp ../include/shm_constants.hpp ../src/processor_main.cpp
		g++  mem_manager.o ../src/processor_main.cpp -o processor -pthread

sync.o: ../include/shm_constants.hpp ../include/sync.hpp ../src/sync.cpp
		g++ -c ../src/sync.cpp -o sync.o

mem_manager.o: ../include/mem_manager.hpp ../include/shm_constants.hpp ../src/mem_manager.cpp
		g++ -c ../src/mem_manager.cpp -o mem_manager.o