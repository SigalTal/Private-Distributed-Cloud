/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: observer_test.cpp
 * ******************************/

#include <cstdio>
#include "observer.hpp"
#define NUM_SUBS 10

using namespace ilrd;

class  Publisher
{
public:
	void Subscribe(Callback<const char*>& sub);
	void UnSubscribe(Callback<const char*>& sub);
	void NotifyAll(const char *str);

private:
	Dispatcher<const char*> m_dispacher;
};

class Subscriber
{
public:

	explicit Subscriber();
	void OnUpdate(const char *str);
	
	Callback<const char *> m_callback;

private:
	int m_id;

};

void Publisher::Subscribe(Callback<const char *> &sub)
{
	m_dispacher.Add(sub);
}

void Publisher::UnSubscribe(Callback<const char *> &sub)
{
	m_dispacher.Remove(sub);
}

void Publisher::NotifyAll(const char *str)
{
	m_dispacher.Notify(str);
}

Subscriber::Subscriber(): m_callback(Function<void(const char*)>(&Subscriber::OnUpdate, this))
{
	static int id = 0;
	m_id = id++;
}

void Subscriber::OnUpdate(const char *str)
{
	std::cout << "Subscriber number " << m_id << " Received " << str << std::endl;
}

int main()
{
	Publisher pub;
	Subscriber subs[NUM_SUBS];

	for(int i = 0; i < NUM_SUBS; ++i)
	{
		pub.Subscribe(subs[i].m_callback);
	}

	pub.NotifyAll("Hello");


	return 0;
}

