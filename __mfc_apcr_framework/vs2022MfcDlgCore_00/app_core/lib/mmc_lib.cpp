/**
 * @file mmc_lib.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "mmc_lib.hpp"
#include "ap.hpp"
/*

MMCE의 모션명령 API Parameter는 기본적으로 아래와 같이 Pulse단위를 사용합니다.

Parameter    기본 단위
위치(거리)    [Pulse]
속도          [Pulse/Sec]
가/감속도     [Pulse/Sec2]
저크          [Pulse/Sec3]
# 보통 속도의 10배를 가감속, 가감속의 10배를 저크로 사용합니다.

Axis Parameter에서 
“Command Data Unit Scale Factor”와 
“Feedback Data Unit Scale Factor” 값을 변경하여 사용자가 원하는 단위로 설정 할 수 있습니다.
예를 들어 24비트 encoder 값과 리드 10mm 볼스크류가 사용된경우 
(pulse of one turn)16'777'216 = 1turn
(pulse of one turn)16'777'216/10 = 1turn/10mm = 1677721.6 = 1mm 발생 펄스
Unit Scale Factor와 Feedback Scale Factor에 같은 값인 1677721.6 셋팅하면 그 이후 
거리, 속도 관련 단위는 1mm, 1mm/s로 통일됨.

이동 거리 100mm를 속도 100mm/s로 이동시키고 가감속 1000mm/ss로 이동하고 저크는 10000mm/sss로 셋팅한다.


정지는 MC_Halt  사용
CommandedPosition  -> MMC 보드가 모터 드라이버에 지령하고 있는 위치 정보
ActualPosition -> 모터 드라이버가 실제 이동한 위치 정보

 InPositionWindowSize-> In-Position Check 오차 범위 설정

 HomingCreepVelocity -> 정밀하게 원점 센서 다운을 체크하기 위해 조정되는 속도

 동작 완료 확인은  MotionComplete 상태정보 참조


'24.08.24 TapingBox 설비 셋팅 과정 확인 내용
 IO coupler ID 1 (in 0-8, out 0-7)
            ID 2 (in 0-6, out 0-6)

  ID 1 
    출력 상태 확인
    MC_IO_READ(0, 1, eMMC::BUF_OUT, 0, 16, 버퍼 주소) // 16 byte 사이즈 버퍼
    입력 상태 확인
    MC_IO_READ(0, 1, eMMC::BUF_IN, 34, 18, 버퍼 주소) // 18 byte 사이즈 버퍼 

    데이터 write
    - array<udword_t, 4> out_reg_data {}; 8개 출력 슬롯으로 16바이트 데이터 구조체 필요
    MC_IO_WRITE(0, 1, 0, 16, &out_reg_data[0][0] )   


  ID 2 
    출력 상태 확인
    MC_IO_READ(0, 2, eMMC::BUF_OUT, 0, 14, 버퍼 주소) // 14 byte 사이즈 버퍼  
    입력 상태 확인
    MC_IO_READ(0, 2, eMMC::BUF_IN, 34, 14, 버퍼 주소) // 14 byte 사이즈 버퍼
  
    데이터 write
    - array<udword_t, 4> out_reg_data {}; 7개 출력 슬롯으로 14바이트 데이터 구조체 필요
                                          마지막 2바이트는 불용
    MC_IO_WRITE(0, 2, 0, 14, &out_reg_data[0][0] )     

////////////////////////////////////////////////////////////////////////////


  #  sub axis 모듈 클래스에서 아래와 같이 상태를 update한다.
   //Network Status Read
    checkNetwork();
   //개별 축 Status Read.
    MC_ReadAxisStatus(m_cfg.master_idx, slave_id, &uStatus);

   //Command Counter Read
    MC_ReadCommandedPosition(m_cfg.master_idx, slave_id, &dCmdPosition);

   //Actual Counter Read
    MC_ReadActualPosition(m_cfg.master_idx, slave_id, &dActPosition);

   //Command Velocity Read
    MC_ReadCommandedVelocity(m_cfg.master_idx, slave_id, &dCmdVelocity);
   
*/


#ifdef USE_HW_LIB_MMC_ETHERCAT


// ## Library 추가하기
#ifdef HW_LIB_MMC_ETHERCAT_X64

#pragma warning(disable : 4091)
#ifdef _AFX
#ifndef _USE_SIMULATION
#pragma comment(lib, "../app_core/lib/MMC/MotionSDK_64bit/NMC_Motion.lib")
#include "../app_core/lib/MMC/MotionSDK_64bit/NMC_Motion.h"
#endif
#else //_AFX

#ifndef _USE_SIMULATION
#pragma comment(lib, "../src/lib/MMC/MotionSDK_64bit/NMC_Motion.lib")
#include "../src/lib/MMC/MotionSDK_64bit/NMC_Motion.h"
#endif

#endif//_AFX
#else //HW_LIB_MMC_ETHERCAT_X64
#pragma comment(lib, "../src/lib/MMC/MotionSDK_32bit/NMC_Motion.lib")
#include "../src/lib/MMC/MotionSDK_32bit/NMC_Motion.h"

#endif //HW_LIB_MMC_ETHERCAT_X64


//#define eCatLib_LoCk(m)   std::lock_guard<std::mutex> safe_lock(m)
#define eCatLib_LoCk(m)
#define M_PI           3.14159265358979323846



namespace hw_lib
{

  mmc_eCatLib::mmc_eCatLib(const apcr::attribute_st &f) :
  apcr::IObj(f)
  {
  }

  mmc_eCatLib::~mmc_eCatLib()
  {
    threadStop();
    close();
  };

  bool mmc_eCatLib::write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...)
  {
    if (fmt)
    {
      va_list args;
      va_start(args, fmt);
      std::vector<char> buffer(1024);
      vsnprintf(buffer.data(), buffer.size(), fmt, args);
      writeLog(level, log.func_name.c_str(), log.file.c_str(), log.line_no, buffer.data());
      va_end(args);
      return true;
    }

    return false;
  }

  /****************************************************
     *  IO interface  overriding
     ****************************************************/
  bool mmc_eCatLib::IsOn(uint32_t addr)
  {
    constexpr auto io_reg_bit_cnt = 32;
    if (addr < (IO::IO_ADDR_BASE_OUTPUT))
    {
      // case in
      uint16_t io_idx   = (uint16_t)(addr - m_cfg.begin_addr_in);
      uint8_t reg_idx  = (uint8_t)(io_idx / io_reg_bit_cnt); //quotient
      uint8_t reg_addr = (uint8_t)(io_idx % io_reg_bit_cnt); //remainder
      return m_deviceReg.inReg[reg_idx](reg_addr);
    }
    // case out
    uint16_t io_idx   = (uint16_t)(addr - m_cfg.begin_addr_out);
    uint8_t reg_idx  = (uint8_t)(io_idx / io_reg_bit_cnt); //quotient
    uint8_t reg_addr = (uint8_t)(io_idx % io_reg_bit_cnt); //remainder
    return m_deviceReg.outReg[reg_idx](reg_addr);
  }

  bool mmc_eCatLib::IsOff(uint32_t addr)
  {
    return !IsOn(addr);
  }

  errno_t mmc_eCatLib::OutputOn(uint32_t addr)
  {
    constexpr auto io_reg_bit_cnt = 32;
    if (addr < (IO::IO_ADDR_BASE_OUTPUT))
      return -1;

    uint16_t io_idx   = (uint16_t)(addr - m_cfg.begin_addr_out);
    uint8_t reg_idx  = (uint8_t)(io_idx / io_reg_bit_cnt); //quotient
    uint8_t reg_addr = (uint8_t)(io_idx % io_reg_bit_cnt); //remainder
    m_deviceReg.outReg[reg_idx](reg_addr, true);

    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::OutputOff(uint32_t addr)
  {
    constexpr auto io_reg_bit_cnt = 32;
    if (addr < (IO::IO_ADDR_BASE_OUTPUT))
      return -1;

    uint16_t io_idx   = (uint16_t)(addr - m_cfg.begin_addr_out);
    uint8_t reg_idx  = (uint8_t)(io_idx / io_reg_bit_cnt); //quotient
    uint8_t reg_addr = (uint8_t)(io_idx % io_reg_bit_cnt); //remainder
    m_deviceReg.outReg[reg_idx](reg_addr, false);

    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::OutputToggle(uint32_t addr)
  {
    constexpr auto io_reg_bit_cnt = 32;
    if (addr < (IO::IO_ADDR_BASE_OUTPUT))
      return -1;

    uint16_t io_idx   = (uint16_t)(addr - m_cfg.begin_addr_out);
    uint8_t reg_idx  = (uint8_t)(io_idx / io_reg_bit_cnt); //quotient
    uint8_t reg_addr = (uint8_t)(io_idx % io_reg_bit_cnt); //remainder
    m_deviceReg.outReg[reg_idx](reg_addr, IsOff(addr));

    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::GetData(void)
  {
    return GetSetIO();
  }

  inline errno_t mmc_eCatLib::GetSetIO()
  {
#ifndef _USE_SIMULATION
    eCatLib_LoCk(m_mutex);

    eMMC::MC_STATUS ret{};
    constexpr auto  MAX_STR = MAX_ERR_LEN;
    std::string     err_msg(MAX_STR, '\0');
    std::string     msg(MAX_STR, '\0');

    // id idx_io_0
    {
      //write
      {
        UINT32 offset = mmc_eCatLib::def_data_offset_start_out;
        ret = eMMC::MC_IO_WRITE(
          m_cfg.master_idx,
          sub_idx::idx_io_0,
          offset,                                  //data offset
          (eMMC::UINT32)mmc_eCatLib::max_cnt_ch_out_id_1,
          (eMMC::UINT8 *)&m_deviceReg.outReg[0][0] //write data
        );

        if (ret != eMMC::MC_OK)                              // Error occur...
        {
          //Get Error Message
          eMMC::MC_GetErrorMessage(ret, MAX_STR, (char *)err_msg.c_str());
          sprintf_s((char *)msg.c_str(), MAX_STR, "Error : %08X, %s", ret, (const char *)err_msg.c_str());
          //Popup the message

          //Retrun;
        }
        else
        {
          //done
        }
      }


      //read input register
      {
        std::array<uint8_t, mmc_eCatLib::max_cnt_ch_in_id_1> buffer{};
        UINT32 offset = mmc_eCatLib::def_data_offset_start_in; //Data offset selected on the IO memory of the device

        ret = eMMC::MC_IO_READ(
          m_cfg.master_idx,
          sub_idx::idx_io_0,
          eMMC::BUF_IN,
          offset,                      //Data Offset
          (eMMC::UINT32)mmc_eCatLib::max_cnt_ch_in_id_1,
          (eMMC::UINT8 *)buffer.data() //Read Data
        );
        if (ret != eMMC::MC_OK)        // Error occur...
        {
          //Get Error Message
          eMMC::MC_GetErrorMessage(ret, MAX_STR, (char *)err_msg.c_str());
          sprintf_s((char *)msg.c_str(), MAX_STR, "Error : %08X, %s", ret, (const char *)err_msg.c_str());
          //Popup the message

          //Retrun;
        }
        else
        {
          //done
          uint8_t *ptr_data = &m_deviceReg.inReg[0][0];
          for (size_t i = 0; i < buffer.size(); ++i)
          {
            *ptr_data = buffer[i];
            ptr_data++;
          }
        }
      }
    }

    // id idx_io_1
    {
      //write
      {
        UINT32 offset = mmc_eCatLib::def_data_offset_start_out;
        ret = eMMC::MC_IO_WRITE(
          m_cfg.master_idx,
          sub_idx::idx_io_1,
          offset,                                  //data offset
          (eMMC::UINT32)mmc_eCatLib::max_cnt_ch_out_id_2,
          (eMMC::UINT8 *)&m_deviceReg.outReg[def_out_reg_size_id_1][0] //write data
        );

        if (ret != eMMC::MC_OK)                              // Error occur...
        {
          //Get Error Message
          eMMC::MC_GetErrorMessage(ret, MAX_STR, (char *)err_msg.c_str());
          sprintf_s((char *)msg.c_str(), MAX_STR, "Error : %08X, %s", ret, (const char *)err_msg.c_str());
          //Popup the message

          //Retrun;
        }
        else
        {
          //done
        }
      }


      //read input register
      {
        std::array<uint8_t, mmc_eCatLib::max_cnt_ch_in_id_2> buffer{};
        UINT32 offset = mmc_eCatLib::def_data_offset_start_in; //Data offset selected on the IO memory of the device

        ret = eMMC::MC_IO_READ(
          m_cfg.master_idx,
          sub_idx::idx_io_1,
          eMMC::BUF_IN,
          offset,                      //Data Offset
          (eMMC::UINT32)mmc_eCatLib::max_cnt_ch_in_id_2,
          (eMMC::UINT8 *)buffer.data() //Read Data
        );
        if (ret != eMMC::MC_OK)        // Error occur...
        {
          //Get Error Message
          eMMC::MC_GetErrorMessage(ret, MAX_STR, (char *)err_msg.c_str());
          sprintf_s((char *)msg.c_str(), MAX_STR, "Error : %08X, %s", ret, (const char *)err_msg.c_str());
          //Popup the message

          //Retrun;
        }
        else
        {
          //done
          uint8_t *ptr_data = &m_deviceReg.inReg[def_in_reg_size_id_1][0];
          for (size_t i = 0; i < buffer.size(); ++i)
          {
            *ptr_data = buffer[i];
            ptr_data++;
          }
        }
      }
    }


    return 0;
#else
    return 0;
#endif
  }

  // MARK:Initialize
  /****************************************************
     *	func
     ****************************************************/
  errno_t mmc_eCatLib::Init(const cfg_t &cfg)
  {
    m_cfg    = cfg;
    
    m_infoDriver._id = m_cfg.master_idx;
    m_isInit = true;

#ifndef _USE_SIMULATION
    apcr::log_info_t data{__FILE__, __FUNCTION__, __LINE__};

    if (m_infoDriver._is_masterRun)
    {
      // restart sequence
      // if (connect() == false)
      // {
      //   write_log(TinyC::Log::level::lvl_err, data, "init connect failed  \n");
      //   return -1;
      // }
    }
    else if (start_init_process() != ERROR_SUCCESS)
    {
      write_log(TinyC::Log::level::lvl_err, data, "init connect failed  \n");
      
      ERR_PRINT("init connect failed");
      return -1;
    }
    else
      write_log(TinyC::Log::level::lvl_info, data, "init MC_POWER ON connect success  \n");

      REPORT_OUT("[OK] init MC_POWER ON connect success!");
    return ERROR_SUCCESS;
#else
    m_IsConnected = true;
    return 0;
#endif
  }

  //read init output regist
  errno_t mmc_eCatLib::get_current_io_reg()
  {
#ifndef _USE_SIMULATION
    using buff_t = std::array<udword_t, def_out_reg_size>;
    eMMC::MC_STATUS result{};
    constexpr auto  MAX_STR = MAX_ERR_LEN;
    std::string     err_msg(MAX_STR, '\0');
    std::string     msg(MAX_STR, '\0');
    buff_t          buffer{};

    result = eMMC::MC_IO_READ(
      m_cfg.master_idx,
      sub_idx::idx_io_0,
      eMMC::BUF_OUT,
      def_data_offset_start_out,   //Data Offset
      (eMMC::UINT32)(mmc_eCatLib::max_cnt_ch_out_id_1),
      (eMMC::UINT8 *)&buffer[0][0] //Read Data
    );
    if (result != eMMC::MC_OK)     // Error occur...
    {
      eMMC::MC_GetErrorMessage(result, MAX_STR, (char *)err_msg.c_str());
      sprintf_s((char *)msg.c_str(), MAX_STR, "Error : %08X, %s", result, (const char *)err_msg.c_str());
      //Popup the message

      ERR_PRINT("MC_ReadActualPosition(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    else
    {
      result = eMMC::MC_IO_READ(
        m_cfg.master_idx,
        sub_idx::idx_io_1,
        eMMC::BUF_OUT,
        def_data_offset_start_out,                       //Data Offset
        (eMMC::UINT32)(mmc_eCatLib::max_cnt_ch_out_id_2),
        (eMMC::UINT8 *)&buffer[def_out_reg_size_id_1][0] //Read Data
      );
      if (result != eMMC::MC_OK)                         // Error occur...
      {
        eMMC::MC_GetErrorMessage(result, MAX_STR, (char *)err_msg.c_str());
        sprintf_s((char *)msg.c_str(), MAX_STR, "Error : %08X, %s", result, (const char *)err_msg.c_str());
        //Popup the message

        ERR_PRINT("MC_ReadActualPosition(%s) fail", statusDescription((int)result).c_str());
        return -1;
      }
      else
      {
        m_deviceReg.outReg = buffer;
      }
    }
#endif
    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::get_master_info(void)
  {
#ifndef _USE_SIMULATION
    uint8_t slave_state = 0;
    using namespace eMMC;
    switch (m_infoDriver._curr_modes)
    {
      //이미 Master Run 상태이고 해당 Master 에 연결된 전체 디바이스가 OP 일 경우 별도 Run 동작 불필요.
    case eMM_RUN:
      for (size_t j = 0; j < m_infoDriver._has_slave_ids.size(); j++)
      {
        //-----------------------------------------------------------------------------
        // Summary : Slave Device 의 현재 EtherCAT State 정보를 가져 옴
        // Parameter(I) : BoardID -> MMCE 보드 Switch 번호 (0~9)
        // Parameter(I) : EcatAddr -> Slave Device의 EtherCAT Address 지정
        // Parameter(O) : data -> Slave Device 의 EtherCAT Slave State 정보
        //-----------------------------------------------------------------------------
        // * EtherCAT Slave State 정보
        // 0x00 : eST_UNKNOWN   -> 인식 불가 상태
        // 0x01 : eST_INIT	    -> 초기화 상태
        // 0x02 : eST_PREOP     -> 동작 전 상태
        // 0x03 : eST_BOOTSTRAP -> Boot 상태
        // 0x04 : eST_SAFEOP	-> 안전 동작 상태
        // 0x08 : eST_OP		-> 정상 동작 상태
        // 0x10 : eST_ACKERR    -> 에러 상태
        //-----------------------------------------------------------------------------
        SlaveGetCurState(m_infoDriver._id, m_infoDriver._has_slave_ids[j], &slave_state);
        if (slave_state != eST_OP)
          m_infoDriver._OP_flag = FALSE;
      }

      if (m_infoDriver._OP_flag == FALSE)
        m_infoDriver._step = info_state_t::MasterStop;
      else
        m_infoDriver._step = info_state_t::RunCplt;
      break;

      //Master Run 상태가 아닐경우 Master Stop 단계부터 수행.
    case eMM_IDLE:
      __fallthrough;
    case eMM_LINKBROKEN:
      __fallthrough;
    case eMM_ERR:
      __fallthrough;
    default:
      m_infoDriver._step = info_state_t::MasterStop;
      break;
    }
    //switch (m_ethcatDevicesInfo._curr_modes)
#endif

    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::search_devices(void)
  {
#ifndef _USE_SIMULATION
    using namespace eMMC;
    std::array<uint16_t, MAX_BOARD_CNT> master_ids{};
    uint16_t                            master_cnt{};
    using device_ids_t = std::vector<uint16_t>;
    //std::vector<device_ids_t> masters_device_ids{};

    //-----------------------------------------------------------------------------
    // Summary : 현재 설치된 모든 보드의 ID List 및 설치보드 개수 가져 옴
    // Parameter(O) : MasterMap[MAX_BOARD_CNT] -> 설치된 보드 ID List (0~9)
    // Parameter(O) : MasterCount -> 설치된 보드 개수
    //-----------------------------------------------------------------------------
    eMMC::MC_STATUS result = MC_GetMasterMap(master_ids.data(), &master_cnt);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_GetMasterMap(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    else
    {
      m_ethcatDevicesInfo._ids.resize(master_cnt);
      for (int i = 0; i < master_cnt; i++)
        m_ethcatDevicesInfo._ids[i] = master_ids[i];

      m_ethcatDevicesInfo._has_slave_ids.resize(master_cnt);
      m_ethcatDevicesInfo._step.resize(master_cnt);
      m_ethcatDevicesInfo._curr_modes.resize(master_cnt);

      std::stringstream ss{"Master ID : "};
      for (int i = 0; i < master_cnt; i++)
      {
        ss << m_ethcatDevicesInfo._ids[i] << " ";
      }
      LOG_OUT("Master Count : %d ", master_cnt);
      LOG_OUT("%s", ss.str().c_str());
    }
    //masters_device_ids.resize(master_cnt);

    // 각 Master Board 별 slave Device 정보 Get
    device_ids_t device_ids{};
    for (int i = 0; i < master_cnt; i++)
    {
      uint16_t total_cnt{};
      //-----------------------------------------------------------------------------
      // Summary : MMCE 보드에 구성 되어 있는 Deivce 의 개수를 가져 옴
      // Parameter(I) : BoardID -> MMCE 보드 Switch 번호 (0~9)
      // Parameter(O) : TotalDeviceCount -> Device Count 합계
      //-----------------------------------------------------------------------------
      MasterGetDeviceCount(m_ethcatDevicesInfo._ids[i], &total_cnt);
      //device_ids.emplace_back(total_cnt);
      device_ids.resize(total_cnt);

      //-----------------------------------------------------------------------------
      // Summary : MMCE 보드에 구성 되어 있는 Deivce ID List 를 가져 옴
      // Parameter(I) : BoardID -> MMCE 보드 Switch 번호 (0~9)
      // Parameter(O) : DeviceIDArray -> Device ID Array (EcatAddr)
      //-----------------------------------------------------------------------------
      MasterGetDeviceID(m_ethcatDevicesInfo._ids[i], device_ids.data());
      m_ethcatDevicesInfo._has_slave_ids[i].resize(total_cnt);
      m_ethcatDevicesInfo._has_slave_ids[i] = device_ids;
    }
    //for (int i = 0; i < master_cnt; i++)


    for (int i = 0; i < master_cnt; i++)
    {
      //-----------------------------------------------------------------------------
      // Summary : 지정한 MMCE 보드의 Master State (EtherCAT Network State)를 가져 옴
      // Parameter(I) : BoardID -> MMCE 보드 Switch 번호 (0~9)
      // Parameter(O) : MasterMode -> Master State 를 리턴
      //-----------------------------------------------------------------------------
      // * Master State
      // 0 : IDLE -> 정지 상태
      // 1 : SCAN -> 스캔 중
      // 2 : RUN -> 정상 동작 상태
      // 3 : INTRANSITION -> 상태 변경 중
      // 4 : ERROR -> 에러 상태
      // 5 : LINKBROKEN -> 통신이 끊긴 상태
      //-----------------------------------------------------------------------------
      MasterGetCurMode(m_ethcatDevicesInfo._ids[i], &m_ethcatDevicesInfo._curr_modes[i]);
      uint8_t slave_state = 0;


      int end_count = 0;
      switch (m_ethcatDevicesInfo._curr_modes[i])
      {
        //이미 Master Run 상태이고 해당 Master 에 연결된 전체 디바이스가 OP 일 경우 별도 Run 동작 불필요.
      case eMM_RUN:
        for (size_t j = 0; j < m_ethcatDevicesInfo._has_slave_ids[i].size(); j++)
        {
          //-----------------------------------------------------------------------------
          // Summary : Slave Device 의 현재 EtherCAT State 정보를 가져 옴
          // Parameter(I) : BoardID -> MMCE 보드 Switch 번호 (0~9)
          // Parameter(I) : EcatAddr -> Slave Device의 EtherCAT Address 지정
          // Parameter(O) : data -> Slave Device 의 EtherCAT Slave State 정보
          //-----------------------------------------------------------------------------
          // * EtherCAT Slave State 정보
          // 0x00 : eST_UNKNOWN   -> 인식 불가 상태
          // 0x01 : eST_INIT	    -> 초기화 상태
          // 0x02 : eST_PREOP     -> 동작 전 상태
          // 0x03 : eST_BOOTSTRAP -> Boot 상태
          // 0x04 : eST_SAFEOP	-> 안전 동작 상태
          // 0x08 : eST_OP		-> 정상 동작 상태
          // 0x10 : eST_ACKERR    -> 에러 상태
          //-----------------------------------------------------------------------------
          SlaveGetCurState(m_ethcatDevicesInfo._ids[i], m_ethcatDevicesInfo._has_slave_ids[i][j], &slave_state);
          if (slave_state != eST_OP)
            m_ethcatDevicesInfo._OP_flag[i] = FALSE;
        }

        if (m_ethcatDevicesInfo._OP_flag[i] == FALSE)
          m_ethcatDevicesInfo._step[i] = info_state_t::MasterStop;
        else
          m_ethcatDevicesInfo._step[i] = info_state_t::RunCplt;
        break;

        //Master Run 상태가 아닐경우 Master Stop 단계부터 수행.
      case eMM_IDLE:
        __fallthrough;
      case eMM_LINKBROKEN:
        __fallthrough;
      case eMM_ERR:
        __fallthrough;
      default:
        m_ethcatDevicesInfo._step[i] = info_state_t::MasterStop;
        break;
      }
      //switch (m_ethcatDevicesInfo._curr_modes[i])
    }
    //for (int i = 0; i < master_cnt; i++)


    //print out : report
#endif
    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::start_init_process()
  {    
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Init();
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Init(%s) fail", statusDescription((int)result).c_str());
      return -1; //generateErrorCode(xxxxx);
    }

    m_stopThread = false;
    m_trd = std::thread(&mmc_eCatLib::thread_masterInit, this);
#endif

    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::end_init_procecss()
  {
    /*MMC BD 정보를 가져온다*/   
    return  get_current_io_reg();
  }


  void mmc_eCatLib::threadStop()
  {
    m_stopThread = true;
    if (m_trd.joinable())
      m_trd.join();
  }

  void mmc_eCatLib::thread_masterInit(void)
  {
    REPORT_OUT("[prc] init procoss thread start!");
    while (!m_stopThread)
    {
      thread_masterInitJob();
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    end_init_procecss();
    REPORT_OUT("[prc] init procoss thread terminatet!");
  }

  void mmc_eCatLib::thread_masterInitJob(void)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result{};
    using namespace apcr;

    if (m_infoDriver._cplt_flag == FALSE)
    {
      switch (m_infoDriver._step)
      {
      case info_state_t::MasterStop: //Master Stop
        result = eMMC::MC_MasterSTOP(m_infoDriver._id);
        m_timers.Flush();
        if (result != eMMC::MC_OK)
        {
          ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
          m_infoDriver._cplt_flag = true;
          m_stopThread = true;
          break;
        }
        m_infoDriver._step        = info_state_t::MasterStop_Ready;
        m_infoDriver._timeout_cnt = 0;
        break;

      case info_state_t::MasterStop_Ready: //Master Stop 완료 대기
        if (m_timers.LessThan(1'000, tm_idx_0))
          break;
        eMMC::MasterGetCurMode(m_infoDriver._id, &m_infoDriver._curr_modes);
        if (m_infoDriver._curr_modes == eMMC::eMM_IDLE)
        {
          //std::this_thread::sleep_for(std::chrono::milliseconds(100));
          m_infoDriver._step        = info_state_t::MasterRun;
          m_infoDriver._timeout_cnt = 0;
        }
        else
        {
          m_infoDriver._timeout_cnt++;
          if (m_infoDriver._timeout_cnt > 300) //Time Out
          {
            ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
            m_infoDriver._cplt_flag = true;
            m_stopThread = true;
          }
        }

        break;

      case info_state_t::MasterRun: //Master Run
        if(m_timers.LessThan(1'000, tm_idx_0))
          break;
        m_timers.Reset(tm_idx_0);
        result = eMMC::MC_MasterRUN(m_infoDriver._id);
        if (result != eMMC::MC_OK)
        {
          ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
          m_infoDriver._cplt_flag = true;
          m_stopThread = true;
          break;
        }
        m_infoDriver._step        = info_state_t::AllDeviceChangeOP_Ready;
        m_infoDriver._timeout_cnt = 0;


        break;

      case info_state_t::AllDeviceChangeOP_Ready: //전체 디바이스 OP 변경 대기
        if (m_timers.LessThan(1'000, tm_idx_0))
          break;
        m_timers.Reset(tm_idx_0);
        m_infoDriver._OP_flag = true;
        for (int j = 0; j < m_infoDriver._has_slave_ids.size(); j++)
        {
          uint8_t slave_state = 0;
          //개별 디바이스의 통신 상태 반환.
          eMMC::SlaveGetCurState(m_infoDriver._id, m_infoDriver._has_slave_ids[j], &slave_state);
          if (slave_state != eMMC::eST_OP)
            m_infoDriver._OP_flag = false;
        }

        if (m_infoDriver._OP_flag == false)
        {
          m_infoDriver._timeout_cnt++;
          if (m_infoDriver._timeout_cnt > 2500) //Time Out
          {
            ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
            m_infoDriver._cplt_flag = true;
            m_stopThread = true;
          }
        }
        else
          m_infoDriver._step = info_state_t::RunCplt;

        break;

      case info_state_t::RunCplt: //완료
       // std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_infoDriver._cplt_flag    = true;
        m_stopThread = true;
        m_infoDriver._is_masterRun = true;

        LOG_PRINT("BoardID[%02d], Master Run Complete", m_infoDriver._id);
        std::cout << std::endl << get_info() << std::endl;
        break;
      }
    }
#endif
  }

  void mmc_eCatLib::threadRun(void)
  {
    LOG_OUT("Thread Start!");
    while (!m_stopThread)
    {
      threadJob();
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    LOG_OUT("Thread Terminatet!");
  }


  void mmc_eCatLib::threadJob(void)
  {
    // master start
    if (m_infoDriver._is_masterRun == false)
      threadJob_masterRun();

    // IO state
    GetSetIO();
  }

  
  void mmc_eCatLib::threadJob_masterRun(void)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result{};

    using namespace apcr;
    if (m_infoDriver._cplt_flag == FALSE)
    {
      switch (m_infoDriver._step)
      {
      case info_state_t::MasterStop: //Master Stop

        result = eMMC::MC_MasterSTOP(m_infoDriver._id);
        if (result != eMMC::MC_OK)
        {
          ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
          m_infoDriver._cplt_flag = true;
          break;
        }
        m_infoDriver._step        = info_state_t::MasterStop_Ready;
        m_infoDriver._timeout_cnt = 0;
        break;

      case info_state_t::MasterStop_Ready: //Master Stop 완료 대기
        eMMC::MasterGetCurMode(m_infoDriver._id, &m_infoDriver._curr_modes);
        if (m_infoDriver._curr_modes == eMMC::eMM_IDLE)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          m_infoDriver._step        = info_state_t::MasterRun;
          m_infoDriver._timeout_cnt = 0;
        }
        else
        {
          m_infoDriver._timeout_cnt++;
          if (m_infoDriver._timeout_cnt > 300) //Time Out
          {
            ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
            m_infoDriver._cplt_flag = true;
          }
        }

        break;

      case info_state_t::MasterRun: //Master Run

        result = eMMC::MC_MasterRUN(m_infoDriver._id);
        if (result != eMMC::MC_OK)
        {
          ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
          m_infoDriver._cplt_flag = true;
          break;
        }
        m_infoDriver._step        = info_state_t::AllDeviceChangeOP_Ready;
        m_infoDriver._timeout_cnt = 0;


        break;

      case info_state_t::AllDeviceChangeOP_Ready: //전체 디바이스 OP 변경 대기
        m_infoDriver._OP_flag = true;
        for (int j = 0; j < m_infoDriver._has_slave_ids.size(); j++)
        {
          uint8_t slave_state = 0;
          //개별 디바이스의 통신 상태 반환.
          eMMC::SlaveGetCurState(m_infoDriver._id, m_infoDriver._has_slave_ids[j], &slave_state);
          if (slave_state != eMMC::eST_OP)
            m_infoDriver._OP_flag = false;
        }

        if (m_infoDriver._OP_flag == false)
        {
          m_infoDriver._timeout_cnt++;
          if (m_infoDriver._timeout_cnt > 2500) //Time Out
          {
            ERR_PRINT("BoardID[%02d], Master Run Fail, Step[%d]", m_infoDriver._id, m_infoDriver._step);
            m_infoDriver._cplt_flag = true;
          }
        }
        else
          m_infoDriver._step = info_state_t::RunCplt;

        break;

      case info_state_t::RunCplt: //완료
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        LOG_PRINT("BoardID[%02d], Master Run Complete", m_infoDriver._id);
        m_infoDriver._cplt_flag    = true;
        m_infoDriver._is_masterRun = true;
        break;
      }
    }
#endif
    return;
  }

  //전체 Device 대한 통신 상태 Read
  bool mmc_eCatLib::checkNetwork(void)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result{};
    uint16_t        device_cnt{};
    uint16_t        working_cnt{};
    uint8_t         device_state{};
    result = eMMC::SlaveGetCurStateAll(
      m_infoDriver._id,
      &device_cnt,
      &working_cnt,
      &device_state);
    if (result != eMMC::MC_OK)
    {
      //Master Run 상태가 아닐 경우 Return Error 반환 됨.
      ERR_PRINT("BoardID[%02d], Master Run Fail", m_infoDriver._id);
      return false;
    }

    //Working Count 가 맞지 않거나, OP 가 아닌 Device 가 존재 한다면
    if (device_cnt != working_cnt || device_state != eMMC::STATUS_COMBINATION_8)
    {
      ERR_PRINT("BoardID[%02d], Master Run Fail", m_infoDriver._id);
      return false;
    }
#endif
    return true;
  }

  errno_t mmc_eCatLib::ReadStatus(sub_idx axis_idx)
  {
    return errno_t();
  }

//개별 Device 통신 체크.
  errno_t mmc_eCatLib::checkSubNetwork(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result{};
    uint16_t        device_cnt{};
    uint16_t        working_cnt{};
    uint8_t         device_state{};
    result = eMMC::SlaveGetCurState(m_infoDriver._id, axis_idx, &device_state);
    if (result != eMMC::MC_OK)
    {
      //Master Run 상태가 아닐 경우 Return Error 반환 됨.
      ERR_PRINT("Device[%02d],  Net Error Fail : State : %d", axis_idx, device_state);
      return false;
    }
#endif
    return true;
  }


  errno_t mmc_eCatLib::getInputReg()
  {
    return ERROR_SUCCESS;
  }

  errno_t mmc_eCatLib::getOutReg()
  {
    return ERROR_SUCCESS;
  }

  void mmc_eCatLib::getOutput()
  {
    return;
  }

  errno_t mmc_eCatLib::setOutputReg(uint32_t value)
  {
    return 0;
  }

  void mmc_eCatLib::close()
  {
  }

  bool mmc_eCatLib::reconnect()
  {
    /* closeNet();
      tim::delay(100);*/
    return false;
  }

  bool mmc_eCatLib::connect()
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Init();
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Init(%s) fail", statusDescription((int)result).c_str());
      return false; //generateErrorCode(xxxxx);
    }

    result = eMMC::MC_MasterSTOP(m_cfg.master_idx);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("eMMC::MC_MasterSTOP error [%s]", statusDescription((int)result).c_str());
      return false;
    }
    else
    {
      LOG_PRINT("eMMC::MC_MasterSTOP success");
    }

    tim::delay(1'000 * 2); //TODO: remove this delay
    result = eMMC::MC_MasterRUN(m_cfg.master_idx);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("eMMC::MC_MasterRUN error[%s]", statusDescription((int)result).c_str());
      return false;
    }
    else
    {
      LOG_PRINT("eMMC::MC_MasterRUN success");
    }


    return true;
#else
    return false;
#endif
  }

  bool mmc_eCatLib::IsConnected()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_infoDriver._is_masterRun;
  }

  bool mmc_eCatLib::Disconnect()
  {
    close();
    return true;
  }

  void mmc_eCatLib::GetOutput()
  {
    getOutput();
  }

  void mmc_eCatLib::GetInput()
  {
    getInputReg();
  }

  uint32_t mmc_eCatLib::GetBeginAddrIN() const
  {
    return m_cfg.begin_addr_in;
  }

  uint32_t mmc_eCatLib::GetBeginAddrOUT() const
  {
    return m_cfg.begin_addr_out;
  }

  hw_io::io_type_e mmc_eCatLib::GetIOType() const
  {
    return m_cfg.io_type;
  }

  uint32_t mmc_eCatLib::get_in_tail_addr()
  {
    return 0;
  }

  uint32_t mmc_eCatLib::get_out_tail_addr()
  {
    return 0;
  }

  std::string mmc_eCatLib::get_info(void)
  {
    std::stringstream ss{};
    ss << "\t ##################### mmc EtheCAT info  ######################" << "\n";
    ss << "\t --------------------------------------------------------------" << "\n"; 
    ss << "\t    coupler id-1 : input module count  - " << std::to_string(max_io_in_ch_id_1) << "\n";
    ss << "\t    coupler id-2 : input module count  - " << std::to_string(max_io_in_ch_id_2) << "\n";
    ss << "\t    coupler id-1 : output module count - " << std::to_string(max_io_out_ch_id_1) << "\n";
    ss << "\t    coupler id-2 : ounput module count - " << std::to_string(max_io_out_ch_id_2) << "\n";   
    ss << "\t --------------------------------------------------------------" << "\n"; 
    ss << "\t      Total  input(X) "<< std::to_string(max_cnt_io_in) <<"(Bit),";
    ss << "\t    output(Y) " << std::to_string(max_cnt_io_out) << "(Bit) \n";
    ss << "\t --------------------------------------------------------------" << "\n"; 
    ss << "\t    Driver Initialize ["<< (m_infoDriver._is_masterRun?"OK":"NG")<<"]\n";

    return ss.str();
  }

  //MARK::io
  /****************************************************
     *	func
     ****************************************************/

  // MARK:master Amp
  /****************************************************
     *	func
     ****************************************************/

  errno_t mmc_eCatLib::InitAmp(sub_idx dev_idx)
  {
#ifndef _USE_SIMULATION
    //Reads the current state of the slave corresponding to the specified EtherCAT address.
    uint32_t    pre_ms = tim::millis();
    eMMC::UINT8 slave_state{};
    while (1)
    {
      eMMC::SlaveGetCurState(m_cfg.master_idx, (eMMC::UINT16)dev_idx, &slave_state);
      if (slave_state == eMMC::eST_OP)
        break;

      if (tim::millis() - pre_ms < 1000)
      {
        ERR_PRINT("eMMC::SlaveGetCurState() timeout");
        return -1;
      }
    }

    tim::delay(100); //TODO: remove delay

    return ERROR_SUCCESS;
#else
    return 0;
#endif

  }

  bool mmc_eCatLib::isAmpFault(sub_idx dev_idx)
  {
#ifndef _USE_SIMULATION
    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, dev_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return false;
    }
    return (bool)(ret_status & eMMC::mcDriveFault);
#else
    return false;
#endif
  }

  errno_t mmc_eCatLib::ResetAmpFault(sub_idx dev_idx)
  {
#ifndef _USE_SIMULATION
    if (eMMC::MC_Reset(m_cfg.master_idx, dev_idx) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Reset() fail");
      return -1;
    }

    tim::delay(300); //TODO: remove delay

    if (eMMC::MC_Power(m_cfg.master_idx, dev_idx, false) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Power() fail");
      return -1;
    }

    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  bool mmc_eCatLib::isMasterRunMode()
  {
#ifndef _USE_SIMULATION
    eMMC::UINT8 ret_mode{};
    if (eMMC::MasterGetCurMode(m_cfg.master_idx, &ret_mode) != eMMC::MC_OK)
    {
      ERR_PRINT("MasterGetCurMode() fail");
      return false;
    }
    return (bool)(ret_mode & eMMC::eMM_RUN);
#else
    return false;
#endif
  }

  // MARK:axis status
  // inline errno_t ReadStatus()
  // {
  //   return 0;
  // }

  // MARK:axis func
  /****************************************************
     *	func
     ****************************************************/
  errno_t mmc_eCatLib::AmpOnOff(sub_idx axis_idx, bool enable)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Power(
      m_cfg.master_idx, // Board ID
      axis_idx,         // Axis number
      enable            // enable(true) - false : Amp Off  true : Amp On

    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Power(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::AmpReset(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Reset(
      m_cfg.master_idx, // Board ID
      axis_idx          // Axis number
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Reset(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::EStop(double dec, sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }

    if (checkAxisExistArgument(axis_idx) != ERROR_SUCCESS)
    {
      // error axis status
      return -1;
    }

    constexpr double jerk = 0.0;
    if (eMMC::MC_Stop(m_cfg.master_idx, axis_idx, true, dec, jerk) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Stop() fail");
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif 
  }

  errno_t mmc_eCatLib::ResetEStop(double dec, sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }

    if (checkAxisExistArgument(axis_idx) != ERROR_SUCCESS)
    {
      // error axis status
      return -1;
    }

    constexpr double jerk = 0.0;
    if (eMMC::MC_Stop(m_cfg.master_idx, axis_idx, false, dec, jerk) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Stop() fail");
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::ServoOn(sub_idx axis_idx)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }

    if (eMMC::MC_Power(m_cfg.master_idx, axis_idx, true) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Power() fail");
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::ServoOff(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }
    if (eMMC::MC_Power(m_cfg.master_idx, axis_idx, false) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Power() fail");
      return -1;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return -1;
    }

    if (ret_status & eMMC::mcPowerOn)
    {
      ERR_PRINT("Not Power Off status");
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::zeroset_absEncoder(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }

    if (def_is_abs_encoder == false)
    {
      return -1;
    }

    if (ServoOff(axis_idx) != ERROR_SUCCESS)
    {
      ERR_PRINT("ServoOff() fail");
      return -1;
    }

    tim::delay(1'000 * 1); //TODO: remove delay

    eMMC::UINT8 ret_mode = 0;
    // change home mode//
    {
      //Mode(6:hm,8:csp)
      constexpr uint16_t mode_home_cmd = 0x0006;
      if (eMMC::MC_ModeChange(m_cfg.master_idx, axis_idx, mode_home_cmd, 0) != eMMC::MC_OK)
      {
        ERR_PRINT("MC_ModeChange() fail");
        return -1;
      }
      //wait change homing mode
      do
      {
        tim::delay(100);
        eMMC::MC_ReadSlaveModeStatus(m_cfg.master_idx, axis_idx, &ret_mode);
      } while (ret_mode != mode_home_cmd);
    }

    tim::delay(100);
    constexpr eMMC::UINT16     sdo_idx = 0x6098;
    constexpr eMMC::UINT8      sub_sdo_idx = 0;
    std::array<eMMC::UINT8, 1> data_method_homing{ 19 }; // current position zero set
    eMMC::UINT32               ret_datasize{};
    // set Homing Method to current position zero set
    {
      if (eMMC::MasterSetSDODataEcatAddr(m_cfg.master_idx, axis_idx, sdo_idx, sub_sdo_idx, (eMMC::UINT32)data_method_homing.size(), &ret_datasize, &data_method_homing[0]) != eMMC::MC_OK)
      {
        ERR_PRINT("eMMC::MasterSetSDODataEcatAddr() error");
        return -1;
      }


      tim::delay(300);
      if (ServoOn(axis_idx) != ERROR_SUCCESS)
      {
        ERR_PRINT("ServoOn() error");
        return -1;
      }

      ResetAmpFault(axis_idx);

      tim::delay(1000);

      //0:Stop,1:Start
      constexpr eMMC::UINT8 cmd_start = 1;
      if (eMMC::MC_SlaveHome(m_cfg.master_idx, axis_idx, cmd_start, 0) != eMMC::MC_OK)
      {
        ERR_PRINT("MC_SlaveHome() error");
      }
    }

    tim::delay(1000);

    // check Homing Done
    {
      constexpr eMMC::UINT16     sdo_idx = 0x6041;
      constexpr eMMC::UINT8      sub_sdo_idx = 0;
      std::array<eMMC::UINT8, 2> sdo_data{};
      //Homing Done 확인
      while (1)
      {
        if (eMMC::MasterGetSDODataEcatAddr(m_cfg.master_idx, axis_idx, sdo_idx, sub_sdo_idx, (eMMC::UINT32)sdo_data.size(), &ret_datasize, sdo_data.data()) != eMMC::MC_OK)
        {
          ERR_PRINT("eMMC::MasterGetSDODataEcatAddr() Error");
          return -1;
        }

        if (*(UINT16*)sdo_data.data() == 0b1000000000000) // check Bit 12
        {
          break;
        }


        tim::delay(10);
      }
    }
    //change mode CSP
    {
      //Mode(6:hm,8:csp)
      constexpr uint16_t mode_csp_cmd = 0x0008;
      if (eMMC::MC_ModeChange(m_cfg.master_idx, axis_idx, mode_csp_cmd, 0) != eMMC::MC_OK)
      {
        ERR_PRINT("MC_ModeChange() fail");
        return -1;
      }

      do
      {
        tim::delay(100);
        eMMC::MC_ReadSlaveModeStatus(m_cfg.master_idx, axis_idx, &ret_mode);
      } while (ret_mode != mode_csp_cmd);

      tim::delay(1000);
    }

    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  bool mmc_eCatLib::isHomeSensorDectected(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return false;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return false;
    }
    return (bool)(ret_status & eMMC::mcHomeAbsSwitch);
#else
    return false;
#endif
  }

  bool mmc_eCatLib::isPosiLimitSensorDectected(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return false;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return false;
    }
    return (bool)(ret_status & eMMC::mcLimitSwitchPos);
#else
    return false;
#endif
  }

  bool mmc_eCatLib::isNegaLimitSensorDectected(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return false;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return false;
    }
    return (bool)(ret_status & eMMC::mcLimitSwitchNeg);
#else
    return false;
#endif
  }

  bool mmc_eCatLib::isErrorStop(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return false;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return false;
    }
    return (bool)(ret_status & eMMC::mcErrorStop);
#else
    return false;
#endif
  }

  // inline int get_orgPriority(sub_idx axis_idx)
  // {
  //   if (axis_idx < sub_idx::idx_axis_0)
  //   {
  //     ERR_PRINT("invalid axis_idx");
  //     return -1;
  //   }

  //   return m_cfg.org_priority[axis_idx];
  // }

  bool mmc_eCatLib::isServoOn(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return false;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return false;
    }
    return (bool)(ret_status & eMMC::mcStandStill);
#else
    return false;
#endif
  }

  errno_t mmc_eCatLib::checkAxisExistArgument(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }

    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return -1;
    }

    if (ret_status & eMMC::mcStandStill)
    {
      return ERROR_SUCCESS;
    }
    ERR_PRINT("Not Stand Still status : status [%d]", ret_status);
    return -1;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::checkAxisState(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    if (axis_idx < sub_idx::idx_axis_0)
    {
      ERR_PRINT("invalid axis_idx");
      return -1;
    }

    if (isServoOn(axis_idx))
      return ERROR_SUCCESS;

    // error case
    uint32_t ret_status{};
    if (eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status) != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus() fail");
      return -1;
    }

    if (ret_status & eMMC::mcPowerOn)
    {
      ERR_PRINT("Axis[%d] Power off state", axis_idx);
    }
    else if (ret_status & eMMC::mcDisabled)
    {
      ERR_PRINT("Axis[%d] Disabled state", axis_idx);
    }
    else if (ret_status & eMMC::mcStopping)
    {
      ERR_PRINT("Axis[%d] Stopping state", axis_idx);
    }
    else if (ret_status & eMMC::mcDriveFault)
    {
      ERR_PRINT("Axis[%d] Drive Fault state", axis_idx);
    }
    else if (ret_status & eMMC::mcAxisWarning)
    {
      ERR_PRINT("Axis[%d] Axis Warning state", axis_idx);
    }
    else if (ret_status & eMMC::mcErrorStop)
    {
      ERR_PRINT("Axis[%d] Error Stop state", axis_idx);
    }
    else
    {
      ERR_PRINT("Axis[%d] is not Servo On", axis_idx);
    }

    return -1;
#else
    return 0;
#endif
  }

  std::string mmc_eCatLib::statusDescription(int err_code)
  {
#ifndef _USE_SIMULATION
    std::vector<char> buffer(512);
    eMMC::MC_STATUS   result = eMMC::MC_GetErrorMessage((uint32_t)err_code, (uint32_t)buffer.size(), &buffer[0]);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_GetErrorMessage fail");
      return "Unknown MMCe Error";
    }

    return std::string{ &buffer[0] };
#else
    return std::string{};
#endif
  }

  double mmc_eCatLib::getAxisActPosition(sub_idx axis_idx, double &ret_pos)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_ReadActualPosition(m_cfg.master_idx, axis_idx, &ret_pos);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadActualPosition(%s) fail", statusDescription((int)result).c_str());
      return 0.0;
    }
    return ret_pos;
#else
    return 0.0;
#endif
  }

  double mmc_eCatLib::getAxisCurrPosition(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    double          ret{ 0.0 };
    eMMC::MC_STATUS result = eMMC::MC_ReadParameter(m_cfg.master_idx, axis_idx, eMMC::mcpCommandedPosition, &ret);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadParameter(%s) fail", statusDescription((int)result).c_str());
      return 0.0;
    }
    return ret;
#else
    return 0.0;
#endif
  }

  double mmc_eCatLib::getAxisCommandedPoition(sub_idx axis_idx, double &ret_pos)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_ReadCommandedPosition(m_cfg.master_idx, axis_idx, &ret_pos);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadCommandedPosition(%s) fail", statusDescription((int)result).c_str());
      return 0.0;
    }
    return ret_pos;
#else
    return 0.0;
#endif
  }

  double mmc_eCatLib::getAxisCommandedVelocity(sub_idx axis_idx, double &ret_vel)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_ReadCommandedVelocity(m_cfg.master_idx, axis_idx, &ret_vel);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadCommandedVelocity(%s) fail", statusDescription((int)result).c_str());
      return 0.0;
    }
    return ret_vel;
#else
    return 0.0;
#endif
  }

  double mmc_eCatLib::getAxisActVelocity(sub_idx axis_idx)
  {
#ifndef _USE_SIMULATION
    double          ret{ 0.0 };
    eMMC::MC_STATUS result = eMMC::MC_ReadActualVelocity(m_cfg.master_idx, axis_idx, &ret);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadActualVelocity(%s) fail", statusDescription((int)result).c_str());
      return 0.0;
    }
    return ret;
#else
    return 0.0;
#endif
  }

  errno_t mmc_eCatLib::getStatus(sub_idx axis_idx, uint32_t &ret_status)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_ReadStatus(m_cfg.master_idx, axis_idx, &ret_status);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadStatus(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::getMotionStatus(sub_idx axis_idx, uint32_t &ret_status)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_ReadMotionState(m_cfg.master_idx, axis_idx, &ret_status);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadMotionState(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

//   errno_t mmc_eCatLib::getAxisInfo(sub_idx axis_idx, uint32_t &ret_info)
//   {
// #ifndef _USE_SIMULATION
//     eMMC::MC_STATUS result = eMMC::MC_ReadAxisInfo(m_cfg.master_idx, axis_idx, &ret_info);
//     if (result != eMMC::MC_OK)
//     {
//       ERR_PRINT("MC_ReadAxisInfo(%s) fail", statusDescription((int)result).c_str());
//       return -1;
//     }
//     return ERROR_SUCCESS;
// #else
//     return 0;
// #endif
//   }

  errno_t mmc_eCatLib::getAxisStatus(sub_idx axis_idx, uint32_t &ret_status)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_ReadAxisStatus(m_cfg.master_idx, axis_idx, &ret_status);
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_ReadAxisStatus(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::moveAbsolute(sub_idx axis_idx, double pos, double vel, double acc, double dec, double jerk, direction_t dir, buffer_mode_t buffer_mode)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_MoveAbsolute(
      m_cfg.master_idx,                 // Board ID
      axis_idx,                         // Axis number
      pos,                              // Target Position
      vel,                              // Max Velocity
      acc,                              // Max Acceleration
      dec,                              // Max Deceleration
      jerk,                             // Max Jerk
      (eMMC::MC_DIRECTION)dir,          // 0:Positive Direction, 1:Shortest_way, 2:Negative_Direction, 3:Current_Direction
      (eMMC::MC_BUFFER_MODE)buffer_mode // Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_MoveAbsolute(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::moveRelative(sub_idx axis_idx, double dist, double vel, double acc, double dec, double jerk, buffer_mode_t buffer_mode)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_MoveRelative(
      m_cfg.master_idx,                 // BoardID
      axis_idx,                         // Axis number
      dist,                             // Distance
      vel,                              // Max Velocity
      acc,                              // Max Acceleration
      dec,                              // Max Deceleration
      jerk,                             // Max Jerk
      (eMMC::MC_BUFFER_MODE)buffer_mode // Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_MoveRelative(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::moveVelocity(sub_idx axis_idx, double vel, double acc, double dec, double jerk, direction_t dir, buffer_mode_t buffer_mode)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_MoveVelocity(
      m_cfg.master_idx,                 // Board ID
      axis_idx,                         // Axis number
      vel,                              // Max Velocity
      acc,                              // Max Acceleration
      dec,                              // Max Deceleration
      jerk,                             // Max Jerk
      (eMMC::MC_DIRECTION)dir,          // 0:Positive Direction, 1:Shortest_way, 2:Negative_Direction, 3:Current_Direction
      (eMMC::MC_BUFFER_MODE)buffer_mode // Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_MoveVelocity(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif 
  }

/**
 * @brief  지정된 Parameter 값으로 홈동작을 시작 한다.
 *         Home 완료는 MC_ReadAxisStatus API 의 IsHomed bit 로 판단한다.
 * 
 * @param axis_idx 
 * @param pos 
 * @param buffer_mode 
 * @return errno_t 
 */
  errno_t mmc_eCatLib::moveHome(sub_idx axis_idx, double pos, buffer_mode_t buffer_mode)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Home(
      m_cfg.master_idx,                 // Board ID
      axis_idx,                         // Axis number
      pos,                              // Position
      (eMMC::MC_BUFFER_MODE)buffer_mode // Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Home(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::moveStop(sub_idx axis_idx, bool is_execute, double dec, double jerk)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Stop(
      m_cfg.master_idx, // Board ID
      axis_idx,         // Axis number
      is_execute,       // Execute
      dec,              // Max Deceleration
      jerk              // Max Jerk
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Stop(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::SetPosition(sub_idx axis_idx, double pos, bool is_relative, execution_mode_t exe_mode)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_SetPosition(
      m_cfg.master_idx,                 // Board ID
      axis_idx,                         // Axis number
      pos,                              // Position
      is_relative,                      // Relative 0 : Absolute(Position) , 1 : Relative (Current Position+Position)
      (eMMC::MC_EXECUTION_MODE)exe_mode // EXECUTION_MODE  0 : mcImmediately (즉시 좌표 값 변경), 1 : mcQueued (이전 동작 완료 후 좌표 값 변경)
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_SetPosition(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::halt(sub_idx axis_idx, double dec, double jerk, buffer_mode_t buffer_mode)
  {
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result = eMMC::MC_Halt(
      m_cfg.master_idx,                 // Board ID
      axis_idx,                         // Axis number
      dec,                              // Max Deceleration
      jerk,                             // Max Jerk
      (eMMC::MC_BUFFER_MODE)buffer_mode // Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
    );
    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_Halt(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }
    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  errno_t mmc_eCatLib::getAxisDigitalInput(sub_idx axis_idx, uint32_t &value_bit)
  {
#ifndef _USE_SIMULATION
    value_bit = 0;
    for (int i = 0; i < 32; i++)
    {
      uint32_t        value = (1 << i);
      bool            ret{};
      eMMC::MC_STATUS result = eMMC::MC_ReadDigitalInput(
        m_cfg.master_idx, // Board ID
        axis_idx,         // Axis number
        value,            // Input Number: Bit Unit
        &ret);
      if (result != eMMC::MC_OK)
      {
        ERR_PRINT("MC_ReadDigitalInput(%s) fail", statusDescription((int)result).c_str());
        return -1;
      }
      else
      {
        value_bit = value_bit | ((ret ? 1 : 0) << i);
      }
    }


    return ERROR_SUCCESS;
#else
    return 0;
#endif
  }

  template <typename ParamValueType>
    requires mcc_param_type_c<ParamValueType>
    errno_t mmc_eCatLib::setParameter(sub_idx axis_idx, param_t param_idx, ParamValueType value)
  {
    
#ifndef _USE_SIMULATION
    eMMC::MC_STATUS result{};
    if constexpr (std::same_as<ParamValueType, bool>)
    {
      result = eMMC::MC_WriteBoolParameter(
        m_cfg.master_idx, // Board ID
        axis_idx,         // Axis number
        param_idx,        // ParameterNum
        value);
    }
    else if constexpr (std::same_as<ParamValueType, uint32_t>)
    {
      result = eMMC::MC_WriteIntParameter(
        m_cfg.master_idx, // Board ID
        axis_idx,         // Axis number
        param_idx,        // ParameterNum
        value);
    }
    else if constexpr (std::same_as<ParamValueType, double>)
    {
      result = eMMC::MC_WriteParameter(
        m_cfg.master_idx, // Board ID
        axis_idx,         // Axis number
        param_idx,        // ParameterNum
        value);
    }
    else
    {
      ERR_PRINT("Unknown ParamValueType");
      return -1;
    }

    if (result != eMMC::MC_OK)
    {
      ERR_PRINT("MC_WriteParameter(%s) fail", statusDescription((int)result).c_str());
      return -1;
    }

    return ERROR_SUCCESS;
#else
    return 0;
#endif
  } 

    template <typename ParamValueType>
      requires mcc_param_type_c<ParamValueType>
    errno_t mmc_eCatLib::getParameter(sub_idx axis_idx, param_t param_idx, ParamValueType& value)
    {
#ifndef _USE_SIMULATION
      eMMC::MC_STATUS result{};
      if constexpr (std::same_as<ParamValueType, bool>)
      {
        result = eMMC::MC_ReadBoolParameter(
          m_cfg.master_idx, // Board ID
          axis_idx,         // Axis number
          param_idx,        // ParameterNum
          &value);
      }
      else if constexpr (std::same_as<ParamValueType, uint32_t>)
      {
        result = eMMC::MC_ReadIntParameter(
          m_cfg.master_idx, // Board ID
          axis_idx,         // Axis number
          param_idx,        // ParameterNum
          &value);
      }
      else if constexpr(std::same_as<ParamValueType, double>)//(std::is_same_v< std::remove_cvref<ParamValueType>
      {
        result = eMMC::MC_ReadParameter(
          m_cfg.master_idx, // Board ID
          axis_idx,         // Axis number
          param_idx,        // ParameterNum
          &value);
      }
      else
      {
        ERR_PRINT("Unknown ParamValueType");
        return -1;
      }

      if (result != eMMC::MC_OK)
      {
        ERR_PRINT("MC_ReadParameter(%s) fail", statusDescription((int)result).c_str());
        return -1;
      }

      return ERROR_SUCCESS;
#else
      return 0;
#endif
    }

    // 명시적 인스턴스화
    template errno_t mmc_eCatLib::getParameter<bool>(sub_idx, param_t, bool&);
    template errno_t mmc_eCatLib::getParameter<double>(sub_idx, param_t, double&);
    template errno_t mmc_eCatLib::getParameter<uint32_t>(sub_idx, param_t, uint32_t&);

    // 명시적 인스턴스화
    template errno_t mmc_eCatLib::setParameter<bool>(sub_idx, param_t, bool);
    template errno_t mmc_eCatLib::setParameter<uint32_t>(sub_idx, param_t, uint32_t);
    template errno_t mmc_eCatLib::setParameter<double>(sub_idx, param_t, double);

  
//////////////////////////////////////////////////////////////////////////////////////
#ifdef _USE_HW_CLI
  /// @brief test_callback
  /// @param argc
  /// @param argv
  /// @return
  int mmc_eCatLib::testCB_func(int argc, char *argv[])
  {
    
#ifndef _USE_SIMULATION
    auto get_data = [](char *int_ptr) -> int
    {
      int ret = 0;
      ret     = (int)strtoul((const char *)int_ptr, (char **)NULL, (int)0);
      return ret;
    };

    auto get_bool = [](char *int_ptr, std::string_view comp) -> bool
    {
      return (std::string(int_ptr).compare(comp) == 0);
    };

     // 비트를 출력하는 헬퍼 함수
      auto to_binary_str = [&](auto value, bool is_reverse = false) ->std::string
      {
        std::bitset<sizeof(value)*8> binary(value);                  // 비트로 변환
        std::string     binary_str = binary.to_string(); // 비트열을 문자열로 변환

        // 4비트씩 공백으로 구분
        std::string formatted;
        if (is_reverse)
        {
          for (size_t i = binary_str.size(); (i+1)  > 0; --i)
          {
            formatted += binary_str[i];
            if (i % 4 == 0 && i != binary_str.size() )
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

      /*
      
    #inReg datas : [17,16,15,14  ...  3,2,1,0] 
      reg[0] : 0000 0000   0000 0000
      reg[1] : 1011 1110   1111 1111
      reg[2] : 1100 0000   0000 0000
      reg[3] : 0000 0000   0000 0000
      */
    auto view_io = [&]()
    {
        constexpr int top_bit_no = 15;
        {
          std::stringstream ss{};

          ss << "\n inReg datas : 상위 비트 우선 출력[17,16,15,14  ...  3,2,1,0] \n";
          // 각 4바이트 데이터 처리
          int reg_idx = 0;
          for (size_t idx = 0; idx < m_deviceReg.inReg.size(); ++idx) 
          {            
            ss << "   >reg[" << reg_idx++ << (reg_idx < 11 ? " ] : " : "] : ");
            // 비트 단위로 출력
            for (int i = top_bit_no; i >= 0; --i) // 상위 비트부터 출력 
            {  
              ss << ((m_deviceReg.inReg[idx].u16Data[0] >> i) & 1 ? "1" : "0");
              if ((top_bit_no - i + 1) % 4 == 0)  // 4비트마다 공백 추가
                ss << " ";
              if ((top_bit_no - i + 1) % (top_bit_no +1) == 0) // 16비트마다 줄 바꿈 추가
                ss << "\n   >reg[" << reg_idx++ << (reg_idx < 11 ? " ] : " : "] : ");
            }
            for (int i = top_bit_no; i >= 0; --i) // 상위 비트부터 출력 
            {
              ss << ((m_deviceReg.inReg[idx].u16Data[1] >> i) & 1 ? "1" : "0");
              if ((top_bit_no - i + 1) % 4 == 0)  // 4비트마다 공백 추가
                ss << " ";
            }
            ss << "\n";
          }
            std::cout << ss.str() << std::endl;
          }

        {
          std::stringstream ss{};

					ss << "\n outReg datas : 상위 비트 우선 출력[17,16,15,14  ...  3,2,1,0] \n";
          // 각 4바이트 데이터 처리
          int reg_idx = 0;
          for (size_t idx = 0; idx < m_deviceReg.outReg.size(); ++idx) 
          {
            ss << "   >reg[" << reg_idx++ << (reg_idx < 11 ? " ] : " : "] : ");
            
            // 비트 단위로 출력
            for (int i = top_bit_no; i >= 0; --i) // 상위 비트부터 출력
            {  
              ss << ((m_deviceReg.outReg[idx].u16Data[0] >> i) & 1 ? "1" : "0");
              if ((top_bit_no - i + 1) % 4 == 0)  // 4비트마다 공백 추가
                ss << " ";
							if ((top_bit_no - i + 1) % (top_bit_no + 1) == 0) // 16비트마다 줄 바꿈 추가
                ss << "\n   >reg[" << reg_idx++ << (reg_idx < 11 ? " ] : " : "] : ");
            }
            // 비트 단위로 출력
            for (int i = top_bit_no; i >= 0; --i) // 상위 비트부터 출력
            {
              ss << ((m_deviceReg.outReg[idx].u16Data[1] >> i) & 1 ? "1" : "0");
              if ((top_bit_no - i + 1) % 4 == 0)  // 4비트마다 공백 추가
                ss << " ";
            }
            ss << "\n";
          }

            std::cout << ss.str() << std::endl;
          } };

    bool ret{};

    enum : int
    {
      arg_cnt_0,
      arg_cnt_1,
      arg_cnt_2,
      arg_cnt_3,
      arg_cnt_4,
      arg_cnt_5,
      arg_cnt_6,
      arg_cnt_7
    };

    switch ((argc - 1))
    {
      case arg_cnt_1:
        {
          if (argv[1])
          {
            if (std::string(argv[1]).compare("info") == 0)
            {
              if(get_master_info() != ERROR_SUCCESS)
              {
                std::cout<<"  [NG] fail get_master_info " <<std::endl;
              }
              else
              {                
                std::cout <<get_info()<<std::endl;
              }
              ret = true;
            } //
            else if (std::string(argv[1]).compare("view_io") == 0)
            {

              /*while (TinyC::Cli::cliKeepLoop())
              {*/
                threadJob();

             /*   tim::delay(20);
              }*/
              ret = true;
              view_io();
            }
            else if (std::string(argv[1]).compare("reconnect") == 0)
            {
              ret = true;
              //if()
            }
            else if (std::string(argv[1]).compare("connect") == 0)
            {
              ret = true;
              if(connect())
              {
                std::cout <<get_info()<<std::endl;
              }
              else
              {
                std::cout<<"  [NG] fail connet " <<std::endl;
              }
            }
            else if (std::string(argv[1]).compare("run") == 0)
            {
              ret    = true;
              eMMC::MC_STATUS result = eMMC::MC_MasterSTOP(m_cfg.master_idx);
              if (result != eMMC::MC_OK)
                ERR_PRINT("eMMC::MC_MasterSTOP error[%s]", statusDescription((int)result).c_str());
              else
                LOG_PRINT("eMMC::MC_MasterSTOP success");

              tim::delay(1000);
              result = eMMC::MC_MasterInit(m_cfg.master_idx);
              if (result != eMMC::MC_OK)
                ERR_PRINT("eMMC::MC_MasterRUN error[%s]", statusDescription((int)result).c_str());
              else
                LOG_PRINT("eMMC::MC_MasterRUN success");

              tim::delay(1000);
              result = eMMC::MC_MasterRUN(m_cfg.master_idx);
              if (result != eMMC::MC_OK)
                ERR_PRINT("eMMC::MC_MasterRUN error[%s]", statusDescription((int)result).c_str());
              else
                LOG_PRINT("eMMC::MC_MasterRUN success");
            }
            else if (std::string(argv[1]).compare("on") == 0)
            {
              ret = true;
              eMMC::MC_STATUS result = eMMC::MC_Init();
              if (result != eMMC::MC_OK)
              {
                ERR_PRINT("MC_Init(%s) fail", statusDescription((int)result).c_str());
                
              }
            }
          }
        }
        break;
      case arg_cnt_2:
        {
          if (argv[1])
          {
            uint32_t data = (uint32_t)get_data(argv[2]);
            if (std::string(argv[1]).compare("set_out") == 0)
            {
              m_deviceReg.outReg[0](data, true);
              threadJob();
              ret = true;
              view_io();
            }
            else if (std::string(argv[1]).compare("reset_out") == 0)
            {
              m_deviceReg.outReg[0](data, false);
              threadJob();
              ret = true;
              view_io();
            }
          }
        }
        break;
      case arg_cnt_3:
        {
          if (argv[1])
          {
          }
        }
        break;
      case arg_cnt_4:
        {
          uint16_t cat_addr = (uint16_t)get_data(argv[2]);
          uint32_t offset = (uint32_t)get_data(argv[3]);
          uint32_t size = (uint32_t)get_data(argv[4]);
            if (std::string(argv[1]).compare("view_in") == 0)
            {
              ret = true;
              std::vector<uint8_t> buff(size);

              eMMC::MC_STATUS ret = eMMC::MC_IO_READ(m_cfg.master_idx, cat_addr, eMMC::BUF_IN, offset, size, (eMMC::UINT8 *)buff.data());

              if (ret != eMMC::MC_OK) // Error occur...
              {
                //Get Error Message
                constexpr size_t buffer_size = 128;
                std::string     err_msg(buffer_size, '\0');
                std::string     msg(buffer_size, '\0');
                eMMC::MC_GetErrorMessage(ret, buffer_size, (char *)err_msg.c_str());
                sprintf_s((char *)msg.c_str(), buffer_size, "Error : %08X, %s", ret, (const char *)err_msg.c_str());

                std::cout << msg << std::endl;

                //Popup the message
              }
              else
              {
                std::stringstream ss{};

                ss << "\n inReg datas : (0,1,2,3 ... 15)\n";
                ss << "    "; // space 4
                int check_dword = 0;
                for (auto &elm : buff)
                {
                  ss << to_binary_str(elm , true);
                  if ((++check_dword % 4) == 0)
                    ss << " \n    ";
                  else
                    ss << " ";
                }

                std::cout << ss.str() << std::endl;
              }
            }
            else if (std::string(argv[1]).compare("view_out") == 0)
            {
              ret = true;
              std::vector<uint8_t> buff(size);

              eMMC::MC_STATUS ret = eMMC::MC_IO_READ(m_cfg.master_idx, cat_addr, eMMC::BUF_OUT, offset, size, (eMMC::UINT8 *)buff.data());

              if (ret != eMMC::MC_OK) // Error occur...
              {
                //Get Error Message
                constexpr size_t buffer_size = 128;
                std::string     err_msg(buffer_size, '\0');
                std::string     msg(buffer_size, '\0');
                eMMC::MC_GetErrorMessage(ret, buffer_size, (char *)err_msg.c_str());
                sprintf_s((char *)msg.c_str(), buffer_size, "Error : %08X, %s", ret, (const char *)err_msg.c_str());

                std::cout << msg << std::endl;

                //Popup the message
              }
              else
              {
                std::stringstream ss{};

                ss << "\n outReg datas : (0,1,2,3 ... 15)\n";
                ss << "    "; // space 4
                int check_dword = 0;
                for (auto &elm : buff)
                {
                    ss << to_binary_str(elm, true);
                    if ((++check_dword % 4) == 0)
                      ss << " \n    ";
                    else
                      ss << " ";
                }

                std::cout << ss.str() << std::endl;
              }
             
            }
            else if (std::string(argv[1]).compare("write_out") == 0)
            {
              ret = true;
              udword_t data {size};
              //std::vector<uint8_t> buff(size);
              constexpr auto data_size = sizeof(data);
              eMMC::MC_STATUS ret = eMMC::MC_IO_WRITE(m_cfg.master_idx, cat_addr, offset, data_size, (eMMC::UINT8 *)&data[0]);

              if (ret != eMMC::MC_OK) // Error occur...
              {
                //Get Error Message
                constexpr size_t buffer_size = 128;
                std::string     err_msg(buffer_size, '\0');
                std::string     msg(buffer_size, '\0');
                eMMC::MC_GetErrorMessage(ret, buffer_size, (char *)err_msg.c_str());
                sprintf_s((char *)msg.c_str(), buffer_size, "Error : %08X, %s", ret, (const char *)err_msg.c_str());

                std::cout << msg << std::endl;

                //Popup the message
              }
              else
              {
                std::stringstream ss{};
                ss << "\n write datas : (0,1,2,3 ... 15)\n";
                ss << "    "; // space 4
                int check_dword = 0;
                for (size_t i = 0; i < 4; i++)
                {
                  ss << to_binary_str(data[i], true);
                  if ((++check_dword % 4) == 0)
                    ss << " \n    ";
                  else
                    ss << " ";
                }
                

                std::cout << ss.str() << std::endl;
              }
             
            }
        }
        break;
      case arg_cnt_5:
        {
          if (argv[1])
          {
            if (std::string(argv[1]).compare("run") == 0)
            {
            }
          }
        }
        break;
      case arg_cnt_6:
        {
        }
        break;
      case arg_cnt_7:
        {
        }
        break;
      default:
        break;
    }
    // end of switch

    if (ret)
      return 0;

    std::cout << "mmc info" << std::endl;
    std::cout << "mmc view_io" << std::endl;
    std::cout << "mmc run " << std::endl;
    std::cout << "mmc on " << std::endl;
    std::cout << "mmc connect " << std::endl;
    std::cout << "mmc reconnect " << std::endl;
    std::cout << "mmc set_out [0:32]" << std::endl;
    std::cout << "mmc reset_out [0:32]" << std::endl;
    
    /*
     cli view_in  coupler id, offset(ch start address), get size
     입력 레지스트 정보를 가져온다. 입력 ch start offset address 34(0x22)부터  
     연결된 ch로 2씩 증가한다. 
     coupler ID 증가된 첫번째 ch은 아래 순서 offset과 동일하게 적용한다. (MMCE Manager Address IO와 다르다) 
     
     ┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐ 
     │ 0x22 ││ 0x24 ││ 0x26 ││ 0x28 ││ 0x2A │ 
     └──────┘└──────┘└──────┘└──────┘└──────┘ ... 

     get size를각 각 채널의 레지스트 사이즈이다
     offset -34(0x22)에서 사이즈 8로 입력하면
     총 8개 레지스트 정보를 읽는다 : 0x22 2개, 0x24 2개, 0x26 2개, 0x28 2개
    */
    std::cout << "mmc view_in [cat_addr] [offset] [size]" << std::endl;

    /*
  cli view_out coupler id, offset(ch start address), get size
  입력 레지스트 정보를 가져온다. 출력  ch start offset address 0(0x00)부터
  coupler ID 증가된 첫번째 ch은 아래 순서 offset과 동일하게 적용한다. (MMCE Manager Address IO와 다르다)
  연결된 ch로 2씩 증가한다.
  ┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐
  │ 0x00 ││ 0x02 ││ 0x04 ││ 0x06 ││ 0x08 │
  └──────┘└──────┘└──────┘└──────┘└──────┘ ...

  get size를각 각 채널의 레지스트 사이즈이다
  offset -0(0x00)에서 사이즈 8로 입력하면
  총 8개 레지스트 정보를 읽는다 : 0x00 2개, 0x02 2개, 0x04 2개, 0x06 2개
 */
    std::cout << "mmc view_out [cat_addr] [offset] [size]" << std::endl; 
    std::cout << "mmc write_out [cat_addr] [offset] [data(DW)]" << std::endl;
#endif
    return 0;
  }
#endif

};

// end of namespace hw_lib

#endif // _USE_HW_LIB_MMC_ETHERCAT
