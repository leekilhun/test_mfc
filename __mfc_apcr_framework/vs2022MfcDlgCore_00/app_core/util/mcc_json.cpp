/**
 * @file mcc_json.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mcc_json.hpp"

namespace TinyC
{

  json_t::json_t(const TinyC::json_t::variant_t &val)
  {
    value = val;
    type  = get_varidx(val.index());
  };

  size_t json_t::valueIndex() const
  {
    return value.index();
  }

  bool json_t::is_obj_type(json_t &value)
  {
    if (value.getType(value.value) == valueType::object)
      return true;
    return false;
  }

  json_t::valueType json_t::get_varidx(size_t idx)
  {
    enum
    {
      _none,
      _jsonobj,
      _strings,
      _ints,
      _reals,
      _bools,
      _str,
      _int,
      _real,
      _bool
    };

    switch (idx)
    {
    case _jsonobj:
      return valueType::object;
    case _strings:
      return valueType::array;
    case _ints:
      return valueType::array;
    case _reals:
      return valueType::array;
    case _bools:
      return valueType::array;
    case _str:
      return valueType::string;
    case _int:
      return valueType::integer;
    case _real:
      return valueType::real;
    case _bool:
      return valueType::boolean;
    case _none:
    default:
      return valueType::none;
    }
  }

  std::string json_t::toJsonString() const
  {
    std::stringstream ss;
    switch (type)
    {
    case valueType::none:
    {
      ss << "null";
    }
    break;
    case valueType::object:
    {
      ss << "{ \n";
      const auto &obj   = std::get<json_obj_t>(value);
      bool        first = true;
      for (const auto &[key, value] : obj)
      {
        if (!first)
        {
          ss << " , ";
        }
        ss << "    \"" << key << "\" : " << value.toJsonString();
        first = false;
      }
      ss << "\n}";
    }
    break;
    case valueType::array:
    {
      ss << "[ \n";

      switch ((valueType)value.index())
      {
      case valueType::object:
      {
        auto &arr      = std::get<json_obj_t>(value);
        bool  is_first = true;
        for (const auto &elem : arr)
        {
          if (!is_first)
          {
            ss << " , \n";
          }
          ss << "    {"
             << "\"" << elem.first << "\" : ";
          ss << elem.second.toJsonString() << "} ";
          is_first = false;
        }
      }
      break;
      case valueType::string:
      {
        bool  first  = true;
        auto &values = std::get<std::vector<std::string>>(value);
        for (const auto &elem : values)
        {
          if (!first)
          {
            ss << " ,    \n";
          }
          ss << "    \"" << elem << "\"";
          first = false;
        }
      }
      break;
      case valueType::integer:
      {
        bool  first  = true;
        auto &values = std::get<std::vector<int>>(value);
        for (const auto &elem : values)
        {
          if (!first)
          {
            ss << " ,    \n";
          }
          ss << "    " << elem;
          first = false;
        }
      }
      break;
      case valueType::real:
      {
        bool  first  = true;
        auto &values = std::get<std::vector<double>>(value);
        for (const auto &elem : values)
        {
          if (!first)
          {
            ss << " ,    \n";
          }
          ss << "    " << elem;
          first = false;
        }
      }
      break;
      case valueType::boolean:
      {
        bool  first  = true;
        auto &values = std::get<std::vector<bool>>(value);
        for (const auto &elem : values)
        {
          if (!first)
          {
            ss << " ,    \n";
          }
          ss << "    " << (elem ? "true" : "false");
          first = false;
        }
      }
      break;
      default:
        break;
      }
      ss << "\n    ]";
    }
    break;
    case valueType::string:
    {
      ss << "\"" << std::get<std::string>(value) << "\"";
    }
    break;
    case valueType::integer:
    {
      ss << std::get<int>(value);
    }
    break;
    case valueType::real:
    {
      ss << std::to_string(std::get<double>(value));
    }
    break;
    case valueType::boolean:
    {
      ss << (std::get<bool>(value) ? "true" : "false");
    }
    break;
    }
    return ss.str();
  }


  //-----------------------------------------------------------------
  //MARK: Json class functions
  //-----------------------------------------------------------------
  bool Json::file_copy(const char *pdest, const char *psour)
  {
    std::ifstream ifs(psour, std::ios::binary);
    if (!ifs.is_open())
    {
      //
      return false;
    }

    std::ofstream ofs(pdest, std::ios::binary);
    if (!ofs.is_open())
    {
      //
      return false;
    }

    //
    //char buffer[def_buffer_max * 4];
    std::vector<char> buffer(def_buffer_max * 4);
    while (ifs.read(buffer.data(), buffer.size())) //sizeof(buffer)
    {
      ofs.write(buffer.data(), buffer.size());
    }

    //
    ofs.write(buffer.data(), ifs.gcount());

    return true;
  }

  bool Json::loadFromFile(const std::string &filename)
  {
    std::ifstream inFile(filename);
    is_loaded = inFile.is_open();
    if (!is_loaded)
    {
      std::cerr << "No a file loaded " << std::endl;
      return false;
    }
    std::string line{};
    while (std::getline(inFile, line))
    {
      lines.emplace_back(line);
    }
    inFile.clear();
    inFile.seekg(0, std::ios::beg);
    std::string str((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    is_loaded = parse(str);
    return is_loaded;
  }

  bool Json::saveToFile(const std::string &file)
  {
    if (is_loaded == false)
    {
      std::cerr << "No a file loaded " << std::endl;
      return false;
    }

    // Get file name
    std::string filename = file;                //std::filesystem::path(file).filename().string();
    size_t      found    = filename.rfind("."); // ".json"
    if (found != std::string::npos)
    {
      filename.erase(found);
    }
    file_copy(std::string{filename + "_old.json"}.c_str(), file.c_str());

    std::ofstream ofs(file);
    if (!ofs.is_open())
      return false;

    for (const auto &line : lines)
    {
      ofs << line << std::endl; //
    }
    ofs.close();

    return true;
  }

  bool Json::replace_line(const json_t &src, size_t line_off)
  {
    if (is_loaded == false)
    {
      std::cerr << "No a file loaded " << std::endl;
      return false;
    }
    bool   is_first = true;
    size_t idx{line_off};

    auto &[index, value] = src;
    json_t::valueType elm_type{};

    if (index == json_t::valueType::array)
    {
      /*case array*/
      if (json_t::get_varidx(value.index()) == json_t::valueType::object)
      {
        auto &get_data = std::get<json_obj_t>(value);

        while (lines[idx].find("{") == std::string::npos)
        {
          idx++;
        }

        // object 객체를 가져온다
        //
        size_t oject_find_line_idx = idx;
        for (auto &elm : get_data)
        {
          // 데이터 정렬과 , 파일 정렬 순서가 다를 수 있기 때문에 모두 찾아야 한다.
          size_t found_line_no{oject_find_line_idx};
          while (lines[found_line_no].find(elm.first) == std::string::npos)
          {
            if (((found_line_no) < lines.size()) == false)
            {
              std::cout << "replace_line () not find keyword" << std::endl;
              return false;
            }
            found_line_no++;
          }

          //size_t pos = lines[idx + cnt].find(elm.first);

          //if (pos != std::string::npos)
          {
            char *start_addr = (char *)strchr(lines[found_line_no].c_str(), ':');
            char *end_addr   = (char *)strchr(lines[found_line_no].c_str(), '}');

            if (end_addr == nullptr)
            {
              //## '25.01 box tapping recipe data 구조 추가에 따른 변경
              //
              auto &arr = elm.second;
              //.second.type;
              size_t arr_idx = arr.value.index();
              if (json_t::get_varidx(arr_idx) == json_t::valueType::object)
              {
                auto &sub_obj  = std::get<json_obj_t>(arr.value);
                auto  so       = sub_obj.begin();
                auto &ssub_obj = std::get<json_obj_t>(so->second.value);

                ++found_line_no; // 줄 바꿈한다.
                for (auto &pair : ssub_obj)
                {
                  size_t      line_idx{found_line_no};
                  std::string target_str = pair.first;
                  while (lines[line_idx].find(target_str) == std::string::npos)
                  {
                    line_idx++;
                  }
                  if (replace_line(pair.second, line_idx) == false)
                  {
                    std::cout << "replace_line () not find keyword" << std::endl;
                    return false;
                  }
                }
                //idx = idx + ssub_obj.size();
                goto goto_ssub_obj_out;
              }


              //## end
            }
            else if (start_addr == nullptr)
            {
              std::cout << "replace_line () not find keyword" << std::endl;
              return false;
            }
            size_t      length = (end_addr - start_addr) - 1;
            std::string dest_str{start_addr + 1, length};
            trim(dest_str);

            std::size_t pos = lines[found_line_no].find(dest_str);
            if (pos != std::string::npos)
            {
              lines[found_line_no].replace(pos, dest_str.length(), elm.second.toJsonString());
              //break;
            }
          }

        goto_ssub_obj_out: // array  형태의 object를 replace를 후 빠져 나온다.
          //oject_find_line_idx = found_line_no;
          found_line_no = 0;
        }
        // end of - for (size_t i = 0; i < get_data.size(); ++i)
      }
      else if (json_t::get_varidx(value.index()) == json_t::valueType::array)
      {
        enum
        {
          _none,
          _jsonobj,
          _strings,
          _ints,
          _reals,
          _bools,
          _str,
          _int,
          _real,
          _bool
        };

        switch (value.index())
        {
        case _jsonobj:
        {
          std::cout << "jsonobj" << std::endl;
        }
        break;
        case _strings:
        {
          //"members": [\"tom\",\"wheel\",\"jason\"],
          char *start_addr = (char *)strchr(lines[idx].c_str(), '[');
          char *end_addr   = (char *)strchr(lines[idx].c_str(), ']');
          if (start_addr == nullptr || end_addr == nullptr)
          {
            std::cout << "replace_line () not find keyword" << std::endl;
            return false;
          }
          size_t length = (end_addr - (start_addr));
          while (length)
          {
            if (*start_addr == '\"')
            {
              break;
            }
            ++start_addr;
            --length;
          }
          std::string dest_str{start_addr, length};
          auto       &names = std::get<std::vector<std::string>>(value);
          std::string sour_str{"\""};
          for (const auto &elm : names)
          {
            sour_str.append(elm + std::string{"\", \""});
          }
          sour_str.erase(sour_str.end() - 3, sour_str.end()); //
          lines[idx].replace((lines[idx].find(dest_str)), dest_str.length(), sour_str);
        }
        break;
        case _ints:
        {
          //"numbers": [ 5, 4, 3, 2, 1 ],
          char *start_addr = (char *)strchr(lines[idx].c_str(), '[');
          char *end_addr   = (char *)strchr(lines[idx].c_str(), ']');
          if (start_addr == nullptr || end_addr == nullptr)
          {
            std::cout << "replace_line () not find keyword" << std::endl;
            return false;
          }
          size_t length = (end_addr - (start_addr));
          while (length)
          {
            if (*start_addr == '-')
              break;

            if (*start_addr > 0x2f && *start_addr < 0x3a)
              break;

            ++start_addr;
            --length;
          }

          std::string dest_str{start_addr, length};
          auto       &nums = std::get<std::vector<int>>(value);
          std::string sour_str{};
          for (const auto &elm : nums)
          {
            sour_str.append(std::to_string(elm) + ", ");
          }
          sour_str.erase(sour_str.end() - 2, sour_str.end()); // 마지막 ", " 제거
          lines[idx].replace((lines[idx].find(dest_str)), dest_str.length(), sour_str);
        }
        break;
        case _reals:
        {
          //"members": [ 1.2, 2.456, 52.3456 ],
          char *start_addr = (char *)strchr(lines[idx].c_str(), '[');
          char *end_addr   = (char *)strchr(lines[idx].c_str(), ']');
          if (start_addr == nullptr || end_addr == nullptr)
          {
            std::cout << "replace_line () not find keyword" << std::endl;
            return false;
          }
          size_t length = (end_addr - (start_addr));

          while (length)
          {
            if (*start_addr == '-')
              break;

            if (*start_addr > 0x2f && *start_addr < 0x3a)
              break;

            ++start_addr;
            --length;
          }
          std::string dest_str{start_addr, length};
          auto       &reals = std::get<std::vector<double>>(value);
          std::string sour_str{};
          for (const auto &elm : reals)
          {
            sour_str.append(std::to_string(elm) + ", ");
          }
          sour_str.erase(sour_str.end() - 2, sour_str.end()); // 마지막 ", " 제거
          lines[idx].replace((lines[idx].find(dest_str)), dest_str.length(), sour_str);
        }
        break;
        case _bools:
        {
          //"checks": [ false, true, true, true, false ],
          char *start_addr = (char *)strchr(lines[idx].c_str(), '[');
          char *end_addr   = (char *)strchr(lines[idx].c_str(), ']');
          if (start_addr == nullptr || end_addr == nullptr)
          {
            std::cout << "replace_line () not find keyword" << std::endl;
            return false;
          }
          size_t length = (end_addr - (start_addr));
          while (length)
          {
            if (*start_addr == 'f' || *start_addr == 'F' || *start_addr == 't' || *start_addr == 'T')
            {
              break;
            }
            ++start_addr;
            --length;
          }

          std::string dest_str{start_addr, length};
          auto       &btns = std::get<std::vector<bool>>(value);
          std::string sour_str{};
          for (const auto &elm : btns)
          {
            sour_str.append((elm ? "true" : "false") + std::string{", "});
          }
          sour_str.erase(sour_str.end() - 2, sour_str.end()); // 마지막 ", " 제거
          lines[idx].replace((lines[idx].find(dest_str)), dest_str.length(), sour_str);
        }
        break;
        default:
          std::cout << "replace_line () No matching case found for value.index" << std::endl;
          return false;
        }
        // end of switch
      }
    }
    else if (index == json_t::valueType::object)
    {
      /*case object*/
    }
    else if (index == json_t::valueType::string)
    {
      /*case string*/
      size_t pos = lines[idx].find(":");
      if (pos == std::string::npos)
      {
        std::cout << "replace_line() not find keyword" << std::endl;
        return false;
      }
      char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
      char *end_addr   = (char *)strchr(lines[idx].c_str(), '}');
      if (start_addr == nullptr || end_addr == nullptr)
      {
        std::cout << "replace_line () not find keyword" << std::endl;
        return false;
      }
      size_t length = (end_addr - (start_addr));
      while (length)
      {
        if (*start_addr == '\"')
          break;

        ++start_addr;
        --length;
        ++pos;
      }
      std::string dest_str{start_addr, length};
      std::string sour_str{"\"" + std::get<std::string>(value) + "\""};
      lines[idx].replace((pos), dest_str.length(), sour_str);
    }
    else if (index == json_t::valueType::integer)
    {
      /*case integer*/
      size_t pos = lines[idx].find(":");
      if (pos == std::string::npos)
      {
        std::cout << "replace_line() not find keyword" << std::endl;
        return false;
      }
      char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
      char *end_addr   = (char *)strchr(lines[idx].c_str(), '}');
      if (start_addr == nullptr || end_addr == nullptr)
      {
        std::cout << "replace_line () not find keyword" << std::endl;
        return false;
      }
      size_t length = (end_addr - (start_addr));
      while (length)
      {
        if (*start_addr == '-')
          break;

        if (*start_addr > 0x2f && *start_addr < 0x3a)
          break;

        ++start_addr;
        --length;
        ++pos;
      }
      std::string dest_str{start_addr, length};
      std::string sour_str{std::to_string(std::get<int>(value))};
      lines[idx].replace((pos), dest_str.length(), sour_str);
    }
    else if (index == json_t::valueType::real)
    {
      /*case real*/
      size_t pos = lines[idx].find(":");
      if (pos == std::string::npos)
      {
        std::cout << "replace_line() not find keyword" << std::endl;
        return false;
      }
      char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
      char *end_addr   = (char *)strchr(lines[idx].c_str(), '}');
      if (start_addr == nullptr || end_addr == nullptr)
      {
        std::cout << "replace_line () not find keyword" << std::endl;
        return false;
      }
      size_t length = (end_addr - (start_addr));
      while (length)
      {
        if (*start_addr == '-')
          break;

        if (*start_addr > 0x2f && *start_addr < 0x3a)
          break;

        ++start_addr;
        --length;
        ++pos;
      }
      std::string dest_str{start_addr, length};
      std::string sour_str{std::to_string(std::get<double>(value))};
      lines[idx].replace((pos), dest_str.length(), sour_str);
    }
    else if (index == json_t::valueType::boolean)
    {
      /*case boolean*/
      size_t pos = lines[idx].find(":");
      if (pos == std::string::npos)
      {
        std::cout << "replace_line() not find keyword" << std::endl;
        return false;
      }
      char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
      char *end_addr   = (char *)strchr(lines[idx].c_str(), '}');
      if (start_addr == nullptr || end_addr == nullptr)
      {
        std::cout << "replace_line () not find keyword" << std::endl;
        return false;
      }
      size_t length = (end_addr - (start_addr));
      while (length)
      {
        if (*start_addr == 'f'
            || *start_addr == 'F'
            || *start_addr == 't'
            || *start_addr == 'T')
        {
          break;
        }

        ++start_addr;
        --length;
        ++pos;
      }
      std::string dest_str{start_addr, length};
      std::string sour_str{std::get<bool>(value) ? "true" : "false"};
      lines[idx].replace((pos), dest_str.length(), sour_str);
    }
    else
    {
      std::cout << "No matching case found for value.index" << std::endl;
      return false;
    }

    return true;
  }

  bool Json::replace_value(const json_obj_t &src_data)
  {
    if (is_loaded == false)
    {
      std::cerr << "No a file loaded " << std::endl;
      return false;
    }
    //MARK:VIEW
    for (auto &jdata : m_objects)
    {
      for (const auto &pair : src_data)
      {
        auto it = jdata.find(pair.first);
        if (it != jdata.end())
        {
          // change object_data
          it->second = pair.second;
          {
            // change lines data
            size_t idx{};
            //for (auto& elm : lines)
            for (idx; idx < lines.size(); ++idx)
            {
              if (lines[idx].find(it->first) != std::string::npos)
              {
                json_t value = it->second;
                switch (value.type)
                {
                case json_t::valueType::object:
                {
                  json_obj_t obj{};
                  obj = std::get<json_obj_t>(value.value);
                  for (auto &pair : obj)
                  {
                    int fine_idx{};
                    while (lines[idx + fine_idx].find(pair.first) == std::string::npos)
                    {
                      fine_idx++;
                    }
                    replace_line(pair.second, idx + fine_idx); //jdata,
                  }
                }
                break;
                case json_t::valueType::array:
                {
                  replace_line(it->second, idx); //jdata,
                }
                break;
                case json_t::valueType::string:
                {
                  std::size_t pos = lines[idx].find(pair.first);
                  pos             = lines[idx].find(":", (pos + pair.first.length()));
                  if (pos == std::string::npos)
                  {
                    std::cout << "replace_value() not find keyword" << std::endl;
                    return false;
                  }
                  char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
                  char *end_addr   = (char *)strchr(lines[idx].c_str(), ',');
                  if (end_addr == nullptr) //fix 2024.08.31
                    end_addr = (char *)lines[idx].c_str() + lines[idx].length();
                  if (start_addr == nullptr || end_addr == nullptr)
                  {
                    std::cout << "replace_value () not find keyword" << std::endl;
                    return false;
                  }
                  size_t length = (end_addr - (start_addr));
                  while (length)
                  {
                    if (*start_addr == '\"')
                    {
                      break;
                    }
                    ++start_addr;
                    --length;
                    ++pos;
                  }

                  std::string dest_str{start_addr, length};
                  std::string sour_str{"\"" + std::get<std::string>(pair.second.value) + "\""};
                  lines[idx].replace((pos), dest_str.length(), sour_str);
                }
                break;

                case json_t::valueType::integer:
                {
                  std::size_t pos = lines[idx].find(pair.first);
                  pos             = lines[idx].find(":", (pos + pair.first.length()));
                  if (pos == std::string::npos)
                  {
                    std::cout << "replace_value() not find keyword" << std::endl;
                    return false;
                  }
                  char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
                  char *end_addr   = (char *)strchr(lines[idx].c_str(), ',');
                  if (end_addr == nullptr) //fix 2024.08.31
                    end_addr = (char *)lines[idx].c_str() + lines[idx].length();
                  if (start_addr == nullptr || end_addr == nullptr)
                  {
                    std::cout << "replace_value () not find keyword" << std::endl;
                    return false;
                  }
                  size_t length = (end_addr - (start_addr));
                  while (length)
                  {
                    if (*start_addr == '-')
                      break;

                    if (*start_addr > 0x2f && *start_addr < 0x3a)
                      break;

                    ++start_addr;
                    --length;
                    ++pos;
                  }
                  std::string dest_str{start_addr, length};
                  std::string sour_str{std::to_string(std::get<int>(pair.second.value))};
                  lines[idx].replace((pos), dest_str.length(), sour_str);
                }
                break;

                case json_t::valueType::real:
                {
                  std::size_t pos = lines[idx].find(pair.first);
                  pos             = lines[idx].find(":", (pos + pair.first.length()));
                  if (pos == std::string::npos)
                  {
                    std::cout << "replace_value() not find keyword" << std::endl;
                    return false;
                  }
                  //lines[idx].replace(pos, .length(), catch_data.toJsonString());
                  char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
                  char *end_addr   = (char *)strchr(lines[idx].c_str(), ',');
                  if (end_addr == nullptr) //fix 2024.08.31
                    end_addr = (char *)lines[idx].c_str() + lines[idx].length();
                  if (start_addr == nullptr || end_addr == nullptr)
                  {
                    std::cout << "replace_value () not find keyword" << std::endl;
                    return false;
                  }
                  size_t length = (end_addr - (start_addr));
                  while (length)
                  {
                    if (*start_addr == '-')
                      break;

                    if (*start_addr > 0x2f && *start_addr < 0x3a)
                      break;

                    ++start_addr;
                    --length;
                    ++pos;
                  }
                  std::string dest_str{start_addr, length};
                  std::string sour_str{std::to_string(std::get<double>(pair.second.value))};
                  lines[idx].replace((pos), dest_str.length(), sour_str);
                }
                break;

                case json_t::valueType::boolean:
                {
                  std::size_t pos = lines[idx].find(pair.first);
                  pos             = lines[idx].find(":", (pos + pair.first.length()));
                  if (pos == std::string::npos)
                  {
                    std::cout << "replace_value() not find keyword" << std::endl;
                    return false;
                  }
                  //lines[idx].replace(pos, .length(), catch_data.toJsonString());
                  char *start_addr = (char *)strchr(lines[idx].c_str(), ':');
                  char *end_addr   = (char *)strchr(lines[idx].c_str(), ',');
                  if (end_addr == nullptr) //fix 2024.08.31
                    end_addr = (char *)lines[idx].c_str() + lines[idx].length();
                  if (start_addr == nullptr || end_addr == nullptr)
                  {
                    std::cout << "replace_value () not find keyword" << std::endl;
                    return false;
                  }
                  size_t length = (end_addr - (start_addr));
                  while (length)
                  {
                    if (*start_addr == 'f'
                        || *start_addr == 'F'
                        || *start_addr == 't'
                        || *start_addr == 'T')
                    {
                      break;
                    }
                    ++start_addr;
                    --length;
                    ++pos;
                  }

                  std::string dest_str{start_addr, length};
                  std::string sour_str{std::get<bool>(pair.second.value) ? "true" : "false"};
                  lines[idx].replace((pos), dest_str.length(), sour_str);
                }
                break;
                case json_t::valueType::none:
                default:
                  break;
                }
                //end of switch
              }
            }
          }
          return true;
        }
      }
    }
    return false;
  }

  std::string Json::toJsonString() const
  {
    if (is_loaded == false)
    {
      std::cerr << "No a file loaded " << std::endl;
      return std::string{};
    }
    std::stringstream ss;

    ss << "{ \n";
    using jvtype = json_t::valueType;

    for (const auto &data : this->m_objects)
    {
      if (data.empty())
        return std::string{};
      if (data.size() == 1)
      {
        auto        elm = data.begin();
        std::string key = elm->first;
        ss << "  \"" << key << "\" : ";
        json_t value = elm->second;
        switch (value.type)
        {
        case json_t::valueType::none:
          ss << "null";
          break;
        case json_t::valueType::object:
        {
          ss << "{ \n";
          auto &obj   = std::get<json_obj_t>(value.value);
          bool  first = true;
          for (const auto &[key, value] : obj)
          {
            if (!first)
            {
              ss << ", \n";
            }
            ss << "    \"" << key << "\" : " << value.toJsonString();
            first = false;
          }
          ss << "\n  }";
        }
        break;
        case json_t::valueType::array:
        {
          ss << "[ \n";

          enum class arry_type : int
          {
            object = 1,
            string,
            integer,
            real,
            boolean,
          };

          switch ((arry_type)value.valueIndex())
          {
          case arry_type::object:
          {
            auto &arr   = std::get<json_obj_t>(value.value);
            bool  first = true;
            for (const auto &elem : arr)
            {
              if (elem.second.type == json_t::valueType::array)
              {
                if (!first)
                  ss << ",\n"; //ss << ", \n    {";//\n

                ss << "  {\n  ";

                ss << "\"" << elem.first << "\" : ";
                ss << elem.second.toJsonString() << "\n  }";
              }
              else
              {
                if (!first)
                  ss << ", \n"; //\n

                ss << "    {";

                ss << "\"" << elem.first << "\" : ";
                ss << elem.second.toJsonString() << "}";
              }
              first = false;
            }
          }
          break;
          case arry_type::string:
          {
            bool  first  = true;
            auto &values = std::get<std::vector<std::string>>(value.value);
            for (const auto &elem : values)
            {
              if (!first)
              {
                ss << " ,    \n";
              }
              ss << "    \"" << elem << "\"";
              first = false;
            }
          }
          break;
          case arry_type::integer:
          {
            bool  first  = true;
            auto &values = std::get<std::vector<int>>(value.value);
            for (const auto &elem : values)
            {
              if (!first)
              {
                ss << " ,    \n";
              }
              ss << "    " << elem;
              first = false;
            }
          }
          break;
          case arry_type::real:
          {
            bool  first  = true;
            auto &values = std::get<std::vector<double>>(value.value);
            for (const auto &elem : values)
            {
              if (!first)
              {
                ss << " ,    \n";
              }
              ss << "    " << elem;
              first = false;
            }
          }
          break;
          case arry_type::boolean:
          {
            bool  first  = true;
            auto &values = std::get<std::vector<bool>>(value.value);
            for (const auto &elem : values)
            {
              if (!first)
              {
                ss << " ,    \n";
              }
              ss << "    " << (elem ? "true" : "false");
              first = false;
            }
          }
          break;
          default:
            break;
          }

          ss << "\n  ], \n";
        }
        break;
        case json_t::valueType::string:
          ss << "\"" << std::get<std::string>(value.value) << "\", \n";
          break;
        case json_t::valueType::integer:
          ss << std::get<int>(value.value) << ", \n";
          break;
        case json_t::valueType::real:
          ss << std::get<double>(value.value) << ", \n";
          break;
        case json_t::valueType::boolean:
          ss << (std::get<bool>(value.value) ? "true" : "false") << ", \n";
          break;
        default:
          break;
        }
      }
      else
      {
        for (const auto &[key, value] : data)
        {
          ss << "\"" << key << "\":" << value.toJsonString();
        }
      }
      ss << "\n";
    }

    ss << "\n } \n";
    return ss.str();
  }

  void Json::trim(std::string &str)
  {
    // 앞에서부터 공백문자 제거
    std::size_t start = str.find_first_not_of(" \t\r\n");
    if (start != std::string::npos)
    {
      str = str.substr(start);
    }
    else
    {
      str.clear();
      return;
    }

    // 뒤에서부터 공백문자 제거
    std::size_t end = str.find_last_not_of(" \t\r\n");
    if (end != std::string::npos)
    {
      str = str.substr(0, end + 1);
    }
    else
    {
      str.clear();
    }
  }

  const char *Json::find_first_char(const std::string &str, char ch)
  {
    std::size_t i          = 0;     //
    std::size_t len        = str.length();
    bool        next_comma = false; // 쉼표 다음에 값이 나올 예정인지 확인하기 위한 플래그
    // 배열 값 파싱 루프
    while (i < len)
    {
      // 공백문자 제거
      while (i < len && (str[i] == ' ' || str[i] == '\t' || str[i] == '\r'))
      {
        ++i;
      }
      if (i == len)
      {
        break;
      }

      //  주석 줄 제거
      if (str[i] == '/' && str[i + 1] == '/')
      {
        i += 2;
        while (i < len && str[i] != '\n')
        {
          i++;
        }
      }

      // ch와 일치하는 문자를 찾으면 해당 위치를 반환
      if (str[i] == ch)
      {
        return &str[i];
      }

      i++;
    }
    return nullptr;
  }

  const char *Json::find_balance_char(const std::string &str, char open_key, char close_key)
  {
    size_t open_pos = str.find(open_key);
    if (open_pos == std::string::npos)
      return &str[0];     // 없는 경우


    size_t close_pos = 0;
    int    balance   = 1; // open_key 를 만나면 1 증가, close_key 를 만나면 1 감소
    size_t idx       = open_pos + 1;
    while (idx < str.size() && balance)
    {
      if (str[idx % str.size()] == open_key)
      {
        balance++;
      }
      else if (str[idx % str.size()] == close_key)
      {
        balance--;
      }
      idx++;
    }
    return &str[idx % str.size()];
  }

  const char *Json::find_last_char(const std::string &str, char ch)
  {
    size_t pos = str.rfind(ch);
    if (pos == std::string::npos)
    {
      // 문자가 문자열에 없는 경우
      return nullptr;
    }
    else
    {
      // 문자가 문자열에 있는 경우
      return &str[pos];
    }
  }

  int Json::replace(std::string &str, const char *before, const char *after)
  {
    int         count = 0;
    std::size_t pos   = 0;
    while ((pos = str.find(before, pos)) != std::string::npos)
    {
      str.replace(pos, strlen(before), after);
      pos += strlen(after);
      count++;
    }
    return count;
  }

  int Json::replace_trim(std::string &str, const char *before, const char *after)
  {
    int         count = 0;
    std::size_t pos   = 0;
    while ((pos = str.find(before, pos)) != std::string::npos)
    {
      std::size_t start_pos = pos;
      std::size_t end_pos   = pos + strlen(before);

      // Remove leading whitespace characters
      while (start_pos > 0 && std::isspace(str[start_pos - 1]))
      {
        start_pos--;
      }

      // Remove trailing whitespace characters
      while (end_pos < str.length() && std::isspace(str[end_pos]))
      {
        end_pos++;
      }

      // Replace the substring
      str.replace(start_pos, end_pos - start_pos, after);
      pos = start_pos + strlen(after);

      count++;
    }
    return count;
  }

  std::string_view Json::trim(std::string_view str)
  {
    // 공백문자 집합
    constexpr std::string_view whitespace{" \t\n\r\f\v"};

    // 앞쪽의 공백문자 제거
    const auto firstNonWhitespace = str.find_first_not_of(whitespace);
    str.remove_prefix(firstNonWhitespace);

    // 뒤쪽의 공백문자 제거
    const auto lastNonWhitespace = str.find_last_not_of(whitespace);
    if (lastNonWhitespace != std::string_view::npos)
    {
      str.remove_suffix(str.size() - lastNonWhitespace - 1);
    }

    return str;
  }

  bool Json::isInteger(const std::string_view &str)
  {
    if (str.empty())
    {
      return false;
    }

    std::size_t i = 0;
    if (str[i] == '+' || str[i] == '-')
    {
      i++;
    }

    if (i >= str.length())
    {
      return false;
    }

    for (; i < str.length(); i++)
    {
      if (!std::isdigit(str[i]))
      {
        return false;
      }
    }

    return true;
  }

  bool Json::isBool(const std::string_view &str)
  {
    if (str.empty())
    {
      return false;
    }

    std::size_t i = 0;
    for (; i < str.length(); i++)
    {
      if (str[i] == 'f' || str[i] == 'F' || str[i] == 't' || str[i] == 'T')
      {
        return true;
      }
    }
    return false;
  }

  bool Json::isReal(const std::string_view &str)
  {
    if (str.empty())
    {
      return false;
    }

    std::size_t i = 0;
    if (str[i] == '+' || str[i] == '-')
    {
      i++;
    }

    bool hasDecimal  = false;
    bool hasExponent = false;
    for (; i < str.length(); i++)
    {
      if (std::isdigit(str[i]))
      {
        continue;
      }
      else if (str[i] == '.')
      {
        if (hasDecimal || hasExponent)
        {
          return false;
        }
        hasDecimal = true;
      }
      else if (str[i] == 'e' || str[i] == 'E')
      {
        if (hasExponent)
        {
          return false;
        }
        hasExponent = true;
        i++;
        if (i >= str.length())
        {
          return false;
        }
        if (str[i] == '+' || str[i] == '-')
        {
          i++;
        }
        if (i >= str.length() || !std::isdigit(str[i]))
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }

    return hasDecimal || hasExponent || std::isdigit(str.back());
  }

  bool Json::parseObject(std::string_view str, json_t &data, std::size_t &ret_idx)
  {
    std::size_t i{};

    auto get_number = [&str, &i](bool &is_double) -> std::string
    {
      std::size_t       cnt{};
      std::stringstream ss{};
      while (i < str.size())
      {
        char c = str[i];
        if (c == '.')
          is_double = true;
        if (c == ',' || c == '}')
          return ss.str();
        ss << c;
        i++;
      }
      return ss.str();
    };

    auto get_string = [&str, &i](auto find_c) -> std::string
    {
      i++; // " next
      size_t head_pos = i;

      std::string ret{};
      char        open_char{};
      switch (find_c)
      {
      case '}': open_char = '{'; break;
      case ']': open_char = '['; break;
      default:
      {
        while (i < str.size())
        {
          char c = str[i];
          if (c == '/')
          {
            if (str[i + 1] == '/')
            {
              i += 1;
              while (str[i] != '\n')
              {
                i++;
              }
              i += 1;
              c  = ' ';
            }
          }

          if (c == find_c)
            return ret;
          ret.push_back(c);
          i++;
        }
        return ret;
      }
      break;
      }
      int balance = 1;
      while (i < str.size())
      {
        if (str[i % str.size()] == open_char)
        {
          balance++;
        }
        else if (str[i % str.size()] == find_c)
        {
          balance--;
        }

        if (balance == 0)
        {
          ret = str.substr(head_pos, (i - head_pos));
          return ret;
        }

        i++;
      }

      return ret;
    };

    auto get_end = [&str, &i]() -> std::size_t
    {
      while (i < str.size())
      {
        char c = str[i];
        if (c == ',' || c == '}')
          return i;
        i++;
      }
      return i;
    };

    auto get_key = [&str, &i]() -> std::string
    {
      std::string ret{};
      while (i < str.size())
      {
        char c = str[i];
        if (c == '\"')
        {
          while (i < str.size())
          {
            ret.push_back(str[++i]);
            if (str[i + 1] == '\"')
            {
              i = i + 2;
              while (i < str.size())
              {
                if (str[i++] == ':')
                  return ret;
              }
            }
          }
        }
        i++;
      }
      return ret;
    };

    while (1)
    {
      char c = str[i];
      if (c == ',' || c == '}')
        break;

      switch (c)
      {
      case '{':
      {
        std::string arg_str{str};
        char       *start_addr = (char *)find_first_char(arg_str, '{');
        char       *end_addr   = (char *)find_balance_char(arg_str, '{', '}');
        size_t      start_idx  = start_addr - (char *)arg_str.c_str();
        size_t      length     = end_addr - start_addr;
        std::string obj_str    = arg_str.substr(start_idx, length);

        json_obj_t obj{};
        size_t     inner_idx = 0;
        while (inner_idx < obj_str.size()) //while (i < str.size())
        {
          size_t ret_idx = 0;
          if ((obj_str[(inner_idx)] == ' ' || obj_str[(inner_idx)] == '}') == false)
          {
            std::string key = get_key();
            if (key.empty())
              break;
            json_t ret_data{};
            size_t pos = obj_str.find(':', (i - key.length())); // + 1;   ":[ ", ": [" 사이띄우기 문제
            //inner_idx = inner_idx + pos;
            if (parseObject(&obj_str[(pos) % obj_str.size()], ret_data, ret_idx))
            {
              inner_idx = ret_idx + pos;
              obj.emplace(key, ret_data);
            }
          }
          inner_idx++;
          i = inner_idx;
        }
        // while
        inner_idx++;
        ret_idx = ret_idx + obj_str.size() + 1; //inner_idx;
        if (obj.size() != 0)
        {
          data = obj;
          return true;
        }
      }
      break;

      case '[':
      {
        data.type       = json_t::valueType::array;
        std::string str = get_string(']');
        ret_idx         = ret_idx + i + 1;

        json_t ret_data;
        parseArray(str, ret_data);
        data.value = ret_data.value;
        return true;
      }
      break;

      case '"':
      {
        data    = get_string('"');
        ret_idx = ret_idx + get_end();
        return true;
      }
      break;

      case 't':
      case 'T':
      case 'f':
      case 'F':
      {
        data    = (c == 't' || c == 'T') ? true : false;
        ret_idx = ret_idx + get_end();
        return true;
      }
      break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '-':
      case '+':
      {
        bool        is_double{};
        std::string cut_str = get_number(is_double);
        if (is_double)
        {
          data = std::stod((char *)cut_str.c_str());
        }
        else
        {
          data = std::stoi((char *)cut_str.c_str());
        }
        ret_idx = ret_idx + i;
        return true;
      }
      break;

      default:
        break;
      }
      // switch
      i++;
    }
    // while (1)

    return false;
  }

  int Json::split_balance(char *arg_str, std::vector<std::string> &argv, char open_key, char close_key, size_t max)
  {
    char       *tok      = nullptr;
    char       *next_ptr = nullptr;
    size_t      str_len{};
    std::string str{arg_str};
    size_t      arg_size = str.size();
    while (str_len < arg_size)
    {
      char       *start_addr = (char *)find_first_char(str, open_key);
      const char *end_addr   = find_balance_char(str, open_key, close_key);
      size_t      length     = end_addr - start_addr;
      size_t      start_idx  = std::string{str.c_str()}.find(open_key);
      if (start_idx != std::string::npos)
      {
        std::string obj_str = str.substr(start_idx, length);
        if (obj_str.empty() == false)
        {
          str.erase(0, start_idx + length);
          //str_len = str_len + length;
          argv.emplace_back(obj_str);
        }
        else
        {
          break;
        }
        str_len = str_len + start_idx + length;
      }
      str_len++;
    }

    return (int)argv.size();
  }

  int Json::split_balance(char *arg_str, char **argv, char open_key, char close_key, size_t max)
  {
    int         argc     = 0;
    char       *tok      = nullptr;
    char       *next_ptr = nullptr;
    size_t      check_length{};
    std::string str{arg_str};
    size_t      arg_size = str.size();
    while (check_length < arg_size)
    {
      char       *start_addr  = (char *)find_first_char(str, open_key);
      const char *end_addr    = find_balance_char(str, open_key, close_key);
      size_t      length      = end_addr - start_addr;
      size_t      start_idx   = std::string{str.c_str()}.find(open_key);
      size_t      obj_str_len = 0;
      if (start_idx != std::string::npos)
      {
        std::string obj_str = str.substr(start_idx, length);
        obj_str_len         = start_idx + length;
        if (obj_str.empty() == false)
        {
          argv[argc] = &arg_str[check_length + (start_idx)]; //- argc
          str.erase(0, obj_str_len);

          argc++;
        }
        else
          break;

        check_length = check_length + obj_str_len;
      }
      else
      {
        check_length++;
        //std::cout<<"mcc_json_parse_break"<<std::endl;
        break;
      }
    }

    return argc;
  }

  void Json::split(const std::string &s, char delimiter, std::vector<std::string> &result, bool ignore_brace)
  {
    std::stringstream ss{};
    std::string       item;
    bool              inside_braces = false;

    int  idx      = 0;
    bool ig_brace = false;
    while (idx < s.size())
    {
      char c = s[idx];
      if (c == '/' && s[idx + 1] == '/')
      {
        while (s[idx] != '\n' && idx < (s.size() - 1))
        {
          idx++;
        }
        idx++;
      }

      if (c == '{')
      {
        if (inside_braces)
        {
          ig_brace = true;
        }
        inside_braces = !inside_braces;
      }

      if (c == '}')
      {
        if (ig_brace)
          ig_brace = false;

        inside_braces = false;
      }

      if (c == delimiter && inside_braces == !(ignore_brace) && ig_brace == false)
      {
        result.emplace_back(ss.str());
        ss.str("");
      }
      else
        ss << c;

      idx++;
    }
    result.emplace_back(ss.str());
  }

  int Json::split(char *arg_str, char **argv, const char *delim_chars, size_t max)
  {
    int   argc     = 0;
    char *tok      = nullptr;
    char *next_ptr = nullptr;

    for (tok = strtok_s(arg_str, delim_chars, &next_ptr); tok; tok = strtok_s(NULL, delim_chars, &next_ptr))
    {
      argv[argc++] = tok;
      if (argc >= max)
        break;
    }
    return argc;
  }

  /**
		 * @brief 
		 * parse array data
		 */
  bool Json::parseArray(std::string_view array_str, json_t &ret_data)
  {
    if (array_str.empty())
      return false;

    auto cnt_char = [](std::string_view str, char c) -> std::size_t
    {
      std::size_t cnt = 0;
      for (auto it = str.begin(); it != str.end(); ++it)
      {
        if (*it == c)
          ++cnt;
      }
      return cnt;
    };

    auto get_key = [](const std::string &str, std::size_t &idx) -> std::string
    {
      std::string ret{};
      while (idx < str.size())
      {
        char c = str[idx];
        if (c == '\"')
        {
          while (idx < str.size())
          {
            ret.push_back(str[++idx]);
            if (str[idx + 1] == '\"')
            {
              idx = idx + 2;
              while (idx < str.size())
              {
                if (str[idx++] == ':')
                  return ret;
              }
            }
          }
        }
        idx++;
      }
      return ret;
    };

    auto get_obj_length = [](const std::string &str) -> int
    {
      int  idx{};
      bool found_brace = false;
      while (idx < str.size())
      {
        char c = str[idx];
        if (c == '{')
          found_brace = true;

        if (c == '}')
        {
          if (found_brace)
            found_brace = false;
          else
            return idx;
        }
        idx++;
      }
      return idx;
    };


    /*
		  "time": {
        "timeout": 5.000000,
        "on_settling_time": 1.000000,
        "no_sensor_delay": 5.000000
      }
			위 형태와 같이 [] or {}  묶음 없이 연속된 데이터 포맷 처리

			'25.3/1 "2025-02-26 14:37:53'235" 처럼 스트링 안에 구분 문자가 들어간 경우
			예외 처리 추가
			*/
    auto is_virtual_array = [](const std::string &str) -> bool
    {
      int  idx{};
      bool find_colon          = false;
      bool find_brace          = false;
      bool in_double_quotation = false;
      while (idx < str.size())
      {
        char c = str[idx];
        idx++;
        // '25.03 따옴표 안에 구분문자 생략
        if (c == '\"')
          in_double_quotation = !in_double_quotation;
        if (in_double_quotation)
          continue;

        if (c == ':')
        {
          if (find_colon == false)
            find_colon = true;
          else
            return true;
        }
        if (c == '{')
        {
          if (find_brace == false)
            find_brace = true;
          else
            return false;
        }

        if (c == '}')
        {
          if (find_colon)
            return false;
        }
      }
      return false;
    };


    const char          delim_chars[] = ",";
    constexpr int       elem_max      = def_buffer_max;
    std::vector<char *> argv(def_buffer_max);
    //char* argv[elem_max] = {};
    bool is_object = (array_str.find('{') != std::string::npos && array_str.find('}') != std::string::npos);

    if (is_object)
    {
      int argc      = split_balance((char *)array_str.data(), argv.data());
      ret_data.type = json_t::valueType::object;

      if (is_virtual_array(std::string(argv[0])))
      {
        //MARK:hidden brace object array data type '24.07.08
        /*
				{
    		  "name": "loc_xx_name_1",
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
    		},	

       아래와 같이 변경한다.
 					//array_obj [index_0...., json_t::object]
				{         

				 "index_0":	  // vobj[name, json_t::object]]
				 [		                                     
				 { "name": "loc_xx_name_1"},   
				 {
				   "time": [
				    { "timeout": 1.000000},
    		    { "on_settling_time": 0.100000},
    		    {"no_sensor_delay": 1.000000}
				    ]
					}
				 ]

				}


				*/
        json_obj_t array_obj;

        std::vector<std::string> out_tokens;
        split(std::string(argv[0]), ',', out_tokens);
        json_t values{};
        values.type = json_t::valueType::object;
        //json_obj_t objs{};
        for (int i = 0; i < out_tokens.size(); i++)
        {
          std::string obj_str(out_tokens[i]);

          json_obj_t vobj{};
          json_t     vjst{};
          vjst.type                         = json_t::valueType::array;
          std::string              vobj_key = "index_" + std::to_string(i);
          std::vector<std::string> in_tokens;
          split(obj_str, ',', in_tokens, false);
          for (auto &token : in_tokens)
          {
            std::size_t end_key{};
            std::string key = get_key(token, end_key);
            json_t      value{};

            if (cnt_char(token, ',') > 1)
            {
              size_t pos1 = token.find('{');
              size_t pos2 = token.find('}');
              if (pos1 == std::string::npos || pos2 == std::string::npos)
              {
                std::cout << " find fail  " << std::endl;
                return false;
              }
              std::string              crp_str = token.substr(pos1 + 1, pos2 - pos1 - 1);
              json_obj_t               elm_obj{};
              std::vector<std::string> inner_tokens;
              split(crp_str, ',', inner_tokens);
              for (auto &elm : inner_tokens)
              {
                std::size_t ret_idx{};
                std::size_t end_key{};
                std::string elm_key = get_key(elm, end_key);
                if (parseObject(&elm[end_key + 1], value, ret_idx))
                {
                  elm_obj.emplace(elm_key, value);
                }
              }
              vobj.emplace(key, elm_obj);
            }
            else
            {
              std::size_t ret_idx{};
              if (parseObject(&token[end_key + 1], value, ret_idx))
              {
                vobj.emplace(key, value);
              }
            }
          }
          values.value = vobj;
          array_obj.emplace(vobj_key, values);
        }

        ret_data.value = array_obj;
        return true;
      }
      else
      {
        /*
				{ "USE_BUZZER": false },       { "USE_DOOR": false },
				{ "USE_MES": false },          { "SHIFT_A_TIME": 7 },
				{ "SHIFT_C_TIME": 21 },        { "RETAY_MAX_CNT": 3 }
			*/
        json_obj_t array_obj;
        for (int i = 0; i < argc; i++)
        {
          if (argv[i])
          {
            std::string obj_str(argv[i]);
            std::size_t end_key{};
            std::string key = get_key(obj_str, end_key);
            json_t      value{};
            std::size_t ret_idx = 0;
            if (parseObject(&obj_str[end_key + 1], value, ret_idx))
            {
              array_obj.emplace(key, value);
            }
            else
              return false;
          }
          // ret_idx++;
        }
        // for  (int i = 0; i < argc; i++)
        ret_data.value = array_obj;
        return true;
      }
    }
    else
    {
      int argc = split((char *)array_str.data(), argv.data(), delim_chars, elem_max);
      /*
				false, true, false ,

				*/
      if (argv[0])
      {
        std::string_view value(trim(argv[0]));
        if (isInteger(value))
        {
          ret_data.type = json_t::valueType::integer;
          std::vector<int> v_datas;
          for (int i = 0; i < argc; i++)
          {
            std::string_view value(trim(argv[i]));
            if (isInteger(value))
              v_datas.emplace_back(std::stoi(std::string(value)));
            else
              return false;

            // ret_idx++;
          }
          // for
          ret_data.value = v_datas;
          return true;
        }
        else if (isReal(value))
        {
          ret_data.type = json_t::valueType::real;
          std::vector<double> v_datas;
          for (int i = 0; i < argc; i++)
          {
            std::string_view value(trim(argv[i]));
            if (isReal(value))
              v_datas.emplace_back(std::stod(std::string(value)));
            else
              return false;

            // ret_idx++;
          }
          // for
          ret_data.value = v_datas;
          return true;
        }
        else if (value.front() == '"' && value.back() == '"')
        {
          ret_data.type = json_t::valueType::string;
          std::vector<std::string> v_datas;
          for (int i = 0; i < argc; i++)
          {
            std::string str(trim(argv[i]));
            trim(str);
            str.erase(0, 1);              // 맨 앞의 " 제거
            str.erase(str.size() - 1, 1); // 맨 뒤의 " 제거
            v_datas.emplace_back(str);
            // ret_idx++;
          }
          // for
          ret_data.value = v_datas;
          return true;
        }
        else if (isBool(value))
        {
          ret_data.type = json_t::valueType::boolean;
          std::vector<bool> v_datas;
          for (int i = 0; i < argc; i++)
          {
            std::string_view value(trim(argv[i]));
            if (value == "true" || value == "TRUE")
              v_datas.emplace_back(true);
            else
              v_datas.emplace_back(false);
            // ret_idx++;
          }
          // for
          ret_data.value = v_datas;
          return true;
        }
        else
        {
          return false;
        }
      }
      // if (argv[0])
    }
    // if (is_object)

    return false;
  }

  bool Json::parse(const std::string &arrayString)
  {
    auto get_str = [&](auto &str) -> std::string
    {
      char       *start_addr = strchr(str, '{');
      char       *end_addr   = strchr(str, '}');
      size_t      length     = (end_addr - start_addr) - 1;
      std::string ret(start_addr + 1, length);

      return ret;
    };

    if (arrayString.empty())
    {
      return false;
    }

    /*
			 Json parses a single { ///// } object and stores it as data.
			 {  /////  }  only one
			*/

    char  *start_addr = (char *)find_first_char(arrayString, '{');
    char  *end_addr   = (char *)find_last_char(arrayString, '}');
    size_t start_idx  = start_addr - (char *)arrayString.c_str();
    size_t length     = end_addr - start_addr;
    // arrayString[i]위치부터 end_addr까지의 문자열을 obj_str에 저장
    std::string obj_str = arrayString.substr(start_idx, length);
    if (length == 0)
    {
      std::cout << "pasing emplty" << std::endl;
      return false;
    }
    // 배열 값 파싱 루프
    std::size_t pos = 1;
    std::size_t end_key{};
    while (pos < length)
    {
      end_key = obj_str.find(':', pos);
      if (end_key == std::string_view::npos)
        break;

      while (pos < length)
      {
        char c = obj_str[pos];
        if (c == '/' && obj_str[pos + 1] == '/')
        {
          while (obj_str[pos] != '\n' && pos < (length - 1))
            pos++;
        }

        if (obj_str[pos] == '\"')
          break;

        pos++;
      }

      if (pos >= length)
        break;

      // key 문자에서 앞뒤로 " 제거
      std::string key(obj_str.data() + pos, end_key - pos);
      pos = +(replace_trim(key, "\"", ""));
      if (key.empty())
        break;
      ;


      pos = end_key + 1;
      json_t           ret_object{};
      std::string_view view_str(&obj_str[pos]);
      if (parseObject(&obj_str[pos], ret_object, pos))
      {
        json_obj_t obj{};
        obj.emplace(key, ret_object);
        m_objects.emplace_back(obj);
        //m_datas.emplace_back(ret_object);
      }
      else
      {
        break;
      }
      pos++;
    }
    // while (pos < length)

    if (m_objects.size() > 0)
      return true;
    else
      return false;
  }


}
// end of namespace TinyC