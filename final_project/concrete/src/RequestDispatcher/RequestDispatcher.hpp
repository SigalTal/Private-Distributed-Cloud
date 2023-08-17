/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: RequestDispatcher.hpp
 * ******************************/
#pragma once

#include <set>	// set<>
#include <unordered_map>	// map<uint32_t, RequestSlicer>
#include <cstdint>
#include <memory>
#include "RequestHost.hpp"

namespace ilrd
{


class RequestDispatcher
{
public:
    explicit RequestDispatcher();
    int NbdRequestsHandle(int *socket);
    int IotHandle(int socket);

private:    
    std::unordered_map<uint32_t, std::shared_ptr<RequestHost> > m_activeRequests;
};




}//ilrd
