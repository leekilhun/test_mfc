/**
 * @file IComm.hpp
 * @author lee kil hun (gns2.lee)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#ifndef _IF_ICOMM_HPP
#define _IF_ICOMM_HPP

#include "def_obj.hpp"

/**
 * @brief communication interface 
 * 
 */
class IComm : virtual public apcr::Icommon
{
public:
    virtual errno_t Open() = 0;
    virtual void Close() = 0;
    virtual bool IsConnected() const = 0;
    virtual void registerCallback(std::function<int(void *, void *)> cb) = 0;
    virtual int SendData(const char *ptr_data, size_t length) = 0;
    virtual int RecvData(std::vector<char>& rev_v) = 0;
};
#endif