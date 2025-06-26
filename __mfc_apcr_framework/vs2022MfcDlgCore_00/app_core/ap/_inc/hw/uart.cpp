/**
 * @file uart.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ap.hpp"
#include "uart.hpp"

using namespace apcr;


#ifdef _USE_HW_UART

serial_comm::serial_comm(const attribute_st &f) :
IObj(f)
{
}

serial_comm::~serial_comm()
{
  if (m_isOpenPort)
    Close();
}

void serial_comm::threadStop()
{
  m_stopThread = true;
  if (m_trd.joinable())
    m_trd.join();
}

void serial_comm::threadRun()
{
  //std::cout << "Thread Start!" << std::endl;
  while (!m_stopThread)
  {
    threadJob();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  //std::cout << "Thread Terminatet!" << std::endl;
}

 // MARK::thread job
void serial_comm::threadJob()
{
  int32_t    length     = 0;
  DWORD      dwRead     = 0;
  OVERLAPPED overlapped = {0};
  overlapped.hEvent     = CreateEvent(NULL, TRUE, FALSE, NULL);
  buff_t arry_buff{};

  if (!overlapped.hEvent) //Handle error
    return;

  uint32_t pre_ms = tim::millis();
  ///////////////////////////////////////////////////////////////////////////////////////
  if (ReadFile(m_hSerial, (LPVOID)arry_buff.data(), (DWORD)def_buffer_length_max, &dwRead, &overlapped))
  {
    if (tim::millis() - pre_ms > 5)
      LOG_OUT("uart read delay : %d ms", tim::millis() - pre_ms);

    // Read completed immediately
    processData(dwRead, arry_buff);
  }
  else
  {
    if (GetLastError() == ERROR_IO_PENDING)
    {
      // 비동기 작업 진행 중, 완료될 때까지 대기
      constexpr DWORD wait_timeout = INFINITE; // 1ms
      DWORD           waitResult   = WaitForSingleObject(overlapped.hEvent, wait_timeout);
      if (waitResult == WAIT_OBJECT_0)
      {
        if (GetOverlappedResult(m_hSerial, &overlapped, &dwRead, FALSE))
          processData(dwRead, arry_buff);
      }
      else if (waitResult == WAIT_TIMEOUT)
      {
        // Handle timeout case
      }
      else
      {
        // Handle other wait results
      }
    }
  }
  CloseHandle(overlapped.hEvent);
}

void serial_comm::processData(DWORD dwRead, buff_t &ref_buff)
{
  if (dwRead)
  {
    // 1분 미만의 짧은 콜백 함수 호출이 필요 시
    if (m_temp_cb.get())
    {
      m_temp_cb(&dwRead, ref_buff.data());
      return;
    }

    if (m_cb && (m_cfg.is_blocked == false))
      m_cb(&dwRead, ref_buff.data());
    else
    {
      for (uint8_t &e : ref_buff)
        m_que.Put(e);
    }
  }
}

errno_t serial_comm::open_port(const char *port_name, uint32_t baud, bool enable_event)
{
  errno_t err_code = 0;

  if (m_isOpenPort)
  {
    threadStop();
    closePort();
  }

  constexpr int SERIAL_COMM_BUFF_MAX = def_buffer_length_max;
  DCB           dcb{};
  COMMTIMEOUTS  timeouts;
  DWORD         dwError;

  char name[MAX_PATH] = {
    0,
  };
  sprintf_s(name, MAX_PATH, "//./%s", port_name);

  m_hSerial = CreateFileA(name, GENERIC_READ | GENERIC_WRITE,
                          0,                                            // 공유 모드 없음
                          NULL,                                         // 기본 보안 속성
                          OPEN_EXISTING,                                // 기존 포트 열기
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 논블록 설정
                          NULL);
  if (m_hSerial == INVALID_HANDLE_VALUE)
  {
    return (errno_t)GetLastError();
  }

  while (1)
  {
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(m_hSerial, &dcb) == FALSE)
      goto gotoErrOut;

    // Set baudrate
    dcb.BaudRate      = (DWORD)baud;
    dcb.ByteSize      = 8;                                    // Data bit = 8bit
    dcb.Parity        = enable_event ? EVENPARITY : NOPARITY; //  parity event
    dcb.StopBits      = ONESTOPBIT;                           // Stop bit = 1
    dcb.fParity       = enable_event ? ODDPARITY : NOPARITY;  //  Parity check
    dcb.fBinary       = 1;                                    // Binary mode
    dcb.fNull         = 0;                                    // Get Null byte
    dcb.fAbortOnError = 0;
    dcb.fErrorChar    = 0;
    // Not using XOn/XOff
    dcb.fOutX = 0;
    dcb.fInX  = 0;
    // Not using H/W flow control
    dcb.fDtrControl     = DTR_CONTROL_DISABLE;
    dcb.fRtsControl     = RTS_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fOutxDsrFlow    = 0;
    dcb.fOutxCtsFlow    = 0;

    if (SetCommState(m_hSerial, &dcb) == FALSE)
      goto gotoErrOut;

    if (SetCommMask(m_hSerial, 0) == FALSE)                                                           // Not using Comm event
      goto gotoErrOut;

    if (SetupComm(m_hSerial, SERIAL_COMM_BUFF_MAX, SERIAL_COMM_BUFF_MAX) == FALSE)                    // Buffer size (Rx,Tx)
      goto gotoErrOut;

    if (PurgeComm(m_hSerial, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE) // Clear buffer
      goto gotoErrOut;

    if (ClearCommError(m_hSerial, &dwError, NULL) == FALSE)
      goto gotoErrOut;

    if (GetCommTimeouts(m_hSerial, &timeouts) == FALSE)
      goto gotoErrOut;

#define NOT_USE_SERIAL_COMM_TIMEOUT

#ifdef NOT_USE_SERIAL_COMM_TIMEOUT
    // Timeout (Not using timeout)
    // Immediatly return
    timeouts.ReadIntervalTimeout         = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.ReadTotalTimeoutConstant    = 1; // must not be zero.
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
#else
    /*
          The maximum time allowed to elapse before the arrival of the next byte on the communications line, in milliseconds.
          If the interval between the arrival of any two bytes exceeds this amount
          */
    timeouts.ReadIntervalTimeout = 50;
    /*
          A constant used to calculate the total time-out period for read operations, in milliseconds
         */
    timeouts.ReadTotalTimeoutConstant = 50;
    /*
           The multiplier used to calculate the total time-out period for read operations, in milliseconds.
          */
    timeouts.ReadTotalTimeoutMultiplier = 10;
    /*
         A constant used to calculate the total time-out period for write operations, in milliseconds.
         */
    timeouts.WriteTotalTimeoutConstant = 50;
    /*
         The multiplier used to calculate the total time-out period for write operations, in milliseconds.
         */
    timeouts.WriteTotalTimeoutMultiplier = 10;
#endif

    if (SetCommTimeouts(m_hSerial, &timeouts) == FALSE)
      goto gotoErrOut;

    EscapeCommFunction(m_hSerial, SETRTS);
    EscapeCommFunction(m_hSerial, SETDTR);
    break;

  gotoErrOut:
    err_code = (errno_t)GetLastError();
    break;
  }

  if (err_code != 0)
  {
    CloseHandle(m_hSerial);
  }
  else
  {
    SetCommMask(m_hSerial, EV_RXCHAR);
    // threadRun();
    m_isOpenPort = true;
    if (m_trd.joinable() == false)
    {
      if (m_cfg.is_blocked == false)
        m_trd = std::thread(&serial_comm::threadRun, this);
    }
  }

  return err_code;
}

bool serial_comm::closePort()
{
  m_isOpenPort = false;
  return CloseHandle(m_hSerial);
}

std::string serial_comm::wstring_to_string(const std::wstring &wstr, unsigned int codepage)
{
  if (wstr.empty())
    return "";

  int str_len = WideCharToMultiByte(
    codepage,
    0, // dwFlag
    wstr.c_str(),
    (int)wstr.size(),
    NULL,
    0,
    NULL,
    NULL);

  // if failed to compute the byte count of the string
  if (str_len == 0)
    return "";

  // prepare a string buffer to
  // hold the converted string
  // do not +1 to str_len,
  // because std::string manages terminating null
  std::string str(str_len, '\0');

  int converted = WideCharToMultiByte(
    codepage,
    0,
    wstr.c_str(),
    (int)wstr.size(),
    &str[0],
    str_len,
    NULL,
    NULL);

  return (converted == 0 ? "" : str);
}

std::vector<std::string> serial_comm::getSerialPorts()
{
  std::vector<std::string> serialPorts;
  wchar_t                  lpTargetPath[5000];
  for (int i = 0; i < 255; i++)
  {
    std::wstring wstr = L"COM" + std::to_wstring(i);
    DWORD        test = QueryDosDeviceW(wstr.c_str(), lpTargetPath, 5000);
    if (test != 0)
    {
      serialPorts.push_back(wstring_to_string(wstr));
    }
  }
  return serialPorts;
}

/****************************************************
 *  overriding
 ****************************************************/
bool serial_comm::write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...)
{
  if (fmt)
  {
    va_list args;
    va_start(args, fmt);
    std::vector<char> buffer(1024);
    vsnprintf(buffer.data(), buffer.size(), fmt, args);
    writeLog(level, log.func_name.c_str(), log.file.c_str(), log.line_no, buffer.data());
    va_end(args);
    return true;
  }

  return false;
}

void serial_comm::alarm_msg(const char *func, const char *file, int line, int err_no, const char *fmt, ...)
{
  if (fmt)
  {
    va_list args;
    va_start(args, fmt);
    std::vector<char> buffer(1024);
    vsnprintf(buffer.data(), buffer.size(), fmt, args);
    writeLog(TinyC::Log::level::lvl_err, func, file, line, buffer.data());
    va_end(args);
    ext_api::ext_alarm(alarm_datas_t{GetObjectIDX().object_idx, GetObjectIDX().instance_idx, err_no, file, func, line, GetLastErrMsg().c_str()});
  }
}

errno_t serial_comm::Init(serial_comm::cfg_t cfg)
{
  m_cfg = cfg;
  apcr::log_info_t data{__FILE__, __FUNCTION__, __LINE__};
  write_log(TinyC::Log::level::lvl_info, data, "init success! \n");

  REPORT_OUT("[OK] uart[%s]::init Success!", m_cfg.port_name.c_str());
  return open_port((const char *)m_cfg.port_name.c_str(), m_cfg.baudrate);
}

void serial_comm::DeInit()
{
  Close();
}

bool serial_comm::IsOpen() const
{
  return m_isOpenPort;
}

errno_t serial_comm::OpenPort()
{
  return open_port((const char *)m_cfg.port_name.c_str(), m_cfg.baudrate);
}

uint8_t serial_comm::Read()
{
  uint8_t ret;
  m_que.Get(&ret);
  return ret; /*m_received_data*/
  ;
}

uint32_t serial_comm::Write(uint8_t *p_data, uint32_t length)
{
  uint32_t ret     = 0;
  DWORD    dwWrite = 0;

  OVERLAPPED overlapped = {0};
  overlapped.hEvent     = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (!overlapped.hEvent)
  {
    // Handle error
    return 0;
  }

  uint32_t pre_ms = tim::millis();
  if (WriteFile(m_hSerial, p_data, (DWORD)length, &dwWrite, &overlapped))
  {
    // Write completed immediately
    // if (tim::millis() - pre_ms > 5)
    //   LOG_OUT("uart send delay : %d ms", tim::millis() - pre_ms);

    ret = dwWrite;
  }
  else
  {
    DWORD error = GetLastError();
    if (error == ERROR_IO_PENDING)
    {
      // Write is still in progress, wait for it to complete
      if (GetOverlappedResult(m_hSerial, &overlapped, &dwWrite, TRUE))
        ret = dwWrite;
      else
      {
        // Handle error
        ret = 0;
      }
    }
    else
    {
      // Handle other errors
      ret = 0;
    }
  }

  CloseHandle(overlapped.hEvent);

  return ret;
}

bool serial_comm::Recovery()
{
  closePort();
  return (OpenPort() == ERROR_SUCCESS);
}

int serial_comm::Available()
{
  return m_que.Available();
}

void serial_comm::Flush()
{
  constexpr int                             SERIAL_COMM_BUFF_MAX = def_buffer_length_max;
  DWORD                                     dwRead               = 0;
  std::array<uint8_t, SERIAL_COMM_BUFF_MAX> arry_buff{};

  ReadFile(m_hSerial, (LPVOID)arry_buff.data(), (DWORD)SERIAL_COMM_BUFF_MAX, &dwRead, NULL);
  if (Available() > 0)
    m_que.Flush();
}

void serial_comm::Get(uint8_t *ptr_data)
{
  m_que.Get(ptr_data);
}

errno_t serial_comm::reopen_port(bool is_enable)
{
  return open_port((const char *)m_cfg.port_name.c_str(), m_cfg.baudrate, is_enable);
}

void serial_comm::registerShortCallback(std::function<int(void *, void *)> cb)
{
  m_temp_cb = cb;
}

void serial_comm::clearShortCallback()
{
  m_temp_cb.clear();
}

/****************************************************
      *  IComm overriding
      ****************************************************/

errno_t serial_comm::Open()
{
  return OpenPort();
}

void serial_comm::Close()
{
  threadStop();
  closePort();
}

bool serial_comm::IsConnected() const
{
  return IsOpen();
}

void serial_comm::registerCallback(std::function<int(void *, void *)> cb)
{
  m_cb = cb;
}

int serial_comm::SendData(const char *ptr_data, size_t length)
{
  return (int)Write((uint8_t *)ptr_data, (uint32_t)length);
}

int serial_comm::RecvData(std::vector<char> &rev_v)
{
  int32_t length = 0;
  DWORD   dwRead = 0;

  buff_t arry_buff{};

  if (ReadFile(m_hSerial, (LPVOID)arry_buff.data(), (DWORD)def_buffer_length_max, &dwRead, NULL) == TRUE)
  {
    while (dwRead)
    {
      rev_v.emplace_back(arry_buff[length]);

      ++length;
      --dwRead;
    }
    return length;
  }
  return 0;
}


#ifdef _USE_HW_CLI
/****************************************************
      *	test code
      ****************************************************/
int serial_comm::testCB_func(int argc, char *argv[])
{
  auto get_data = [](char *int_ptr) -> int
  {
    int ret = 0;
    ret     = (int)strtoul((const char *)int_ptr, (char **)NULL, (int)0);
    return ret;
  };
  std::string name{argv[0]};
  bool        ret{};

  enum : int
  {
    arg_cnt_0,
    arg_cnt_1,
    arg_cnt_2,
    arg_cnt_3
  };

  switch ((argc - 1))
  {
  case arg_cnt_1:
  {
    if (argv[1])
    {
      if (std::string(argv[1]).compare("info") == 0)
      {
        std::cout << "info [" << this << "] success " << std::endl;
        ret = true;
      }
      else if (std::string(argv[1]).compare("search") == 0)
      {
        std::vector<std::string> ports = this->getSerialPorts();
        if (ports.empty())
          std::cout << "search success but not searched any port" << std::endl;
        else
        {
          for (const auto &elm : ports)
          {
            std::cout << elm << std::endl;
          }
        }
        ret = true;
      }
      else if (std::string(argv[1]).compare("close") == 0)
      {
        this->Close();
        std::cout << "close success" << std::endl;
        ret = true;
      }
    }
  }
  break;
  case arg_cnt_2:
  {
  }
  break;
  case arg_cnt_3:
  {
    if (argv[1])
    {
      if (std::string(argv[1]).compare("open") == 0)
      {
        std::string port_name(argv[2]);
        uint32_t    baudrate = (uint32_t)get_data(argv[3]);

        if (this->open_port((const char *)port_name.c_str(), baudrate) == ERROR_SUCCESS)
          std::cout << "open [" << argv[2] << "] [" << argv[3] << "] success" << std::endl;
        else
          std::cout << "open [" << argv[2] << "] [" << argv[3] << "] failed !" << std::endl;
        ret = true;
      }
    }
  }
  break;
  default:
    break;
  }
  // end of switch

  if (ret)
    return 0;

  std::cout << name << " info" << std::endl;
  std::cout << name << " search" << std::endl;
  std::cout << name << " close" << std::endl;
  std::cout << name << " open [name] [buadrate]" << std::endl;

  return -1;
}
#endif

#endif // _USE_HW_UART