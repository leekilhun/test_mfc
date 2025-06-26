/**
 * @file ap_vlog.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ap.hpp"
#include "ap_io.hpp"

namespace apcr
{
  vLog::~vLog()
  {
    threadStop();
  }

  errno_t vLog::Init(const cfg_t &cfg)
  {
    m_cfg    = cfg;
    m_isInit = true;

    apcr::log_info_t              data{__FILE__, __FUNCTION__, __LINE__};
    apcr::net_udp_client_t::cfg_t ucfg{};
    ucfg.ip_str  = m_cfg.ip_str;
    ucfg.port_no = m_cfg.port;
    m_cfg.ptr_client->InitSocket(ucfg);
    m_cfg.ptr_client->registerCallback(std::bind(&vLog::NetCallback, this, std::placeholders::_1, std::placeholders::_2));

    m_cfg.ptr_client->Open();
    return ERROR_SUCCESS;
  }

  errno_t vLog::NetCallback(void *w_parm, void *r_param)
  {
    if (w_parm == nullptr)
      return -1;
    int         length  = *((int *)w_parm);
    std::string ret_str = std::string{(reinterpret_cast<const char *>(r_param)), (size_t)length}; // read data from server (TCP/IP)

    std::cout << ret_str << std::endl;
    return ERROR_SUCCESS;
  }

  int vLog::Available(step_obj_idx obj)
  {
    return m_qLogs[obj].Available();
  }

  bool vLog::Put(step_obj_idx obj, const std::string &str)
  {
    return m_qLogs[obj].Put(str);
  }

  bool vLog::Get(step_obj_idx obj, std::string *str)
  {
    return m_qLogs[obj].Get(str);
  }

  void vLog::Flush(step_obj_idx obj)
  {
    m_qLogs[obj].Flush();
  }

  errno_t vLog::StartThread()
  {
    m_trd = std::thread(&vLog::threadRun, this);
    return err_OK;
  }

  errno_t vLog::StopThread()
  {
    threadStop();
    return err_OK;
  }

  void vLog::ThreadJob()
  {
    threadJob();
  }

  void vLog::threadStop()
  {
    m_stopThread = true;
    if (m_trd.joinable())
      m_trd.join();
  }

  void vLog::threadRun(void)
  {
    // std::cout << "Thread Start!" << std::endl;
    REPORT_OUT("[prc] Thread Start!");
    while (!m_stopThread)
    {
      threadJob();
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    REPORT_OUT("Thread Terminatet!");
  }

  void vLog::threadJob(void)
  {
    //---------------------------------
    //MARK:job
    for (auto &log : m_qLogs)
    {
      // if (log.Available())
      // {
      //   std::string str;
      //   if (log.Get(&str))
      //   {
      //     m_cfg.ptr_client->SendData(str.c_str(), str.size());
      //   }
      // }
    }
  }


#ifdef _USE_HW_CLI
  ///////////////////////////////////////////////////////////////////////////////////////

  /// @brief test_callback
  /// @param argc
  /// @param argv
  /// @return
  int vLog::testCB_func(int argc, char *argv[])
  {
    auto get_data = [](char *int_ptr) -> int
    {
      int ret = 0;
      ret     = (int)strtoul((const char *)int_ptr, (char **)NULL, (int)0);
      return ret;
    };

    auto get_bool = [](char *int_ptr, std::string_view comp) -> bool
    {
      return (std::string(int_ptr).compare(comp) == 0);
    };

    bool ret{};

    enum : int
    {
      arg_cnt_0,
      arg_cnt_1,
      arg_cnt_2,
      arg_cnt_3,
      arg_cnt_4,
      arg_cnt_5,
      arg_cnt_6,
      arg_cnt_7
    };

    switch ((argc - 1))
    {
    case arg_cnt_1:
    {
      if (argv[1])
      {
        if (std::string(argv[1]).compare("info") == 0)
        {
        } //
      }
    }
    break;
    case arg_cnt_2:
    {
      if (argv[1])
      {
      }
    }
    break;
    case arg_cnt_3:
    {
      if (argv[1])
      {
      }
    }
    break;
    case arg_cnt_4:
    {
    }
    break;
    case arg_cnt_5:
    {
      if (argv[1])
      {
      }
    }
    break;
    case arg_cnt_6:
    {
    }
    break;
    case arg_cnt_7:
    {
    }
    break;
    default:
      break;
    }
    // end of switch

    if (ret)
      return 0;

    std::cout << "log info" << std::endl;

    return 0;
  }
#endif
}
// end of namespace apcr