/**
 * @file def_err.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-10-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once
#ifndef _AP_DEF_ERR_HPP_
#define _AP_DEF_ERR_HPP_

#include "ap_def.hpp"

namespace eRR
{
  static auto set_err_code = [](auto obj)
  {
    return (static_cast<int>(obj) * 1'000);
  };


  enum _io
  {
    IO_NOT_DEFINED = set_err_code(apcr::ObjType::HL_IO),
    IO_MODULE_OPEN,
    IO_OUTPUT_OUT_OF_RANGE,
    IO_NO_MATCH,
    IO_MODULE_NO_RESPONSE,
    IO_INVALID_ADDRESS_NUMVER
  };

  enum _uart
  {
    SERIAL_COMM_NOT_DEFINED = set_err_code(apcr::ObjType::HL_SERIAL),
    SERIAL_TIMEOUT,
    SERIAL_INVALID_COMMAND
  };

  enum _net
  {
    SOCKET_COMM_NOT_DEFINED = set_err_code(apcr::ObjType::HL_SOCKET),
    SOCKET_TIMEOUT,
    SOCKET_INVALID_COMMAND
  };

  enum _ext_lib
  {
    LIB_NOT_DEFINED = set_err_code(apcr::ObjType::HL_EXT_LIB),
    LIB_NO_RESPONSE,
    LIB_TIMEOUT,
    LIB_INVALID_COMMAND
  };

  enum _cyl
  {
    CYLINDER_NOT_DEFINED = set_err_code(apcr::ObjType::EN_CYLINDER),
    CYLINDER_TIMEOUT
  };

  enum _vac
  {
    VACUUM_NOT_DEFINED = set_err_code(apcr::ObjType::EN_VACUUM),
    VACUUM_TIMEOUT
  };

}
// end of namespace


#endif //_AP_DEF_ERR_HPP_
