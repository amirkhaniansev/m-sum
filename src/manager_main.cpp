#include "../include/mem_manager.hpp"
#include "../include/sync.hpp"
#include "../include/shm_constants.hpp"

#include <iostream>

int main(int argc, char** argv) 
{   
    m_sum::mem_manager shmm(LENGTH, SLICE_LENGTH);

    try
    {
        shmm.dump();

        m_sum::init();
        m_sum::subsribe();
        m_sum::wait();

        std::cerr << "Result is ready. Now dumping..." << std::endl;

        shmm.dump();
        shmm.report();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    shmm.destroy_sync();
    shmm.detach_shm();
    shmm.delete_shm();

    m_sum::destroy();

    return 0;
}