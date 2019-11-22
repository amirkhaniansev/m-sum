#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <sys/types.h>
#include <unistd.h>

namespace m_sum {
    struct shm_header {
        pid_t man_id;
        pthread_mutex_t r_lock;
        pthread_mutexattr_t r_lock_attr;
        int it;
        int length;
        int slice_length;
        int arr_offset;
        int result_arr_offset;
    };

    class mem_manager {
        public:
            mem_manager();
            mem_manager(int length, int slice_length);
            shm_header* get_header();
            int* get_arr();
            int* get_result_arr();
            int acquire();
            void detach_shm();
            void delete_shm();
            void dump();
            void report();
            void destroy_sync();
            void add_result(int result);
        private:
            int length;
            int slice_length;
            int overall_size;
            int shm_id;
            int acquire_it;
            void* shm_ptr;
        protected:
            int* get_ptr(int offset);
            void init(int length, int slice_length, int flags);
    };
};

#endif