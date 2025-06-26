/**
 * @file ap_io.cpp
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
  //-------------------------------------------------
  //MARK: io table class
  //-------------------------------------------------
#pragma region io table class, script data parsing

  io_desc::io_desc(const std::string &file_info) // file_info = "path/file_name.json"
  {
    m_iofile   = file_info;
    m_isOpened = TinyC::Json::loadFromFile(file_info);
    std::vector<std::string> v_strs{};
    std::string              str_fname{file_info.c_str()};
    if (trans::string_split(str_fname, v_strs, "\\") > 0)
      str_fname = v_strs.back();
    if (m_isOpened == false)
    {
      ERR_PRINT("file_info :%s  is not opened", str_fname.c_str());
      return;
    }

    if (m_objects.size() != static_cast<size_t>(data_reg_idx::_max))
    {
      ERR_PRINT("json data is unmatching with enum data_reg_idx::_max");
      return;
    }

    size_t ret = io_dat_descript();
    if (ret == 0)
    {
      ERR_PRINT("json data loading failed - count 0");
      return;
    }

    REPORT_OUT("[OK] success to open file_info : %s ", str_fname.c_str());
  }

  template <typename TypeStr>
    requires mcc::str_or_wstr_type_c<TypeStr> || mcc::ptr_str_or_ptr_wstr_type_c<TypeStr>
  TypeStr io_desc::set_desc(TypeStr desc, uint32_t addr)
  {
    if (m_isOpened == false)
    {
      LOG_OUT("No a file loaded");
      return TypeStr{};
    }
    if (addr < IO::IO_ADDR_BASE_OUTPUT)
    {
      if (m_data_reg_map[static_cast<size_t>(data_reg_idx::input)].find(addr) == m_data_reg_map[static_cast<size_t>(data_reg_idx::input)].end())
      {
        LOG_OUT("No a address [%d]", addr);
        return TypeStr{};
      }

      if constexpr (std::is_same_v<TypeStr, std::string>)
      {
        m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr] = desc;
        return m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr];
      }
      else if constexpr (std::is_same_v<TypeStr, const char *>)
      {
        m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr] = std::string{desc};
        return desc; // m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr];
      }
      else if constexpr (std::is_same_v<TypeStr, const wchar_t *>)
      {
        m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr] = mcc::conversion::utf16_to_utf8(desc);
        return desc;
      }
      else
      {
        std::string descript                                           = mcc::conversion::wstring_to_string(desc);
        m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr] = descript;
        return desc; // mcc::conversion::string_to_wstring(m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr]);
      }
    }
    // case output
    if (m_data_reg_map[static_cast<size_t>(data_reg_idx::output)].find(addr) == m_data_reg_map[static_cast<size_t>(data_reg_idx::output)].end())
    {
      LOG_OUT("No a address [%d]", addr);
      return TypeStr{};
    }

    if constexpr (std::is_same_v<TypeStr, std::string>)
    {
      m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr] = desc;
      return m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr];
    }
    else if constexpr (std::is_same_v<TypeStr, const char *>)
    {
      m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr] = std::string{desc};
      return desc; // m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr];
    }
    else if constexpr (std::is_same_v<TypeStr, const wchar_t *>)
    {
      m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr] = mcc::conversion::utf16_to_utf8(desc);
      return desc;
    }
    else                                                      // if constexpr (std::is_same_v<TypeStr, std::wstring>)
    {
      std::string descript                                            = mcc::conversion::wstring_to_string(desc);
      m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr] = descript;
      return desc;                                            // mcc::conversion::string_to_wstring(m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr]);
    }
    return TypeStr{};
  }

  errno_t io_desc::load_desc_data(const std::string &file_info) // file_info = "path/file_name.json"
  {
    m_iofile   = file_info;
    m_isOpened = TinyC::Json::loadFromFile(file_info);
    if (m_isOpened == false)
    {
      ERR_PRINT("file_info :%s  is not opened", file_info.c_str());
      return -1;
    }
    if (m_objects.size() != static_cast<size_t>(data_reg_idx::_max))
    {
      ERR_PRINT("json data is unmatching with enum data_reg_idx::_max");
      return -1;
    }

    size_t ret = io_dat_descript();
    if (ret == 0)
    {
      ERR_PRINT("json data loading failed - count 0");
      return -1;
    }

    REPORT_OUT("success to open file_info\n   :%s", file_info.c_str());
    return ERROR_SUCCESS;
  }

  template <typename TypeStr>
    requires mcc::str_or_wstr_type_c<TypeStr>
  TypeStr io_desc::to_string(uint32_t addr)
  {
    if (m_isOpened == false)
    {
      LOG_OUT("No a file loaded");
      return TypeStr{};
    }

    if (addr < IO::IO_ADDR_BASE_OUTPUT)
    {
      if (m_data_reg_map[static_cast<size_t>(data_reg_idx::input)].find(addr) == m_data_reg_map[static_cast<size_t>(data_reg_idx::input)].end())
      {
        LOG_OUT("No a address [%d]", addr);
        return TypeStr{};
      }

      if constexpr (std::is_same_v<TypeStr, std::string>)
      {
        return m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr];
      }
      else
      {
        return mcc::conversion::string_to_wstring(m_data_reg_map[static_cast<size_t>(data_reg_idx::input)][addr]);
      }
    }
    // case output
    if (m_data_reg_map[static_cast<size_t>(data_reg_idx::output)].find(addr) == m_data_reg_map[static_cast<size_t>(data_reg_idx::output)].end())
    {
      LOG_OUT("No a address [%d]", addr);
      return TypeStr{};
    }
    if constexpr (std::is_same_v<TypeStr, std::string>)
    {
      return m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr];
    }
    else // if constexpr (std::is_same_v<TypeStr, std::wstring>)
    {
      return mcc::conversion::string_to_wstring(m_data_reg_map[static_cast<size_t>(data_reg_idx::output)][addr]);
    }
    return TypeStr{};
  }

  errno_t io_desc::apply_data()
  {
    // m_data_reg_map[0][1001] = "changed";
    for (size_t i = 0; i < m_data_reg_map.size(); i++)
    {
      TinyC::json_obj_t obj;
      for (auto &[idx, desc] : m_data_reg_map[i])
        obj.emplace(desc, TinyC::json_t{(int)idx});

      TinyC::json_t elm;
      elm.type         = TinyC::json_t::valueType::array;
      elm.value        = obj;
      obj[elm_name[i]] = elm;
      TinyC::Json::replace_value(obj);
    }


    return ERROR_SUCCESS;
  }

  size_t io_desc::io_dat_descript(void)
  {
    //TinyC::Json::m_objects;
    int idx = 0;

    data_reg_t *maps = m_data_reg_map.data();
    for (const auto &obj : TinyC::Json::m_objects)
    {
      auto          elm   = obj.begin();
      std::string   key   = elm->first;
      TinyC::json_t value = elm->second;
      if (value.type == TinyC::json_t::valueType::array)
      {
        auto &arr = std::get<TinyC::json_obj_t>(value.value);
        for (const auto &io_elm : arr)
        {
          io_desc::data_t data{};
          if (key.compare(elm_name[static_cast<int>(data_reg_idx::input)]) == 0)
            data.type = IO::type::in;
          else // if (key.compare("output") == 0)
            data.type = IO::type::out;

          data.name = io_elm.first;
          data.addr = (uint32_t)std::get<int>(io_elm.second.value);

          // m_datas.emplace_back(data);
          maps->insert({data.addr, data.name});
        }
      }
      else
      {
        ERR_PRINT("io type is not defined");
        return 0;
      }
      maps++;
      LOG_PRINT("load complated %s : count (%d)", key.c_str(), std::get<TinyC::json_obj_t>(value.value).size());
    }

    return m_data_reg_map.size();
  }

#pragma endregion

  //-------------------------------------------------
  //MARK: io class
  //-------------------------------------------------
#pragma region io class

  apIO::apIO(const apcr::attribute_st &f) :
  apcr::IObj(f)
  {
  }

  apIO::~apIO()
  {
    threadStop();
  }

  bool apIO::write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...)
  {
    if (fmt)
    {
      va_list args;
      va_start(args, fmt);
      std::vector<char> buffer(1024);
      vsnprintf(buffer.data(), buffer.size(), fmt, args);
      writeLog(level, log.func_name.c_str(), log.file.c_str(), log.line_no, buffer.data()); // message.c_str());
      va_end(args);
      return true;
    }

    return false;
  }

  bool apIO::IsOn(uint32_t addr)
  {
    if (m_regIO.devices.size() == 0)
      return false;

    IIO *ptr_io = m_regIO.get_obj(addr);
    if (ptr_io == nullptr)
    {
      ERR_PRINT("IO object is not registered. addr=%d", addr);
      return false;
    }
    return ptr_io->IsOn(addr);
  }

  bool apIO::IsOff(uint32_t addr)
  {
    return !(IsOn(addr));
  }

  errno_t apIO::OutputOn(uint32_t addr)
  {
    if (m_regIO.devices.size() == 0)
      return err_NO_DEVICES;

    IIO *ptr_io = m_regIO.get_obj(addr);
    if (ptr_io == nullptr)
    {
      ERR_PRINT("IO object is not registered. addr=%d", addr);
      return err_OUT_ADDRESS_RANGE;
    }
    return ptr_io->OutputOn(addr);
  }

  errno_t apIO::OutputOff(uint32_t addr)
  {
    if (m_regIO.devices.size() == 0)
      return err_NO_DEVICES;

    IIO *ptr_io = m_regIO.get_obj(addr);
    if (ptr_io == nullptr)
    {
      ERR_PRINT("IO object is not registered. addr=%d", addr);
      return err_OUT_ADDRESS_RANGE;
    }
    return ptr_io->OutputOff(addr);
  }

  errno_t apIO::OutputToggle(uint32_t addr)
  {
    if (m_regIO.devices.size() == 0)
      return err_NO_DEVICES;

    IIO *ptr_io = m_regIO.get_obj(addr);
    if (ptr_io == nullptr)
    {
      ERR_PRINT("IO object is not registered. addr=%d", addr); //0x%08X
      return -2;
    }
    return ptr_io->OutputToggle(addr);
  }

  errno_t apIO::GetData(void)
  {
    return 0;
  }

  /****************************************************
     *	func
     ****************************************************/
  errno_t apIO::Init(const cfg_t &cfg)
  {
    m_cfg    = cfg;
    m_isInit = true;

    apcr::log_info_t data{__FILE__, __FUNCTION__, __LINE__};
    write_log(TinyC::Log::level::lvl_info, data, "init success  reg_device max [%d]\n", m_cfg.reg_device_max);

    return ERROR_SUCCESS;
  }

  errno_t apIO::StartThread()
  {
    m_trd = std::thread(&apIO::threadRun, this);
    return err_OK;
  }

  errno_t apIO::StopThread()
  {
    threadStop();
    return err_OK;
  }

  void apIO::ThreadJob()
  {
    threadJob();
  }

  void apIO::threadStop()
  {
    m_stopThread = true;
    if (m_trd.joinable())
      m_trd.join();
  }

  void apIO::threadRun(void)
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

  void apIO::threadJob(void)
  {
    for (auto &elm : m_regIO.devices)
    {
      elm.ptr_io->GetData();
    }
  }

  errno_t apIO::AddDevice(device_t &device)
  {
    if (m_regIO.devices.size() >= m_cfg.reg_device_max)
    {
      ERR_PRINT("regist device full !");
      return err_FULL_REGIST_DEVICE;
    }
    ++m_regIO.idx;
    m_regIO.devices.emplace_back(device);

    return ERROR_SUCCESS;
  }

  const char *apIO::get_err_desc(err_idx err)
  {
    switch (err)
    {
    case err_NO_DEVICES:         return "NO DEVICES";
    case err_OUT_ADDRESS_RANGE:  return "OUT OF ADDRESS RANGE";
    case err_FULL_REGIST_DEVICE: return "FULL REGIST DEVICE";
    default:                     break;
    }
    return "Not Define Error";
  }

  int apIO::testCB_func(int argc, char *argv[])
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
          ret = true;
          TinyC::Cli::cliPrintf("registed device cnt :%d\n", m_regIO.devices.size());
          size_t idx{};
          for (auto &dev : m_regIO.devices)
          {
            size_t in_cnt  = hw_io::_in_length(dev.io_type);
            size_t out_cnt = hw_io::_out_length(dev.io_type);
            TinyC::Cli::cliPrintf("[%d] %s :  IN(%d) %d ~ %d, OUT(%d) %d ~ %d \n", idx++, dev.name.c_str(), in_cnt, dev.begin_addr_in, dev.begin_addr_in + (uint32_t)(in_cnt - 1), out_cnt, dev.begin_addr_out, dev.begin_addr_out + (uint32_t)(out_cnt - 1));
          }

        } //
      }
    }
    break;
    case arg_cnt_2:
    {
      if (argv[1])
      {
        if (std::string(argv[1]).compare("is_on") == 0)
        {
          uint32_t addr = (uint32_t)get_data(argv[2]);
          ret           = true;
          TinyC::Cli::cliPrintf("addr[%d] is_on: %d \n", addr, IsOn(addr));
        }
      }
    }
    break;
    case arg_cnt_3:
    {
      if (argv[1])
      {
        if (std::string(argv[1]).compare("set_out") == 0)
        {
          uint32_t addr  = (uint32_t)get_data(argv[2]);
          bool     is_on = (bool)get_data(argv[3]);
          ret            = true;
          if (is_on)
            TinyC::Cli::cliPrintf("addr[%d] set_out: %d \n", addr, OutputOn(addr));
          else
            TinyC::Cli::cliPrintf("addr[%d] set_out: %d \n", addr, OutputOff(addr));
        }
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

    std::cout << "io info" << std::endl;
    std::cout << "io is_on  [addr] " << std::endl;
    std::cout << "io set_out [addr] [1:0]" << std::endl;

    return 0;
  }
#pragma endregion
}
// end of namespace apcr