/**
 * @file IIO.hpp
 * @author lee kil hun (gns2.lee)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#ifndef __IF_IIO_HPP__
#define __IF_IIO_HPP__

#include "def_obj.hpp"

/**
 * @brief io device interface
 * 
 */
#if 1
class IIO: virtual public apcr::Icommon
{
public:
    virtual bool IsOn(uint32_t ) = 0;
    virtual bool IsOff(uint32_t ) = 0;
    virtual errno_t OutputOn(uint32_t) = 0;
    virtual errno_t OutputOff(uint32_t) = 0;
    virtual errno_t OutputToggle(uint32_t) = 0;
    virtual errno_t GetData(void) = 0;

};
#endif
#endif // __IF_IIO_HPP__