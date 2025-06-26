/**
 * @file ap_reg.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include "ap.hpp"
#include "ap_reg.hpp"

namespace apcr
{
  //-------------------------------------------------
  //MARK: app register class
  //-------------------------------------------------
#pragma region io class

  void apReg::init()
  {
  }

  void apReg::get()
  {
  }

  constexpr const char *apReg::get_status_str(state_t idx)
  {
    return status_str[static_cast<size_t>(idx)].c_str();
  }

  constexpr const char *apReg::get_option_str(option_t idx)
  {
    return option_str[static_cast<size_t>(idx)].c_str();
  }

#pragma endregion


  //-------------------------------------------------
  //MARK: description class
  //-------------------------------------------------
#pragma region io class

  apDesc::apDesc(const std::string &file_info) // file_info = "path/file_name.json"
  {
    m_file     = file_info;
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

    if (pasing_data() != ERROR_SUCCESS)
    {
      ERR_PRINT("json data loading failed");
      return;
    }

    REPORT_OUT("[OK] success to open file_info : %s ", str_fname.c_str());
  }

  errno_t apDesc::pasing_data()
  {
    TinyC::Json::m_objects;
    for (const auto &obj : TinyC::Json::m_objects)
    {
      auto          sets  = obj.begin();
      std::string   key   = sets->first;
      TinyC::json_t value = sets->second;
      size_t        data_cnt{};
      switch (get_reg_idx(key))
      {
      case data_reg_idx::error:
        data_cnt = m_errdesc(value);
        break;

      case data_reg_idx::menu:
        data_cnt = m_menu.set_descript(value);
        break;

      default:
        break;
      }
      if (data_cnt)
      {
        REPORT_OUT("[OK]  insert %s descript data cnt : %d", key.c_str(), data_cnt);
        data_cnt = 0;
      }
    }

    return 0;
  }

  void apDesc::apply_data()
  {
    using namespace TinyC;

    Json::replace_value(m_errdesc.to_json_obj());
    Json::replace_value(m_menu.to_json_obj());
  }

  TinyC::json_obj_t apDesc::make_json_obj(data_reg_idx idx, TinyC::json_t value)
  {
    TinyC::json_obj_t ret{};
    ret.emplace(EnumToString(idx), value);
    return ret;
  }

  errno_t apDesc::load_data(const std::string &file_info) // file_info = "path/file_name.json"
  {
    m_file = file_info;
    m_objects.clear();
    lines.clear();
    m_isOpened = TinyC::Json::loadFromFile(file_info);
    std::vector<std::string> v_strs{};
    std::string              str_fname{file_info.c_str()};
    if (trans::string_split(str_fname, v_strs, "\\") > 0)
      str_fname = v_strs.back();
    if (m_isOpened == false)
    {
      ERR_PRINT("file_info :%s  is not opened", str_fname.c_str());
      return -1;
    }
    if (m_objects.size() != static_cast<size_t>(data_reg_idx::_max))
    {
      ERR_PRINT("json data is unmatching with enum data_reg_idx::_max");
      return -1;
    }

    if (pasing_data() != ERROR_SUCCESS)
    {
      ERR_PRINT("json data loading failed");
      return -1;
    }

    REPORT_OUT("success to open file_info :%s", str_fname.c_str());
    return ERROR_SUCCESS;
  }

  template <typename TypeStr>
    requires mcc::str_or_wstr_type_c<TypeStr>
  TypeStr apDesc::err_to_string(uint32_t addr, descript_t::language_t trans)
  {
    if (m_isOpened == false)
    {
      LOG_OUT("No a file loaded");
      return TypeStr{};
    }

    if (m_errdesc._data_map[trans].find(addr) == m_errdesc._data_map[trans].end())
    {
      LOG_OUT("No a address [%d]", addr);
      return TypeStr{};
    }
    if constexpr (std::is_same_v<TypeStr, std::string>)
    {
      return m_errdesc._data_map[trans][addr];
    }
    else // if constexpr (std::is_same_v<TypeStr, std::wstring>)
    {
      return mcc::conversion::string_to_wstring(m_errdesc._data_map[trans][addr]);
    }
    return TypeStr{};
  }

  template <typename TypeStr>
    requires mcc::str_or_wstr_type_c<TypeStr>
  TypeStr apDesc::menu_to_string(uint32_t addr, descript_t::language_t trans)
  {
    if (m_isOpened == false)
    {
      LOG_OUT("No a file loaded");
      return TypeStr{};
    }

    if (m_menu._data_map[trans].find(addr) == m_menu._data_map[trans].end())
    {
      LOG_OUT("No a address [%d]", addr);
      return TypeStr{};
    }
    if constexpr (std::is_same_v<TypeStr, std::string>)
    {
      return m_menu._data_map[trans][addr];
    }
    else // if constexpr (std::is_same_v<TypeStr, std::wstring>)
    {
      return mcc::conversion::string_to_wstring(m_menu._data_map[trans][addr]);
    }
    return TypeStr{};
  }

  bool apDesc::SaveToFile()
  {
    if (m_isOpened == false)
    {
      LOG_PRINT("No a file loaded");
      return false;
    }
    apply_data();
    return TinyC::Json::saveToFile(m_file);
  }

  std::string apDesc::toJsonString() const
  {
    if (m_isOpened == false)
    {
      LOG_PRINT("No a file loaded");
      return std::string{};
    }
    return TinyC::Json::toJsonString();
  }

  void apDesc::replace_data()
  {
    apply_data();
  }

#pragma endregion
}
// end of namespace apcr