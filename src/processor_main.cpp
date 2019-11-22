/**
 * GNU General Public License Version 3.0, 29 June 2007
 * Entry pint for processor.
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