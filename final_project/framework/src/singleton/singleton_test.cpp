/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: function_test.cpp
 * ******************************/


#include "singleton.hpp"
#include <string>
#include <iostream>

using namespace ilrd;
using namespace std;

int main()
{
    Singleton<std::string> str1;
    Singleton<std::string> str2;

    str1->assign("hello");
    std::string str(str2->c_str());
    std::cout << str << std::endl;
    std::cout << str2->c_str() << std::endl;

    std::cout << str1->c_str() << std::endl;

    return 0;
}