/**
 * GNU General Public License Version 3.0, 29 June 2007
 * Manager entry point.
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