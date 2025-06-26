/**
 * @file ap_vlog.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-03-24
 * - log server
 * - log client에 대한 interface 제공
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once
#ifndef __AP_VLOG_HPP__
#define __AP_VLOG_HPP__

#include "ap_def.hpp"

namespace apcr
{

  class vLog
  {
    //---------------------------------------------------
    //	data
    //---------------------------------------------------

  public:
    enum err_idx
    {
      err_OK        = 0,
      err_NO_DEFINE = -1,
    };

    struct cfg_t
    {
      net_udp_client_t *ptr_client{};
      std::string       ip_str{};
      int               port{};
      std::string       name{};
      cfg_t()  = default;
      ~cfg_t() = default;

      cfg_t(const cfg_t &other)            = default; // copy constructor
      cfg_t &operator=(const cfg_t &other) = default; // copy assignment
      cfg_t(cfg_t &&other)                 = default; // move constructor
      cfg_t &operator=(cfg_t &&other)      = default; // move assignment
    } m_cfg{};


  private:
    bool       m_isInit{};
    std::mutex m_mutex{};

    std::thread                        m_trd{};
    std::atomic<bool>                  m_stopThread{};
    std::function<int(void *, void *)> m_cb{};

  public:
    using ques_t = std::vector<TinyC::Que<std::string>>;
    ques_t m_qLogs{soi_max};

    //---------------------------------------------------
    //	Constructor
    //---------------------------------------------------

  public:
    vLog() = default;
    ~vLog();

    //---------------------------------------------------
    //	overriding
    //---------------------------------------------------


    //---------------------------------------------------
    //	func
    //---------------------------------------------------

    errno_t Init(const cfg_t &cfg);
    errno_t NetCallback(void *w_parm, void *r_param);
    int Available(step_obj_idx obj);
    bool Put(step_obj_idx obj, const std::string &str);
    bool Get(step_obj_idx obj, std::string *str);
    void Flush(step_obj_idx obj);
    errno_t StartThread();
    errno_t StopThread();
    void ThreadJob();
  private:
    void threadStop();
    void threadRun(void);
    void threadJob(void);

  public:
#ifdef _USE_HW_CLI
    ///////////////////////////////////////////////////////////////////////////////////////

    /// @brief test_callback
    /// @param argc
    /// @param argv
    /// @return
    int testCB_func(int argc, char *argv[]);
#endif
  };
}
// end of namespace apcr

#endif // __AP_VLOG_HPP__