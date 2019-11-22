/**
 * GNU General Public License Version 3.0, 29 June 2007
 * Implemenation of synchronization operations.
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

#include "../include/sync.hpp"
#include "../include/shm_constants.hpp"

#include <pthread.h>
#include <string.h>
#include <signal.h>

#include <stdexcept>
#include <iostream>

void m_sum::init()
{
    std::cerr << "INITIALIZING..." << std::endl;

    std::cerr << "INITIALIZING MUTEX..." << std::endl;
    std::cerr << "INITIALIZING MUTEX ATTRIBUTE ..." << std::endl;
    pthread_mutexattr_t m_attr;
    if(pthread_mutexattr_init(&m_attr) != 0)
        throw std::runtime_error(strerror(errno));
    std::cerr << "INITIALIZED MUTEX ATTRIBUTE ..." << std::endl;    
    if(pthread_mutex_init(&m_sum::glit_lock, &m_attr) != 0)
        throw std::runtime_error(strerror(errno));
    std::cerr << "INITIALIZED MUTEX..." << std::endl;

    std::cerr << "INITIALIZING CONDITIONAL VARIABLE..." << std::endl;
    std::cerr << "INITIALIZING CONDITIONAL VARIABLE ATTRIBUTE..." << std::endl;
    pthread_condattr_t c_attr;
    if(pthread_condattr_init(&c_attr) != 0)
        throw std::runtime_error(strerror(errno));
    std::cerr << "INITIALIZED CONDITIONAL VARIABLE ATTRIBUTE..." << std::endl;
    if(pthread_cond_init(&m_sum::cond_v, &c_attr) != 0)
        throw std::runtime_error(strerror(errno));
    std::cerr << "INITIALIZED CONDITIONAL VARIABLE..." << std::endl;
    
    std::cerr << "INITIALIZED..." << std::endl;
}

void m_sum::destroy()
{
    if(pthread_mutex_destroy(&m_sum::glit_lock) != 0)
        std::cerr << strerror(errno);

    if(pthread_cond_destroy(&m_sum::cond_v) != 0)
        std::cerr << strerror(errno);
}

void m_sum::subsribe()
{
    std::cerr << "SUBSCRIBING..." << std::endl;

    struct sigaction act;
    act.sa_handler = [](int i) {
        if(pthread_mutex_lock(&m_sum::glit_lock) != 0) {
            std::cerr << strerror(errno);
            return;
        }  

        std::cerr << "Global ITERATOR BEFORE : " << m_sum::it << std::endl;
        m_sum::it++;
        std::cerr << "Global ITERATOR AFTER : " << m_sum::it << std::endl;
        if(pthread_cond_signal(&m_sum::cond_v) != 0)
            std::cerr << strerror(errno);

        if(pthread_mutex_unlock(&m_sum::glit_lock) != 0)
            std::cerr << strerror(errno);
    };

    if(sigaction(SIGUSR1, &act, NULL) != 0)
        throw std::runtime_error(strerror(errno));
    
    std::cerr << "SUBSCRIBED..." << std::endl;  
}

void m_sum::wait()
{
    if(pthread_mutex_lock(&m_sum::glit_lock) != 0)
        throw std::runtime_error(strerror(errno));

    std::cerr << "WAITING..." << std::endl;    
    
    while (m_sum::it != LENGTH / SLICE_LENGTH)
        pthread_cond_wait(&m_sum::cond_v, &m_sum::glit_lock);
    
    std::cerr << "WAITED..." << std::endl;
    std::cerr << "Global ITERATOR : " << m_sum::it << std::endl;

    if(pthread_mutex_unlock(&m_sum::glit_lock) != 0)
        throw std::runtime_error(strerror(errno));
}