/**
 * GNU General Public License Version 3.0, 29 June 2007
 * Test for checking the flow.
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