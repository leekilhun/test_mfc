/**
 * @file api_cmd.hpp
 * @author lee kil hun (pba7189@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-12
 *       2025-06-04 : 
 *         out write register를 주기적으로 클래스의 상태를 컨트롤러에 보낸다
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef AP__INC_API_CMD_HPP__
#define AP__INC_API_CMD_HPP__

#include "ap_def.hpp"

namespace apcr
{

  class api_cmd :  public Icommon, public xprotocol_cmd
  {
    //****************************************************
    //   data
    // ****************************************************

  public:
    enum tx_t : uint16_t
    {
      TX_OK_RESPONSE  = 0xAA55,
      TX_REQ_MCU_DATA = 0x0001,
      TX_REQ_MCU_INFO = 0x0002,
      //
      TX_CTRL_IO_OUT     = 0x0010,
      TX_CTRL_CYL        = 0x0011,
      TX_CTRL_VAC        = 0x0012,
      TX_CTRL_REG_OPTION = 0x0013,
      //
      TX_CTRL_INITIALIZE = 0x001A,
      TX_CTRL_VIRTUAL_SW = 0x001B,
      //
      TX_CTRL_MOT_ORIGIN      = 0x0020,
      TX_CTRL_MOT_ONOFF       = 0x0021,
      TX_CTRL_MOT_MOVE        = 0x0022,
      TX_CTRL_MOT_STOP        = 0x0023,
      TX_CTRL_MOT_JOG         = 0x0024,
      TX_CTRL_MOT_LIMIT       = 0x0025,
      TX_CTRL_MOT_ZEROSET     = 0x0026,
      TX_CTRL_MOT_RELMOVE     = 0x0027,
      TX_CTRL_MOT_CLEAR_ALARM = 0x0028,
      TX_CTRL_MOT_CHANGE_VEL  = 0x0029,
      TX_CTRL_MOT_MOVE_VEL    = 0x002A,
      TX_CTRL_MOT_RELMOVE_VEL = 0x002B,
      TX_CTRL_MOT_VEL_JOG     = 0x002C,


      TX_MODE_FW_DOWNLOAD = 0xF000,
    };

    // RX (secondary -> main )
    enum rx_t : uint16_t
    {
      RX_OK_RESPONSE = 0x55AA,

      RX_MCU_DATA  = 0x0001,
      RX_MCU_INFO  = 0x0002,
      RX_FIRM_INFO = 0x0051,
      RX_MSG_ERROR = 0x0052,
    };

    enum doStep : uint8_t
    {
      STEP_INIT,
      STEP_TODO,
      STEP_TIMEOUT,
      STEP_WAIT_RETURN,
      STEP_WAIT_REPARSE,

      STEP_STATE_UPDATE,
      STEP_STATE_UPDATE_START,
      STEP_STATE_UPDATE_WAIT,
      STEP_STATE_UPDATE_END,
    };

    enum job_status_idx : uint8_t
    {
      jsi_0, 
      jsi_1,
      jsi_2,
      jsi_3,
      jsi_4,
      jsi_5,
      jsi_6,
      jsi_7,
  
      jsi_do_update_state,
      jsi_9,
      jsi_10,
      jsi_11,
      jsi_12,
      jsi_13,
      jsi_14,
      jsi_15,
  
      jsi_16,
      jsi_17,
      jsi_18,
      jsi_19,
      jsi_20,
      jsi_21,
      jsi_22,
      jsi_23,
  
      jsi_24,
      jsi_25,
      jsi_26,
      jsi_27,
      jsi_28,
      jsi_29,
      jsi_30,
      jsi_31,
  
      jsi_max
    } ;
    udword_t m_jobflag{};
    enum tm_t : uint8_t
    {
      tm_idx_0,
      tm_idx_1,
      tm_idx_comm_check,
      tm_idx_log_out,
      tm_idx_wire_reg_tx,
      tm_idx_max,
    };
    Timer<tm_idx_max> m_timers{};
    struct cfg_t
    {
      xprotocol_cmd::cfg_t comm_cfg{};
      apcr::app_data_t          *ptr_data{};

      cfg_t()                              = default;
      ~cfg_t()                             = default;
      cfg_t(const cfg_t &other)            = default; // copy constructor
      cfg_t &operator=(const cfg_t &other) = default; // copy assignment
      cfg_t(cfg_t &&other)                 = default; // move constructor
      cfg_t &operator=(cfg_t &&other)      = default; // move assignment
    } m_cfg{};

    apcr::machine_step_t m_step{};
    uint32_t m_elaps{};

    std::string m_toolinfo{};
    bool        m_rcvflag{}; // receive flag
    TinyC::Que<logDat> m_logQ{ 256 };    
    check_comm_t m_check_comm{};

    std::function<int(void*, void*)> m_cb{};

    
  public:
    /****************************************************
     *  Constructor
     ****************************************************/
    api_cmd(const apcr::attribute_st &f) ;

    api_cmd() = default;
    ~api_cmd();

    /****************************************************
     *  overriding
     ****************************************************/
    bool write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...) override;

  public:
    /****************************************************
     *  func
     ****************************************************/
    errno_t Init(api_cmd::cfg_t cfg);

    void ThreadJob();

    bool is_comm_lift();

    errno_t UartCallback(void *w_parm, void *l_parm);

    void registerCallback(std::function<int(void*, void*)> cb);

    void parsing_data(xprotocol_cmd::packet_st &packet);

    // tx_t별 templete 함수 send_data
    template <typename TxType, typename... Args>
    bool send_data(TxType tx, Args... args);

    //MARK:funcs
    /****************************************************
    ****************************************************/
    errno_t set_task_req_mcu_data(bool onoff = true);

    errno_t ok_resp();

		errno_t do_key_start();

    errno_t do_key_stop();

    errno_t do_key_reset();

    errno_t do_enable();

    errno_t do_disable();

    errno_t do_motor_enc_reset();

    errno_t do_motor_alarm_reset();

    errno_t do_motor_org();

    errno_t do_motor_jog(int dir);

    errno_t do_motor_move(int steps, uint16_t rpm, uint16_t acc = 100, uint16_t dec = 100);

    errno_t do_motor_move_rel(int steps, uint16_t rpm, uint16_t acc = 100, uint16_t dec = 100);

    errno_t do_system_init();

    errno_t do_motor_stop();

    errno_t do_write_out_reg(uint32_t out_reg);

    errno_t do_write_reg_option(uint32_t reg_option);
    
    
    void doRunStep();
  


#ifdef _USE_HW_CLI
    //---------------------------------------------------
    //MARK:cli   test code
    //---------------------------------------------------
    int testCB_func(int argc, char *argv[]);

#endif
  };
}
// namespace apcr
#endif //AP__INC_API_CMD_HPP__