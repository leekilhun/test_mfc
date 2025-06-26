/*****************************************************************//**
 * \file   mcc_log.hpp
 * \brief
 *        '25.01.01 로그 출력 형식 추가
 * \author gns2.lee (gns2.lee@samsung.com)
 * \date   March 2024
 *********************************************************************/


#pragma once
#ifndef _COMMON_MCC_LOG_HPP
#define _COMMON_MCC_LOG_HPP

//-----------------------------------------------------------------------------
// machine control cpp log class
//-----------------------------------------------------------------------------
// include -  mcc_types, mcc_conversion, mcc_utils
// application 개발에 필요한 로그 출력 
//-----------------------------------------------------------------------------
#include "mcc_types.hpp"
#include "mcc_conversion.hpp"
#include "mcc_utils.hpp"


namespace TinyC
{

	// ANSI 색상 코드 정의
constexpr const char* ANSI_TEXT_GREEN_COLOR = "\033[32m";
constexpr const char* ANSI_TEXT_YELLOW_COLOR = "\033[33m";
constexpr const char* ANSI_TEXT_RESET_COLOR = "\033[0m";
constexpr const char* ANSI_TEXT_RED_COLOR = "\033[31m";

constexpr int MCC_LOG_VLIST_BUF_MAX = 1024;
constexpr int MCC_LOG_BUFF_LIST_MAX = 4096;
constexpr const char* mcc_log_def_log_path = "./Log/";

//#define LoG_LoCk(m)   std::lock_guard<std::mutex> safe_lock(m)
#define LoG_LoCk(m)
class mcc_log
{

public:
	enum level
	{
		lvl_info,
		lvl_err,
		lvl_warning,
		lvl_max
	};
	struct dat_st
	{
		std::string level{};
		std::string date{};
		std::string time{};
		std::string message{};
		std::string file{};
		std::string func{};
		uint32_t line_no {};
		uint32_t obj_id {};

    dat_st()                             = default;
    ~dat_st()                            = default;
    dat_st(const dat_st &rhs)            = default; // copy constructor
    dat_st &operator=(const dat_st &rhs) = default; // copy assignment operator
    dat_st(dat_st &&rhs)                 = default; // move constructor
    dat_st &operator=(dat_st &&rhs)      = default; // move assignment operator

		void SetLevel(std::string_view str) { level = str; }
		void SetDate(std::string_view str) { date = str; }
		void SetTime(std::string_view str) { time = str; }
		void SetMsg(std::string_view str) { message = str; }
		void SetFile(std::string_view str) { file = str; }
		void SetFunc(std::string_view str) { func = str; }
		void SetLineNo(uint32_t ln) { line_no = ln; }
		void SetObjId(uint32_t id) { obj_id = id; }

		std::string GetLogMessage() const
		{
			std::string str = tim::NowStr()
				+ "[(" + file + "),(" + func + "),(" + std::to_string(line_no) + ")] "
				+ level + " : " + message;

			return str;
		}
	};
	
  struct data_t
  {
    std::string date;
    std::string func;
    std::string type;
    std::string desc;
  };

	struct LogFileInfo
  {
    std::string filename;
    std::string date;
  };

  using group_logs = std::map<std::string, std::vector<LogFileInfo>>;

	static uint32_t trace_prc_pre_tick;
	static std::mutex log_mutex;
	static TinyC::Que<mcc_log::dat_st> log_table[mcc_log::lvl_max];
  mcc_log()                              = default; // default constructor
  ~mcc_log()                             = default; // default destructor
  mcc_log(const mcc_log &rhs)            = default; // copy constructor
  mcc_log &operator=(const mcc_log &rhs) = default; // copy assignment operator
  mcc_log(mcc_log &&rhs)                 = default; // move constructor
  mcc_log &operator=(mcc_log &&rhs)      = default; // move assignment operator

  template <typename = int>
	void LogPrint(const char* str, ...)
	{
		if (str)
		{
			va_list args;
			va_start(args, str);

			vfprintf(stdout, str, args);

			va_end(args);
		}
	}

	template <typename = int>
	void LogInfo(const char* str, ...)
	{
		if (str)
		{
			va_list args;
			va_start(args, str);

			vfprintf(stdout, str, args);

			va_end(args);
		}
	}

  /**
   * @brief 
   * 
   * @param file_name 
   * @param func_name 
   * @param line_no 
   * @param str 
   * @param ... 
	형식 [date(H:M:S:ms)] [로그 string] [(파일명),(함수),(라인)] 
	정규형식 패턴 - std::regex log_pattern(R"(\[(.*?)\] \[(.*?)\] \[(.*?)\] : (.*))");
	[09:41:38'115] RegisterComponent Result Success!(HL_SERIAL_BASE, uart_bcr) [(hw.cpp),(hw_init),(71)] 
   */
	static void report_prc(const char* file_name, const char* func_name, const int line_no, const char* str, ...);
	

  /*
	형식 [date(H:M:S:ms)] [(파일명),(함수),(라인)] [로그 string]
	정규형식 패턴 - std::regex log_pattern(R"(\[(.*?)\] \[(.*?)\] \[(.*?)\] : (.*))");
	[09:41:38'115] [(hw.cpp),(hw_init),(71)] RegisterComponent Result Success!(HL_SERIAL_BASE, uart_bcr)
	*/
	static void Log_Print(const char* file_name, const char* func_name, const int line_no, const char* str, ...);
	
	static void trace_prc(const char* file_name, const char* func_name, const int line_no, const char* str, ...);
	
	static void trace_prc(const char* str, ...);

	static uint32_t PutLog(level loglevel, const int obj, const char* file, const char* func, const int line, const char* fmt, ...);

	static dat_st GetLog(level loglevel = lvl_info);

	static dat_st PopLog(uint32_t addr, level loglevel = lvl_info);

	static uint32_t AvailableLog(level loglevel = lvl_info);

	static std::string getLogFileName(const std::string& file);

	static void writeToFile(const std::string& file, const std::string& message, bool prepend = false);

	static bool WriteLog(
		level loglevel,
		const int obj,
		const char* file_info,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...);

	static bool WriteLogMsg(
		level loglevel,
		const int obj,
		const char* file_info,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...);


  static errno_t view_log(const char* log_file, std::vector<data_t> &ret);

  static std::vector<data_t> view_log(const char* log_file);



  static errno_t file_group(group_logs &ret);
};
// end of mcc_log


	/* 클래스 객체 */
	class Log
	{
	public:
		enum level
		{
			lvl_info,
			lvl_err,
			lvl_warning,
			lvl_max
		};

	private:
    using tbl_t = std::vector<std::string>;
    std::vector<tbl_t> m_logTable{lvl_max};
    std::mutex         m_mutex{};

  public:
    Log()                          = default;                 // default constructor
    ~Log()                         = default;                 // default destructor
    Log(const Log &rhs)            = default;                 // copy constructor
    Log &operator=(const Log &rhs) = default;                 // copy assignment operator
    Log(Log &&rhs)                 = default;                 // move constructor
    Log &operator=(Log &&rhs)      = default;                 // move assignment operator

  private:
		void writeToFile(const std::string& file, const std::string& message, bool prepend = false);

		std::string getLogFileName(const std::string& file);

		std::string trim_char(const std::string& input, bool is_front = true, const std::string& trim_char = "./");

	public:
		bool WriteLog(
			level loglevel,
			const int obj,  // chagne inst_no
			const char* file_info, //path & log  "uart/uart.log"
			const char* file, // 
			const char* func,
			const int line,
			const char* fmt, ...);


		bool WriteLog(
			level loglevel,
			const int obj,  // chagne inst_no
			const char* file_info, //path & log  "uart/uart.log"
			const char* file, // 
			const char* func,
			const int line,
			std::string_view msg);

		std::string GetLastLogMessage(TinyC::Log::level level = TinyC::Log::level::lvl_err);
  };
}



#endif
// end of _COMMON_MCC_LOG_HPP