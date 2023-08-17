/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: consts.hpp
 * ******************************/

#pragma once

#include <cstdint>

namespace ilrd
{


class Consts
{
public:
	static uint32_t GetFragmentationSize()
    {
        static uint32_t fragmentation_size = 1024;

        return fragmentation_size;
    }

    static uint32_t GetNumOfIot()
    {
        static uint32_t num_of_iot = 1;
    }

};
}//ilrd
