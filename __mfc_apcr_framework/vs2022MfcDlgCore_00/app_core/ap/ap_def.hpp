/**
 * @file ap_def.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-11-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef __AP_DEF_HPP__
#define __AP_DEF_HPP__

#include "def_obj.hpp"
#include "def_io.hpp"
#include "def_err.hpp"

namespace IO
{
  constexpr auto IO_OUTPUT_MAX       = 3000;
  constexpr auto IO_INPUT_MAX        = 3000;
  constexpr auto IO_ADDR_BASE_INPUT  = 1000 + 0;
  constexpr auto IO_ADDR_BASE_OUTPUT = IO_INPUT_MAX + IO_ADDR_BASE_INPUT;
}
//----------------------------------------------
//MARK: system io hw define
//----------------------------------------------
namespace hw_io
{
  enum class comm_mode_e : int
  {
    RS_485,
    ETHERNET,
  };

  enum class net_client_type
  {
    UDP,
    TCP,
  };

  enum class io_type_e : int
  {
    I16O16,
    I16,
    O16,
    O32,
    I32,
    InOn, //input n, output n
  };

  static size_t _in_length(io_type_e io_type)
  {
    switch (io_type)
    {
    case io_type_e::I16O16: return 16;
    case io_type_e::I16:    return 16;
    case io_type_e::I32:    return 32;
    case io_type_e::O16:    __fallthrough;
    case io_type_e::O32:    __fallthrough;
    default:                return 0;
    }
    return 0;
  }

  static size_t _out_length(io_type_e io_type)
  {
    switch (io_type)
    {
    case io_type_e::I16O16: return 16;
    case io_type_e::O16:    return 16;
    case io_type_e::O32:    return 32;
    case io_type_e::I16:    __fallthrough;
    case io_type_e::I32:    __fallthrough;
    default:                return 0;
    }
    return 0;
  }

  template <typename P, typename T>
  concept hw_io_c = std::same_as<P, comm_mode_e> && std::same_as<T, io_type_e>;

  template <typename P, typename T>
  concept net_io_c = std::same_as<P, net_client_type> && std::same_as<T, io_type_e>;

  namespace fastech_type
  {
    enum ezi_io_en
    {
      _I16    = 150,
      _I8O8   = 155,
      _O16    = 160,
      _I32    = 151,
      _I16O16 = 156,
      O32     = 161,
    };

    static std::string to_string(ezi_io_en type)
    {
      switch (type)
      {
      case _I16:    return "I16";
      case _I8O8:   return "I8O8";
      case _I16O16: return "I16O16";
      case _I32:    return "I32";
      case O32:     return "O32";
      default:      return "UNKNOWN";
      }
      return "UNKNOWN";
    }
  }
  // end of namespace fastech_type


  enum class contact_t
  {
    A,
    B,
    PA, // parallel a contact signal
    PB, // parallel b contact signal
  };
}
// end of namespace hw_io


//----------------------------------------------
//MARK: default define
//----------------------------------------------
enum tm_t : uint8_t
{
  tm_idx_0,
  tm_idx_1,
  tm_idx_2,
  tm_idx_3,
  tm_idx_4,
  tm_idx_5,
  tm_idx_6,
  tm_idx_7,
  tm_idx_8,
  tm_idx_9,
  tm_idx_max,
};


//#ifndef _REMOTE_USE_MMC
#define _USE_SIMULATION
//#endif

#define _USE_HW_UART
#define _USE_HW_SOCKET

#define _USE_HW_CLI

#endif // __AP_DEF_HPP__