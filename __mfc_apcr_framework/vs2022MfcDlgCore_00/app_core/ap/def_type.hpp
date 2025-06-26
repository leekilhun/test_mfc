/**
 * @file def_type.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief
 *     - 시스템 구성에 필요한 기본적인 type 정의와 peripheral base address 정의
 *  
 * @version 0.2
 * @date 2025-04-07
 *  
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef __AP_DEF_TYPE_HPP__
#define __AP_DEF_TYPE_HPP__

#include "def.hpp"
#include "util/utils.hpp"


namespace apcr
{
  //#####################################################################
  //---------------------------------------------------------------------
  //MARK: digit no - object type [xxx] + defined error no [xxx]
  //---------------------------------------------------------------------
  typedef int errcode;

  //---------------------------------------------------------------------
  // 9-digit no - object type [xxx] + defined err-no [xxx] + instance_no [xxx]
  // object type[xxx] + error no[xxx] + instance_no [xxx]
  //---------------------------------------------------------------------
  typedef long obj_errno;

  //---------------------------------------------------------------------
  //MARK:   application peripheral base address.
  //---------------------------------------------------------------------
  constexpr auto APP_PERIPH_BASE = 0x00000000UL; //  0x00100000UL~;


  //---------------------------------------------------------------------
  //MARK:  peripherals
  //---------------------------------------------------------------------
  //@@ Hardware Layer
  constexpr auto HW_IO_BASE        = (APP_PERIPH_BASE + 0x00000010UL); // max 240 object
  constexpr auto HW_SERIAL_BASE    = (APP_PERIPH_BASE + 0x00000100UL); // max 32 object
  constexpr auto HW_SOCKET_BASE    = (APP_PERIPH_BASE + 0x00000120UL); // max 16 object
  constexpr auto HW_CSOCKET_BASE   = (APP_PERIPH_BASE + 0x00000130UL); // max 8 object
  constexpr auto HW_USOCKET_BASE   = (APP_PERIPH_BASE + 0x00000138UL); // max 8 object
  constexpr auto HW_UCSOCKET_BASE  = (APP_PERIPH_BASE + 0x00000140UL); // max 8 object
  constexpr auto HW_LIB_BASE       = (APP_PERIPH_BASE + 0x00000150UL);
  constexpr auto HW_EXT_CNTRL_BASE = (APP_PERIPH_BASE + 0x00000160UL);

  //@@ Engine Layer
  constexpr auto EN_CYLINDER_BASE = (APP_PERIPH_BASE + 0x00000200UL); // max 512 object
  constexpr auto EN_VACUUM_BASE   = (APP_PERIPH_BASE + 0x00000400UL); // max 512 object
  constexpr auto EN_MOTOR_BASE    = (APP_PERIPH_BASE + 0x00000600UL); // max 80 object

  constexpr auto EN_AXIS_BASE = (APP_PERIPH_BASE + 0x00000650UL);     // max 176(256-80) object

  constexpr auto EN_ROLLER_BASE = (APP_PERIPH_BASE + 0x00000700UL);

  constexpr auto EN_IR_BASE      = (APP_PERIPH_BASE + 0x00000750UL);
  constexpr auto EN_BCR_BASE     = (APP_PERIPH_BASE + 0x00000757UL);
  constexpr auto EN_ACMOTOR_BASE = (APP_PERIPH_BASE + 0x00000760UL);
  constexpr auto EN_VSAGENT_BASE = (APP_PERIPH_BASE + 0x00000770UL); //
  constexpr auto EN_MES_BASE     = (APP_PERIPH_BASE + 0x00000780UL);

  //@@ Control Layer
  constexpr auto CN_MODULE_BASE = (APP_PERIPH_BASE + 0x00001000UL);

  
  constexpr auto CN_TASK_BASE = (APP_PERIPH_BASE + 0x00001100UL);


  //@@ Application Programming Interface Layer
  constexpr auto API_MODULE_BASE = (APP_PERIPH_BASE + 0x00002000UL);

  //
  constexpr auto MFC_UI_BASE = (APP_PERIPH_BASE + 0x10000000UL);

  //---------------------------------------------------------------------
  //---------------------------------------------------------------------
  //#####################################################################
}
// namespace apcr

//-----------------------------------------------------------------------
//MARK: override operator
//-----------------------------------------------------------------------
inline constexpr unsigned char operator""_byte(unsigned long long value)
{
  return static_cast<uint8_t>(value);
}

inline constexpr unsigned int operator""_ms(unsigned long long value)
{
  return static_cast<unsigned int>(value);
}

inline constexpr unsigned int operator""_um(unsigned long long value)
{
  return static_cast<unsigned int>(value);
}

inline constexpr unsigned int operator""_mm(unsigned long long value)
{
  return static_cast<unsigned int>(value);
}

inline constexpr double operator""_mm(long double value)
{
  return static_cast<double>(value);
}

inline constexpr double operator""_rpm(long double value)
{
  return static_cast<double>(value);
}

//template <typename Type>
//  requires mcc::str_or_wstr_type_c<Type>
//Type double2str(double val, int digit = 2)
//{
//  int32_t wrt_val = static_cast<int32_t>(val * 1000.0f); //
//  Type    ret{};
//  if constexpr (std::is_same_v < Type, std::string>)
//  {
//    ret = std::to_string(wrt_val / 1'000) + "." + std::to_string(std::abs(wrt_val % 1'000));
//    // '.'이후 digit 만큼 남긴다
//    ret = ret.substr(0, ret.find('.') + digit + 1);
//  }
//  else if constexpr (std::is_same_v<Type, std::wstring>)
//  {
//    std::wstring str_f = std::to_wstring(wrt_val / 1'000);
//    std::wstring str_r = std::to_wstring(std::abs(wrt_val % 1'000));
//
//    ret = str_f + L"." + str_r;
//    ret = ret.substr(0, ret.find('.') + digit + 1);
//  }
//  return ret;
//}

template <typename Type>
  requires mcc::str_or_wstr_type_c<Type>
Type double2str(double val, int digit = 2)
{
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(digit) << val;

  if constexpr (std::is_same_v<Type, std::string>)
  {
    return oss.str();
  }
  else if constexpr (std::is_same_v<Type, std::wstring>)
  {
    std::wostringstream woss;
    woss << std::fixed << std::setprecision(digit) << val;
    return woss.str();
  }
}

// auto float2str = [](double val, uint8_t digit = 2)->std::wstring
// 	{
// 		int32_t wrt_val = static_cast<int32_t>(val * 1000.0f); //
// 		std::wstring ret{};
// 		ret = std::to_wstring(wrt_val / 1'000) + L"." + std::to_wstring(std::abs(wrt_val % 1'000));
// 		// '.'이후 digit 만큼 남긴다
// 		ret = ret.substr(0, ret.find('.') + digit + 1);
// 		return ret;
// 	};

#endif // __AP_DEF_TYPE_HPP__