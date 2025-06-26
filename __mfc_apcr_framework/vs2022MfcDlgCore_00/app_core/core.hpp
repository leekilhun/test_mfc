/**
 * @file main.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-11-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#ifndef _CORE_HPP
#define _CORE_HPP

/*  
mcc lib include 

1. "mcc_types.hpp"
2. "mcc_conversion.hpp"
3. "mcc_utils.hpp"  

-> mcc_log, mcc_json, mcc_cli

*/
#include "mcc_types.hpp"
#include "mcc_conversion.hpp"
#include "mcc_utils.hpp"

#include "util/mcc_cli.hpp"
#include "util/mcc_json.hpp"
#include "util/mcc_log.hpp"

#define SySDaTAFiLE_INFO           "system_data.json"
#define SySLOGFiLE_INFO            "system.log"
#define SySLOGFiLE_ERR             "sysError.log"
#define AuTOLOGFiLE_ERR            "autoError.log"

#define __LOGFUNC__                __FUNCTION__
#define SYSLOG_INFO(obj, fmt, ...) TinyC::mcc_log::PutLog(TinyC::mcc_log::lvl_info, obj, (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_PRINT(fmt, ...)        TinyC::mcc_log::WriteLogMsg(TinyC::mcc_log::lvl_info, 0, SySLOGFiLE_INFO, (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define ERR_PRINT(fmt, ...)        TinyC::mcc_log::WriteLogMsg(TinyC::mcc_log::lvl_err, 0, SySLOGFiLE_ERR, (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_OUT(fmt, ...)          TinyC::mcc_log::Log_Print((__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_OUT(fmt, ...)        TinyC::mcc_log::trace_prc((__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define REPORT_OUT(fmt, ...)       TinyC::mcc_log::report_prc((__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)


//MARK:alarm data
/****************************************************
 ****************************************************/
struct alarm_datas_t
{
  uint32_t    object_idx{};
  uint32_t    instance_idx{};
  int         err_no{};
  const char *file{};
  const char *func{};
  int         line{};
  const char *msg{};

  alarm_datas_t()  = default;
  ~alarm_datas_t() = default;

  alarm_datas_t(uint32_t obj, uint32_t inst, int err_no, const char *file, const char *func, const int line, const char *msg) :
  object_idx{obj},
  instance_idx{inst},
  err_no{err_no},
  file{file},
  func{func},
  line{line},
  msg{msg}
  {
  }

  alarm_datas_t(const alarm_datas_t &other)            = default; // copy constructor
  alarm_datas_t &operator=(const alarm_datas_t &other) = default; // copy assignment
};

//MARK:external api interface for mfc callback function
/****************************************************
 ****************************************************/
namespace ext_api
{
  void registerMfcCallback(std::function<int(void *, void *)> cb);

  void ext_alarm(alarm_datas_t &&alarm_msg);
}

//end of namespace ext_api

#endif //_CORE_HPP