/**
 * @file socket.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "ap.hpp"
 #include "socket.hpp"


using namespace apcr;

#ifdef _USE_HW_SOCKET

#define templateSocketClass template <typename ClassType> requires has_socket_mode_c<ClassType>

templateSocketClass
NetComm<ClassType>::NetComm(const apcr::attribute_st &f) :
apcr::IObj(f)
{
}

templateSocketClass
NetComm<ClassType>::~NetComm()
{
  if (m_isOpenPort)
  {
    Close();
  }
}

templateSocketClass
bool NetComm<ClassType>::write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...)
{
  if (fmt)
  {
    va_list args;
    va_start(args, fmt);
    std::vector<char> buffer(1024);
    vsnprintf(buffer.data(), buffer.size(), fmt, args);
    std::string message{buffer.data()};
    if (m_lastlog != message)
    {
      m_lastlog = message;
      writeLog(level, log.func_name.c_str(), log.file.c_str(), log.line_no, buffer.data());
    }
    va_end(args);
    return true;
  }
  return false;
}

templateSocketClass
bool NetComm<ClassType>::recovery()
{
  if constexpr (std::is_same_v<ClassType, TcpClient>)
  {
    m_instance->Close();
    //return m_instance->InitSocket(m_cfg.ip_str, m_cfg.port_no) && m_instance->Connect();
    if (m_instance->InitSocket(m_cfg.ip_str, m_cfg.port_no))
      if (m_instance->Connect())
        return true;
  }
  return false;
}

templateSocketClass
void NetComm<ClassType>::threadStop()
{
  m_liftThread = false;
  if (m_trd.joinable())
    m_trd.join();
}

templateSocketClass
void NetComm<ClassType>::threadJob()
{
  std::string recv_str{};
  constexpr std::size_t Count     = 1024;
  constexpr std::size_t ByteCount = sizeof(char) * Count + 1024;

  char buffer[ByteCount]{}; // stack memory
  // std::pmr::monotonic_buffer_resource pool{buffer, ByteCount};
  Pool_t                 pool{buffer, ByteCount};
  std::pmr::vector<char> ret_buf{&pool};
  std::vector<char>      rec_v{};
  m_instance->Receive(rec_v);
  if (rec_v.size() > 0)
  {
    {
      // TODO:
      // ping echo 문자는 콜백함수로 보내지 않는다.
      if (std::string{rec_v.data(), rec_v.size()} != "ping")
      {
        if (m_cb)
        {
          int size = (int)rec_v.size();
          m_cb(&size, rec_v.data());
        }
        else
        {
          for (const char &elm : rec_v)
          {
            //  m_Que.Put((uint8_t)elm);
          }
        }
      }
    }

    apcr::log_info_t data{__FILE__, __FUNCTION__, __LINE__};
    write_log(TinyC::Log::level::lvl_info, data, "received data : %s! \n", std::string{rec_v.begin(), rec_v.end()}.c_str());
    // std::cout << "received data : " << std::string{rec_v.begin(), rec_v.end()} <<std::endl;
  }
}

templateSocketClass
void NetComm<ClassType>::threadRun()
{
  std::cout << "\t> Thread Start!" << std::endl;
  m_liftThread = true;

  if constexpr (std::is_same_v<ClassType, TcpClient>)
  {
    while (m_liftThread)
    {
      threadJob();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  else if constexpr (std::is_same_v<ClassType, TcpServer>)
  {
    while (m_liftThread)
    {
      m_instance->registerCallback(m_cb);
      m_instance->Run();
    }
  }
  else if constexpr (std::is_same_v<ClassType, UdpClient>)
  {
    while (m_liftThread)
    {
      threadJob();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  else if constexpr (std::is_same_v<ClassType, UdpServer>)
  {
    while (m_liftThread)
    {
      m_instance->registerCallback(m_cb);
      m_instance->Run();
    }
  }
  std::cout << "\t> Thread Terminatet!" << std::endl;
}

templateSocketClass
std::string NetComm<ClassType>::GetMode() const
{
  switch (m_SocketMode)
  {
  case NetMode_e::TCP_Server: return "TCP_Server";
  case NetMode_e::TCP_Client: return "TCP_Client";
  case NetMode_e::UDP_Server: return "UDP_Server";
  case NetMode_e::UDP_Client: return "UDP_Client";
  default:                    return "";
  }
}

templateSocketClass
bool NetComm<ClassType>::InitSocket(cfg_t cfg)
{
  m_cfg = cfg;

  apcr::log_info_t data{__FILE__, __FUNCTION__, __LINE__};
  if (m_instance->InitSocket(cfg.ip_str, cfg.port_no))
  {
    write_log(TinyC::Log::level::lvl_info, data, "init success!\n");

    REPORT_OUT("[OK] cnet[%s]::init Success!", attribute.obj_name.c_str());
    return true;
  }

  write_log(TinyC::Log::level::lvl_err, data, "init fail!\n");
  ERR_PRINT("cnet[%s]::init fail!", attribute.obj_name.c_str());
  return false;
}

templateSocketClass
bool NetComm<ClassType>::Connect(bool is_block)
{
  m_isBlocked = is_block;
  if constexpr (std::is_same_v<ClassType, TcpServer> || std::is_same_v<ClassType, UdpServer>)
  {
    if (m_instance->OpenServer())
    {
      m_isOpenPort = true;
      m_trd        = std::thread(&NetComm::threadRun, this);
      return true;
    }
  }
  else if constexpr (std::is_same_v<ClassType, TcpClient> || std::is_same_v<ClassType, UdpClient>)
  {
    bool ret = m_instance->Connect();
    if (ret)
      m_isOpenPort = true;
    if (!m_liftThread && !is_block)
      m_trd = std::thread(&NetComm::threadRun, this);
    return ret;
  }
  return false;
}

templateSocketClass
bool NetComm<ClassType>::IsBlocked() const
{
  return m_isBlocked;
}

templateSocketClass
errno_t NetComm<ClassType>::Open()
{
  return Connect() ? ERROR_SUCCESS : -1;
}

templateSocketClass
void NetComm<ClassType>::Close()
{
  m_instance->Close();
  threadStop();
}

templateSocketClass
bool NetComm<ClassType>::IsConnected() const
{
  if constexpr (std::is_same_v<ClassType, TcpServer>)
    return m_instance->IsClientConnected();
  else
    return m_isOpenPort;
}

templateSocketClass
void NetComm<ClassType>::registerCallback(std::function<int(void *, void *)> cb)
{
  m_cb = cb;
}

templateSocketClass
int NetComm<ClassType>::SendData(const char *ptr_data, size_t length)
{
  std::vector<char> data;
  data.reserve(length);
  for (size_t i = 0; i < length; ++i)
    data.push_back(ptr_data[i]);
  return m_instance->Send(data);
}

templateSocketClass
int NetComm<ClassType>::RecvData(std::vector<char> &rev_v)
{
  if constexpr (std::is_same_v<ClassType, TcpClient> || std::is_same_v<ClassType, UdpClient>)
  {
    m_instance->Receive(rev_v);
    return static_cast<int>(rev_v.size());
  }
  return 0;
}

#ifdef _USE_HW_CLI
templateSocketClass
int NetComm<ClassType>::testCB_func(int argc, char *argv[])
{
  auto get_data = [](char *int_ptr) -> int
  {
    return strtoul(int_ptr, nullptr, 0);
  };

  bool ret = false;
  switch (argc - 1)
  {
  case 1:
  {
    if (argv[1])
    {
      if (argv[1] == "info")
      {
        std::cout << "> net instance address  : [" << this << "] " << std::endl;
        std::cout << "> net mode \t\t: [" << GetMode() << "] " << std::endl;
        std::cout << "> net ip  and port \t: [" << m_cfg.ip_str << "] [" << m_cfg.port_no << "] " << std::endl;
        std::cout << "> net status \t\t: [" << (IsConnected() ? "connected" : "disconnected") << "] " << std::endl;
        ret = true;
      }
      else if (argv[1] == "close")
      {
        Close();
        std::cout << "net close success" << std::endl;
        ret = true;
      }
      else if (argv[1] == "send_test")
      {
        auto pre_ms = tim::millis();
        SendData("test message", 12);
        std::cout << "SendData  [" << tim::millis() - pre_ms << "]ms" << std::endl;
        ret = true;
      }
      else if (argv[1] == "recovery")
      {
        bool recovery_result = recovery();
        if (recovery_result)
        {
          std::cout << "net recovery success" << std::endl;
          ret = true;
        }
        else
        {
          std::cout << "net recovery failed" << std::endl;
          ret = false;
        }
      }
    }
  }
  break;
  case 3:
  {
    if (argv[1] == "open")
    {
      std::string ip_str(argv[2]);
      uint32_t    port = get_data(argv[3]);
      ret              = true;
    }
  }
  break;
  default: break;
  }
  // end of switch
  if (!ret)
  {
    std::cout << attribute.obj_name << " info" << std::endl;
    std::cout << attribute.obj_name << " close" << std::endl;
    std::cout << attribute.obj_name << " recovery" << std::endl;
    std::cout << attribute.obj_name << " send_test" << std::endl;
  }
  return ret ? 0 : -1;
}
#endif

// 명시적 템플릿 인스턴스화
template class NetComm<apcr::UdpClient>;
template class NetComm<apcr::TcpClient>;
template class NetComm<apcr::TcpServer>;
template class NetComm<apcr::UdpServer>;


//template class NetComm<apcr::UdpClient>;

#endif // _USE_HW_SOCKET