/**
 * @file mcc_log.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "mcc_log.hpp"
 namespace TinyC
{
  void mcc_log::report_prc(const char *file_name, const char *func_name, const int line_no, const char *str, ...)
  {
    LoG_LoCk(log_mutex);
    if (str)
    {
      va_list args;
      va_start(args, str);
      std::string filename = std::filesystem::path(file_name).filename().string();

      std::vector<std::string> v_strs{};
      std::string              add_time{};
      std::string              input_str = str;
      std::string              colored_str;

      // Check if string starts with [xxx] pattern
      if (input_str.size() > 2 && input_str[0] == '[' && input_str.find(']') != std::string::npos)
      {
        size_t bracket_end = input_str.find(']');
        colored_str        = ANSI_TEXT_YELLOW_COLOR + input_str.substr(0, bracket_end + 1) + ANSI_TEXT_RESET_COLOR + input_str.substr(bracket_end + 1);
      }
      else
      {
        colored_str = input_str;
      }

      // 기존 코드에서 std::string{str} 부분을 colored_str로 교체
      if (trans::string_split(func_name, v_strs, "::") > 0)
        add_time = ANSI_TEXT_GREEN_COLOR + tim::TimeStr() + ANSI_TEXT_RESET_COLOR + colored_str + ANSI_TEXT_YELLOW_COLOR + "[(" + std::string(filename) + "),(" + v_strs.back() + "),(" + std::to_string(line_no) + ")]" + ANSI_TEXT_RESET_COLOR + "\n";
      else
        add_time = ANSI_TEXT_GREEN_COLOR + tim::TimeStr() + ANSI_TEXT_RESET_COLOR + colored_str + ANSI_TEXT_YELLOW_COLOR + "[(" + std::string(filename) + "),(" + func_name + "),(" + std::to_string(line_no) + ")]" + ANSI_TEXT_RESET_COLOR + "\n";
      vfprintf(stdout, (const char *)add_time.c_str(), args);
      va_end(args);
    }
  }

  void mcc_log::Log_Print(const char* file_name, const char* func_name, const int line_no, const char* str, ...)
	{
		LoG_LoCk(log_mutex);
		if (str)
		{
			va_list args;
      va_start(args, str);
      std::vector<char> buffer(1024);
      vsnprintf(buffer.data(), buffer.size(), str, args);
			std::string filename = std::filesystem::path(file_name).filename().string();
      std::string msg{buffer.data()};
			std::vector<std::string> v_strs{};
      std::string add_time{};
      //if(trans::string_split(func_name, v_strs, "::") > 0)
      //  add_time =  tim::TimeStr() + "[(" + std::string(filename) + "),(" + v_strs.back() + "),(" + std::to_string(line_no) + ")] " + str + "\n";
      //else
      //  add_time = tim::TimeStr() + "[(" + std::string(filename) + "),(" + func_name + "),(" + std::to_string(line_no) + ")] " + str + "\n";

			if(trans::string_split(func_name, v_strs, "::") > 0)
        add_time = tim::TimeStr() + " " + msg + "  [(" + std::string(filename) + "),(" + v_strs.back() + "),(" + std::to_string(line_no) + ")]";
			else
        add_time = tim::TimeStr() + " " + msg + "  [(" + std::string(filename) + "),(" + func_name + "),(" + std::to_string(line_no) + ")]";

			std::cout << add_time << std::endl;

      //vfprintf(stdout, (const char*)add_time.c_str(), args);
			va_end(args);
		}
	}

  void mcc_log::trace_prc(const char* file_name, const char* func_name, const int line_no, const char* str, ...)
	{
		LoG_LoCk(log_mutex);
		if (str)
		{
			va_list args;
			va_start(args, str);
			std::string filename = std::filesystem::path(file_name).filename().string();

			trace_prc_pre_tick = tim::millis() - trace_prc_pre_tick;
			std::vector<std::string> v_strs{};
      std::string add_time{};
      if(trans::string_split(func_name, v_strs, "::") > 0)
        add_time = "[(" + std::string(filename) + "),(" + v_strs.back() + "),(" + std::to_string(line_no) + ")] " + tim::TimeStr()+ "[" + std::to_string(trace_prc_pre_tick) + "] " + str + "\n";
      else
        add_time = "[(" + std::string(filename) + "),(" + func_name + "),(" + std::to_string(line_no) + ")] " + tim::TimeStr() + "[" + std::to_string(trace_prc_pre_tick) + "] "+ str + "\n";

			vfprintf(stdout, (const char*)add_time.c_str(), args);
			trace_prc_pre_tick = tim::millis();
			va_end(args);
		}
	}

  void mcc_log::trace_prc(const char* str, ...)
	{
		LoG_LoCk(log_mutex);
		if (str)
		{
			va_list args;
			va_start(args, str);
			trace_prc_pre_tick = tim::millis() - trace_prc_pre_tick;
			std::string add_time = tim::TimeStr() + "[" + std::to_string(trace_prc_pre_tick) + "] " + str + "\n";
			vfprintf(stdout, (const char*)add_time.c_str(), args);
			trace_prc_pre_tick = tim::millis();
			va_end(args);
		}
	}

	uint32_t mcc_log::PutLog(level loglevel, const int obj, const char* file, const char* func, const int line, const char* fmt, ...)
	{
		LoG_LoCk(log_mutex);
		dat_st data{};
		if (fmt)
		{
			char buf[MCC_LOG_VLIST_BUF_MAX];
			va_list args;
			va_start(args, fmt);
			std::string filename = std::filesystem::path(file).filename().string();

			std::vector<std::string> v_strs{};
      std::string add_time{};
      if(trans::string_split(func, v_strs, "::") > 0)
        add_time = "[(" + std::string(filename) + "),(" + v_strs.back() + "),(" + std::to_string(line) + ")] [" + tim::NowStr() + "] " + fmt;
      else
        add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] [" + tim::NowStr() + "] " + fmt;

			data.SetDate(tim::NowStr());
			data.SetFile(filename);
			data.SetFunc(func);
			data.SetLineNo(line);

			std::vsnprintf(buf, sizeof(buf), fmt, args);
			data.SetMsg(std::string(buf));

			va_end(args);
		}
		else
		{
			return 0;
		}

		switch (loglevel)
		{
		case lvl_err:
			data.SetLevel("[ERR]");
			break;
		case lvl_warning:
			data.SetLevel("[WAR]");
			break;
		default: // info
			data.SetLevel("[INF]");
			break;
		}
		log_table[loglevel].Put(data);

		return (uint32_t)log_table[loglevel].Available();
	}

	mcc_log::dat_st mcc_log::GetLog(level loglevel)
	{
		dat_st data{};
		log_table[loglevel].Get(&data);
		return data;
	}

	mcc_log::dat_st mcc_log::PopLog(uint32_t addr, level loglevel)
	{
		dat_st data{};
		log_table[loglevel].Pop(addr, &data);
		return data;
	}

	uint32_t mcc_log::AvailableLog(level loglevel)
	{
		return (uint32_t)log_table[loglevel].Available();
	}

	std::string mcc_log::getLogFileName(const std::string& file)
	{
		// Get file name
		std::string filename = std::filesystem::path(file).filename().string();
		// ".cpp" delete
		size_t found = filename.rfind("."); // ".cpp" , ".hpp" 제거 
		if (found != std::string::npos)
		{
			filename.erase(found);
		}

		// Create log file name
		std::stringstream ss;
		ss << mcc_log_def_log_path << tim::DateStr().c_str() << "_" << filename;//<< ".log";

		// Check if the log directory exists
		if (!std::filesystem::is_directory(mcc_log_def_log_path))
		{
			std::filesystem::create_directory(mcc_log_def_log_path);
		}

		// Check if the log file exist
		if (std::filesystem::exists(ss.str() + ".log"))
		{
			file::FileCopy(std::string(ss.str() + "_old.log").c_str(),
				std::string((ss.str() + ".log")).c_str());
		}

		ss << ".log";
		return ss.str();
	}

	void mcc_log::writeToFile(const std::string& file, const std::string& message, bool prepend)
	{

		auto utf8_to_wstring = [](const std::string& str) ->std::wstring
			{
				if (str.empty()) return std::wstring();

				int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
				std::wstring wide_str(size_needed, 0);
				MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wide_str[0], size_needed);

				return wide_str;
			};

		// Wide String → UTF-8 변환
		auto wstring_to_utf8 = [](const std::wstring& wide_str) ->std::string
			{
				if (wide_str.empty()) return std::string();

				int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), (int)wide_str.size(), NULL, 0, NULL, NULL);
				std::string str(size_needed, 0);
				WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), (int)wide_str.size(), &str[0], size_needed, NULL, NULL);

				return str;
			};

		// UTF-8로 인코딩 설정
		std::string source_str = message;
		// 한글 메시지 UTF-8 변환
		std::wstring wide_message = utf8_to_wstring(source_str); //mcc::conversion::utf8_to_utf16(source_str.c_str());
		std::string utf8_message = wstring_to_utf8(wide_message);



		// 파일 존재 여부 확인
		//bool file_exists = std::filesystem::exists(file);

		// 파일 출력 스트림 열기 (UTF-8 with BOM 보장)
		std::ofstream ofs(file, std::ios::out | std::ios::app | std::ios::binary); // 바이너리 모드 사용
		//std::ofstream ofs(file, std::ios::out | std::ios::app); // Append 모드로 파일 열기
		
		if (ofs.good())
		{
			//if (!file_exists) // 파일이 없을 경우 BOM 추가
			//{
			//	constexpr char bom[] = "\xEF\xBB\xBF"; // UTF-8 BOM
			//	ofs.write(bom, sizeof(bom) - 1);
			//}

			if (prepend)
			{
				// 파일 맨 앞에 쓰는 로직 (필요시 유지)
				ofs.seekp(0, std::ios::beg);
				ofs << ofs.rdbuf(); // 기존 내용 복사
				ofs << utf8_message << std::endl; // 새 메시지 추가
			}
			else
			{
				// 메시지를 파일에 추가
				ofs << message << std::endl;
			}
		}

		ofs.close();
	}

	bool mcc_log::WriteLog(
		level loglevel,
		const int obj,
		const char* file_info,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...)
	{
		LoG_LoCk(log_mutex);
		if (fmt)
		{
			dat_st data{};

			va_list args;
			va_start(args, fmt);

			std::string filename = std::filesystem::path(file).filename().string();

			//std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] " + fmt;
      std::vector<std::string> v_strs{};
      std::string str_func{func};
      if (trans::string_split(func, v_strs, "::") > 0)
        str_func = v_strs.back();
			data.SetDate(tim::NowStr());
			data.SetFile(filename);
			data.SetFunc(str_func);
			data.SetLineNo(line);

			// snprintf(buf, sizeof(buf), fmt, args);
			
			constexpr uint32_t max_size = 512;
			std::string message(max_size,'\0');			
			int needed = vsnprintf(&message[0], max_size, fmt, args);
			if (needed < 0)
			{
				std::cout << "vsnprintf failed" << std::endl;
				va_end(args);
				return false;
			}

			message.resize(needed);

			data.SetMsg(message);
			va_end(args);

			// Get log file name
			std::string log_file_name = getLogFileName(file);
			std::string ctg_str = loglevel == lvl_info ? "[INF]" : (loglevel == lvl_warning ? "[WAR]" : "[ERR]");

			data.SetLevel(ctg_str);

			// put que buffer
			log_table[loglevel].Put(data);

			// Write log message to file
			writeToFile(log_file_name, data.GetLogMessage());
		}
		return true;
  }

	bool mcc_log::WriteLogMsg(
		level loglevel,
		const int obj,
		const char* file_info,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...)
	{
		LoG_LoCk(log_mutex);
		if (fmt)
		{
			dat_st data{};
			va_list args;
			va_start(args, fmt);

			std::string filename = std::filesystem::path(file).filename().string();

			//std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] " + fmt;
      std::vector<std::string> v_strs{};
      std::string str_func{func};
      if (trans::string_split(func, v_strs, "::") > 0)
        str_func = v_strs.back();
			data.SetDate(tim::NowStr());
			data.SetFile(filename);
			data.SetFunc(str_func);
			data.SetLineNo(line);

			// snprintf(buf, sizeof(buf), fmt, args);
			
			constexpr uint32_t max_size = 512;
			std::string message(max_size,'\0');			
			int needed = vsnprintf(&message[0], max_size, fmt, args);
      
      if (needed < 0)
			{
				std::cout << "vsnprintf failed" << std::endl;
				va_end(args);
				return false;
			}
			// 에러 정보를 cmd에 출력
			if(loglevel == lvl_err)
      {
        std::vector<std::string> v_strs{};
        std::string              add_time{};
        // 형식 출력
        add_time = ANSI_TEXT_GREEN_COLOR + tim::TimeStr() + ANSI_TEXT_RESET_COLOR + 
                  ANSI_TEXT_RED_COLOR + "[ NG ] " + ANSI_TEXT_RESET_COLOR + 
                  std::string{&message[0]} + 
                  ANSI_TEXT_YELLOW_COLOR + "[(" + std::string(filename) + "),(" + 
                  str_func + "),(" + std::to_string(line) + ")]" + 
                  ANSI_TEXT_RESET_COLOR + "\n";
        vfprintf(stdout, (const char *)add_time.c_str(), args);
      }
			message.resize(needed);
			std::string n_str{};
      for (auto &c : message)
      {
        if (c == '\n')
          break;
        n_str += c; //'\n'; // 개행 문자 제외
      }
			data.SetMsg(n_str);
      va_end(args);
      std::stringstream ss;
      ss << mcc_log_def_log_path << std::string{file_info};


      std::string log_file_name = getLogFileName(file_info); //ss.str();//
      std::string ctg_str       = loglevel == lvl_info ? "[INF]" : (loglevel == lvl_warning ? "[WAR]" : "[ERR]");

      data.SetLevel(ctg_str);

			// put que buffer
			log_table[loglevel].Put(data);

			writeToFile(log_file_name, data.GetLogMessage());     
		}
		return true;
  }

  errno_t mcc_log::view_log(const char* log_file, std::vector<data_t> &ret)
	{
		// 파일 읽기
    std::ifstream file(log_file);
    if (!file.is_open())
    {
        std::cerr << "파일을 열 수 없습니다: " << log_file << std::endl;
        return -1;
    }

		// 데이터 저장할 벡터
    std::vector<data_t> log_data;

    // 로그 라인을 읽고 저장
    std::string line;
    //std::regex log_pattern(R"(\[(.*?)\] \[\((.*?)\)\] \[(.*?)\] : (.*))");
		std::regex log_pattern(R"(\[(.*?)\] \[(.*?)\] \[(.*?)\] : (.*))");
    std::smatch match;

    while (std::getline(file, line))
    {
        if (std::regex_search(line, match, log_pattern) && match.size() == 5)
        {
            // data_t 구조체로 데이터 저장
            data_t data;
            data.date = match[1].str();
            data.func = match[2].str();
            data.type = match[3].str();
            data.desc = match[4].str();

            log_data.emplace_back(data);
        }
    }

    file.close();

		ret = log_data;
		
		return ERROR_SUCCESS;
	} 

  std::vector<mcc_log::data_t> mcc_log::view_log(const char* log_file)
	{
		// 파일 읽기
    std::ifstream file(log_file);
    if (!file.is_open())
    {
        std::cerr << "파일을 열 수 없습니다: " << log_file << std::endl;
        return std::vector<data_t>{};
    }

		// 데이터 저장할 벡터
    std::vector<data_t> log_data;

    // 로그 라인을 읽고 저장
    std::string line;
    //std::regex log_pattern(R"(\[(.*?)\] \[\((.*?)\)\] \[(.*?)\] : (.*))");
		std::regex log_pattern(R"(\[(.*?)\] \[(.*?)\] \[(.*?)\] : (.*))");
    std::smatch match;

    while (std::getline(file, line))
    {
        if (std::regex_search(line, match, log_pattern) && match.size() == 5)
        {
            // data_t 구조체로 데이터 저장
            data_t data;
            data.date = match[1].str();
            data.func = match[2].str();
            data.type = match[3].str();
            data.desc = match[4].str();

            log_data.emplace_back(data);
        }
    }

    file.close();

    // 데이터 출력 (확인용)
    // for (const auto& entry : log_data)
    // {
    //     std::cout << "Date: " << entry.date << "\n";
    //     std::cout << "Func: " << entry.func << "\n";
    //     std::cout << "Type: " << entry.type << "\n";
    //     std::cout << "Desc: " << entry.desc << "\n";
    //     std::cout << "-----------------------\n";
    // }
		return log_data;
  }

  errno_t mcc_log::file_group(group_logs &ret)
  {
    namespace fs = std::filesystem;
    // 현재 작업 디렉토리 확인
    fs::path cwd = fs::current_path();
    std::cout << "Current Working Directory: " << cwd << std::endl;

    // Log 폴더 경로 설정
    fs::path log_dir = cwd / "Log";

    // 날짜별 로그 파일들을 저장할 맵 (자동 정렬됨)
    group_logs grouped_logs;

    // 정규식으로 날짜와 파일명을 구분
    std::regex log_pattern(R"(\[(\d{4}-\d{2}-\d{2})\]_(.*)\.log)");

    // Log 폴더가 존재하는지 확인
    if (fs::exists(log_dir) && fs::is_directory(log_dir))
    {
      // Log 폴더의 파일을 반복하며 날짜별로 그룹화
      for (const auto &entry : fs::directory_iterator(log_dir))
      {
        const std::string filename = entry.path().filename().string();
        std::smatch       match;

        if (std::regex_search(filename, match, log_pattern) && match.size() == 3)
        {
          std::string date     = match[1].str(); // 날짜 추출
          std::string filetype = match[2].str(); // 파일 종류

          LogFileInfo log_info{filename, date};
          grouped_logs[date].push_back(log_info);
        }
      }

      // 날짜별로 그룹화된 파일 출력
      // for (const auto &[date, logs] : grouped_logs)
      // {
      //   std::cout << "Date: " << date << "\n";
      //   for (const auto &log : logs)
      //   {
      //     std::cout << "  - " << log.filename << "\n";
      //   }
      //   std::cout << "------------------------\n";
      // }
    }
    else
    {
      std::cerr << "Log 폴더가 존재하지 않습니다: " << log_dir << std::endl;
      return -1;
    }

		ret = grouped_logs;
    return ERROR_SUCCESS;
  }

  //--------------------------------------------------------
  //MARK: log class
  //--------------------------------------------------------
  void Log::writeToFile(const std::string &file, const std::string &message, bool prepend)
  {
    std::ofstream ofs(file, std::ios::out | std::ios::app); // Keep existing file content and open in append mode
    if (ofs.good())
    {
      if (prepend)
      {
        ofs.seekp(0, std::ios::beg);                        // Go to the beginning of the file
        ofs << message;                                     //<< std::endl; // add to front
      }
      else
      {
        ofs << message;                                     //<< std::endl; // add to back
      }
    }
    ofs.close();
  }

  std::string Log::getLogFileName(const std::string &file)
  {
    // Get file name
    std::string filename = std::filesystem::path(file).filename().string();
    // ".cpp" delete
    size_t found = filename.find(".cpp"); // ".cpp"
    if (found != std::string::npos)
    {
      filename.erase(found, 4);
    }

    // Create log file name
    std::stringstream ss;
    ss << mcc_log_def_log_path << tim::DateStr().c_str() << "_" << filename; //<< ".log";

    // Check if the log directory exists
    if (!std::filesystem::is_directory(mcc_log_def_log_path))
    {
      std::filesystem::create_directory(mcc_log_def_log_path);
    }

    // Check if the log file exist
    if (std::filesystem::exists(ss.str() + ".log"))
    {
      file::FileCopy(std::string((ss.str() + ".log")).c_str(), std::string(ss.str() + "_old.log").c_str());
    }

    ss << ".log";
    return ss.str();
  }

  std::string Log::trim_char(const std::string &input, bool is_front, const std::string &trim_char)
  {
    if (is_front)
    {
      size_t start = input.find_first_not_of(trim_char);
      if (start == std::string::npos)
      {            //
        return ""; //
      }
      return input.substr(start);
    }
    //
    size_t end = input.find_last_of(trim_char);

    if (end == std::string::npos)
      return input; // "/"

    return input.substr(end + 1);
  }

  bool Log::WriteLog(
    level       loglevel,
    const int   obj,       // chagne inst_no
    const char *file_info, //path & log  "uart/uart.log"
    const char *file,      //
    const char *func,
    const int   line,
    const char *fmt,
    ...)
  {
    LoG_LoCk(m_mutex);
    if (fmt)
    {
      mcc_log::dat_st data{};
      char            buf[MCC_LOG_VLIST_BUF_MAX];
      va_list         args;
      va_start(args, fmt);

      std::string filename = std::filesystem::path(file).filename().string();
      //std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] ";

      std::vector<std::string> v_strs{};
      std::string              str_func{func};
      if (trans::string_split(func, v_strs, "::") > 0)
        str_func = v_strs.back();
      data.SetDate(tim::NowStr());
      data.SetFile(filename);
      data.SetFunc(str_func);
      data.SetLineNo(line);

      std::vsnprintf(buf, sizeof(buf), fmt, args); // snprintf(buf, sizeof(buf), fmt, args);

      std::string message = std::string(buf);
      data.SetMsg(message);
      va_end(args);


      std::stringstream ss{};
      ss << mcc_log_def_log_path << trim_char(file_info); //trim_char(file_info);

      std::vector<std::string> folders{};
      std::istringstream       iss(ss.str());
      std::string              folder;

      while (std::getline(iss, folder, '/'))
      {
        folders.emplace_back(folder);
      }

      ss.str("");
      if (folders.size() > 1)
      {
        for (std::size_t i = 0; i < (folders.size() - 1); ++i)
        {
          ss << folders[i] << "/";
          if (!std::filesystem::is_directory(ss.str()))
          {
            std::filesystem::create_directory(ss.str());
          }
        }
      }
      else
      {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "]log path format mis matching !" << std::endl;
        return false;
      }

      /*ss << folders[0] << "/" << folders[1] << "/" << folders[2] << "/";

				if (!std::filesystem::is_directory(ss.str()))
				{
					std::filesystem::create_directory(ss.str());
				}*/
      //size_t found = folders.back().rfind(".");
      //folders.back() = folders.back().substr(0, found); //".log"  remove
      ss.str("");
      for (std::size_t i = 0; i < (folders.size() - 1); ++i)
      {
        ss << folders[i] << "/";
      }

      size_t found = folders.back().rfind("."); // ".cpp" , ".hpp"  remove
      if (found != std::string::npos)
      {
        folders.back().erase(found);
      }
      ss << tim::DateStr().c_str() << "_" << folders.back();

      // Check if the log file exist
      if (std::filesystem::exists(ss.str() + ".log"))
      {
        file::FileCopy(std::string(ss.str() + "_old.log").c_str(),
                       std::string((ss.str() + ".log")).c_str());
      }
      ss << ".log";
      std::string ctg_str = loglevel == lvl_info ? "[INF]" : (loglevel == lvl_warning ? "[WAR]" : "[ERR]");
      // Write log message to file
      data.SetLevel(ctg_str);
      message = ctg_str + message;
      m_logTable[loglevel].emplace_back(message);
      // Write log message to file
      writeToFile(ss.str(), data.GetLogMessage());
    }
    return true;
  }

  bool Log::WriteLog(
    level            loglevel,
    const int        obj,       // chagne inst_no
    const char      *file_info, //path & log  "uart/uart.log"
    const char      *file,      //
    const char      *func,
    const int        line,
    std::string_view msg)
  {
    LoG_LoCk(m_mutex);

    mcc_log::dat_st data{};
    std::string     filename = std::filesystem::path(file).filename().string();
    //std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] ";

    std::vector<std::string> v_strs{};
    std::string              str_func{func};
    if (trans::string_split(func, v_strs, "::") > 0)
      str_func = v_strs.back();
    data.SetDate(tim::NowStr());
    data.SetFile(filename);
    data.SetFunc(str_func);
    data.SetLineNo(line);
    std::string message{msg};
    data.SetMsg(message);
    std::stringstream ss{};
    ss << mcc_log_def_log_path << trim_char(file_info); //trim_char(file_info);
    std::vector<std::string> folders{};
    std::istringstream       iss(ss.str());
    std::string              folder;

    while (std::getline(iss, folder, '/'))
    {
      folders.emplace_back(folder);
    }

    ss.str("");
    if (folders.size() > 1)
    {
      for (std::size_t i = 0; i < (folders.size() - 1); ++i)
      {
        ss << folders[i] << "/";
        if (!std::filesystem::is_directory(ss.str()))
        {
          std::filesystem::create_directory(ss.str());
        }
      }
    }
    else
    {
      std::cout << "[" << __FILE__ << "][" << __LINE__ << "]log path format mis matching !" << std::endl;
      return false;
    }


    ss.str("");
    for (std::size_t i = 0; i < (folders.size() - 1); ++i)
    {
      ss << folders[i] << "/";
    }

    size_t found = folders.back().rfind("."); // ".cpp" , ".hpp" 제거
    if (found != std::string::npos)
    {
      folders.back().erase(found);
    }
    ss << tim::DateStr().c_str() << "_" << folders.back();

    // Check if the log file exist
    if (std::filesystem::exists(ss.str() + ".log"))
    {
      file::FileCopy(std::string(ss.str() + "_old.log").c_str(),
                     std::string((ss.str() + ".log")).c_str());
    }
    ss << ".log";
    std::string ctg_str = loglevel == lvl_info ? "[INF]" : (loglevel == lvl_warning ? "[WAR]" : "[ERR]");
    // Write log message to file
    data.SetLevel(ctg_str);
    message = ctg_str + message;
    m_logTable[loglevel].emplace_back(message);
    // Write log message to file
    writeToFile(ss.str(), data.GetLogMessage());
    return true;
  }

  std::string Log::GetLastLogMessage(TinyC::Log::level level)
  {
    if (m_logTable[lvl_err].size() == 0)
      return std::string{"emplty last log message!"};

    std::string log_msg = m_logTable[lvl_err].back();
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
}
// end of namespace TinyC