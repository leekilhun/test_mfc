/**
 * @file downloader_peeler.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-05-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef _UTIL_DOWNLOAD_PEELER_HPP
#define _UTIL_DOWNLOAD_PEELER_HPP


#include "ap_def.hpp"

namespace apcr
{
 struct downloader_peeler
 {
  static constexpr unsigned short fw_crc_table[256] = {0x0000,
		0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
		0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027,
		0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D,
		0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B,
		0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
		0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF,
		0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5,
		0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093,
		0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
		0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197,
		0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE,
		0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
		0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
		0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F,
		0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176,
		0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123,
		0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
		0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104,
		0x8101, 0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D,
		0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B,
		0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
		0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C,
		0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
		0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3,
		0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
		0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7,
		0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E,
		0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A, 0x829B,
		0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
		0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC,
		0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5,
		0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243,
		0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
		0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
		0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E,
		0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208,
		0x820D, 0x8207, 0x0202};


  static constexpr uint32_t FW_FILE_ADDR       = 0x8005000;
  static constexpr uint32_t jump_fw            = 1; // true
  static constexpr uint32_t FLASH_MAGIC_NUMBER = 0x5555AAAA;
  static constexpr int      TX_BLOCK_LENGTH    = 512;

  static constexpr uint8_t BOOT_ERR_WRONG_CMD   = 0x01;
  static constexpr uint8_t BOOT_ERR_LED         = 0x02;
  static constexpr uint8_t BOOT_ERR_FLASH_ERASE = 0x03;
  static constexpr uint8_t BOOT_ERR_WRONG_RANGE = 0x04;
  static constexpr uint8_t BOOT_ERR_FLASH_WRITE = 0x05;
  static constexpr uint8_t BOOT_ERR_BUF_OVF     = 0x06;
  static constexpr uint8_t BOOT_ERR_NO_RESPONSE = 0xFE;

  static constexpr uint8_t CMD_STX = 0x02;
  static constexpr uint8_t CMD_ETX = 0x03;

  static constexpr int CMD_MAX_DATA_LENGTH   = 1024;
  static constexpr int CMD_MAX_PACKET_LENGTH = (CMD_MAX_DATA_LENGTH + 8);
  static constexpr int PACKET_BUFF_LENGTH    = CMD_MAX_PACKET_LENGTH;

  enum BOOT_CMD_TYPE : uint8_t
  {
    BOOT_CMD_READ_BOOT_VERSION = 0x00,
    BOOT_CMD_READ_BOOT_NAME,
    BOOT_CMD_READ_FIRM_VERSION,
    BOOT_CMD_READ_FIRM_NAME,
    BOOT_CMD_FLASH_ERASE,
    BOOT_CMD_FLASH_WRITE,
    BOOT_CMD_JUMP_TO_FW = 0x08,
  };

public:
  struct cfg_t
  {
    std::shared_ptr<uart_t> ptr_uart{};

    cfg_t() = default;
    ~cfg_t() = default;
  } _cfg{};

  struct firm_tag_st
  {
    uint32_t magic_number{};

    //-- fw info
    //
    uint32_t addr_tag{};
    uint32_t addr_fw{};
    uint32_t size_tag{};

    //-- tag info
    //
    uint32_t                tag_flash_type{};
    uint32_t                tag_flash_start{};
    uint32_t                tag_flash_end{};
    uint32_t                tag_flash_length{};
    uint32_t                tag_flash_crc{};
    uint32_t                tag_length{};
    std::array<uint8_t, 32> tag_date_str{};
    std::array<uint8_t, 32> tag_time_str{};
  };
  size_t _file_size{};
  std::vector<uint8_t> _file_buf{};

  struct packet_st
  {
    using buff_t = std::array<uint8_t, CMD_MAX_PACKET_LENGTH>;
    uint8_t        cmd{};
    uint8_t        dir{};
    uint8_t        error{};
    uint16_t       length{};
    uint8_t        checksum{};
    uint8_t        rx_checksum{};
    buff_t         buffer{};
    uint8_t        buffer_idx{};
    uint8_t       *data{};
    uint16_t       data_cnt{};
    uint32_t       resp_ms{};
    machine_step_t state{};

		packet_st() {
			constexpr auto buffer_data_start_idx = 6;
			this->data = &this->buffer[buffer_data_start_idx];
		};
    // copy constructor
    packet_st(const packet_st &other) = default;
    // copy assignment
    packet_st &operator=(const packet_st &other) = default;
    // move constructor
    packet_st(packet_st &&other) = default;
    // move assignment
    packet_st &operator=(packet_st &&other) = default;
    ~packet_st()                            = default;

    uint8_t BufferAdd(uint8_t rx_data)
    {
      buffer[buffer_idx % CMD_MAX_PACKET_LENGTH] = rx_data;
      buffer_idx         = (buffer_idx + 1) % CMD_MAX_PACKET_LENGTH;
      return buffer_idx;
    }

    inline bool DataAdd(uint8_t rx_data)
    {
      BufferAdd(rx_data);
      data[data_cnt] = rx_data;
      ++data_cnt;
      return (data_cnt == length ? true : false);
    }

    void BufferClear()
    {
      buffer.fill(0);
      buffer_idx = 0;
      data_cnt   = 0;
      checksum   = 0;
      state.SetStep(0);
    }
  } _packet{};

  bool _isConnected{};
  bool _waitResp{};
  std::function<int(void *, void *)> _cb{};
  /****************************************************
   *	constructor
   ****************************************************/
  downloader_peeler()  = default;
  ~downloader_peeler()
  {
    if (_cfg.ptr_uart)
    {
      _cfg.ptr_uart->clearShortCallback();
    }

  };



  /****************************************************
   *	function
   ****************************************************/
  inline std::string get_peeler_fw_err_str(uint8_t idx)
  {
    switch (idx)
    {
      case BOOT_ERR_WRONG_CMD:
        return "BOOT_ERR_WRONG_CMD";
      case BOOT_ERR_FLASH_ERASE:
        return "BOOT_ERR_FLASH_ERASE";
      case BOOT_ERR_WRONG_RANGE:
        return "BOOT_ERR_WRONG_RANGE";
      case BOOT_ERR_FLASH_WRITE:
        return "BOOT_ERR_FLASH_WRITE";
      case BOOT_ERR_BUF_OVF:
        return "BOOT_ERR_BUF_OVF";
      case BOOT_ERR_NO_RESPONSE:
        return "BOOT_ERR_NO_RESPONSE";
      default:
        return "NO_DEFINED_ERROR";
    }
  }

  inline void registerCallback(std::function<int(void *, void *)> cb)
  {
    _cb = cb;
  }
  
  inline errno_t UartCallback(void *w_parm, void *l_parm)
  {
    if (w_parm == nullptr)
      return -1;
    int length = *((int *)w_parm);
    int index  = 0;

		for (index = 0; index < length; index++)
		{
			uint8_t data = *((uint8_t*)l_parm + index);
			if (recievePacket(data))
			{
				_waitResp = false;
				receiveCplt();
			}
		}
    return ERROR_SUCCESS;
  }

  inline void fwCrc(uint16_t *p_crc_cur, uint8_t data_in)
  {
    // TODO:
    uint16_t crc;
    uint16_t i;

    crc = *p_crc_cur;

    i          = ((unsigned short)(crc >> 8) ^ data_in) & 0xFF;
    *p_crc_cur = (crc << 8) ^ fw_crc_table[i];
  };

  inline bool IsReceiveCmdOK()
  {
    constexpr uint8_t CMD_OK = 0x00;
    return (_packet.error == CMD_OK);
  }

  inline int SendFirmCmd(BOOT_CMD_TYPE cmd_type, uint8_t *ptr_data, uint32_t length)
  {
    if (_cfg.ptr_uart->IsConnected() == false)
      return -1;

    /*
      | STX  |  CMD  | Dir   | ERROR | Length(L/H) |Data      | Checksum |ETX |
      | :--- |:------|:----- |:----- |:------------|:---------| :------  |-----|
      | 0x02 | 1byte | 1byte | 1byte | 2 byte      |Data 0～n | 1byte    |0x03 |
      */

    constexpr uint8_t    CMD_DIR_M_TO_S = 0;
    constexpr uint8_t    CMD_OK = 0x00;
    std::vector<uint8_t> datas{
      CMD_STX,
      cmd_type,
      CMD_DIR_M_TO_S,
      CMD_OK,
      LoBYTe(length),
      HiBYTe(length)};
    for (uint32_t i = 0; i < length; i++)
      datas.emplace_back(ptr_data[i]);

    uint8_t crc = 0;
    for (uint32_t i = 1; i < datas.size(); i++)
      crc ^= datas[i];

    datas.emplace_back(crc);
    datas.emplace_back(CMD_ETX);

     std::cout << "TxData.Data: " << std::hex;
     for (const auto &elm : datas)
       std::cout << (int)elm << " ";
     std::cout << std::endl;

     return  _cfg.ptr_uart->SendData((const char *)datas.data(), datas.size());
  }

  inline bool SendFirmCmdRxResp(BOOT_CMD_TYPE cmd_type, uint8_t *p_data, uint32_t length, uint32_t timeout = 500)
  {
    _packet.BufferClear();
    if (SendFirmCmd(cmd_type, p_data, length) > 0)
    {
      constexpr auto BOOT_ERR_NO_RESPONSE = 0xFE;
      _packet.error = BOOT_ERR_NO_RESPONSE;
      uint32_t pre_ms = tim::millis();

      _waitResp = true;
      while (_waitResp)
      {
        if ((tim::millis() - pre_ms) > timeout)
        {
          _waitResp = false;
          return false;
        }
        //Sleep(50);
        tim::delay(1);
      }
      return true;
    }
    return false;
  }

  inline bool recievePacket(uint8_t rx_data)
  {
    /*
      | STX  |  CMD  | Dir   | ERROR | Length(L/H) |Data      | Checksum |ETX |
      | :--- |:------|:----- |:----- |:------------|:---------| :------  |-----|
      | 0x02 | 1byte | 1byte | 1byte | 2 byte      |Data 0～n | 1byte    |0x03 |
      */

    enum : uint8_t
    {
      STATE_WAIT_STX      = 0,
      STATE_WAIT_CMD      = 1,
      STATE_WAIT_DIR      = 2,
      STATE_WAIT_ERROR    = 3,
      STATE_WAIT_LENGTH_L = 4,
      STATE_WAIT_LENGTH_H = 5,
      STATE_WAIT_DATA     = 6,
      STATE_WAIT_CHECKSUM = 7,
      STATE_WAIT_ETX      = 8,
    };

    constexpr int timeout_ms = 100;

    if (_packet.state.MoreThan(timeout_ms))
      _packet.BufferClear();

    switch (_packet.state.GetStep())
    {
      case STATE_WAIT_STX:
        _packet.BufferClear();
        if (rx_data == CMD_STX)
        {
          _packet.BufferAdd(rx_data);
          _packet.state.SetStep(STATE_WAIT_CMD);
        }
        break;

      case STATE_WAIT_CMD:
        _packet.cmd       = rx_data;
        _packet.checksum ^= rx_data;
        _packet.BufferAdd(rx_data);
        _packet.state.SetStep(STATE_WAIT_DIR);
        break;

      case STATE_WAIT_DIR:
        _packet.dir       = rx_data;
        _packet.checksum ^= rx_data;
        _packet.BufferAdd(rx_data);
        _packet.state.SetStep(STATE_WAIT_ERROR);
        break;

      case STATE_WAIT_ERROR:
        _packet.error     = rx_data;
        _packet.checksum ^= rx_data;
        _packet.BufferAdd(rx_data);
        _packet.state.SetStep(STATE_WAIT_LENGTH_L);
        break;

      case STATE_WAIT_LENGTH_L:
        _packet.length    = rx_data;
        _packet.checksum ^= rx_data;
        _packet.BufferAdd(rx_data);
        _packet.state.SetStep(STATE_WAIT_LENGTH_H);
        break;

      case STATE_WAIT_LENGTH_H:
        _packet.length   |= (rx_data << 8);
        _packet.checksum ^= rx_data;
        _packet.BufferAdd(rx_data);
        _packet.data_cnt = 0;
        if (_packet.length > 0)
        {
          _packet.state.SetStep(STATE_WAIT_DATA);
        }
        else
          _packet.state.SetStep(STATE_WAIT_CHECKSUM);
        break;

      case STATE_WAIT_DATA:
        _packet.checksum ^= rx_data;
        if (_packet.DataAdd(rx_data))
          _packet.state.SetStep(STATE_WAIT_CHECKSUM);
        break;

      case STATE_WAIT_CHECKSUM:
        _packet.BufferAdd(rx_data);
        _packet.rx_checksum = rx_data;
        _packet.state.SetStep(STATE_WAIT_ETX);
        break;

      case STATE_WAIT_ETX:
        _packet.BufferAdd(rx_data);
        _packet.state.SetStep(STATE_WAIT_STX);
        if ((rx_data == CMD_ETX) && (_packet.checksum == _packet.rx_checksum))
          return true;
        break;

      default:
        return false;
    }
    // end of  switch

    return false;
  }

private:
  inline void receiveCplt()
  {
    if (_cb)
    {
      _cb(nullptr, &_packet);
    }
  }

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//MARK:fw download functions

public:
	inline void registShortcallback()
	{
		if (_cfg.ptr_uart)
			_cfg.ptr_uart->registerShortCallback(std::bind(&downloader_peeler::UartCallback, this, std::placeholders::_1, std::placeholders::_2));
	}

	inline void clearShortcallback()
	{
		if (_cfg.ptr_uart)
			_cfg.ptr_uart->clearShortCallback();
	}

  void progress_bar(int progress, int total, int barWidth = 40)
  {
#ifdef __USE_CONSOLE_PROGRESS_BAR__
    constexpr int COLOR_BLACK         = 0;
    constexpr int COLOR_BLUE          = 1;
    constexpr int COLOR_GREEN         = 2;
    constexpr int COLOR_CYAN          = 3;
    constexpr int COLOR_RED           = 4;
    constexpr int COLOR_MAGENTA       = 5;
    constexpr int COLOR_YELLOW        = 6;
    constexpr int COLOR_WHITE         = 7;
    constexpr int COLOR_GRAY          = 8;
    constexpr int COLOR_LIGHT_BLUE    = 9;
    constexpr int COLOR_LIGHT_GREEN   = 10;
    constexpr int COLOR_LIGHT_CYAN    = 11;
    constexpr int COLOR_LIGHT_RED     = 12;
    constexpr int COLOR_LIGHT_MAGENTA = 13;
    constexpr int COLOR_LIGHT_YELLOW  = 14;
    constexpr int COLOR_LIGHT_WHITE   = 15;

    auto setConsoleTextColor = [&](int color) -> void
    {
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      SetConsoleTextAttribute(hConsole, color);
    };

    float progressRatio = static_cast<float>(progress) / total;
    int   barProgress   = static_cast<int>(progressRatio * barWidth);

    setConsoleTextColor(COLOR_LIGHT_YELLOW); // color yellow text
    std::cout << "  [";
    setConsoleTextColor(COLOR_LIGHT_GREEN);  // color green text
    for (int i = 0; i < barWidth; ++i)
    {
      if (i < barProgress)
      {
        std::cout << "#";
      }
      else
      {
        std::cout << " ";
      }
    }
    setConsoleTextColor(COLOR_LIGHT_YELLOW); // color yellow text
    std::cout << "] " << std::to_string(static_cast<int>(progressRatio * 100)) << "%\r";
    std::cout.flush();
    setConsoleTextColor(COLOR_WHITE);        // reset default color
#endif //__USE_CONSOLE_PROGRESS_BAR__
  }

  inline errno_t get_file_info(std::string_view file_name)
  {
    std::ifstream file(std::string(file_name), std::ios::binary);
    if (file.is_open())
    {
      file.seekg(0, std::ios::end);
      _file_size = file.tellg();
      file.seekg(0, std::ios::beg);

      _file_buf.resize(_file_size);
      file.read((char *)_file_buf.data(), _file_size);

      file.close();
      return 0;
    }
    else
    {
      std::cout << "Unable to open " << file_name << std::endl;
      return -1;
    }
  }

  inline bool add_tag_to_bin(const char *dest_file, const char *src_file)
  {
    uint16_t     t_crc = 0;
    firm_tag_st *p_tag;

    if (!strcmp(src_file, dest_file))
    {
      ERR_PRINT("src file(%s) and dst file(%s) is same! \n", src_file, dest_file);
      return false;
    }

    /*
				Calculate the distance from the end of the file to the beginning of the file (src_len),
				Then the code that does the work of moving the file pointer to the beginning of the file.
				*/

    p_tag = (firm_tag_st *)_file_buf.data();

    if (p_tag->magic_number == FLASH_MAGIC_NUMBER)
    {
      std::cout << "  already magic number" << std::endl;
      return true;
    }
    if (p_tag->magic_number != 0xAAAA5555)
    {
      std::cout << "  wrong magic number 0x" << std::hex << p_tag->magic_number << std::endl;
      return false;
    }

    /* Calculate CRC16 */
    size_t   i;
    uint32_t tag_size;
    tag_size = p_tag->size_tag;
    for (i = 0; i < _file_size - tag_size; i++)
    {
      fwCrc(&t_crc, _file_buf[tag_size + i]);
    }

    p_tag->magic_number     = FLASH_MAGIC_NUMBER;
    p_tag->tag_flash_start  = p_tag->addr_fw;
    p_tag->tag_flash_end    = p_tag->addr_fw + ((uint32_t)_file_size - tag_size);
    p_tag->tag_flash_length = p_tag->tag_flash_end - p_tag->tag_flash_start;
    p_tag->tag_length       = tag_size;
    // std::memcpy(&p_tag->tag_date_str, __DATE__, 32);
    // std::memcpy(&p_tag->tag_time_str, __TIME__, 32);
    memcpy(p_tag->tag_date_str.data(), __DATE__, p_tag->tag_date_str.size());
    memcpy(p_tag->tag_time_str.data(), __TIME__, p_tag->tag_time_str.size());

    p_tag->tag_flash_crc = t_crc;

    std::ofstream re_file(dest_file, std::ios::binary);
    if (re_file.is_open())
    {
      re_file.write((const char *)_file_buf.data(), _file_size);
      if (!re_file.good()) // not
      {
        // Write operation failed, handle the error
        ERR_PRINT("  total write fail! \n");
        re_file.close();
        return false;
      }
      else
      {
        std::cout << "  created file  : " << dest_file << " (" << (int)((_file_size) / 1024) << " KB)" << std::endl;
        std::cout << "  tag fw start  : 0x" << std::hex << p_tag->tag_flash_start << std::endl;
        std::cout << "  tag fw end    : 0x" << std::hex << p_tag->tag_flash_end << std::endl;
        std::cout << "  tag crc       : 0x" << std::hex << p_tag->tag_flash_crc << std::endl;
        std::cout << "  tag date      : " << p_tag->tag_date_str.data() << std::endl;
        std::cout << "  tag time      : " << p_tag->tag_time_str.data() << std::endl;
      }
    }
    else
    {
      ERR_PRINT("  unable to open dst file(%s)", dest_file);
      return false;
    }
    re_file.close();
    return true;
  }

  inline errno_t boot_cmd_read_boot_version(std::string &boot_ver)
  {
    // TODO: 여기에 구현 코드 추가.
    uint8_t err_code{};
    if (SendFirmCmdRxResp(BOOT_CMD_READ_BOOT_VERSION, NULL, 0, 500) == false)
    {
      ERR_PRINT("boot version check fail");
      return -1;
    }
    if (IsReceiveCmdOK())
    {
      for (int i = 0; i < _packet.length; i++)
      {
        boot_ver = boot_ver + (char)_packet.data[i];
      }
      err_code = ERROR_SUCCESS;
    }
    else
    {
      err_code = (errno_t)_packet.error;
    }
    return err_code;
  }

  inline errno_t boot_cmd_read_boot_name(std::string &boot_name)
  {
    uint8_t err_code{};
    if (SendFirmCmdRxResp(BOOT_CMD_READ_BOOT_NAME, NULL, 0, 500) == false)
    {
      ERR_PRINT("boot name check fail");
      return -1;
    }
    if (IsReceiveCmdOK())
    {
      for (int i = 0; i < _packet.length; i++)
      {
        boot_name = boot_name + (char)_packet.data[i];
      }
      err_code = ERROR_SUCCESS;
    }
    else
    {
      err_code = (errno_t)_packet.error;
    }
    return err_code;
  }

  inline errno_t boot_cmd_read_firm_version(std::string &firm_ver)
  {
    uint8_t err_code{};
    if (SendFirmCmdRxResp(BOOT_CMD_READ_FIRM_VERSION, NULL, 0, 500) == false)
    {
      ERR_PRINT("firm version check fail");
      return -1;
    }
    if (IsReceiveCmdOK())
    {
      for (int i = 0; i < _packet.length; i++)
      {
        firm_ver = firm_ver + (char)_packet.data[i];
      }
      err_code = ERROR_SUCCESS;
    }
    else
    {
      err_code = (errno_t)_packet.error;
    }
    return err_code;
  }

  inline errno_t boot_cmd_read_firm_name(std::string &firm_name)
  {
    uint8_t err_code{};
    if (SendFirmCmdRxResp(BOOT_CMD_READ_FIRM_NAME, NULL, 0, 500) == false)
    {
      ERR_PRINT("firm name check fail");
      return -1;
    }
    if (IsReceiveCmdOK())
    {
      for (int i = 0; i < _packet.length; i++)
      {
        firm_name = firm_name + (char)_packet.data[i];
      }
      err_code = ERROR_SUCCESS;
    }
    else
    {
      err_code = (errno_t)_packet.error;
    }
    return err_code;
  }

  inline errno_t boot_cmd_flash_erase(uint32_t addr, uint32_t length, uint32_t timeout)
  {
    uint8_t              err_code{};
    std::vector<uint8_t> datas{};
    datas.emplace_back((uint8_t)(addr >> 0));
    datas.emplace_back((uint8_t)(addr >> 8));
    datas.emplace_back((uint8_t)(addr >> 16));
    datas.emplace_back((uint8_t)(addr >> 24));
    datas.emplace_back((uint8_t)(length >> 0));
    datas.emplace_back((uint8_t)(length >> 8));
    datas.emplace_back((uint8_t)(length >> 16));
    datas.emplace_back((uint8_t)(length >> 24));
    if (SendFirmCmdRxResp(BOOT_CMD_FLASH_ERASE, datas.data(), (uint32_t)datas.size(), timeout) == false)
    {
      ERR_PRINT("flash erase fail");
      return -1;
    }
    if (IsReceiveCmdOK())
    {
      err_code = ERROR_SUCCESS;
    }
    else
    {
      err_code = (errno_t)_packet.error;
    }
    return err_code;
  }

  inline errno_t boot_cmd_flash_write(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
  {
    uint8_t err_code{};
    if (length > CMD_MAX_DATA_LENGTH)
    {
      err_code = BOOT_ERR_BUF_OVF;
      return err_code;
    }
    std::vector<uint8_t> datas{};
    datas.emplace_back((uint8_t)(addr >> 0));
    datas.emplace_back((uint8_t)(addr >> 8));
    datas.emplace_back((uint8_t)(addr >> 16));
    datas.emplace_back((uint8_t)(addr >> 24));
    datas.emplace_back((uint8_t)(length >> 0));
    datas.emplace_back((uint8_t)(length >> 8));
    datas.emplace_back((uint8_t)(length >> 16));
    datas.emplace_back((uint8_t)(length >> 24));
    for (size_t i = 0; i < length; i++)
    {
      datas.emplace_back(p_data[i]);
    }
    if (SendFirmCmdRxResp(BOOT_CMD_FLASH_WRITE, datas.data(), (uint32_t)datas.size(), timeout) == false)
    {
      ERR_PRINT("flash write fail");
      return -1;
    }
    if (IsReceiveCmdOK())
      err_code = ERROR_SUCCESS;
    else
      err_code = (errno_t)_packet.error;
    return err_code;
  }

  inline errno_t boot_cmd_jump_to_fw()
  {
    uint8_t err_code{};
    if (SendFirmCmdRxResp(BOOT_CMD_JUMP_TO_FW, NULL, 0, 1000 * 1) == false)
    {
      ERR_PRINT("jump to fw fail");
      return -1;
    }
    if (IsReceiveCmdOK())
    {
      err_code = ERROR_SUCCESS;
    }
    else
    {
      err_code = (errno_t)_packet.error;
    }
    return err_code;
  }

/****************************************************
  2. Request information from mcu through CMD object and reflect received information
 ****************************************************/
  inline errno_t upload_main(std::string_view file_info)
  {
    if (file_info.find(".bin") == std::string::npos)
    {
      ERR_PRINT("file extention is not .bin");
      return -1;
    }

    if (std::filesystem::exists(file_info) == false)
    {
      ERR_PRINT("file not found");
      return -1;
    }

    std::string dest_file_name(file_info);
    dest_file_name += ".fw";

    // regist callback function
    registShortcallback();
    // 1. boot start
    if (get_file_info(file_info) != ERROR_SUCCESS)
    {
      ERR_PRINT("file get information fail");
      return -1;
    }

    std::cout << "  file addr : " << FW_FILE_ADDR << std::endl;
    std::cout << "  file name : " << file_info << std::endl;
    std::cout << "  file size : " << std::to_string(_file_size) << " bytes" << std::endl;
    std::cout << "  file run  : " << (jump_fw ? "true" : "false") << std::endl
              << std::endl;
    std::cout << "  add tag..." << std::endl
              << std::endl;

    if (add_tag_to_bin(dest_file_name.c_str(), file_info.data()) == false)
    {
      ERR_PRINT("add tag fail");
      return -1;
    }


    std::string boot_ver{};
    std::string boot_name{};
    std::string firm_ver{};
    std::string firm_name{};
    errno_t     err_code{};
    uint32_t    pre_time{};
    uint32_t    exe_time{};


    // 2. boot version check
    if (boot_cmd_read_boot_version(boot_ver) == ERROR_SUCCESS)
    {
      std::cout << "  boot ver      : " << boot_ver << std::endl;
    }
    else
    {
      ERR_PRINT("boot version check fail");
      return -1;
    }

    if (boot_cmd_read_boot_name(boot_name) == ERROR_SUCCESS)
    {
      std::cout << "  boot name     : " << boot_name << std::endl;
    }
    else
    {
      ERR_PRINT("boot name check fail");
      return -1;
    }

    if (boot_cmd_read_firm_version(firm_ver) == ERROR_SUCCESS)
    {
      std::cout << "  firm ver      : " << firm_ver << std::endl;
    }
    else
    {
      ERR_PRINT("firm version check fail");
      return -1;
    }

    if (boot_cmd_read_firm_name(firm_name) == ERROR_SUCCESS)
    {
      std::cout << "  firm name     : " << firm_name << std::endl;
    }
    else
    {
      ERR_PRINT("firm name check fail");
      return -1;
    }

    //-- Flash Erase
    pre_time = tim::millis();
    err_code = boot_cmd_flash_erase(FW_FILE_ADDR, file::GetFileSize(file_info.data()), 1000 * 5);
    if (err_code == ERROR_SUCCESS)
    {
      std::cout << std::endl;
      std::cout << "  flash erase success" << std::endl;
      std::cout << "  flash erase time : " << std::to_string(tim::millis() - pre_time) << " ms" << std::endl;
    }
    else
    {
      ERR_PRINT("flash erase fail %s", get_peeler_fw_err_str(err_code));
      return -1;
    }

    //-- Flash Write
    std::ifstream fw_file(dest_file_name, std::ios::binary);
    if (fw_file.is_open() == false)
    {
      ERR_PRINT("file open fail");
      return -1;
    }
    /********************************************************************
	    2.1 Cut the fw data as much as TX_BLOCK_LENGTH to send to mcu
		 ********************************************************************/
    uint32_t addr{};
    uint32_t len{};
    bool write_done{};
    std::array<uint8_t, TX_BLOCK_LENGTH> tx_buf{};
    uint16_t write_percent{};
    uint16_t pre_percent{};

    // progressBar.SetRange(0, 100);
    addr = FW_FILE_ADDR;
    pre_time = tim::millis();
    std::cout << std::endl;
    std::cout << "  file write start" << std::endl;
    while (!fw_file.eof())
    {
      /* code */
      fw_file.read((char *)tx_buf.data(), TX_BLOCK_LENGTH);
      len      = (uint32_t)fw_file.gcount();
      err_code = boot_cmd_flash_write(addr, tx_buf.data(), len, 1000 * 5);
      if (err_code == ERROR_SUCCESS)
      {
        addr += len;

        write_percent = (uint16_t)(((float)(addr - FW_FILE_ADDR) / (float)_file_size) * 100);
        // progressBar.SetPos(write_percent);

        if ((write_percent / 10) != pre_percent)
        {
          // std::cout << "  flash write \t: " << std::to_string(write_percent) << "%" << std::endl;
          progress_bar(write_percent, 100);
          pre_percent = (write_percent / 10);
        }

        if ((addr - FW_FILE_ADDR) >= (uint32_t)_file_size)
        {
          write_done = true;
          break;
        }
      }
      else
      {
        ERR_PRINT("bootCmdFlashWrite fail : 0x%x, %s", addr, get_peeler_fw_err_str(err_code));
        break;
      }
    }
    fw_file.close();
    exe_time = tim::millis() - pre_time;

    std::cout << std::endl;
    std::cout << "  file write end" << std::endl
              << std::endl;

    /********************************************************************
				2.2 Validate and jump to fw
			 ********************************************************************/

    if (write_done == true)
    {
      std::cout << "flash write \t: OK (" << std::to_string(exe_time) << "ms)" << std::endl;
      if (jump_fw)
      {
        pre_time = tim::millis();
        err_code = boot_cmd_jump_to_fw();
        exe_time = tim::millis() - pre_time;
        if (err_code == ERROR_SUCCESS)
        {
          std::cout << "jump to fw \t: OK (" << std::to_string(exe_time) << "ms)" << std::endl;
          // threadLife = false;
        }
        else
        {
          ERR_PRINT("jump to fw fail : %d", err_code);
        }
      }
    }
    else
    {
      ERR_PRINT("flash write fail");
    }

    /****************************************************
			 3. end of process
			 ****************************************************/
    return ERROR_SUCCESS;
  }
 };
}
// namespace apcr
#endif //_UTIL_DOWNLOAD_PEELER_HPP