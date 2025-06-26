/**
 * @file uart.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * -4/16 add mode blocking
 * @version 0.1
 * @date 2024-05-20 parity check active
 * 
 * 5/27 :  A callback function used for a temporary purpose, which will be used for less than a minute.
 * 
 * 11/18:    tiny 프렝임워크에서 사용 할 수 있도록 수정
 * 11/21:    read/ write 함수 : non-block으로 변경
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once
#ifndef HW_UART_HPP
#define HW_UART_HPP


#include "ap_def.hpp"

namespace apcr
{

#ifdef _USE_HW_UART

  class serial_comm : public IComm
  {
  public:
   static constexpr auto def_buffer_length_max = 4096;
    using data_type = uint8_t;
    using buff_t    = std::array<uint8_t, def_buffer_length_max>;

    struct cfg_t
    {
      std::string port_name{};
      uint32_t    baudrate{};
      bool        is_blocked{};

      cfg_t(const char *s, uint32_t baud, bool block = false) :
      port_name{s},
      baudrate{baud},
      is_blocked{block} {}

      cfg_t()  = default;
      ~cfg_t() = default;

      cfg_t(const cfg_t &rhs)            = default; // copy constructor
      cfg_t &operator=(const cfg_t &rhs) = default; // copy assignment operator
      cfg_t(cfg_t &&rhs)                 = default; // move constructor
      cfg_t &operator=(cfg_t &&rhs)      = default; // move assignment operator

      cfg_t &operator()(std::string port_name, uint32_t baudrate)
      {
        this->port_name = port_name;
        this->baudrate  = baudrate;
        return *this;
      }
    } m_cfg{};

  private:
    // A callback function used for a temporary purpose, which will be used for less than a minute.
    struct use_less_than_1m_cb
    {
    private:
      std::function<int(void *, void *)> _func{};
      uint32_t                           reg_ms{};

    public:
      void set(std::function<int(void *, void *)> func)
      {
        _func  = func;
        reg_ms = tim::millis();
      }

      std::function<int(void *, void *)> get()
      {
        if (tim::millis() - reg_ms > (1'000 * 60))
        {
          _func = nullptr;
          return nullptr;
        }
        return _func;
      }

      void clear() noexcept
      {
        _func = nullptr;
      }

      void operator=(std::function<int(void *, void *)> func)
      {
        set(func);
      }

      int operator()(void *w_parm, void *l_parm)
      {
        if (_func == nullptr)
          return -1;
        return _func(w_parm, l_parm);
      }
    };

    TinyC::Que<data_type>              m_que{def_buffer_length_max};
    HANDLE                             m_hSerial{};
    bool                               m_isOpenPort{};
    std::thread                        m_trd{};
    std::atomic<bool>                  m_stopThread{};
    std::function<int(void *, void *)> m_cb{};

    use_less_than_1m_cb m_temp_cb{};


  public:
    serial_comm(const apcr::attribute_st &f);

    serial_comm()                                  = default; // default constructor
    serial_comm(const serial_comm &rhs)            = default; // copy constructor
    serial_comm &operator=(const serial_comm &rhs) = default; // copy assignment operator
    serial_comm(serial_comm &&rhs)                 = default; // move constructor
    serial_comm &operator=(serial_comm &&rhs)      = default; // move assignment operator

    ~serial_comm();

  private:
    void threadStop();
    void threadRun();
    void threadJob();
    void processData(DWORD dwRead, buff_t &ref_buff);
    errno_t open_port(const char *port_name, uint32_t baud, bool enable_event = false);
    bool closePort();
    std::string wstring_to_string(const std::wstring &wstr, unsigned int codepage = CP_UTF8);
    std::vector<std::string> getSerialPorts();

  public:
    /****************************************************
      *  overriding
      ****************************************************/
    bool write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...) override;

  private:
    void alarm_msg(const char *func, const char *file, int line, int err_no, const char *fmt, ...);

  public:
    /****************************************************
      *  func
      ****************************************************/
    errno_t Init(serial_comm::cfg_t cfg);
    void DeInit();
    bool IsOpen() const;
    errno_t OpenPort();
    uint8_t Read();
    uint32_t Write(uint8_t *p_data, uint32_t length);
    bool Recovery();
    int Available();
    void Flush();
    void Get(uint8_t *ptr_data);
    errno_t reopen_port(bool is_enable);
    void registerShortCallback(std::function<int(void *, void *)> cb);
    void clearShortCallback();

    /****************************************************
      *  IComm overriding
      ****************************************************/
    errno_t Open();
    void Close();
    bool IsConnected() const;
    void registerCallback(std::function<int(void *, void *)> cb);
    int SendData(const char *ptr_data, size_t length);
    int RecvData(std::vector<char> &rev_v);

#ifdef _USE_HW_CLI
    /****************************************************
      *	test code
      ****************************************************/
    int testCB_func(int argc, char *argv[]);
#endif
  };

  // end of class serial_comm

#endif
  // _USE_HW_UART

}

// end of namespace apcr

#endif // HW_UART_HPP