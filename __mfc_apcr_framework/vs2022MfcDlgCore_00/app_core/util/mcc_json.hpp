/*****************************************************************//**
 * \file   mcc_json.hpp
 * \brief
 *   3/10
 *   3/19 
 *   5/9 1520 줄 size_t pos = obj_str.find(':') ;// + 1;   ":[ ", ": [" 사이띄우기 문제
 *   5/15 fixed almost
 *   7/10 virtual array object  추가
 *   '25.01.27 recipe data 포맷에 대한 수정  p518
 *   '25.02.26  1706 버그 수정
 *   '25.3/1 <is_virtual_array> "2025-02-26 14:37:53'235" 처럼 스트링 안에 구분 문자가 들어간 경우 예외 처리 추가
 * \author gns2.lee (gns2.lee@samsung.com)
 * \date   March 2024
 *
 *
 * @copyright Copyright (c) 2024
 *
 *********************************************************************/


#pragma once
#ifndef _COMMON_MCC_JSON_HPP
#define _COMMON_MCC_JSON_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <numeric>
#include <type_traits>
#include <string>
#include <cstring>
#include <vector>
#include <deque>
#include <exception>
#include <variant>
#include <tuple>
#include <array>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <list>
#include <any>
#include <functional>
#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <iterator>
#include <execution>
#include <ratio>
#include <optional>
#include <string_view>
#include <memory_resource>
#include <fstream>


#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>


namespace TinyC
{
	struct json_t;

	typedef std::map<std::string, json_t> json_obj_t;

	/**
	 * json data 타입 .
	 */
  struct json_t
  {
  public:
    enum class valueType : int
    {
      none,
      object,
      array,
      string,
      integer,
      real,
      boolean,
    } type = valueType::none;

    using variant_t = std::variant<std::monostate, json_obj_t, std::vector<std::string>, std::vector<int>, std::vector<double>, std::vector<bool>, std::string, int, double, bool>;

    variant_t value{};

    json_t() = default;
    json_t(const variant_t &val);

    json_t(const json_t &rhs)            = default; // copy constructor
    json_t &operator=(const json_t &rhs) = default; // copy assignment operator
    json_t(json_t &&rhs)                 = default; // move constructor
    json_t &operator=(json_t &&rhs)      = default; // move assignment operator

    ~json_t() = default;

    size_t           valueIndex() const;
    static bool      is_obj_type(json_t &value);
    static valueType get_varidx(size_t idx);

    template <typename T>
    json_t operator=(const T &value)
    {
      type        = getType(value);
      this->value = value;
      return *this;
    }

    template <typename T>
    valueType getType(const T &value)
    {
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return valueType::none;
      }
      else if constexpr (std::is_same_v<T, std::string>)
      {
        return valueType::string;
      }
      else if constexpr (std::is_same_v<T, bool>)
      {
        return valueType::boolean;
      }
      else if constexpr (std::is_same_v<T, int>)
      {
        return valueType::integer;
      }
      else if constexpr (std::is_same_v<T, double>)
      {
        return valueType::real;
      }
      else if constexpr (std::is_same_v<T, std::vector<std::string>>)
      {
        return valueType::array;
      }
      else if constexpr (std::is_same_v<T, std::vector<int>>)
      {
        return valueType::array;
      }
      else if constexpr (std::is_same_v<T, std::vector<double>>)
      {
        return valueType::array;
      }
      else if constexpr (std::is_same_v<T, std::vector<bool>>)
      {
        return valueType::array;
      }
      else if constexpr (std::is_same_v<T, json_obj_t>)
      {
        return valueType::object;
      }
      else if constexpr (std::is_same_v<T, json_t>)
      {
        return valueType::object;
      }
      else
      {
        throw std::runtime_error("Unsupported type for json_t");
        return valueType::none;
      }
    }

    std::string toJsonString() const;
  };
  // end of struct json_t




	/**
	 * json 타입의 데이터를 사용하기 위한 클래스 .
	 */
	class Json
	{
  private:
    static constexpr auto def_buffer_max = 4096;
	public:
		Json() = default;
		virtual ~Json() = default;
    Json(const Json &rhs)            = default; // copy constructor
    Json &operator=(const Json &rhs) = default; // copy assignment operator
    Json(Json &&rhs)                 = default; // move constructor
    Json &operator=(Json &&rhs)      = default; // move assignment operator
		std::vector<std::string> lines{};
		std::vector<json_obj_t> m_objects{};

		bool is_loaded{};

		bool file_copy(const char *pdest, const char *psour);
		bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& file);
		bool replace_line(const json_t& src, size_t line_off);		
    bool replace_value(const json_obj_t &src_data);
    std::string toJsonString() const;
		void trim(std::string& str);
		const char* find_first_char(const std::string& str, char ch);
		const char* find_balance_char(const std::string& str, char open_key = '{', char close_key = '}');
		const char* find_last_char(const std::string& str, char ch);
		int replace(std::string& str, const char* before, const char* after);
		int replace_trim(std::string& str, const char* before, const char* after);
		std::string_view trim(std::string_view str);
		bool isInteger(const std::string_view& str);
		bool isBool(const std::string_view& str);
		bool isReal(const std::string_view& str);
		bool parseObject(std::string_view str, json_t& data, std::size_t& ret_idx);
		int split_balance(char *arg_str, std::vector<std::string> &argv, char open_key = '{', char close_key = '}', size_t max = def_buffer_max); 
		int split_balance(char *arg_str, char **argv, char open_key = '{', char close_key = '}', size_t max = def_buffer_max);
    void split(const std::string &s, char delimiter, std::vector<std::string> &result, bool ignore_brace = true);
    int split(char* arg_str, char** argv, const char* delim_chars, size_t max = 1024);
    bool parseArray(std::string_view array_str, json_t& ret_data);
		bool parse(const std::string& arrayString);
	};
	// end of class Json
}
// end of namespace TinyC

#endif // _COMMON_MCC_LOG_HPP