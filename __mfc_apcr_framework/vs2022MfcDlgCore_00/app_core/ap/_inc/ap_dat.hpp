/**
 * @file ap_dat.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief
 * @version 0.1
 * @date 2024-05-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#ifndef AP__INC_AP_DAT_HPP
#define AP__INC_AP_DAT_HPP
#include "ap_def.hpp"

namespace ap_data
{

  //------------------------------------------
  //MARK: cylinder
  //------------------------------------------
#pragma region cylinder data
  class cylinder : public TinyC::Json
  {
    static constexpr const char *str_struct_name = "name";
    static constexpr const char *str_struct_time = "time";

    static constexpr const char *str_timeout         = "timeout";
    static constexpr const char *str_on_settling     = "on_settling_time";
    static constexpr const char *str_no_sensor_delay = "no_sensor_delay";

  public:
    /****************************************************
     *	define json data
     ****************************************************/

    /**
     * json 
     */
    enum class data_reg_idx : int
    {
      none = -1,
      cylinder,
      _max,
    };

    struct elm_t
    {
      enum data_e
      {
        timeout,
        on_settling,
        no_sensor_delay,
        _max_data,
      };

      using type_t = double;
      type_t _timeout{0.0};
      type_t _on_settling{0.0};
      type_t _no_sensor_delay{0.0};

      elm_t() = default;

      elm_t(type_t a, type_t b, type_t c) :
      _timeout(a),
      _on_settling(b),
      _no_sensor_delay(c)
      {
      }

      ~elm_t() = default;

      elm_t(const elm_t &rhs)            = default; // copy constructor
      elm_t &operator=(const elm_t &rhs) = default; // copy assignment operator
      elm_t(elm_t &&rhs)                 = default; // move constructor
      elm_t &operator=(elm_t &&rhs)      = default; // move assignment operator

      inline TinyC::json_obj_t ret_json_obj()
      {
        TinyC::json_obj_t ret{};
        ret.emplace(str_timeout, _timeout);
        ret.emplace(str_on_settling, _on_settling);
        ret.emplace(str_no_sensor_delay, _no_sensor_delay);
        return ret;
      }

      inline uint32_t get_ms(data_e data_type)
      {
        if (data_type == data_e::timeout)
          return (uint32_t)(_timeout * 1'000.0);
        else if (data_type == data_e::on_settling)
          return (uint32_t)(_on_settling * 1'000.0);
        else if (data_type == data_e::no_sensor_delay)
          return (uint32_t)(_no_sensor_delay * 1'000.0);
        return 0;
      }
    };

    // end of struct elm_t_t

    struct data_t
    {
      std::string name{};
      elm_t       cfg_times{};

      data_t(const char *name, const elm_t &cfg_times) :
      name(name),
      cfg_times(cfg_times) {};
      data_t(const char *name, elm_t::type_t a, elm_t::type_t b, elm_t::type_t c) :
      name(name),
      cfg_times{a, b, c} {};
      data_t()  = default;
      ~data_t() = default;

      data_t(const data_t &rhs)            = default; // copy constructor
      data_t &operator=(const data_t &rhs) = default; // copy assignment operator
      data_t(data_t &&rhs)                 = default; // move constructor
      data_t &operator=(data_t &&rhs)      = default; // move assignment operator
    };

    std::vector<data_t> m_datas{};

  private:
    bool        m_isOpened{};
    std::string m_file{};

  public:
    /****************************************************
   *  Constructor
   ****************************************************/
    cylinder(const std::string &cfg_file);

    cylinder()  = default;
    ~cylinder() = default;

    /****************************************************
     *	func
     ****************************************************/

  public:
    errno_t     load_data(const std::string &cfg_file);
    bool        SaveToFile();
    std::string toJsonString() const;
    errno_t     apply_data();
    data_t     *get_data(INST_IDX::_CYLINDER idx);
    data_t     *find_data_name(std::string_view name);

  private:
    errno_t pasing_data();
    bool    replace_line_data(std::string_view key, TinyC::json_obj_t &values, int limitline);
  };

  // end of class cylinder_data_t


  static std::string DEFAULT_CYCLINDER_DATA = R"(
// keep format 
{
  "cylinder": [
    {
      "name": "loc_xx_name_1",  // must unique name
      "time": {
        "timeout": 1.000000,
        "on_settling_time": 0.100000,
        "no_sensor_delay": 1.000000
      }
    },
    {
      "name": "loc_xx_name_2",
      "time": {
        "timeout": 1.000000,
        "on_settling_time": 0.100000,
        "no_sensor_delay": 1.000000
      }
    }
  ]
}
  )";

  static std::string check_make_cyl_data_path(const char *file_name)
  {
    std::filesystem::path data_dir = file::get_data_fold_path();

    if (!std::filesystem::exists(data_dir))
    {
      std::filesystem::create_directory(data_dir);
      ERR_PRINT("Data directory created.");
    }

    std::filesystem::path dat_file = data_dir / file_name;

    if (!std::filesystem::exists(dat_file))
    {
      std::ofstream out(dat_file);
      out << DEFAULT_CYCLINDER_DATA;
      out.close();
      ERR_PRINT("io_dat.json created with default content.");
    }
    // else
    // {
    //   std::cout << "dat_file.json already exists." << std::endl;
    // }

    return dat_file.string();
  }
#pragma endregion


  //------------------------------------------
  //MARK: configration data
  //------------------------------------------
#pragma region configration data
 static std::string default_app_cfg_format = R"(
 // keep format
 {
  "serial_no": 3333,
  "machine_name": "new description",
  "equipment_code": "DENO-C-V1",  
  "common": [
    { "PASSWORD": "1234" },
    { "LANGUAGE_IDX": 0 } // 0 = English, 1 = Korean, 
  ],
  "operation": [
    { "KEEP_LOG_DATA_DAYS": 7}
  ],

  "serial_comm": {
    "uart_bcr": [
      { "DESC": "communication with Module" },
      { "PORT_NAME": "COM1" },
      { "BAUDRATE": 115200 }
    ]
  },

  "socket_net":  [
    {
      "net_loger": [
        { "DESC": "loopback loger" },
        { "IP": "127.0.0.1" },
        { "PORT": 6000 },
        { "MODE": "UDP_CLIENT" }
      ]
    }
  ]


}
  )";

  constexpr const char *def_app_cfg_file_name = "app_cfg.json";

  static std::string check_make_app_cfg_data(const char *file_name)
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
      out << default_app_cfg_format;
      out.close();
      ERR_PRINT("created with default content. [%s]", file_name);
      // std::cout << file_name << " created with default content." << std::endl;
    }
    // else
    // {
    // 	std::cout << file_name << " already exists." << std::endl;
    // }

    return config_file.string();
  }

  struct appConf : public TinyC::Json
  {
        // static constexpr const char *str_serial_no      = "serial_no";
    // static constexpr const char *str_machine_name   = "machine_name";
    // static constexpr const char *str_equipment_code = "equipment_code";
    // static constexpr const char *str_common         = "common";
    // static constexpr const char *str_operation      = "operation";
    // static constexpr const char *str_serial_comm    = "serial_comm";
    // static constexpr const char *str_socket_net     = "socket_net";

  public:
   //--------------------------------------------------
   //MARK: define json data
   //--------------------------------------------------
    enum class data_reg_idx : int
    {
      none = -1,
      serial_no,
      machine_name,
      equipment_code,
      common,
      operation,
      serial_comm,
      socket_net,
      _max,
    };

    static inline const char *elm_str[] = {
      "serial_no",
      "machine_name",
      "equipment_code",
      "common",
      "operation",
      "serial_comm",
      "socket_net"};

    template <typename T>
      requires std::is_same_v<T, data_reg_idx>
    static std::string EnumToString(T value)
    {
      if (static_cast<int>(value) >= static_cast<int>(T::_max))
        return ""; // error

      return std::string{elm_str[static_cast<int>(value)]};
    }

    inline data_reg_idx get_reg_idx(const std::string_view &str)
    {
      //for(auto & str : elm_str)
      for (int i = 0; i < static_cast<int>(data_reg_idx::_max); i++)
      {
        if (str == elm_str[i])
          return static_cast<data_reg_idx>(i);
      }

      return data_reg_idx::none;
    }

  //--------------------------------------------------
  //MARK:	data
  //--------------------------------------------------
  template <typename jsonType>
  jsonType get_data(TinyC::json_t::variant_t data)
  {
    if (auto value = std::get_if<jsonType>(&data))
      return jsonType{*value};

    std::cout << "Not json Type" << std::endl;
    return jsonType{};
  }

   //--------------------------------------------------
   //MARK:	making json data structure
   //--------------------------------------------------
    template <typename DataType>
    struct data_t
    {
      using type_t = DataType;
      type_t       _value{};
      data_reg_idx _reg_idx{};
      std::string  _name{};

      data_t() = delete;

      data_t(data_reg_idx reg_idx) :
      _reg_idx(reg_idx)
      {
        _name = EnumToString(_reg_idx);
      };

      type_t &operator=(const type_t value)
      {
        _value = value;
        return _value;
      }

      type_t set(const type_t value)
      {
        _value = value;
        return _value;
      }

      type_t get() const
      {
        return _value;
      }

      TinyC::json_t to_json() const
      {
        return TinyC::json_t{(type_t)_value};
      }

      TinyC::json_obj_t to_json_obj() const
      {
        TinyC::json_obj_t obj;
        obj[_name] = to_json();
        return obj;
      }
    };

    data_t<int>         m_serialno{data_reg_idx::serial_no};
    data_t<std::string> m_machine_name{data_reg_idx::machine_name};
    data_t<std::string> m_equipment_code{data_reg_idx::equipment_code};

    struct common_t
    {
    private:
      static constexpr auto str_password     = "PASSWORD";
      static constexpr auto str_language_idx = "LANGUAGE_IDX";

    public:
      enum class elm_idx
      {
        PASSWORD,
        LANGUAGE_IDX
      };
      std::string _password{};
      int         _language_idx{};

      template <typename ElmType, typename ValueType>
        requires std::is_same_v<ElmType, elm_idx>
      ValueType set(ElmType type, ValueType value)
      {
        if constexpr (std::is_same_v<ValueType, std::string> || std::is_same_v<ValueType, const char *>)
        {
          switch (type)
          {
          case elm_idx::PASSWORD:
            _password = value;
            return value;
          }
        }
        else if constexpr (std::is_same_v<ValueType, int>)
        {
          if (type == elm_idx::LANGUAGE_IDX)
            _language_idx = value;
        }
        return value;
      }

      common_t &operator()(const std::string &label, TinyC::json_t &value)
      {
        if (label == str_password)
          _password = std::get<std::string>(value.value);
        else if (label == str_language_idx)
          _language_idx = std::get<int>(value.value);
        return *this;
      }

      TinyC::json_obj_t to_json_obj() const
      {
        TinyC::json_obj_t obj;
        obj.emplace(str_password, TinyC::json_t{_password});
        obj.emplace(str_language_idx, TinyC::json_t{_language_idx});
        TinyC::json_t elm;
        elm.type  = TinyC::json_t::valueType::array;
        elm.value = obj;
        TinyC::json_obj_t ret;
        ret[EnumToString(data_reg_idx::common)] = elm;
        return ret;
      }

    } m_common{};

    struct operation_t
    {
    private:
      static constexpr auto str_keep_logdata_days = "KEEP_LOG_DATA_DAYS";

    public:
      enum class elm_idx
      {
        KEEP_LOG_DATA_DAYS,
      };

      int _keep_logdata_days{};

      operation_t &operator()(const std::string &label, TinyC::json_t &vlaue)
      {
        if (label == str_keep_logdata_days)
          _keep_logdata_days = std::get<int>(vlaue.value);

        return *this;
      }

      TinyC::json_obj_t to_json_obj() const
      {
        TinyC::json_obj_t obj;
        obj.emplace(str_keep_logdata_days, TinyC::json_t{_keep_logdata_days});
        TinyC::json_t elm;
        elm.type  = TinyC::json_t::valueType::array;
        elm.value = obj;
        TinyC::json_obj_t ret;
        ret[EnumToString(data_reg_idx::operation)] = obj;
        return ret;
      }
    } m_operation{};

    struct serial_comm_t
    {
    private:
      static constexpr auto str_descript  = "DESC";
      static constexpr auto str_port_name = "PORT_NAME";
      static constexpr auto str_baud_rate = "BAUDRATE";

    public:
      enum
      {
        elms_module,
        elms_max
      };

      std::array<std::string, elms_max> elm_name = {"uart_module"};

      struct elm_t
      {
        enum class elm_idx
        {
          DESCRIPTION,
          PORT_NAME,
          BAUD_RATE,
        };
        std::string _descript{};
        std::string _port_name{};
        uint32_t    _baud_rate{};
      };

      std::array<elm_t, elms_max> elms{};

      serial_comm_t &operator()(const std::string &label, TinyC::json_t &value)
      {
        auto set_data = [&](auto idx, const auto &js_data)
        {
          auto &arr = std::get<TinyC::json_obj_t>(js_data.value);
          for (const auto &[key, value] : arr)
          {
            if (key == str_descript)
              elms[idx]._descript = std::get<std::string>(value.value);
            else if (key == str_port_name)
              elms[idx]._port_name = std::get<std::string>(value.value);
            else if (key == str_baud_rate)
              elms[idx]._baud_rate = std::get<int>(value.value);
          }
          return &elms[idx];
        };

        if (label == elm_name[elms_module])
          set_data(elms_module, value);

        return *this;
      }

      TinyC::json_obj_t to_json_obj() const
      {
        TinyC::json_obj_t obj;
        TinyC::json_t     elm;
        elm.type = TinyC::json_t::valueType::array;

        for (size_t i = 0; i < elms_max; i++)
        {
          TinyC::json_obj_t elm_obj;
          elm_obj.emplace(str_descript, TinyC::json_t{elms[i]._descript});
          elm_obj.emplace(str_port_name, TinyC::json_t{elms[i]._port_name});
          elm_obj.emplace(str_baud_rate, TinyC::json_t{(int)elms[i]._baud_rate});
          elm.value = elm_obj;
          obj.emplace(elm_name[i], elm);
        }
        TinyC::json_obj_t ret;
        ret[EnumToString(data_reg_idx::serial_comm)] = obj;
        return ret;
      }


    } m_serial_comm{};

    struct socket_comm_t
    {
    private:
      static constexpr auto str_descript = "DESC";
      static constexpr auto str_ip       = "IP";
      static constexpr auto str_port     = "PORT";
      static constexpr auto str_mode     = "MODE";

    public:
      enum
      {
        _loger,
        _max
      };

      std::array<std::string, _max> elm_name = {"net_loger"};

      struct elm_t
      {
        enum class elm_idx
        {
          DESC,
          IP,
          PORT,
          MODE,
        };
        std::string _descript{};
        std::string _ip{};
        uint32_t    _port{};
        std::string _mode{};
      };

      std::array<elm_t, _max> elms{};

      socket_comm_t &operator()(const std::string &label, TinyC::json_t &value)
      {
        auto set_data = [&](auto idx, const auto &js_data)
        {
          auto &arr = std::get<TinyC::json_obj_t>(js_data.value);
          for (const auto &[key, value] : arr)
          {
            if (key == str_descript)
              elms[idx]._descript = std::get<std::string>(value.value);
            else if (key == str_ip)
              elms[idx]._ip = std::get<std::string>(value.value);
            else if (key == str_port)
              elms[idx]._port = std::get<int>(value.value);
            else if (key == str_mode)
              elms[idx]._mode = std::get<std::string>(value.value);
          }
          return &elms[idx];
        };

        if (label == elm_name[_loger])
          set_data(_loger, value);

        return *this;
      }

      TinyC::json_obj_t to_json_obj() const
      {
        TinyC::json_obj_t obj;
        TinyC::json_t     elm;
        elm.type = TinyC::json_t::valueType::array;

        for (size_t i = 0; i < _max; i++)
        {
          TinyC::json_obj_t elm_obj;
          elm_obj.emplace(str_descript, TinyC::json_t{elms[i]._descript});
          elm_obj.emplace(str_ip, TinyC::json_t{elms[i]._ip});
          elm_obj.emplace(str_port, TinyC::json_t{(int)elms[i]._port});
          elm_obj.emplace(str_mode, TinyC::json_t{elms[i]._mode});
          elm.value = elm_obj;
          obj.emplace(elm_name[i], elm);
        }
        TinyC::json_obj_t ret;
        ret[EnumToString(data_reg_idx::socket_net)] = obj;
        return ret;
      }


    } m_socket_comm{};

    bool        m_isOpened{};
    std::string m_file{};

    //---------------------------------------------------
    //MARK:	constructor and destructor
    //---------------------------------------------------

  public:
    appConf(const std::string &cfg_file);
    appConf()  = default;
    ~appConf() = default;

    appConf(const appConf &other)            = default; // copy constructor
    appConf &operator=(const appConf &other) = default; // copy assignment
    appConf(appConf &&other)                 = default; // move constructor
    appConf &operator=(appConf &&other)      = default; // move assignment

  private:
    errno_t pasing_data();
    void apply_data();
    TinyC::json_obj_t make_json_obj(data_reg_idx idx, TinyC::json_t value);

    /****************************************************
	 *	func
	 ****************************************************/
  public:
    errno_t load_data(const std::string &cfg_file) ;
    bool SaveToFile();
    std::string toJsonString() const;
    void replace_data();
  };

  // end of struct app_cfg
#pragma endregion

}
// end of namespace ap_data

//---------------------------------------------------
//MARK: define use data type
//---------------------------------------------------
struct mcu_data_align_32_t;
using mcudat_t = mcu_data_align_32_t;

//---------------------------------------------------
//MARK:branch status data structure
//---------------------------------------------------
struct mcu_data_align_32_t
{
public:
  enum iodata_idx
  {
    idi_io32_0,
    idi_max,
  };

  enum motor_data_idx
  {
    mdi_x,
    mdi_max,
  };

  enum data_idx
  {
    di_data,
    di_max,
  };

  using iodata_t    = std::array<uint32_t, idi_max>;
  using mt_pulse_t  = std::array<uint32_t, mdi_max>; //mt = motor
  using mt_status_t = std::array<uint32_t, mdi_max>; //mt = motor
  using datas_t     = std::array<uint32_t, di_max>;

  uint32_t reg_state{};
  uint32_t reg_opt{};                                //
  uint32_t reg_err{};                                //

  iodata_t io_X{};                                   //
  iodata_t io_Y{};                                   //

  mt_pulse_t  mt_poses{};
  mt_status_t mt_stats{};
  datas_t     datas{};

  mcu_data_align_32_t()  = default;
  ~mcu_data_align_32_t() = default;

  mcu_data_align_32_t(const mcu_data_align_32_t &rhs)            = default; // copy constructor
  mcu_data_align_32_t &operator=(const mcu_data_align_32_t &rhs) = default; // copy assignment operator
  mcu_data_align_32_t(mcu_data_align_32_t &&rhs)                 = default; // move constructor
  mcu_data_align_32_t &operator=(mcu_data_align_32_t &&rhs)      = default; // move assignment operator

  size_t get_size()
  {
    return sizeof(*this);
  }
};

template <typename TypeDef>
struct ap_mcuData
{
  using io_elm_t     = uint32_t;
  using mtpose_elm_t = uint32_t;
  using mtstat_elm_t = uint32_t;
  using datas_elm_t  = uint32_t;

  TypeDef    m_data{};
  std::mutex m_mutex{};

  ap_mcuData()  = default;
  ~ap_mcuData() = default;

  ap_mcuData(const ap_mcuData &rhs)            = default; // copy constructor
  ap_mcuData &operator=(const ap_mcuData &rhs) = default; // copy assignment operator
  ap_mcuData(ap_mcuData &&rhs)                 = default; // move constructor
  ap_mcuData &operator=(ap_mcuData &&rhs)      = default; // move assignment operator

  TypeDef &set_data(TypeDef &data)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data = data;
    return m_data;
  }

  TypeDef &get_data()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_data;
  }

  udword_t get_state()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return udword_t{m_data.reg_state};
  }

  udword_t get_opt()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return udword_t{m_data.reg_opt};
  }

  udword_t get_err()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return udword_t{m_data.reg_err};
  }

  udword_t get_ioX()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return udword_t{m_data.io_X[0]};
  }

  udword_t get_ioY()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return udword_t{m_data.io_Y[0]};
  }

  mtpose_elm_t get_mt_pose(int idx)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_data.mt_poses[idx];
  }

  mtpose_elm_t get_mt_stat(int idx)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_data.mt_stats[idx];
  }

  datas_elm_t get_mcu_data(int idx)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_data.datas[idx];
  }

  void set_state(uint32_t state)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data.reg_state = state;
  }

  void set_opt(uint32_t opt)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data.reg_opt = opt;
  }

  void set_ioY(uint32_t ioY)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data.io_Y[0] = ioY;
  }

  void set_ioX(uint32_t ioX)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data.io_X[0] = ioX;
  }
};

//MARK: - log_dat
struct logDat
{
  static constexpr uint8_t def_buffer_size_max = 60;
  static constexpr uint8_t header_info         = 0x11;
  static constexpr uint8_t header_warning      = 0x12;
  static constexpr uint8_t header_error        = 0x13;

  struct head_t
  {
    uint8_t header{header_info};
    uint8_t error_no{};
    uint8_t obj_idx{};
    uint8_t step_no{};

    head_t()  = default;
    ~head_t() = default;

    head_t(const head_t &rhs)            = default; // copy constructor
    head_t &operator=(const head_t &rhs) = default; // copy assignment operator
    head_t(head_t &&rhs)                 = default; // move constructor
    head_t &operator=(head_t &&rhs)      = default; // move assignment operator

    void operator()(uint8_t h, uint8_t no, uint8_t id, uint8_t step)
    {
      header   = h;
      error_no = no;
      obj_idx  = id;
      step_no  = step;
    }
  };

  logDat() = default;

  logDat(head_t *h, const char *str)
  {
    log_buff.head = *h;
    std::memcpy(&log_buff.log[0], str, def_buffer_size_max);
  }

  logDat(head_t *h, char *str)
  {
    log_buff.head = *h;
    std::memcpy(&log_buff.log[0], str, def_buffer_size_max);
  }

  ~logDat() = default;

  logDat(const logDat &rhs)            = default; // copy constructor
  logDat &operator=(const logDat &rhs) = default; // copy assignment operator
  logDat(logDat &&rhs)                 = default; // move constructor
  logDat &operator=(logDat &&rhs)      = default; // move assignment operator

  struct dat_t
  {
    using buff_t = std::array<char, def_buffer_size_max>;
    head_t head{};
    buff_t log{};

    dat_t()  = default;
    ~dat_t() = default;

    dat_t(const dat_t &rhs)            = default; // copy constructor
    dat_t &operator=(const dat_t &rhs) = default; // copy assignment operator
    dat_t(dat_t &&rhs)                 = default; // move constructor
    dat_t &operator=(dat_t &&rhs)      = default; // move assignment operator

    inline void init()
    {
      head.header   = header_info;
      head.error_no = 0;
      head.obj_idx  = 0;
      head.step_no  = 0;
      log.fill(0);
    }

    void operator()(uint8_t h, uint8_t no, uint8_t id, uint8_t step, const char *str)
    {
      head.header   = h;
      head.error_no = no;
      head.obj_idx  = id;
      head.step_no  = step;
      std::memcpy(&log[0], str, def_buffer_size_max);
    }

    void operator()(head_t *h, const char *str)
    {
      head = *h;
      std::memcpy(&log[0], str, def_buffer_size_max);
    }
  } log_buff{};

  inline char *GetBuffer()
  {
    return &log_buff.log[0];
  }

  inline uint8_t GetBufferLen() const
  {
    return (uint8_t)log_buff.log.size();
  }
};
#endif // AP__INC_AP_DAT_HPP