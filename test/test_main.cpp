#include "../include/shm_constants.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>
#include <future>

int main(int argc, char** argv)
{
    int count = LENGTH / SLICE_LENGTH;
    std::vector<std::future<int>> v;
    for(int i = 0; i < count; i++) {
        v.push_back(
            std::async([]()
            {
                pid_t processor_id = fork();
                if(processor_id < 0) {
                    std::cerr << "Unable to create processor..." << std::endl;
                    return 1;
                }

                if(processor_id == 0) {
                    if(execlp("./processor", "./processor", NULL) != 0) {
                        std::cerr << "Unable to execute processor..." << std::endl;
                        return 1;
                    }
                }

                return 0;
            }));
    }

    for(int i = 0; i < count; i++)
        v[i].wait();

    return 0;
}