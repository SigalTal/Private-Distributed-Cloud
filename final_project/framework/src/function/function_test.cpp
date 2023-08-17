/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: function_test.cpp
 * ******************************/


#include <cstdio>
#include <iostream>
#include "function.hpp"

using namespace ilrd;
class Foo
{
	public:

	int foo(int x)
	{
		std::cout<<x<<std::endl;
		return x;
	}
};

int Bar(int x)
{

	std::cout<<x<<std::endl;
	return x;
}

int main()
{
	Foo ff;
	Function<int(void)> f1(&Foo::foo, &ff, 6);
	Function<int(int)> f3(&Foo::foo, &ff);
	f1();
	f3(6);

	Function<int(void)> f2(&Bar, 17);
	Function<int(int)> f4(&Bar);
	f2();
	f4(17);
	

	return 0;
}
