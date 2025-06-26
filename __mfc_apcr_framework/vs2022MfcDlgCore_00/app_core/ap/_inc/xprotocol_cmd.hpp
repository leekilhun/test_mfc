/**
 * @file xprotocol_cmd.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-05-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef AP__INC_XPROTOCOL_CMD_HPP_
#define AP__INC_XPROTOCOL_CMD_HPP_


/*

main(), secondary( )


TX  (main -> secondary) request information or action
obj_id [option] 0 is all or ignore
| STX0 | STX1 | CMD   | objId | Data Length |Data      | Checksum |
| :--- |:-----|:------|:----- |:------------|:---------| :------  |
| 0x4A | 0x4C | 2byte | 2byte | 2 byte      |Data 0～n | 1byte    |


Message
RX (secondary -> main) provide information
obj_id [option] 0 is all or ignore
| STX0 | STX1 | CMD  | objId | Data Length |Data      | Checksum |
| :--- |:-----|:------|:----- |:------------|:---------| :------  |
| 0x4A | 0x4C | 2byte | 2byte | 2 byte      |Data 0～n | 1byte    |


uint8_t check_sum = 0;
for (int i=0; i<index; i++)
{
  check_sum += p_cmd->packet.buffer[i];
}
check_sum = (~check_sum) + 1;
p_cmd->packet.buffer[index++] = check_sum;

*/

#include "ap_def.hpp"

namespace apcr
{
  struct xprotocol_cmd
  {
    static constexpr uint8_t CMD_STX0 = 0x4A;
    static constexpr uint8_t CMD_STX1 = 0x4C;

    static constexpr int CMD_MAX_DATA_LENGTH   = 1024;
    static constexpr int CMD_MAX_PACKET_LENGTH = (CMD_MAX_DATA_LENGTH + 8);
    static constexpr int PACKET_BUFF_LENGTH    = CMD_MAX_PACKET_LENGTH;

    struct cfg_t
    {
      uart_t::cfg_t           uartcfg{};
      std::shared_ptr<uart_t> ptr_uart{};

      cfg_t()                              = default;
      ~cfg_t()                             = default;
      cfg_t(const cfg_t &other)            = default; // copy constructor
      cfg_t &operator=(const cfg_t &other) = default; // copy assignment
      cfg_t(cfg_t &&other)                 = default; // move constructor
      cfg_t &operator=(cfg_t &&other)      = default; // move assignment
    } m_cfg{};

    using buff_t = std::array<uint8_t, CMD_MAX_PACKET_LENGTH>;

    struct packet_st
    {
      using step_t = apcr::machine_step_t;
      uint16_t cmd_type{};
      uint16_t obj_id{};
      uint16_t length{};
      uint8_t  checksum{};
      uint8_t  rx_checksum{};
      buff_t   buffer{};
      uint8_t  buffer_idx{};
      uint8_t *data{};
      uint16_t data_cnt{};
      uint32_t resp_ms{};
      step_t   state{};

      packet_st()                                  = default;
      ~packet_st()                                 = default;
      packet_st(const packet_st &other)            = default; // copy constructor
      packet_st &operator=(const packet_st &other) = default; // copy assignment
      packet_st(packet_st &&other)                 = default; // move constructor
      packet_st &operator=(packet_st &&other)      = default; // move assignment

      uint8_t BufferAdd(uint8_t rx_data)
      {
        checksum += rx_data;

        buffer[buffer_idx] = rx_data;
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
    } m_packet{};

    bool m_isConnected{};
    uint32_t m_prems{}; // elaps time
    std::function<int(void *, void *)> m_cb{};
    uint8_t m_reqFlag; // request flag: after send data, request return flag 1, return flag 0

    /****************************************************
     *  Constructor
     ****************************************************/

  public:
    xprotocol_cmd() = default;

    xprotocol_cmd(std::function<int(void *, void *)> cb)
    {
      m_cb = cb;
    };

    ~xprotocol_cmd() = default;

    /****************************************************
     *  func
     ****************************************************/
    inline errno_t init(cfg_t &cfg)
    {
      m_cfg = cfg;

      if (m_cfg.ptr_uart->IsConnected())
      {
      }

      if (m_cfg.ptr_uart->Init(m_cfg.uartcfg) == ERROR_SUCCESS)
      {
        REPORT_OUT("[OK] init Success! Uart %s, baud[%d]", m_cfg.uartcfg.port_name.c_str(), m_cfg.uartcfg.baudrate);
        m_isConnected = m_cfg.ptr_uart->IsConnected();
        m_cfg.ptr_uart->registerCallback(std::bind(&xprotocol_cmd::UartCallback, this, std::placeholders::_1, std::placeholders::_2));
      }
      else
      {
        ERR_PRINT("init fail! Uart %s, baud[%d]", m_cfg.uartcfg.port_name.c_str(), m_cfg.uartcfg.baudrate);
        return -1;
      }
      return ERROR_SUCCESS;
    }

    inline bool recovery()
    {
      m_isConnected = m_cfg.ptr_uart->Recovery();
      return m_isConnected;
    }

    inline bool IsAvailableComm()
    {
      return (m_reqFlag == 0);
    }

    inline bool isConnected()
    {
      //m_isConnected = ;
      return m_isConnected;
    }

    inline errno_t UartCallback(void *w_parm, void *l_parm)
    {
      if (w_parm == nullptr)
        return -1;
      int length = *((int *)w_parm);
      int index  = 0;

      for (index = 0; index < length; index++)
      {
        uint8_t data = *((uint8_t *)l_parm + index);
        if (recievePacket(data))
          receiveCplt();
      }

      return ERROR_SUCCESS;
    }

    inline void registerCallback(std::function<int(void *, void *)> cb)
    {
      m_cb = cb;
    }

    // inline errno_t SendCmd(uint8_t *ptr_data, uint32_t length)
    // {
    //   /*
    //     | STX0 | STX1 | CMD   | objId | Data Length |Data      | Checksum |
    //     | :--- |:-----|:------|:----- |:------------|:---------| :------  |
    //     | 0x4A | 0x4C | 2byte | 2byte | 2 byte      |Data 0～n | 1byte    |
    //   */

    //   std::vector<uint8_t> datas{CMD_STX0, CMD_STX1 };
    //   for (uint32_t i = 0; i < length; i++)
    //     datas.emplace_back(ptr_data[i]);

    //   uint8_t crc = 0;
    // 	for (uint32_t i = 1; i < datas.size(); i++)
    // 		crc ^= datas[i];

    //   datas.emplace_back(crc);

    //   std::cout << "TxData.Data: " << std::hex;
    //   for (const auto &elm : datas)
    //     std::cout << (int)elm << " ";
    //   std::cout << std::endl;

    //   if (m_cfg.ptr_uart->SendData((const char*)datas.data(), datas.size()))
    //     return ERROR_SUCCESS;
    //   return -1;
    // }

    inline uint32_t write(uint8_t *data, const uint32_t size)
    {
      m_reqFlag = 1; // request return flag
      m_prems   = tim::millis();
      return m_cfg.ptr_uart->Write(data, size);
    }

    inline bool recievePacket(uint8_t rx_data)
    {
      /*
        | STX0 | STX1 | Type  | objId | Data Length |Data      | Checksum |
        | :--- |:-----|:------|:----- |:------------|:---------| :------  |
        | 0x4A | 0x4C | 2byte | 2byte | 2 byte      |Data 0～n | 1byte    |
      */

      enum : uint8_t
      {
        STATE_WAIT_STX0,
        STATE_WAIT_STX1,
        STATE_WAIT_CMD_L,
        STATE_WAIT_CMD_H,
        STATE_WAIT_OBJ_ID_L,
        STATE_WAIT_OBJ_ID_H,
        STATE_WAIT_LENGTH_L,
        STATE_WAIT_LENGTH_H,
        STATE_WAIT_DATA,
        STATE_WAIT_CHECKSUM
      };

      constexpr int timeout_ms = 100;

      if (m_packet.state.MoreThan(timeout_ms))
        m_packet.BufferClear();

      switch (m_packet.state.GetStep())
      {
      case STATE_WAIT_STX0:
        if (rx_data == CMD_STX0)
        {
          m_packet.BufferClear();
          m_packet.BufferAdd(rx_data);
          m_packet.state.SetStep(STATE_WAIT_STX1);
        }
        break;

      case STATE_WAIT_STX1:
        if (rx_data == CMD_STX1)
        {
          m_packet.BufferAdd(rx_data);
          m_packet.state.SetStep(STATE_WAIT_CMD_L);
        }
        break;

      case STATE_WAIT_CMD_L:
        m_packet.cmd_type = rx_data;
        m_packet.BufferAdd(rx_data);
        m_packet.state.SetStep(STATE_WAIT_CMD_H);
        break;

      case STATE_WAIT_CMD_H:
        m_packet.cmd_type |= (rx_data << 8);
        m_packet.BufferAdd(rx_data);
        m_packet.state.SetStep(STATE_WAIT_OBJ_ID_L);
        break;

      case STATE_WAIT_OBJ_ID_L:
        m_packet.obj_id = rx_data;
        m_packet.BufferAdd(rx_data);
        m_packet.state.SetStep(STATE_WAIT_OBJ_ID_H);
        break;

      case STATE_WAIT_OBJ_ID_H:
        m_packet.obj_id |= (rx_data << 8);
        m_packet.BufferAdd(rx_data);
        m_packet.state.SetStep(STATE_WAIT_LENGTH_L);
        break;

      case STATE_WAIT_LENGTH_L:
        m_packet.length = rx_data;
        m_packet.BufferAdd(rx_data);
        m_packet.state.SetStep(STATE_WAIT_LENGTH_H);
        break;

      case STATE_WAIT_LENGTH_H:
        m_packet.length |= (rx_data << 8);
        m_packet.BufferAdd(rx_data);

        if (m_packet.length == 0)
          m_packet.state.SetStep(STATE_WAIT_CHECKSUM);
        else if (m_packet.length < (CMD_MAX_DATA_LENGTH + 1))
          m_packet.state.SetStep(STATE_WAIT_DATA);
        else
          m_packet.state.SetStep(STATE_WAIT_STX0);
        break;

      case STATE_WAIT_DATA:
        // assign data address
        if (m_packet.data_cnt++ == 0)
          m_packet.data = &m_packet.buffer[m_packet.buffer_idx];

        // check length
        if (m_packet.data_cnt == m_packet.length)
          m_packet.state.SetStep(STATE_WAIT_CHECKSUM);

        m_packet.BufferAdd(rx_data);
        break;

      case STATE_WAIT_CHECKSUM:
        m_packet.rx_checksum = rx_data;
        m_packet.checksum    = (~m_packet.checksum) + 1;
        m_packet.state.SetStep(STATE_WAIT_STX0);
        // std::cout << "STATE_WAIT_CHECKSUM  checksum_recv [0x" << std::hex << (int)m_packet.checksum_recv
        //<<"] ,  cal checksum [0x"<< int(m_packet.checksum) << "] "<<std::endl;
        if (m_packet.checksum == m_packet.rx_checksum)
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
      m_reqFlag        = 0;
      m_packet.resp_ms = tim::millis() - m_prems;
      if (m_cb)
      {
        m_cb(nullptr, &m_packet);
      }
    }
  };
}
// end of namespace apcr

#endif // AP__INC_XPROTOCOL_CMD_HPP_