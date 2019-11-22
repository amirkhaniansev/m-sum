#include "../include/mem_manager.hpp"
#include "../include/shm_constants.hpp"

#include <signal.h>
#include <string.h>

#include <iostream>

int main(int argc, char** argv)
{
    m_sum::mem_manager shmm;
    try
    {
        m_sum::shm_header* header = shmm.get_header();
        shmm.dump();
        
        int it = shmm.acquire();
        int* arr = shmm.get_arr();
        int start_index = it * header->slice_length;
        int end_index = start_index + header->slice_length;
        int result = 0;
        for(int i = start_index; i < end_index; i++)
            result += arr[i];

        shmm.add_result(result);
        std::cerr << "RESULT : " << result << std::endl;
        std::cerr << "SIGNAL SENDING..." << std::endl;
        if(kill(header->man_id, SIGIO) != 0) {
            std::cerr << "Unable to send USER SIGNAL 1" << std::endl;
            std::cerr << strerror(errno) << std::endl;
        }
        else {
            std::cerr << "SIGNAL SENT..." << std::endl;
        }
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }

    std :: cerr << "Now dumping..." << std::endl;

    shmm.dump();
    shmm.detach_shm();

    return 0;
}