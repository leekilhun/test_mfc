/**
 * @file ap_io.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief  to manager io device
 * @version 0.1
 * @date 2024-04-11
 *       06/19 - extention io type
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#ifndef AP__INC_AP_IO_HPP_
#define AP__INC_AP_IO_HPP_

#include "ap_def.hpp"

namespace apcr
{
  //------------------------------------------
  //MARK: io table class
  //------------------------------------------
#pragma region io table class, script data parsing

  struct io_desc : public TinyC::Json
  {
  public:
    enum class data_reg_idx : int
    {
      none = -1,
      input,
      output,
      _max,
    };

    using elm_names_t    = std::array<std::string, static_cast<int>(data_reg_idx::_max)>;
    elm_names_t elm_name = {"input", "output"};

    struct data_t
    {
      IO::type    type{};
      std::string name{};
      uint32_t    addr{};
    };

    using data_reg_t  = std::unordered_map<uint32_t, std::string>;
    using data_regs_t = std::array<data_reg_t, static_cast<int>(data_reg_idx::_max)>;
    data_regs_t m_data_reg_map{};

    template <typename T>
      requires std::is_same_v<T, data_reg_idx>
    std::string type_to_string(T value)
    {
      switch (value)
      {
      case T::input:  return elm_name[static_cast<int>(data_reg_idx::input)];
      case T::output: return elm_name[static_cast<int>(data_reg_idx::output)];
      }
      return "";
    }

    std::vector<data_t> m_datas{};
    std::string         m_iofile{};

    bool m_isOpened{};
    /****************************************************
     *	constructor
     ****************************************************/

  public:
    io_desc(const std::string &file_info);
    io_desc()  = default;
    ~io_desc() = default;

    io_desc(const io_desc &other)            = default; // copy constructor
    io_desc &operator=(const io_desc &other) = default; // copy assignment
    io_desc(io_desc &&other)                 = default; // move constructor
    io_desc &operator=(io_desc &&other)      = default; // move assignment

    /****************************************************
     *	func
     ****************************************************/
    template <typename TypeStr = std::string>
      requires mcc::str_or_wstr_type_c<TypeStr> || mcc::ptr_str_or_ptr_wstr_type_c<TypeStr>
    TypeStr set_desc(TypeStr desc, uint32_t addr);

    template <typename TypeStr = std::string>
      requires mcc::str_or_wstr_type_c<TypeStr>
    TypeStr to_string(uint32_t addr);

    errno_t load_desc_data(const std::string &file_info);

    errno_t apply_data();

  private:
    size_t io_dat_descript(void);
  };

#pragma endregion

  //MARK: ap_io
  //****************************************************
	//****************************************************
  class apIO : public IIO
  {
    /****************************************************
     *	data
     ****************************************************/
  public:
    enum err_idx
    {
      err_OK = 0,
      err_NO_DEVICES = -1,
      err_OUT_ADDRESS_RANGE = -2,
      err_FULL_REGIST_DEVICE= -3,
    };

    struct device_t
    {
      IIO             *ptr_io{}; //
      std::string      name{};
      hw_io::io_type_e io_type{};
      uint32_t         begin_addr_in{};
      uint32_t         begin_addr_out{};
      uint32_t         avail_range_in{};
      uint32_t         avail_range_out{};

      device_t()  = default;
      ~device_t() = default;

      device_t(const device_t &other)            = default; // copy constructor
      device_t &operator=(const device_t &other) = default; // copy assignment
      device_t(device_t &&other)                 = default; // move constructor
      device_t &operator=(device_t &&other)      = default; // move assignment

      bool is_valid(uint32_t addr) const
      {
        /*if (io_type == hw_io::io_type_e::InOn)
        {
          return false;
        }*/
        if (addr < (IO::IO_ADDR_BASE_OUTPUT))
        {
          return (addr >= begin_addr_in) && (addr < avail_range_in);
        }
        return (addr >= begin_addr_out) && (addr < avail_range_out);
      }
    };

    struct reg_device_t
    {
      uint32_t              idx{};
      std::vector<device_t> devices{};

      IIO *get_obj(uint32_t addr)
      {
        for (auto &dev : devices)
        {
          if (dev.is_valid(addr))
            return dev.ptr_io;
        }
        ERR_PRINT("device not found");
        return nullptr;
      }

      errno_t operator+(device_t &device)
      {
        ++this->idx;
        this->devices.emplace_back(device);
        return ERROR_SUCCESS;
      }

    } m_regIO{};

    struct cfg_t
    {
      INST_IDX::_IO reg_device_max{};

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
    /****************************************************
     *	Constructor
     ****************************************************/
  public:
    apIO(const apcr::attribute_st &f);
    apIO() = default;
    ~apIO();

    /****************************************************
     *  overriding
     ****************************************************/
  public:
    bool write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...) override;
    bool IsOn(uint32_t addr) override;
    bool IsOff(uint32_t addr) override;
    errno_t OutputOn(uint32_t addr) override;
    errno_t OutputOff(uint32_t addr) override;
    errno_t OutputToggle(uint32_t addr) override;
    errno_t GetData(void) override;

    /****************************************************
     *	func
     ****************************************************/
  public:
    errno_t Init(const cfg_t &cfg);
    errno_t StartThread();
    errno_t StopThread();
    void ThreadJob();
  private:
    void threadStop();
    void threadRun(void);
    void threadJob(void);
  public:
    errno_t AddDevice(device_t &device);
    const char *get_err_desc(err_idx err);

#ifdef _USE_HW_CLI
    ///////////////////////////////////////////////////////////////////////////////////////

    /// @brief test_callback
    /// @param argc
    /// @param argv
    /// @return
    int testCB_func(int argc, char *argv[]);
#endif
  };

  // end of class apIO

}

// end of namespace apcr

#endif // AP__INC_AP_IO_HPP_