/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: Observer.cpp					
 * ******************************/

#pragma once
#include <set>

#include "function.hpp"

namespace ilrd
{

template<typename>
class Callback;


template<typename EVENT>
class Dispatcher
{

public:
    ~Dispatcher();
    void Add(const Callback<EVENT>& callback);
    void Remove(const Callback<EVENT>& callback);
    void Notify(EVENT event); 
    
private:
    std::set<const Callback<EVENT>*> m_subscribers;

};

template<typename EVENT>
class Callback
{
public:
    Callback(const Function<void(EVENT)>& func);
    ~Callback();
    void BindDispatcher(const Dispatcher<EVENT>& dispatcher) const;//assgin pointer to the the dispatcher
    void DisConnect() const;
private:
    friend class Dispatcher<EVENT>;
    Function<void(EVENT)> m_function;
    mutable Dispatcher<EVENT> *m_dispatcher;   
};

template <typename EVENT>
Dispatcher<EVENT>::~Dispatcher()
{
    typename std::set<const Callback<EVENT>* >::const_iterator iter;

	for(iter = m_subscribers.begin();
		iter != m_subscribers.end();
		++iter)
	{
		(*iter)->DisConnect();
	}

}

template <typename EVENT>
void Dispatcher<EVENT>::Add(const Callback<EVENT> &callback)
{
    m_subscribers.insert(&callback);
    callback.BindDispatcher(*this);
}

template <typename EVENT>
void Dispatcher<EVENT>::Remove(const Callback<EVENT> &callback)
{
    m_subscribers.erase(&callback);
    callback.DisConnect();
}

template <typename EVENT>
void Dispatcher<EVENT>::Notify(EVENT event)
{
    typename std::set<const Callback<EVENT>* >::const_iterator iter;

    for(iter=m_subscribers.begin(); iter!=m_subscribers.end(); ++iter)
    {
        (*iter)->m_function(event);
    }
}

template <typename EVENT>
Callback<EVENT>::Callback(const Function<void(EVENT)> &func):
m_function(func)
{}

template <typename EVENT>
Callback<EVENT>::~Callback()
{
    m_dispatcher->Remove(*this);
}

template <typename EVENT>
void Callback<EVENT>::BindDispatcher(const Dispatcher<EVENT> &dispatcher) const
{
    m_dispatcher = const_cast<Dispatcher<EVENT>*>(&dispatcher);
}

template <typename EVENT>
void Callback<EVENT>::DisConnect() const
{
    m_dispatcher = NULL;
}

}
