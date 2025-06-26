/**
 * @file api_cmd.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 
#include "ap.hpp"
#include "ap_dat.hpp"


using namespace apcr;

api_cmd::api_cmd(const apcr::attribute_st &f) :
apcr::IObj(f)
{
}

api_cmd::~api_cmd() {

};

bool api_cmd::write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...)
{
  if (fmt)
  {
    va_list args;
    va_start(args, fmt);
    std::vector<char> buffer(1024);
    vsnprintf(buffer.data(), buffer.size(), fmt, args);
    //writeLog(level, log.file.c_str(), log.func_name.c_str(), log.line_no, buffer.data());
    constexpr int obj_idx = 0;
    TinyC::mcc_log::WriteLog((TinyC::mcc_log::level)level, obj_idx, "api_cmd.log", log.file.c_str(), log.func_name.c_str(), log.line_no, buffer.data());

    va_end(args);
    return true;
  }

  return false;
}

errno_t api_cmd::Init(api_cmd::cfg_t cfg)
{
  m_cfg = cfg;

  apcr::log_info_t data{__FILE__, __FUNCTION__, __LINE__};
  write_log(TinyC::Log::level::lvl_info, data, "init success! \n");

  if (xprotocol_cmd::init(m_cfg.comm_cfg) == ERROR_SUCCESS)
  {
    xprotocol_cmd::registerCallback(std::bind(&api_cmd::UartCallback, this, std::placeholders::_1, std::placeholders::_2));
    REPORT_OUT("[OK] xprotocol_cmd::init Success!");
  }
  else
  {
    ERR_PRINT("xprotocol_cmd::init Fail!");
  }

  return ERROR_SUCCESS;
}

void api_cmd::ThreadJob()
{
  if (m_jobflag(jsi_do_update_state))
  {
    if (m_timers.MoreThan(3'000, tm_idx_comm_check))
    {
      m_timers.Reset(tm_idx_comm_check);
      m_check_comm.update();


      if (is_comm_lift() == false)
      {
        m_check_comm.reset_flag();
        LOG_OUT("recovery comm");
        xprotocol_cmd::m_cfg.ptr_uart->Recovery();
      }
    }
  }

  if (m_timers.MoreThan(20, tm_idx_wire_reg_tx))
  {
    m_timers.Reset(tm_idx_wire_reg_tx);
    //--------------------------------------------------
    //MARK: send out reg
    //--------------------------------------------------
    uint32_t out_reg = (get_ioY()).u32D;
    do_write_out_reg(out_reg);
  }


  doRunStep();
}

bool api_cmd::is_comm_lift()
{
  return m_check_comm.is_lift();
}

errno_t api_cmd::UartCallback(void *w_parm, void *l_parm)
{
  // w_parm;
  if (m_cb)
    m_cb(w_parm, l_parm);

  m_check_comm.reset_flag();
  apcr::xprotocol_cmd::packet_st data = *(apcr::xprotocol_cmd::packet_st *)l_parm;
  parsing_data(data);

  return ERROR_SUCCESS;
}

void api_cmd::registerCallback(std::function<int(void *, void *)> cb)
{
  m_cb = cb;
}

void api_cmd::parsing_data(xprotocol_cmd::packet_st &packet)
{
  //LOG_OUT("cmd type[0x%02X], resp[%d]ms", packet.cmd_type, packet.resp_ms);
  using dat_t = mcudat_t;

  // 비트를 출력하는 헬퍼 함수
  // auto to_binary_str = [&](uint32_t value) ->std::string
  // {
  //   std::bitset<32> binary(value);                  // 32비트로 변환
  //   std::string     binary_str = binary.to_string(); // 비트열을 문자열로 변환

  //   // 4비트씩 공백으로 구분
  //   std::string formatted;
  //   for (size_t i = 0; i < binary_str.size(); ++i)
  //   {
  //     formatted += binary_str[i];
  //     if ((i + 1) % 4 == 0 && i != binary_str.size() - 1)
  //       formatted += ' ';
  //   }
  //   return formatted;
  // };


  // 비트를 출력하는 헬퍼 함수
  auto to_binary_str = [&](auto value, bool is_reverse = false) -> std::string
  {
    std::bitset<sizeof(value) * 8> binary(value);                   // 비트로 변환
    std::string                    binary_str = binary.to_string(); // 비트열을 문자열로 변환

    // 4비트씩 공백으로 구분
    std::string formatted;
    if (is_reverse)
    {
      for (size_t i = binary_str.size(); (i + 1) > 0; --i)
      {
        formatted += binary_str[i];
        if (i % 4 == 0 && i != binary_str.size())
          formatted += ' ';
      }
    }
    else
    {
      for (size_t i = 0; i < binary_str.size(); ++i)
      {
        formatted += binary_str[i];
        if ((i + 1) % 4 == 0 && i != binary_str.size() - 1)
          formatted += ' ';
      }
    }

    return formatted;
  };

  switch (packet.cmd_type)
  {
  case rx_t::RX_OK_RESPONSE: break;
  case rx_t::RX_MCU_DATA:
  {
    ok_resp();
    //std::memcpy(&receive_date, (const char *)packet.data, packet.length);//

    //// *m_cfg.ptr_data = *(reinterpret_cast<mcudat_t*>(packet.data));
    std::memcpy(m_cfg.ptr_data, (const char *)packet.data, packet.length);
    ////*m_cfg.ptr_data = *(reinterpret_cast<mcudat_t*>(packet.data));
    ////if (receive_date.io_X.at(0))
    ////  std::cout << "mcd_data - reg_in      : " << to_binary_str(receive_date.io_X.at(0)) << std::endl;
    //std::cout << "mcd_data - reg_in      : " << to_binary_str(m_cfg.ptr_data->get_ioX().get()) << std::endl; //io_X[dat_t::idi_io32_0]) << std::endl;

    //std::cout << "mcd_data - reg_state   : " << to_binary_str(m_cfg.ptr_data->reg_state) << std::endl;
    //std::cout << "mcd_data - reg_option  : " << to_binary_str(m_cfg.ptr_data->reg_opt) << std::endl;
    //std::cout << "mcd_data - reg_error   : " << to_binary_str(m_cfg.ptr_data->reg_err) << std::endl;
    //std::cout << "mcd_data - reg_in      : " << to_binary_str(m_cfg.ptr_data->io_X[dat_t::idi_io32_0]) << std::endl;
    //std::cout << "mcd_data - reg_out     : " << to_binary_str(m_cfg.ptr_data->io_Y[dat_t::idi_io32_0]) << std::endl;
    //std::cout << "mcd_data - mt_1_pulse  : " << (double(m_cfg.ptr_data->mt_poses[dat_t::mdi_1]) / 1000) << std::endl;
    //std::cout << "mcd_data - mt_1_status : " << to_binary_str(m_cfg.ptr_data->mt_stats[dat_t::mdi_1]) << std::endl;
    //std::cout << "mcd_data - mt_2_pulse  : " << (double(m_cfg.ptr_data->mt_poses[dat_t::mdi_2]) / 1000) << std::endl;
    //std::cout << "mcd_data - mt_2_status : " << to_binary_str(m_cfg.ptr_data->mt_stats[dat_t::mdi_2]) << std::endl;
    //std::cout << "mcd_data - data-1      : " << (double(m_cfg.ptr_data->datas[dat_t::di_1]) / 1000) << std::endl;
    //std::cout << "mcd_data - data-2      : " << (double(m_cfg.ptr_data->datas[dat_t::di_2]) / 1000) << std::endl;
  }
  break;

  case rx_t::RX_MCU_INFO:
  {
    ok_resp();

    //strlcpy(&m_toolinfo[0], (const char *)&packet.data, m_toolinfo.size());
    std::memcpy(&m_toolinfo[0], (const char *)&packet.data, packet.length);
  }
  break;

  case rx_t::RX_MSG_ERROR:
  {
    logDat log_data = *(reinterpret_cast<logDat *>(packet.data));

    m_logQ.Put(log_data);
    /*std::cout << "log_data - header : " << int(log_data.log_buff.head.header) << std::endl;
				std::cout << "log_data - error_no : " << int(log_data.log_buff.head.error_no) << std::endl;
				std::cout << "log_data - obj_idx : " << int(log_data.log_buff.head.obj_idx) << std::endl;
				std::cout << "log_data - step_no : " << int(log_data.log_buff.head.step_no) << std::endl;
				std::cout << "log_data - log : " << &log_data.log_buff.log[0] << std::endl;*/
  }
  break;
  default: break;
  }
  // end of switch
}

// tx_t별 templete 함수 send_data
template <typename TxType, typename... Args>
bool api_cmd::send_data(TxType tx, Args... args)
{
  if constexpr (std::is_same_v<TxType, api_cmd::tx_t> == false)
  {
    LOG_PRINT("not same type");
    return false;
  }
  constexpr int data_length_max = PACKET_BUFF_LENGTH;

  using data_t = std::array<uint8_t, data_length_max>;
  data_t datas{};

  enum : uint8_t
  {
    idx_stx0,
    idx_stx1,
    idx_type_l,
    idx_type_h,
    idx_objid_l,
    idx_objid_h,
    idx_length_l,
    idx_length_h,
    idx_data
  };
   
  // constexpr uint16_t obj_id   = 0x0000;
  uint16_t length    = 0;
  uint16_t cmd_type  = tx;
  datas[idx_stx0]    = CMD_STX0;
  datas[idx_stx1]    = CMD_STX1;
  datas[idx_type_l]  = LoBYTe(cmd_type);
  datas[idx_type_h]  = HiBYTe(cmd_type);
  datas[idx_objid_l] = 0;
  datas[idx_objid_h] = 0;

  auto set_objid = [&datas](auto obj_id) -> uint16_t
  {
    datas[idx_objid_l] = LoBYTe(obj_id);
    datas[idx_objid_h] = HiBYTe(obj_id);
    return (uint16_t)sizeof(obj_id);
  };


  // Create an array with explicit type
  // std::tuple<Args...>
  // datas(args...);

  // 인수가 1개 이상인 경우, 공통 타입을 가지고 array 데이터를 만든다.
  if constexpr (sizeof...(args) > 0)
  {
    using elems_t = std::common_type_t<Args...>;
    using value_t = std::array<elems_t, sizeof...(args)>;
    value_t values{args...};

    auto set_data = [&packet = datas](auto offset, auto src) -> uint16_t
    {
      std::memcpy(&packet[idx_data + offset], &src, sizeof(src));
      return (uint16_t)sizeof(src);
    };

    if constexpr (sizeof...(args) == 1)
    {
      switch (tx)
      {
      case tx_t::TX_CTRL_MOT_CLEAR_ALARM: __fallthrough;
      case tx_t::TX_CTRL_MOT_ZEROSET:     __fallthrough;
      case tx_t::TX_CTRL_MOT_STOP:        __fallthrough;
      case tx_t::TX_CTRL_MOT_ORIGIN:      __fallthrough;
      case tx_t::TX_CTRL_MOT_LIMIT:       __fallthrough;
      case tx_t::TX_CTRL_VIRTUAL_SW:
      {
        set_objid((uint16_t)values[0]);
      }
      break;

      case tx_t::TX_CTRL_IO_OUT:
      {
        length = set_data(0, (uint32_t)values[0]);
      }
      break;

      case tx_t::TX_CTRL_REG_OPTION:
      {
        length = set_data(0, (uint32_t)values[0]);
      }
      break;

      default:
        LOG_PRINT("Error! Wrong type.");
        return false;
      }
    }
    else if constexpr (sizeof...(args) == 2)
    {
      switch (tx)
      {
      case tx_t::TX_CTRL_MOT_ONOFF:
      {
        enum : uint8_t
        {
          _objid,
          _on_off
        };

        set_objid((uint16_t)values[_objid]);
        length  = 0;
        length += set_data(length, (uint16_t)values[_on_off]);
      }
      break;
      case tx_t::TX_CTRL_MOT_JOG:
      {
        enum : uint8_t
        {
          _objid,
          _data1                                         // velocity
        };

        set_objid((uint16_t)values[_objid]);
        length  = 0;
        length += set_data(length, (int)values[_data1]); // rpm (-rpm은 역방향)
      }
      break;

      default: LOG_PRINT("Error! Wrong type."); return false;
      }
    }
    else if constexpr (sizeof...(args) == 3)
    {
    }
    else if constexpr (sizeof...(args) == 4)
    {
      switch (tx)
      {
      case tx_t::TX_CTRL_MOT_MOVE: __fallthrough;
      case tx_t::TX_CTRL_MOT_RELMOVE:
      {
        enum : uint8_t
        {
          _data1,                                        // steps
          _data2,                                        // rpm
          _data3,                                        //accerate
          _data4,                                        // decelate
        };

        length  = 0;
        length += set_data(length, (int)values[_data1]); // steps
        length += set_data(length, (int)values[_data2]); // rpm (-rpm은 역방향)
        length += set_data(length, (int)values[_data3]); //
        length += set_data(length, (int)values[_data4]); //
      }
      break;
      default: LOG_PRINT("Error! Wrong type."); return false;
      }
    }
    else
    {
      LOG_PRINT("Error! Too many arguments.");
      return false;
    }
  }
  else
  {
    switch (tx)
    {
    case tx_t::TX_REQ_MCU_DATA:    __fallthrough;
    case tx_t::TX_REQ_MCU_INFO:    __fallthrough;
    case tx_t::TX_CTRL_INITIALIZE: __fallthrough;
    case tx_t::TX_OK_RESPONSE:     break;

    default:                       LOG_PRINT("Error! Wrong type."); return false;
    }
    // end of switch
  }

  datas[idx_length_l] = (uint8_t)(length >> 0);
  datas[idx_length_h] = (uint8_t)(length >> 8);

  uint32_t idx      = (uint32_t)idx_data + length;
  uint8_t  checksum = 0;
  for (const auto &elm : datas)
    checksum += elm;
  checksum   = (~checksum) + 1;
  datas[idx] = checksum;

  length = idx + 1;

  return (xprotocol_cmd::write(datas.data(), length) > 0);
}

//MARK:funcs
/****************************************************
    ****************************************************/
errno_t api_cmd::set_task_req_mcu_data(bool onoff)
{
  m_jobflag(jsi_do_update_state, onoff);
  return ERROR_SUCCESS;
}

errno_t api_cmd::ok_resp()
{
  return (send_data(TX_OK_RESPONSE) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_key_start()
{
  uint16_t vkey = (uint16_t)apcr::apReg::vkey_start;
  return (send_data(TX_CTRL_VIRTUAL_SW, vkey) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_key_stop()
{
  uint16_t vkey = (uint16_t)apcr::apReg::vkey_stop;
  return (send_data(TX_CTRL_VIRTUAL_SW, vkey) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_key_reset()
{
  uint16_t vkey = (uint16_t)apcr::apReg::vkey_reset;
  return (send_data(TX_CTRL_VIRTUAL_SW, vkey) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_enable()
{
  constexpr uint16_t mot_id = apcr::ap_obj::MOTOR_MAX;
  constexpr uint16_t on     = 1;
  return (send_data(TX_CTRL_MOT_ONOFF, mot_id, on) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_disable()
{
  constexpr uint16_t mot_id = apcr::ap_obj::MOTOR_MAX;
  constexpr uint16_t on     = 0;
  return (send_data(TX_CTRL_MOT_ONOFF, mot_id, on) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_enc_reset()
{
  constexpr uint16_t mot_id = apcr::ap_obj::MOTOR_MAX;
  return (send_data(TX_CTRL_MOT_ZEROSET, mot_id) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_alarm_reset()
{
  constexpr uint16_t mot_id = apcr::ap_obj::MOTOR_MAX;
  return (send_data(TX_CTRL_MOT_CLEAR_ALARM, mot_id) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_org()
{
  constexpr uint16_t mot_id = apcr::ap_obj::MOTOR_MAX;
  return (send_data(TX_CTRL_MOT_ORIGIN, mot_id) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_jog(int dir)
{
  constexpr uint16_t mot_id = apcr::ap_obj::MOTOR_MAX;
  return (send_data(TX_CTRL_MOT_JOG, mot_id, dir) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_move(int steps, uint16_t rpm, uint16_t acc, uint16_t dec)
{
  return (send_data(TX_CTRL_MOT_MOVE, steps, rpm, acc, dec) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_move_rel(int steps, uint16_t rpm, uint16_t acc, uint16_t dec)
{
  return (send_data(TX_CTRL_MOT_RELMOVE, steps, rpm, acc, dec) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_system_init()
{
  return (send_data(TX_CTRL_INITIALIZE) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_motor_stop()
{
  constexpr uint16_t mot_max_id = apcr::ap_obj::MOTOR_MAX;
  return (send_data(TX_CTRL_MOT_STOP, mot_max_id) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_write_out_reg(uint32_t out_reg)
{
  return (send_data(TX_CTRL_IO_OUT, out_reg) ? ERROR_SUCCESS : ERROR_FAIL);
}

errno_t api_cmd::do_write_reg_option(uint32_t reg_option)
{
  return (send_data(TX_CTRL_REG_OPTION, reg_option) ? ERROR_SUCCESS : ERROR_FAIL);
}

//MARK: do run step
void api_cmd::doRunStep()
{
  constexpr uint8_t  step_retry_max  = 3;
  constexpr uint32_t step_wait_delay = 30;


  switch (m_step.GetStep())
  {
  case STEP_INIT:
  {
    m_step.sub_step  = 0;
    m_step.retry_cnt = 0;


    LOG_OUT("STEP_INIT");
    m_step.SetStep(STEP_TODO);
  }
  break;
  //MARK: to do
  //---------------------------------------------------
  //   to do
  //---------------------------------------------------
  case STEP_TODO:
  {
    udword_t opt_reg               = m_cfg.ptr_data->get_opt();
    bool     is_option_auto_update = opt_reg(apcr::apReg::OP_auto_update);
    bool     is_do_update          = m_jobflag(jsi_do_update_state);
    if (is_option_auto_update == false && is_do_update)
      m_step.SetStep(STEP_STATE_UPDATE);
  }
  break;

  //---------------------------------------------------
  //   timeout
  //---------------------------------------------------
  case STEP_TIMEOUT:
  {
    //LOG_PRINT("STEP_TIMEOUT recovery result[%d]", xprotocol_tool::m_cfg.ptr_uart->Recovery());

    m_step.SetStep(STEP_TODO);
  }
  break;

  //---------------------------------------------------
  //   wait return
  //---------------------------------------------------
  case STEP_WAIT_RETURN:
  {
    m_step.SetStep(STEP_TODO);
  }
  break;
  //MARK:update
  //---------------------------------------------------
  //   update
  //---------------------------------------------------
  case STEP_STATE_UPDATE:
  {
    m_step.sub_step  = 0;
    m_step.retry_cnt = 0;
    m_step.SetStep(STEP_STATE_UPDATE_START);
  }
  break;

  case STEP_STATE_UPDATE_START:
  {
    send_data(tx_t::TX_REQ_MCU_DATA);
    m_step.SetStep(STEP_STATE_UPDATE_WAIT);
  }
  break;

  case STEP_STATE_UPDATE_WAIT:
  {
    if (IsAvailableComm())
      m_step.SetStep(STEP_STATE_UPDATE_END);
    else
    {
      if (m_step.LessThan(step_wait_delay))
        break;
      if (m_step.retry_cnt++ < step_retry_max)
      {
        m_step.SetStep(STEP_STATE_UPDATE_WAIT);
        break;
      }

      m_step.SetStep(STEP_TIMEOUT);
    }
  }
  break;

  case STEP_STATE_UPDATE_END:
  {
    m_step.sub_step  = 0;
    m_step.retry_cnt = 0;
    m_step.SetStep(STEP_TODO);
  }
  break;
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  default: break;
  }
  // end of switch
}


#ifdef _USE_HW_CLI
//MARK:cli
//---------------------------------------------------
//   test code
//--------------------------------------------------
int api_cmd::testCB_func(int argc, char *argv[])
{
  auto get_data = [](char *int_ptr) -> int
  {
    int ret = 0;
    ret     = (int)strtoul((const char *)int_ptr, (char **)NULL, (int)0);
    return ret;
  };
  std::string name{argv[0]};
  bool        ret{};

  enum : int
  {
    arg_cnt_0,
    arg_cnt_1,
    arg_cnt_2,
    arg_cnt_3
  };

  switch ((argc - 1))
  {
  case arg_cnt_1:
  {
    if (argv[1])
    {
      if (std::string(argv[1]).compare("info") == 0)
      {
      }
      else if (std::string(argv[1]).compare("data") == 0)
      {
        ret                        = true;
        constexpr uint8_t ret_line = 10;
        using dat_t                = mcudat_t;
        /*************************************************
             * mcu data register
             *-----------------------------------------------
             * io_X      : 00000000 00000000 00000000 00000000
             * io_Y      : 00000000 00000000 00000000 00000000
             * reg_opt   : 00000000 00000000 00000000 00000000
             * reg_err   : 00000000 00000000 00000000 00000000
             * reg_stat  : 00000000 00000000
             * m1-status : 10000000 00000000
             * m1-pulse  : 0
             *************************************************/
        auto print_reg = [&]()
        {
          udword_t io_X{m_cfg.ptr_data->get_ioX()};
          udword_t io_Y{m_cfg.ptr_data->get_ioY()};
          udword_t reg_opt{m_cfg.ptr_data->get_opt()};
          udword_t reg_err{m_cfg.ptr_data->get_err()};
          udword_t reg_stat{m_cfg.ptr_data->get_state()};
          udword_t mt_1_stat{(uint32_t)m_cfg.ptr_data->get_mt_stat(dat_t::mdi_x)};
          udword_t mt_1_pulse{(uint32_t)m_cfg.ptr_data->get_mt_pose(dat_t::mdi_x)};

          // 2진수 문자열로 변환하는 람다 함수
          auto to_1010 = [](uint32_t val, uint8_t len = 8) -> std::string
          {
            std::string bin;
            for (int i = 0; i < len; i++)
              bin += ((val >> i) & 0x01) ? '1' : '0';
            return bin;
          };

          TinyC::Cli::cliPrintf(" * io_X      : %s %s %s %s\n",
                                to_1010(io_X[0], 8).c_str(),
                                to_1010(io_X[1], 8).c_str(),
                                to_1010(io_X[2], 8).c_str(),
                                to_1010(io_X[3], 8).c_str());
          TinyC::Cli::cliPrintf(" * io_Y      : %s %s %s %s\n",
                                to_1010(io_Y[0], 8).c_str(),
                                to_1010(io_Y[1], 8).c_str(),
                                to_1010(io_Y[2], 8).c_str(),
                                to_1010(io_Y[3], 8).c_str());
          TinyC::Cli::cliPrintf(" * reg_opt   : %s %s %s %s\n",
                                to_1010(reg_opt[0], 8).c_str(),
                                to_1010(reg_opt[1], 8).c_str(),
                                to_1010(reg_opt[2], 8).c_str(),
                                to_1010(reg_opt[3], 8).c_str());
          TinyC::Cli::cliPrintf(" * reg_err   : %s %s %s %s\n",
                                to_1010(reg_err[0], 8).c_str(),
                                to_1010(reg_err[1], 8).c_str(),
                                to_1010(reg_err[2], 8).c_str(),
                                to_1010(reg_err[3], 8).c_str());
          TinyC::Cli::cliPrintf(" * reg_stat  : %s %s\n",
                                to_1010(reg_stat[0], 8).c_str(),
                                to_1010(reg_stat[1], 8).c_str());
          TinyC::Cli::cliPrintf(" * m1-status : %s %s \n",
                                to_1010(mt_1_stat[0], 8).c_str(),
                                to_1010(mt_1_stat[1], 8).c_str());
          TinyC::Cli::cliPrintf(" * m1-pulse  : %d\n", mt_1_pulse);
          TinyC::Cli::cliPrintf(" *************************************************/\n");

          TinyC::Cli::cliPrintf("\x1B[%dA", ret_line);
        };

        TinyC::Cli::cliPrintf("/*************************************************\n");
        TinyC::Cli::cliPrintf(" * mcu data register\n");
        TinyC::Cli::cliPrintf(" *-----------------------------------------------\n");

        while (TinyC::Cli::cliKeepLoop())
        {
          print_reg();
          tim::delay(100);
        }
      }
    }
  }
  break;
  case arg_cnt_2:
  {
  }
  break;
  case arg_cnt_3:
  {
    if (argv[1])
    {
      if (std::string(argv[1]).compare("set_out") == 0)
      {
        int      idx = get_data(argv[2]);
        int      val = get_data(argv[3]);
        udword_t io_Y{m_cfg.ptr_data->get_ioY()};
        io_Y(idx, val);
        send_data(TX_CTRL_IO_OUT, io_Y.get());
        TinyC::Cli::cliPrintf("set_out idx[%d] val[%d]\n", idx, val);
      }
    }
  }
  break;
  default:
    break;
  }
  // end of switch

  if (ret)
    return 0;

  std::cout << name << " info" << std::endl;
  std::cout << name << " data" << std::endl;
  std::cout << name << " set_out idx[0:7] [0:1](off 0, on 1)" << std::endl;
  return -1;
}
#endif