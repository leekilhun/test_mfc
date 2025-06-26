/**
 * @file def_io.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024_04_11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once
#ifndef _AP_DEF_IO_HPP_
#define _AP_DEF_IO_HPP_

#include "ap_def.hpp"

namespace IO
{
  //Register the I/O device to be used.
  // namespace device_idx
  // {
  // 	enum
  // 	{
  // 		reg_fastectnet_i16o16_0,
  // 		reg_max
  // 	};

  // }
  enum class type
  {
    in,
    out
  };

  namespace in
  {
    enum pin
    {
      X000_ = 1000, //1000  Module slave ID1
      X001_,
      X002_,
      X003_,
      X004_,
      X005_,
      X006_,
      X007_,

      X008_,
      X009_,
      X00A_,
      X00B_,
      X00C_,
      X00D_,
      X00E_,
      X00F_,


      max,
    };
  }

  namespace out
  {
    enum pin
    {
      Y000_ = 2000, //
      Y001_,
      Y002_,
      Y003_,
      Y004_,
      Y005_,
      Y006_,
      Y007_,

      Y008_,
      Y009_,
      Y00A_,
      Y00B_,
      Y00C_,
      Y00D_,
      Y00E_,
      Y00F_,

      max,
    };
  }


}


#endif //_DEF_IO_HPP_
