/**
 * @file def_sys.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-04-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once
#ifndef _DEF_SYS_HPP_
#define _DEF_SYS_HPP_

#include "def_type.hpp"

namespace apcr
{

  //--------------------------------------------------------------
  //MARK: system object를 등록한다.
  //--------------------------------------------------------------
  enum class ObjType
  {
    NONE,


    // Hardware Layer
    HL_IO = 100,
		HL_SERIAL, //serial port
		HL_SOCKET,  //tcp server
		HL_CSOCKET, //tcp client
		HL_USOCKET, //udp server
    HL_UCSOCKET, //udp client
		HL_EXTCTRL,
		HL_EXT_LIB,

		// Engine Layer
		EN_CYLINDER = 200,
		EN_VACUUM,
		EN_MOTOR,
		EN_AXIS,


    // Control Layer
    CN_MODULE = 300,

    CN_TASK = 320,

		// API Layer
		API_MODULE = 400,

  };
  // end of enum class ObjType


  //--------------------------------------------------------------
  //MARK: object 데이터 구조, log 데이터 구조
  //--------------------------------------------------------------
  struct attribute_st
  {
    ObjType     obj_type{};
    uint32_t    object_no{};
    uint32_t    instance_no{};
    uint32_t    logKeeping_days{};
    std::string obj_name{};
    std::string logFile_name{};

    attribute_st() = default;

    attribute_st(ObjType objtype, uint32_t objno, uint32_t ins_no, uint32_t keepday, std::string obj_str, std::string log_file) :
    obj_type{objtype},
    object_no{objno},
    instance_no{ins_no},
    logKeeping_days{keepday},
    obj_name{obj_str},
    logFile_name{log_file}
    {
    }

    ~attribute_st() = default;

    attribute_st(const attribute_st &rhs)            = default; // copy constructor
    attribute_st &operator=(const attribute_st &rhs) = default; // copy assignment operator
    attribute_st(attribute_st &&rhs)                 = default; // move constructor
    attribute_st &operator=(attribute_st &&rhs)      = default; // move assignment operator
  };
  // end of struct attribute_st

  struct obj_idx_t
  {
    uint32_t object_idx{};
    uint32_t instance_idx{};
  };
  // end of struct obj_idx_t

  struct IObj
  {
    //-----------------------------------------------
    //MARK:err define
    //-----------------------------------------------
    enum err_t
    {
      err_success       = 0,
      err_invalid_param = -1,
      err_not_found     = -2,
      err_not_instanced = -3,
      err_unknown       = -100,
    };

    static constexpr const char *err_str[] = {
      "sucess",
      "invalid param",
      "not found",
      "not instanced",
      "unknown"
    };


    using log_t = TinyC::Log;
    attribute_st attribute{};

    /** Log */
    std::unique_ptr<log_t> log{};

    IObj(const attribute_st &f) :
    attribute{f},
    log{std::make_unique<log_t>()} {}

    IObj()          = default;
    virtual ~IObj() = default;

    IObj(const IObj &rhs)            = default; // copy constructor
    IObj &operator=(const IObj &rhs) = default; // copy assignment operator
    IObj(IObj &&rhs)                 = default; // move constructor
    IObj &operator=(IObj &&rhs)      = default; // move assignment operator

    inline bool writeLog(TinyC::Log::level level, const char *func, const char *file, const int line, const char *fmt, ...) const
    {
      bool    ret{};
      va_list args;
      va_start(args, fmt);
      constexpr uint32_t max_size = 512;
      std::string        message(max_size, '\0');
      int                needed = vsnprintf(&message[0], max_size, fmt, args);
      message.resize(needed);
      va_end(args);


      int obj_no = attribute.object_no + attribute.instance_no;
      ret        = log->WriteLog(level, obj_no, attribute.logFile_name.c_str(), file, func, line, message);

      return ret;
    }

    inline std::string GetLastErrMsg(void)
    {
      std::string log_msg = log->GetLastLogMessage(TinyC::Log::level::lvl_err);
      size_t      start   = log_msg.find('[');
      size_t      end     = log_msg.find(']');
      // 대괄호가 없는 경우 전체 문자열을 반환
      if (start == std::string::npos || end == std::string::npos || end <= start)
      {
        return log_msg; // 대괄호가 없거나 잘못된 경우
      }

      // 대괄호를 제외한 부분 추출
      return log_msg.substr(0, start) + log_msg.substr(end + 1);
    }

    inline obj_idx_t GetObjectIDX(void)
    {
      obj_idx_t ret{attribute.object_no, attribute.instance_no};
      return ret;
    }
  };
  // end of struct IObj

  template <typename P>
  concept obj_c = std::same_as<P, ObjType>;

  template <typename DataType>
    requires obj_c<DataType>
  struct ObjDataDef_t
  {
    DataType    dataType{};
    uint32_t    baseAddr{};
    uint32_t    instanceNo{};
    std::string objName{};
    std::string logFileName{};
    IObj       *ptrObj{};
    uint32_t    logKeepingDays{};
    bool        is_Reg{};

    ObjDataDef_t() = default;
    ObjDataDef_t(DataType t) :
    dataType{t},
    baseAddr{},
    instanceNo{},
    objName{},
    logFileName{},
    ptrObj{},
    logKeepingDays{},
    is_Reg{false} {};
    ObjDataDef_t(DataType t, uint32_t addr, uint32_t i_no, const char *name, const char *l_name) :
    dataType{t},
    baseAddr{addr},
    instanceNo{i_no},
    objName{name},
    logFileName{l_name},
    ptrObj{},
    logKeepingDays{},
    is_Reg{false} {};
    ~ObjDataDef_t() = default;
  };
  // end of Obj_st

  template <typename DataType>
  ObjDataDef_t(DataType t) -> ObjDataDef_t<DataType>;

  struct log_info_t
  {
    std::string file{};
    std::string func_name{};
    int         line_no{};

    //default constructor
    log_info_t(const char *f, const char *n, int line) :
    file{f},
    func_name{n},
    line_no{line} {}

    log_info_t() = default;

    log_info_t(const log_info_t &rhs)            = default; //copy constructor
    log_info_t &operator=(const log_info_t &rhs) = default; //copy assignment operator
    log_info_t(log_info_t &&rhs)                 = default; //move constructor
    log_info_t &operator=(log_info_t &&rhs)      = default; //move assignment operator

    log_info_t &operator()(const char *f, const char *n, int line)
    {
      this->file      = f;
      this->func_name = n;
      this->line_no   = line;
      return *this;
    }
  };
  // end of struct log_info_t

  //----------------------------------------------------------------
  //MARK: common interface
  //----------------------------------------------------------------
  struct Icommon : virtual IObj
  {
    virtual bool write_log(TinyC::Log::level level, log_info_t log, const char *fmt, ...) = 0;
  };
  // end of struct Icommon


}
// end of namespace apcr


#endif // _DEF_SYS_HPP_