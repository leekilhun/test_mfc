/**
 * @file ap_dat.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ap.hpp"
#include "ap_dat.hpp"


using namespace apcr;

namespace ap_data
{
//--------------------------------------------------------------------------------------------------------------
#pragma region cylinder data
  cylinder::cylinder(const std::string &cfg_file)
  {
    m_file     = cfg_file;
    m_isOpened = TinyC::Json::loadFromFile(cfg_file);
    std::vector<std::string> v_strs{};
    std::string              str_fname{cfg_file.c_str()};
    if (trans::string_split(str_fname, v_strs, "\\") > 0)
      str_fname = v_strs.back();
    if (m_isOpened == false)
    {
      ERR_PRINT("cfg_file :%s  is not opened", str_fname.c_str());
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

    /* test */
    // apply_data();
    // SaveToFile();

    REPORT_OUT("[OK] success to open file_info : %s ", str_fname.c_str());
  }

  errno_t cylinder::load_data(const std::string &cfg_file)
  {
    m_file = cfg_file;
    m_objects.clear();
    lines.clear();
    m_isOpened = TinyC::Json::loadFromFile(cfg_file);
    std::vector<std::string> v_strs{};
    std::string              str_fname{cfg_file.c_str()};
    if (trans::string_split(str_fname, v_strs, "\\") > 0)
      str_fname = v_strs.back();
    if (m_isOpened == false)
    {
      ERR_PRINT("cfg_file :%s  is not opened", str_fname.c_str());
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

    REPORT_OUT("success to open file_info : %s ", str_fname.c_str());
    return ERROR_SUCCESS;
  }

  bool cylinder::SaveToFile()
  {
    if (m_isOpened == false)
    {
      LOG_PRINT("No a file loaded");
      return false;
    }

    apply_data();
    return TinyC::Json::saveToFile(m_file);
  }

  std::string cylinder::toJsonString() const
  {
    if (m_isOpened == false)
    {
      LOG_PRINT("No a file loaded");
      return std::string{};
    }
    return TinyC::Json::toJsonString();
  }

  errno_t cylinder::apply_data()
  {
    TinyC::json_obj_t obj;
    for (auto &[key, value] : m_datas)
    {
      constexpr int find_limit_line = elm_t::_max_data + 2;

      obj = value.ret_json_obj();
      if (replace_line_data(key, obj, find_limit_line) == false)
      {
        ERR_PRINT("replace_line_data error");
        return -1;
      }
    }

    return ERROR_SUCCESS;
  }

  cylinder::data_t *cylinder::get_data(INST_IDX::_CYLINDER idx)
  {
    if ((idx + 1) > m_datas.size())
    {
      ERR_PRINT("idx is out of range");
      return nullptr;
    }

    uint32_t id  = apcr::EN_CYLINDER_BASE + idx;
    using data_t = apcr::attribute_st;
    data_t ret   = apcr::get_object_data<data_t>(id);

    return find_data_name(ret.obj_name);
  }

  cylinder::data_t *cylinder::find_data_name(std::string_view name)
  {
    for (size_t i = 0; i < m_datas.size(); ++i)
    {
      if (m_datas[i].name == name)
      {
        return &m_datas[i];
      }
    }
    ERR_PRINT("do not fine a had same name data");
    return nullptr;
  }

  errno_t cylinder::pasing_data()
  {
    for (const auto &obj : TinyC::Json::m_objects)
    {
      auto          sets  = obj.begin();
      std::string   key   = sets->first;
      TinyC::json_t value = sets->second;
      if (value.type == TinyC::json_t::valueType::array)
      {
        auto &arr = std::get<TinyC::json_obj_t>(value.value);
        REPORT_OUT("[OK] register cylinder data : %d ", arr.size());
        for (const auto &elm : arr) //등록한 데이터 만큼 반복한다.
        {
          auto &[key, type_obj] = elm;
          auto &in_values       = std::get<TinyC::json_obj_t>(type_obj.value);

          data_t data{};

          enum
          {
            input_name,
            input_time
          };

          size_t idx{};
          for (auto it = in_values.begin(); it != in_values.end(); ++it)
          {
            if (idx == input_name)
            {
              if (it->first == str_struct_name)
                data.name = std::get<std::string>(it->second.value);
              else
                ERR_PRINT("Wrong input name");
            }
            else
            {
              auto &cfg_tim = std::get<TinyC::json_obj_t>(it->second.value);
              for (const auto &tims : cfg_tim)
              {
                double val = std::get<double>(tims.second.value);
                if (tims.first == str_timeout)
                  data.cfg_times._timeout = val;
                else if (tims.first == str_on_settling)
                  data.cfg_times._on_settling = val;
                else if (tims.first == str_no_sensor_delay)
                  data.cfg_times._no_sensor_delay = val;
              }
            }
            idx++;
          }

          m_datas.emplace_back(data);
        }
      }
      else
      {
        ERR_PRINT("io type is not defined");
        return 0;
      }
    }
    return 0;
  }

  bool cylinder::replace_line_data(std::string_view key, TinyC::json_obj_t &values, int limitline)
  {
    if (TinyC::Json::is_loaded == false)
    {
      std::cerr << "No a file loaded " << std::endl;
      return false;
    }

    for (int i = 0; i < TinyC::Json::lines.size(); i++)
    {
      if (TinyC::Json::lines[i].find(key) != std::string::npos)
      {
        int                      fine_line{};
        std::vector<std::string> capture_lines{};

        for (int j = i; j < (i + limitline); j++)
          capture_lines.emplace_back(TinyC::Json::lines[j]);

        for (auto &[elm_key, elm_value] : values)
        {
          for (auto &line : capture_lines)
          {
            if (line.find(elm_key) != std::string::npos)
            {
              //std::cout << "Found key : " << elm_key << std::endl;
              size_t pos = line.find(":");
              if (pos != std::string::npos)
              {
                std::string       number_str{line.substr(pos + 1)};
                std::stringstream ss{};
                for (int i = 0; i < number_str.size(); ++i)
                {
                  if ((number_str[i] >= '0' && number_str[i] <= '9') // 0 ~ 9
                      || (number_str[i] == '.'
                          || (number_str[0] == '-' || number_str[0] == '+')))
                  {
                    ss << number_str[i];
                  }
                }

                line.replace(line.find(ss.str()), ss.str().size(), std::to_string(std::get<double>(elm_value.value)).substr(0, ss.str().size()));
              }
              else
              {
                ERR_PRINT("Cannot find delimiter ':'");
                return false;
              }

              break;
            }
          }
        }
        /*  changed lines -> origin line */
        for (int j = 0; j < limitline; j++)
          TinyC::Json::lines[i + j] = capture_lines[j];
      }
    }

    return true;
  }

#pragma endregion
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
#pragma region configration data

  appConf::appConf(const std::string &cfg_file)
  {
    m_file     = cfg_file;
    m_isOpened = TinyC::Json::loadFromFile(cfg_file);
    if (m_isOpened == false)
    {
      ERR_PRINT("cfg_file :%s  is not opened", cfg_file.c_str());
      return;
    }


    std::vector<std::string> v_strs{};
    std::string              str_fname{cfg_file.c_str()};
    if (trans::string_split(str_fname, v_strs, "\\") > 0)
      str_fname = v_strs.back();
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

  errno_t appConf::pasing_data()
  {
    TinyC::Json::m_objects;
    for (const auto &obj : TinyC::Json::m_objects)
    {
      auto          sets  = obj.begin();
      std::string   key   = sets->first;
      TinyC::json_t value = sets->second;
      switch (get_reg_idx(key))
      {
      case data_reg_idx::serial_no:
        m_serialno = std::get<int>(value.value);
        break;

      case data_reg_idx::machine_name:
        m_machine_name = std::get<std::string>(value.value);
        break;

      case data_reg_idx::equipment_code:
        m_equipment_code = std::get<std::string>(value.value);
        break;

      case data_reg_idx::common:
      {
        auto &arr = std::get<TinyC::json_obj_t>(value.value);
        for (const auto &elms : arr)
        {
          std::string   key = elms.first;
          TinyC::json_t val = elms.second;
          m_common(key, val);
        }
      }
      break;
      case data_reg_idx::operation:
      {
        auto &arr = std::get<TinyC::json_obj_t>(value.value);
        for (const auto &elms : arr)
        {
          std::string   key = elms.first;
          TinyC::json_t val = elms.second;
          m_operation(key, val);
        }
      }
      break;
      case data_reg_idx::serial_comm:
      {
        auto &arr = std::get<TinyC::json_obj_t>(value.value);
        for (const auto &elm : arr)
        {
          std::string   key = elm.first;
          TinyC::json_t val = elm.second;
          m_serial_comm(key, val);
        }
      }
      break;
      case data_reg_idx::socket_net:
      {
        auto &arr = std::get<TinyC::json_obj_t>(value.value);
        for (const auto &elm : arr)
        {
          std::string   key = elm.first;
          TinyC::json_t val = elm.second;
          m_socket_comm(key, val);
        }
      }
      break;

      default: break;
      }
    }

    return 0;
  }

  void appConf::apply_data()
  {
    using namespace TinyC;

    Json::replace_value(m_serialno.to_json_obj());
    Json::replace_value(m_machine_name.to_json_obj());
    Json::replace_value(m_equipment_code.to_json_obj());
    Json::replace_value(m_common.to_json_obj());
    Json::replace_value(m_operation.to_json_obj());
    Json::replace_value(m_serial_comm.to_json_obj());
    Json::replace_value(m_socket_comm.to_json_obj());
  }

  TinyC::json_obj_t appConf::make_json_obj(data_reg_idx idx, TinyC::json_t value)
  {
    TinyC::json_obj_t ret{};
    ret.emplace(EnumToString(idx), value);
    return ret;
  }

  /****************************************************
	 *	func
	 ****************************************************/
  errno_t appConf::load_data(const std::string &cfg_file) // file_info = "path/file_name.json"
  {
    m_file = cfg_file;

    m_objects.clear();
    lines.clear();
    m_isOpened = TinyC::Json::loadFromFile(cfg_file);

    std::vector<std::string> v_strs{};
    std::string              str_fname{cfg_file.c_str()};
    if (trans::string_split(str_fname, v_strs, "\\") > 0)
      str_fname = v_strs.back();
    if (m_isOpened == false)
    {
      ERR_PRINT("cfg_file :%s  is not opened", str_fname.c_str());
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

    REPORT_OUT("success to open file_info : %s ", str_fname.c_str());
    return ERROR_SUCCESS;
  }

  bool appConf::SaveToFile()
  {
    if (m_isOpened == false)
    {
      LOG_PRINT("No a file loaded");
      return false;
    }
    apply_data();
    return TinyC::Json::saveToFile(m_file);
  }

  std::string appConf::toJsonString() const
  {
    if (m_isOpened == false)
    {
      LOG_PRINT("No a file loaded");
      return std::string{};
    }
    return TinyC::Json::toJsonString();
  }

  void appConf::replace_data()
  {
    apply_data();
  }

#pragma endregion
//--------------------------------------------------------------------------------------------------------------
}
