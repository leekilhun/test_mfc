/**
 * @file ap_reg.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-08-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once
#ifndef AP__INC_AP_REG_HPP_
#define AP__INC_AP_REG_HPP_


#include "ap_def.hpp"

namespace apcr
{

  namespace ap_obj
  {
    enum MOTOR : uint8_t
    {
      MOTOR_X,
      MOTOR_MAX,
    };
  }

  class apReg
  {
  public:
    enum virtual_key_e : uint8_t
    {
      vkey_start,
      vkey_stop,
      vkey_reset,
      vkey_max,
    };

    enum state_t : uint8_t
    {
      ST_auto_running,
      ST_emg_stop,
      ST_auto_ready,       // task 수행이 가능한 상태
      ST_auto_stop,        // 초기화 완료 후 정지 상태,
      ST_detect_safe_sensor,
      ST_motor_on,
      ST_system_init_cplt, // motor oringin complete, state init complete
      ST_alarm_status,     // 에러 이벤트가 발생된 상태, 리셋 후 복구

      ST_job_cplt,
      ST_job_fail,
      ST_12,
      ST_13,
      ST_14,
      ST_factory_initial,
      ST_16,
      ST_initializing,

      ST_20,
      ST_21,
      ST_mode_manual,
      ST_mode_auto,
      ST_24,
      ST_25,
      ST_26,
      ST_27,

      ST_30,
      ST_31,
      ST_32,
      ST_33,
      ST_34,
      ST_35,
      ST_36,
      ST_not_defined,

      STATE_max
    };

    inline static std::string status_str[] = {
      "auto_running",
      "emg_stop",
      "auto_ready",
      "auto_stop",
      "on_safe_sensor",
      "motor_on",
      "sys_init_cplt",
      "alarm_status",
      "job_cplt",
      "job_fail",
      "st_12",
      "st_13",
      "st_14",
      "factory_initial",
      "st_16",
      "initializing",

      "st_20",
      "st_21",
      "mode_manaul",
      "mode_auto",
      "st_24",
      "st_25",
      "st_26",
      "st_27",

      "st_30",
      "st_31",
      "st_32",
      "st_33",
      "st_34",
      "st_35",
      "st_36",
      "not_defined"};

    enum option_t : uint8_t
    {
      OP_use_beep,
      OP_use_dryrun,
      OP_not_use_doorsensor,
      OP_not_use_safesensor,
      OP_not_use_estop,
      OP_use_retry,
      OP_06,
      OP_07,

      OP_10,
      OP_11,
      OP_12,
      OP_13,
      OP_14,
      OP_mode_auto,
      OP_not_use_motor,
      OP_not_use_lcd,

      OP_20,
      OP_21,
      OP_22,
      OP_23,
      OP_24,
      OP_25,
      OP_26,
      OP_27,

      OP_30,
      OP_31,
      OP_32,
      OP_33,
      OP_34,
      OP_35,
      OP_36,
      OP_auto_update,

      OPTION_max
    };

    inline static std::string option_str[] = {
      "use_beep",
      "use_dryrun",
      "!_use_doorsensor",
      "!_use_safesensor",
      "!_use_estop",
      "use_retry",
      "opt_06",
      "opt_07",
      "opt_10",
      "opt_11",
      "opt_12",
      "opt_13",
      "opt_14",
      "mode_auto",
      "!_use_motor",
      "!_use_lcd",
      "opt_20",
      "opt_21",
      "opt_22",
      "opt_23",
      "opt_24",
      "opt_25",
      "opt_26",
      "opt_27",
			"opt_30",
			"opt_31",
      "opt_32",
      "opt_33",
      "opt_34",
      "opt_35",
			"opt_36",
      "auto_update",
    };

    udword_t _option{};
    udword_t _state{};

  public:
    /****************************************************
     *	constructor
     ****************************************************/
    apReg() = default;
    ~apReg()  = default;

    void init();
    void get();
    static constexpr const char *get_status_str(state_t idx);
    static constexpr const char *get_option_str(option_t idx);
  };

  //MARK:error/menu descript
  static std::string default_descript_format = R"(

//keep format
{
  "ERROR": {
    "ENGLISH": [
      { "IO_NOT_DEFINED": 100000 },
      { "IO_MODULE_OPEN": 100001 },
      { "IO_OUTPUT_OUT_OF_RANGE": 100002 },      
      { "IO_NO_MATCH": 100003 },
      { "IO_MODULE_NO_RESPONSE": 100004 },      
      { "IO_INVALID_ADDRESS_NUMVER": 100005 },

      { "SERIAL_COMM_NOT_DEFINED": 101000 },
      { "SERIAL_TIMEOUT": 101001 },
      { "SERIAL_INVALID_COMMAND": 101002 },

      { "SOCKET_COMM_NOT_DEFINED": 102000 },
      { "SOCKET_TIMEOUT": 102001 },
      { "SOCKET_INVALID_COMMAND": 102002 },

      { "LIB_NOT_DEFINED": 103000 },
      { "LIB_NO_RESPONSE": 103001 },
      { "LIB_TIMEOUT": 103002 },
      { "LIB_INVALID_COMMAND": 103003 },

      { "CYLINDER_NOT_DEFINED": 200000 },
      { "CYLINDER_TIMEOUT": 200001 },

      { "VACUUM_NOT_DEFINED": 201000 },
      { "VACUUM_TIMEOUT": 201001 },

      { "AXIS_NOT_DEFINED": 203000 },
      { "AXIS_TIMEOUT": 203001 },
      { "AXIS_NOT_READY": 203002 },
      { "AXIS_INTERLOCK": 203003 },

      { "ROLLER_NOT_DEFINED": 204000 },
      { "ROLLER_TIMEOUT": 204001 },

      { "IR_NOT_DEFINED": 205000 },
      { "IR_TIMEOUT": 205001 },

      { "ACM_NOT_DEFINED": 206000 },
      { "ACM_TIMEOUT": 206001 },

      { "MODULE_NOT_DEFINED": 300000 },
      { "IO_MANAGER_NOT_DEFINED": 300001 },
      { "IO_MANAGER_TIMEOUT": 300002 },
      { "OP_NOT_DEFINED": 300003 },
      { "OP_TIMEOUT": 300004 },
      { "AUTO_NOT_DEFINED": 300005 },
      { "AUTO_TIMEOUT": 300006 },
      { "AUTO_NOT_INITIALIZED": 300007 },
      { "AUTO_NOT_SUPPLY_AIR": 300008 },
      { "AUTO_DOOR_OPEND": 300009 },

      { "PORT_NOT_DEFINED": 320000 },
      { "PORT_TIMEOUT": 320001 },
      { "PORT_CYLINDER_TIMEOUT": 320002 },
      { "PORT_AXIS_TIMEOUT": 320003 },

      { "LIFT_NOT_DEFINED": 330000 },
      { "LIFT_TIMEOUT": 330001 },
      { "LIFT_CYLINDER_TIMEOUT": 330002 },
      { "LIFT_AXIS_TIMEOUT": 330003 },

      { "TURN_NOT_DEFINED": 335000 },
      { "TURN_TIMEOUT": 335001 },
      { "TURN_CYLINDER_TIMEOUT": 335002 },
      { "TURN_AXIS_TIMEOUT": 335003 },

      { "TAPING_NOT_DEFINED": 340000 },
      { "TAPING_TIMEOUT": 340001 },
      { "TAPING_CYLINDER_TIMEOUT": 340002 },
      { "TAPING_AXIS_TIMEOUT": 340003 },
      { "TAPING_TAPE_EMPTY": 340004 },

      { "OUT_NOT_DEFINED": 345000 },
      { "OUT_TIMEOUT": 345001 },
      { "OUT_CYLINDER_TIMEOUT": 345002 },
      { "OUT_AXIS_TIMEOUT": 345003 }

    ],
    "KOREA": [
      { "(IO)정의되지 않은 에러": 100000 },
      { "(IO)_MODULE_OPEN": 100001 },
      { "(IO)_OUTPUT_OUT_OF_RANGE": 100002 },      
      { "(IO)_NO_MATCH": 100003 },
      { "(IO)_MODULE_NO_RESPONSE": 100004 },      
      { "(IO)_INVALID_ADDRESS_NUMVER": 100005 },

      { "(SERIAL)정의되지 않은 에러": 101000 },
      { "(SERIAL)_TIMEOUT": 101001 },
      { "(SERIAL)_INVALID_COMMAND": 101002 },

      { "(SOCKET)정의되지 않은 에러": 102000 },
      { "(SOCKET)_TIMEOUT": 102001 },
      { "(SOCKET)_INVALID_COMMAND": 102002 },

      { "(LIB)정의되지 않은 에러": 103000 },
      { "(LIB)_NO_RESPONSE": 103001 },
      { "(LIB)_TIMEOUT": 103002 },
      { "(LIB)_INVALID_COMMAND": 103003 },

      { "(CYLINDER)정의되지 않은 에러": 200000 },
      { "(CYLINDER)_TIMEOUT": 200001 },

      { "(VACUUM)정의되지 않은 에러": 201000 },
      { "(VACUUM)_TIMEOUT": 201001 },

      { "(AXIS)정의되지 않은 에러": 203000 },
      { "(AXIS)_TIMEOUT": 203001 },
      { "(AXIS)_NOT_READY": 203002 },
      { "(AXIS)_INTERLOCK": 203003 },

      { "(ROLLER)정의되지 않은 에러": 204000 },
      { "(ROLLER)_TIMEOUT": 204001 },

      { "(IR)정의되지 않은 에러": 205000 },
      { "(IR)_TIMEOUT": 205001 },

      { "(ACM)정의되지 않은 에러": 206000 },
      { "(ACM)_TIMEOUT": 206001 },

      { "(MODULE)정의되지 않은 에러": 300000 },
      { "IO_MANAGER_NOT_DEFINED": 300001 },
      { "IO_MANAGER_TIMEOUT": 300002 },
      { "OP_NOT_DEFINED": 300003 },
      { "OP_TIMEOUT": 300004 },
      { "AUTO_NOT_DEFINED": 300005 },
      { "AUTO_TIMEOUT": 300006 },
      { "AUTO_NOT_INITIALIZED": 300007 },
      { "AUTO_NOT_SUPPLY_AIR": 300008 },
      { "AUTO_DOOR_OPEND": 300009 },

      { "(PORT)정의되지 않은 에러": 320000 },
      { "(PORT)_TIMEOUT": 320001 },
      { "(PORT)_CYLINDER_TIMEOUT": 320002 },
      { "(PORT)_AXIS_TIMEOUT": 320003 },

      { "(LIFT)정의되지 않은 에러": 330000 },
      { "(LIFT)_TIMEOUT": 330001 },
      { "(LIFT)_CYLINDER_TIMEOUT": 330002 },
      { "(LIFT)_AXIS_TIMEOUT": 330003 },

      { "(TURN)정의되지 않은 에러": 335000 },
      { "(TURN)_TIMEOUT": 335001 },
      { "(TURN)_CYLINDER_TIMEOUT": 335002 },
      { "(TURN)_AXIS_TIMEOUT": 335003 },

      { "(TAPING)정의되지 않은 에러": 340000 },
      { "(TAPING)_TIMEOUT": 340001 },
      { "(TAPING)_CYLINDER_TIMEOUT": 340002 },
      { "(TAPING)_AXIS_TIMEOUT": 340003 },
      { "(TAPING)_TAPE_EMPTY": 340004 },

      { "(OUT)정의되지 않은 에러": 345000 },
      { "(OUT)_TIMEOUT": 345001 },
      { "(OUT)_CYLINDER_TIMEOUT": 345002 },
      { "(OUT)_AXIS_TIMEOUT": 345003 }
    ]
  },
  "MENU": {
    "ENGLISH": [
      { "START": 101 },
      { "STOP": 102 },
      { "RESET": 103 }

    ],
    "KOREA": [
      { "시작": 101 },
      { "정지": 102 },
      { "초기화": 103 }
    ]
  }

}

)";

  static std::string check_make_descript_data(const char *file_name)
  {
    std::filesystem::path data_dir = file::get_data_fold_path();

    if (!std::filesystem::exists(data_dir))
    {
      std::filesystem::create_directory(data_dir);
      ERR_PRINT("Data directory created.");
    }

    std::filesystem::path config_file = data_dir / file_name;

    if (!std::filesystem::exists(config_file))
    {
      std::ofstream out(config_file);
      out << default_descript_format;
      out.close();
      ERR_PRINT("created with default content. [%s]", file_name);
    }
    // else
    // {
    //   std::cout << file_name << " already exists." << std::endl;
    // }

    return config_file.string();
  }

  struct apDesc : public TinyC::Json
  {
  private:

  public:
    enum class data_reg_idx : int
    {
      none = -1,
      error,
      menu,
      _max,
    };

    std::array<std::string, static_cast<int>(data_reg_idx::_max)> elm_name = {"ERROR", "MENU"};

    template <typename T>
      requires std::is_same_v<T, data_reg_idx>
    std::string EnumToString(T value)
    {
      switch (value)
      {
      case T::error: return elm_name[static_cast<int>(data_reg_idx::error)];
      case T::menu:  return elm_name[static_cast<int>(data_reg_idx::menu)];
      }
      return "";
    }

    inline data_reg_idx get_reg_idx(const std::string_view &str)
    {
      if (str == elm_name[static_cast<int>(data_reg_idx::error)])
        return data_reg_idx::error;
      else if (str == elm_name[static_cast<int>(data_reg_idx::menu)])
        return data_reg_idx::menu;


      return data_reg_idx::none;
    }

    /****************************************************
	 *	data
	 ****************************************************/
    template <typename jsonType>
    jsonType get_data(TinyC::json_t::variant_t data)
    {
      if (auto value = std::get_if<jsonType>(&data))
        return jsonType{*value};

      std::cout << "Not json Type" << std::endl;
      return jsonType{};
    }

    struct descript_t
    {
      enum language_t
      {
        english,
        korea,
        _max,
      };

      std::array<std::string, _max>                               elm_name = {"ENGLISH", "KOREA"};
      std::array<std::unordered_map<uint32_t, std::string>, _max> _data_map;

      size_t set_descript(TinyC::json_t objs)
      {
        std::unordered_map<uint32_t, std::string> *maps   = _data_map.data();
        auto                                       insert = [&](TinyC::json_obj_t objs) -> void
        {
          auto &datas = objs;
          for (const auto [k, v] : datas)
          {
            uint32_t err_no = (uint32_t)std::get<int>(v.value);
            maps->insert({err_no, k});
          }
        };

        auto &arr = std::get<TinyC::json_obj_t>(objs.value);
        for (const auto &elms : arr)
        {
          std::string   key = elms.first;
          TinyC::json_t val = elms.second;
          if (val.type == TinyC::json_t::valueType::array)
          {
            if ((key == elm_name[english])
                || (key == elm_name[korea]))
            {
              insert(std::get<TinyC::json_obj_t>(val.value));
            }
          }
          else
          {
            ERR_PRINT("type is not defined");
            return 0;
          }
          maps++;
        }

        int idx{};
       /* for (auto &data : _data_map)
        {
          for (auto &[k, v] : data)
            LOG_OUT("no : %d , name : %s", k, v.c_str());

          LOG_OUT("inset %s description data cnt : %d", elm_name[idx++].c_str(), data.size());
        }*/
        return _data_map.size();
      }

      size_t operator()(TinyC::json_t &objs)
      {
        return set_descript(objs);
      }

      TinyC::json_obj_t to_json_obj() const
      {
        TinyC::json_obj_t obj;
        TinyC::json_obj_t datas;

        for (auto [no, val] : _data_map[english])
          datas.emplace(std::string{val}, TinyC::json_t{(int)no});
        obj.emplace(elm_name[english], TinyC::json_t{datas});

        datas.clear();
        for (auto [no, val] : _data_map[korea])
          datas.emplace(std::string{val}, TinyC::json_t{(int)no});
        obj.emplace(elm_name[korea], TinyC::json_t{datas});

        return obj;
      }
    };

    // end of struct descript_t

    descript_t  m_errdesc{};
    descript_t  m_menu{};
    std::string m_file{};
    bool        m_isOpened{};
    /****************************************************
     *	constructor
     ****************************************************/

  public:
    apDesc(const std::string &file_info) ;

    apDesc()  = default;
    ~apDesc() = default;

    apDesc(const apDesc &other)            = default; // copy constructor
    apDesc &operator=(const apDesc &other) = default; // copy assignment
    apDesc(apDesc &&other)                 = default; // move constructor
    apDesc &operator=(apDesc &&other)      = default; // move assignment


  private:
    errno_t pasing_data();

    void apply_data();

  private:
    TinyC::json_obj_t make_json_obj(data_reg_idx idx, TinyC::json_t value);

    /****************************************************
     *	func
     ****************************************************/

  public:
    errno_t load_data(const std::string &file_info);

    template <typename TypeStr = std::string>
      requires mcc::str_or_wstr_type_c<TypeStr>
    TypeStr err_to_string(uint32_t addr, descript_t::language_t trans = descript_t::english);

    template <typename TypeStr = std::string>
      requires mcc::str_or_wstr_type_c<TypeStr>
    TypeStr menu_to_string(uint32_t addr, descript_t::language_t trans = descript_t::english);

    bool SaveToFile();

    std::string toJsonString() const;

    void replace_data();
  };
}

// end of namespace apcr

#endif /* AP__INC_AP_REG_HPP_ */
