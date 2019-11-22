/**
 * GNU General Public License Version 3.0, 29 June 2007
 * Implementation of memory manager.
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

#include "../include/mem_manager.hpp"
#include "../include/shm_constants.hpp"

#include <string.h>
#include <errno.h>
#include <signal.h>

#include <stdexcept>
#include <iostream>

void m_sum::mem_manager::init(int length, int slice_length, int flags)
{
    this->length = length;
    this->slice_length = slice_length;

    this->overall_size = sizeof(m_sum::shm_header) + (length + (length / slice_length)) * sizeof(int);
    this->shm_id = shmget(SHM_KEY, this->overall_size, flags);
    if(this->shm_id == -1) {
        std::cerr << "GET Error : " << strerror(errno) << std::endl;
        throw std::runtime_error("Unable to create or access shared memory.");        
    }

    void* result = shmat(this->shm_id, NULL, 0);
    if(result == (void*)-1) {
        std::cerr << "ATTACH Error : " << strerror(errno) << std::endl;
        throw std::runtime_error("Unable to attach to shared memory.");
    }

    this->shm_ptr = result;
}

m_sum::mem_manager::mem_manager()
{
    this->init(LENGTH,SLICE_LENGTH, 0660);
    
    m_sum::shm_header* header = this->get_header();
    if(header->it == LENGTH / SLICE_LENGTH)
        throw std::runtime_error("No more job to do.");
}

m_sum::mem_manager::mem_manager(int length, int slice_length)
{
    this->init(length, slice_length, IPC_CREAT | 0660);

    m_sum::shm_header header;
    if(pthread_mutexattr_init(&header.r_lock_attr) != 0)
        throw std::runtime_error(strerror(errno));
    if(pthread_mutexattr_setpshared(&header.r_lock_attr, PTHREAD_PROCESS_SHARED) != 0)
        throw std::runtime_error(strerror(errno));
    if(pthread_mutex_init(&header.r_lock, &header.r_lock_attr) != 0)
        throw std::runtime_error(strerror(errno));

    header.length = length;
    header.slice_length = slice_length;
    header.it = 0;
    header.man_id = getpid();

    header.arr_offset = sizeof(m_sum::shm_header);
    header.result_arr_offset = sizeof(m_sum::shm_header) + LENGTH * sizeof(int);
    int* arr = (int*)(((char*)this->shm_ptr) + header.arr_offset);
    for(int i = 0; i < this->length; i++)
        arr[i] = 1;
    
    int* result_arr = (int*)(((char*)this->shm_ptr) + header.result_arr_offset);
    for(int i = 0; i < this->length / this->slice_length; i++)
        result_arr[i] = 0;

    memcpy(this->shm_ptr, &header, sizeof(m_sum::shm_header));
}

m_sum::shm_header* m_sum::mem_manager::get_header()
{
    return (m_sum::shm_header*)this->shm_ptr;
}

int* m_sum::mem_manager::get_arr()
{
    return this->get_ptr(this->get_header()->arr_offset);
}

int* m_sum::mem_manager::get_result_arr()
{
    return this->get_ptr(this->get_header()->result_arr_offset);
}

void m_sum::mem_manager::detach_shm()
{
    if(shmdt(this->shm_ptr) != 0)
        std::cerr << "DETACH Error : " << strerror(errno) << std::endl;
}

void m_sum::mem_manager::delete_shm()
{
    if(shmctl(this->shm_id, IPC_RMID, NULL) != 0) 
        std::cerr << "DELETE Error : " << strerror(errno) << std::endl;
}

void m_sum::mem_manager::dump()
{
    m_sum::shm_header* header = this->get_header();
    std::cerr << "Length : " << header->length << std::endl
              << "Slice Length : " << header->slice_length << std::endl
              << "Manager ID : " << header -> man_id << std::endl
              << "Iterator : " << header -> it << std::endl
              << "SHM PTR : " << this->shm_ptr << std::endl
              << "Arr PTR : " << this->get_arr() << std::endl
              << "Result PTR : " << this->get_result_arr();

    std::cerr << std::endl;
    std::cerr << "Fist 10 Items" << std::endl;
    for(int i = 0; i < 10; i++)
        std::cerr << this->get_arr()[i] << " ";

    std::cerr << std::endl;
    std::cerr << "Last 10 Items" << std::endl;
    for(int i = header->length - 1; i >= header->length - 11; i--)
        std::cerr << this->get_arr()[i] << " ";

    std::cerr << std::endl;
    std::cerr << "Result Items" << std::endl;
    for(int i = 0; i < header->length / header->slice_length; i++)
        std::cerr << this->get_result_arr()[i] << " ";

    std::cerr << std::endl;
}

int m_sum::mem_manager::acquire()
{
    int* result_arr = this->get_result_arr();
    m_sum::shm_header* header = this->get_header();
    if(header->it == LENGTH / SLICE_LENGTH)
        throw std::runtime_error("No job to do...");

    if(pthread_mutex_lock(&header->r_lock) != 0)
        throw std::runtime_error(strerror(errno));
    this->acquire_it = header->it;
    header->it++;
    if(pthread_mutex_unlock(&header->r_lock) != 0)
        throw std::runtime_error(strerror(errno));

    return this->acquire_it;
}

void m_sum::mem_manager::add_result(int result)
{
    this->get_result_arr()[this->acquire_it] = result;
}

void m_sum::mem_manager::report()
{
    int* result_arr = this->get_result_arr();
    int result = 0;
    int length = LENGTH / SLICE_LENGTH;
    for(int i = 0; i < length; i++)
        result += result_arr[i];

    std::cerr << "OVERALL RESULT : " << result << std::endl;
    std::cerr << "Now dumping..." << std::endl;

    this->dump();
}

void m_sum::mem_manager::destroy_sync()
{
    m_sum::shm_header* header = this->get_header();
    if(pthread_mutex_destroy(&header->r_lock) != 0)
        std::cerr << strerror(errno);
    if(pthread_mutexattr_destroy(&header->r_lock_attr) != 0)
        std::cerr << strerror(errno);
}

int* m_sum::mem_manager::get_ptr(int offset)
{
    return (int*)(((char*)this->shm_ptr) + offset);
}