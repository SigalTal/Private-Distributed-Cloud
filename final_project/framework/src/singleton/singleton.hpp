/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: function.hpp
 * ******************************/

#pragma once 

#include <atomic>
#include <cstdlib>	/* atexit */


namespace ilrd
{

template<typename T>
class Singleton
{
public:
    explicit Singleton();
    T* operator->();
    ~Singleton() = default; 

    Singleton(Singleton &other) = delete;        //prevent copy
    void operator=(const Singleton &) = delete;    //prevent assignment
private:
    static void CleanUp();
    static T *m_instance;
};// class Singleton

template<typename T>
T* Singleton<T>::m_instance = nullptr; 

template <typename T>
Singleton<T>::Singleton()
{
    if(nullptr== m_instance)
    {
        static bool ctor_started = 0; 
        if(!__atomic_test_and_set(&ctor_started, __ATOMIC_SEQ_CST))
        {
            __atomic_store_n(&m_instance, new T, __ATOMIC_SEQ_CST);
            atexit(&Singleton::CleanUp);
        }
        else
        {
            while(__atomic_load_n(&m_instance, __ATOMIC_SEQ_CST)==nullptr);
        }
    }   
}

template <typename T>
T *Singleton<T>::operator->()
{
    return m_instance;
}


template <typename T>
void Singleton<T>::CleanUp()
{
    delete m_instance;
    m_instance = reinterpret_cast<T*>(0xdeadbeef);
}

} // namespace ilrd
