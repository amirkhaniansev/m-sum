/**
 * GNU General Public License Version 3.0, 29 June 2007
 * Header file for memory manager.
 * Copyright (C) <2019>
 *      Authors: <amirkhaniansev>  <amirkhanyan.sevak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Full notice : https://github.com/amirkhaniansev/m-sum/tree/master/LICENSE
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

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