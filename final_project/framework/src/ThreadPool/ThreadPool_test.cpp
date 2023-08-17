/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: ThreadPool_test.cpp
 * ******************************/

#include <iostream>
#include <unistd.h>
#include <cstring>
#include "ThreadPool.hpp" 


typedef struct ranges
{
	int start;
	int finish;
}range_t;

int arr[100];
range_t g_start[] = {{0, 24},{25, 49},{50,74}, {75, 100}};
const int NUM_OF_THREADS = 4;

/* ---------------- Sorting Related Functions -----------------*/

static int IsSortedList(int* array, size_t size) /* 0 for not sorted, otherwise sorted */
{
    size_t i = 0;
    
	for(i = 0; i < (size-1); ++i)
	{
		if(array[i] > array[i + 1])
		{
			return (0);
		}
	}
		
	return (1);
}

void swap (int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;

}

void SelectionSort(int* array, size_t size)
{
	size_t j= 0 , i =0 ,min = 0 ;
	
	while(size-1 > i)
	{	
		min = i;
		j = i+1;
		while(size > j)
		{
			if(*(array + j) < *(array+ min))
			{
				min = j;
			}
			++j;
		}
		swap ((array+i), (array+min));
		++i;
	}
	printf("Selection done.\n");
}

int ExampleFunc(int num)
{
	SelectionSort(arr+g_start[num].start,25);
   
    return 0;
}

using namespace ilrd;

int main()
{
    int i = 0;
    ThreadPool pool;
    Function<int(void)> funcs[NUM_OF_THREADS];

    Function<int(void)> f1(ExampleFunc, 0);
    Function<int(void)> f2(ExampleFunc, 1);
    Function<int(void)> f3(ExampleFunc, 2);
    Function<int(void)> f4(ExampleFunc, 3);

	for(i = 0; i < 100; ++i)
	{
		arr[i] = rand() % 1000;
	}

    ThreadPool::Future future[NUM_OF_THREADS];

    future[0] = pool.Async(f1);
    future[1] = pool.Async(f2);
    future[2] = pool.Async(f3);
    future[3] = pool.Async(f4);

    for(i = 0; i < NUM_OF_THREADS; ++i)
    {
        if(future[i].Wait())
        {
            std::cout << "Error!" << std::endl;
            return 1;
        }
    }
    

    sleep(3);

	std::cout << "\n\n\n";

	size_t ctr = 0;
	for(i = 0; i < 100; i+=25)
	{
		if(IsSortedList(arr+i, 25))
		{
			std::cout << "Thread " << i/25 << ": Sorted Correctly!" << std::endl;
			++ctr;
		}
		else
		{
			std::cout << "Thread " << i/25 << ": Failed Sorting!" << std::endl;
		}
	}
	if(ctr == NUM_OF_THREADS)
	{
		std::cout << "------------- Test Succssed --------------" << std::endl;
	}

	std::cout << "\n\n\n";

   pool.ShutDown();
/* 	pool.Cancel();
 */    
    return 0;
}




