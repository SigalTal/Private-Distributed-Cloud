/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: function.hpp
 * ******************************/

#pragma once 

#include <iostream>
#include <memory>

namespace ilrd
{

template<typename>
class Function;

//----------------------------------------------------------------------------------------------------//
template<typename RET>
class Function<RET(void)>
{
public:
    explicit Function(){}

    template<typename BIND_ARG, typename T>
    explicit Function(RET (T::*callable)(BIND_ARG arg), T *object, BIND_ARG arg);
    
    template<typename BIND_ARG>
    explicit Function(RET (*callable)(BIND_ARG arg), BIND_ARG arg);
    virtual ~Function() {}
    
    RET operator()();    //Will Activate derived RUN();
      
private:
    class IFunction;
    std::shared_ptr<IFunction> m_ifunction;

    template<typename BIND_ARG>
    class FreeFunction;

    template<typename BIND_ARG, typename T>
    class MemberFunction;
};

//----------------------------------------------------------------------------------------------------//
template<typename RET>
class Function<RET(void)>::IFunction
{
public:
    virtual ~IFunction() {}
    virtual RET Run() = 0; //pure virtual

};

//----------------------------------------------------------------------------------------------------//
template<typename RET>
template<typename BIND_ARG>
class Function<RET(void)>::FreeFunction: public Function<RET(void)>::IFunction
{
public:
    explicit FreeFunction(RET (*callable)(BIND_ARG arg), BIND_ARG arg);
    virtual ~FreeFunction() {}
    virtual RET Run();
private:
    RET (*m_callable)(BIND_ARG arg);
    BIND_ARG m_arg;
};

template<typename RET>
template<typename BIND_ARG, typename T>
class Function<RET(void)>::MemberFunction: public Function<RET(void)>::IFunction
{
public:
    explicit MemberFunction(RET (T::*callable)(BIND_ARG arg), T* object, BIND_ARG arg);
    virtual ~MemberFunction() {}
    virtual RET Run();

private:
    RET (T::*m_callable)(BIND_ARG arg);
    T* m_object;
    BIND_ARG m_arg;
};

//----------------------------------------------------------------------------------------------------//

//--------------------------------------------FUNCTION------------------------------------------------//

template <typename RET>
template <typename BIND_ARG, typename T>
Function<RET(void)>::Function(RET (T::*callable)(BIND_ARG arg), T *object, BIND_ARG arg):
m_ifunction(new MemberFunction<BIND_ARG, T>(callable,object,arg))
{}

template <typename RET>
template <typename BIND_ARG>
Function<RET(void)>::Function(RET (*callable)(BIND_ARG arg), BIND_ARG arg):
m_ifunction(new FreeFunction<BIND_ARG>(callable,arg))
{}

template <typename RET>
RET Function<RET(void)>::operator()()
{
   return m_ifunction->Run();
}
//----------------------------------------------------------------------------------------------------//

//---------------------------------------FREE-FUNCTION------------------------------------------------//
template <typename RET>
template <typename BIND_ARG>
Function<RET(void)>::FreeFunction<BIND_ARG>::FreeFunction(RET (*callable)(BIND_ARG arg), BIND_ARG arg):
m_callable(callable),m_arg(arg)
{}

template <typename RET>
template <typename BIND_ARG>
inline RET Function<RET(void)>::FreeFunction<BIND_ARG>::Run()
{
   return m_callable(m_arg);
}

//----------------------------------------------------------------------------------------------------//

//---------------------------------------MEMBER-FUNCTION----------------------------------------------//
template <typename RET>
template <typename BIND_ARG, typename T>
Function<RET(void)>::MemberFunction<BIND_ARG, T>::MemberFunction(RET (T::*callable)(BIND_ARG arg), T *object, BIND_ARG arg):
m_callable(callable), m_object(object), m_arg(arg)
{}

template <typename RET>
template <typename BIND_ARG, typename T>
RET Function<RET(void)>::MemberFunction< BIND_ARG ,T>::Run()
{
   return (m_object->*m_callable)(m_arg);
}

//----------------------------------------------------------------------------------------------------//

//---------------------------------------FUNCTION WITH BIND ARG---------------------------------------//
template<typename RET, typename INVOCATION_ARG>
class Function<RET(INVOCATION_ARG)>
{
public:
    explicit Function(){}

    template<typename T>
    explicit Function(RET (T::*callable)(INVOCATION_ARG arg), T *object);
    
    explicit Function(RET (*callable)(INVOCATION_ARG arg));
    virtual ~Function() {}
    
    RET operator()(INVOCATION_ARG arg) const;    //Will Activate derived RUN();
      
private:
    class IFunction;
    std::shared_ptr<IFunction> m_ifunction;

    class FreeFunction;

    template<typename T>
    class MemberFunction;
};

template<typename RET, typename INVOCATION_ARG>
class Function<RET(INVOCATION_ARG)>::IFunction
{
public:
    virtual ~IFunction() {}
    virtual RET Run(INVOCATION_ARG arg) = 0; //pure virtual

};

//----------------------------------------------------------------------------------------------------//
template<typename RET, typename INVOCATION_ARG>
class Function<RET(INVOCATION_ARG)>::FreeFunction: public Function<RET(INVOCATION_ARG)>::IFunction
{
public:
    explicit FreeFunction(RET (*callable)(INVOCATION_ARG arg));
    virtual ~FreeFunction() {}
    virtual RET Run(INVOCATION_ARG arg);
private:
    RET (*m_callable)(INVOCATION_ARG arg);
};

template<typename RET, typename INVOCATION_ARG>
template<typename T>
class Function<RET(INVOCATION_ARG)>::MemberFunction: public Function<RET(INVOCATION_ARG)>::IFunction
{
public:
    explicit MemberFunction(RET (T::*callable)(INVOCATION_ARG arg), T* object);
    virtual ~MemberFunction() {}
    virtual RET Run(INVOCATION_ARG arg);

private:
    RET (T::*m_callable)(INVOCATION_ARG arg);
    T* m_object;
};

//----------------------------------------------------------------------------------------------------//

template <typename RET, typename INVOCATION_ARG>
template <typename T>
Function<RET(INVOCATION_ARG)>::Function(RET (T::*callable)(INVOCATION_ARG arg), T *object):
m_ifunction(new MemberFunction<T>(callable,object))
{}

template <typename RET, typename INVOCATION_ARG>
Function<RET(INVOCATION_ARG)>::Function(RET (*callable)(INVOCATION_ARG arg)):
m_ifunction(new FreeFunction(callable))
{}

template <typename RET, typename INVOCATION_ARG>
RET Function<RET(INVOCATION_ARG)>::operator()(INVOCATION_ARG arg) const
{
    return m_ifunction->Run(arg);
}

template <typename RET, typename INVOCATION_ARG>
Function<RET(INVOCATION_ARG)>::FreeFunction::FreeFunction(RET (*callable)(INVOCATION_ARG arg)):
m_callable(callable)
{}

template <typename RET, typename INVOCATION_ARG>
inline RET Function<RET(INVOCATION_ARG)>::FreeFunction::Run(INVOCATION_ARG arg)
{
    return m_callable(arg);
}

template <typename RET, typename INVOCATION_ARG>
template <typename T>
Function<RET(INVOCATION_ARG)>::MemberFunction<T>::MemberFunction(RET (T::*callable)(INVOCATION_ARG arg), T *object):
m_callable(callable), m_object(object)
{}

template <typename RET, typename INVOCATION_ARG>
template <typename T>
inline RET Function<RET(INVOCATION_ARG)>::MemberFunction<T>::Run(INVOCATION_ARG arg)
{
    return (m_object->*m_callable)(arg);
}

} // namespace ilrd