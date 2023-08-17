/*********************************
 * Reviewer: 
 * Author: Mike Meyers
 * File: select.hpp
 * ******************************/

#include <cstddef>
#include <sys/time.h>
#include <sys/types.h>
/* #include <sys/select.h> */
#include "select.hpp"
#include "monitor.hpp"
#include "reactor.hpp"


using namespace ilrd;

int print(char x)
{
    std::cin>>x;
    std::cout << x << " key pressed" << std::endl;
    return 1;
}


int main()
{

    Select s1;
    Reactor<Select> s2;

    s2.RegisterReadHandler(0,Function<int(void)>(print, 'a'));

    s2.Run();
    return 0;
}