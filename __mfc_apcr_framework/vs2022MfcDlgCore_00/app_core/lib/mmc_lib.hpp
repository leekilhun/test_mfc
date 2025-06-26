/**
 * @file mmc_lib.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef _LIB_MMC_LIB_HPP_
#define _LIB_MMC_LIB_HPP_

#include "ap_def.hpp"



namespace hw_lib
{
  // Concept 정의
 
  template<typename T>
  concept mcc_param_type_c = std::same_as<T, bool> || std::same_as<T, uint32_t> || std::same_as<T, double>;


  #ifdef USE_HW_LIB_MMC_ETHERCAT

  class mmc_eCatLib : public IIO
  {
  public:
    //MARK:configuration for MMC Lib hardware
    //////////////////////////////////////////////////////////////////////////////////////////////
    //To define and use the app according to the system configuration when configuring the app

   static constexpr int max_master_board_cnt = 4;
  /*
  ## block3 demo equipment 구성

  etheCAT 망에 구성된 device ID
  slave id 1 (io Module NX-ECC201 coupler V1.2)
      NX-ID5342 (입력 16bit) * 6개   4 * 32bits
      NX-OD5121 (출력 16bit) * 6개   4 * 32bits

      
   */
  enum sub_idx : uint16_t
  {
    idx_io_0   = 1,
    idx_axis_0 = 3,
    idx_axis_1 = 4,
    idx_axis_2 = 5,
    idx_axis_3 = 6,
    idx_axis_4 = 7,
    idx_axis_5 = 8,
    idx_max
  };


//MARK:define bit map 
/****************************************************
 ****************************************************/
  // 채널별 byte 수 (16 bit - 2byte)
  static constexpr auto def_ch_byte_cnt = 2;

  //사용되는 IO coupler의 cnt 
  static constexpr auto def_io_coupler_cnt = 2; 

//****************************************************
//  입력으로 사용되는 IO 모듈에 대해 정의합니다.
//****************************************************

  // coupler id-1 : 입력 모듈 8채널 
  static constexpr uint32_t max_io_in_ch_id_1  = 6;  
  // coupler id-2 : 입력 모듈 9채널
  static constexpr uint32_t max_io_in_ch_id_2  = 0;
  // 입력 채널에서 사용되는 bit cnt
  static constexpr uint32_t def_ch_in_cnt = 16;
  // 입력으로 사용되는 실제 최대 bit cnt
  static constexpr uint32_t max_cnt_io_in = (max_io_in_ch_id_1 + max_io_in_ch_id_2) * def_ch_in_cnt;

  //couplerID 1의 최대 입력 채널 byte cnt
  static constexpr auto max_cnt_ch_in_id_1 = max_io_in_ch_id_1 * def_ch_byte_cnt;
  //couplerID 2의 최대 입력 채널 byte cnt
  static constexpr auto max_cnt_ch_in_id_2 = max_io_in_ch_id_2 * def_ch_byte_cnt;
  //최대 입력 채널 byte cnt
  //static constexpr auto max_cnt_ch_in = max_cnt_ch_in_id_1 + max_cnt_ch_in_id_2;

  // coupler ID 1 - 4byte의 dword로 관리하는 입력 register 의 사이즈
  static constexpr auto def_in_reg_size_id_1  = ((max_cnt_ch_in_id_1) + ((max_cnt_ch_in_id_1) % sizeof(udword_t))) / sizeof(udword_t);
  // coupler ID 2 - 4byte의 dword로 관리하는 입력 register 의 사이즈
  static constexpr auto def_in_reg_size_id_2  = ((max_cnt_ch_in_id_2) + ((max_cnt_ch_in_id_2) % sizeof(udword_t))) / sizeof(udword_t);
  // 사이즈 4byte(dword)로 관리하는 입력 register 의 사이즈 (m_deviceReg.inReg)
  static constexpr auto def_in_reg_size = def_in_reg_size_id_1 + def_in_reg_size_id_2;




//****************************************************
//  출력으로 사용되는 IO 모듈에 대해 정의합니다.
//****************************************************

  // coupler id-1 : 8채널의 출력 모듈
  static constexpr uint32_t max_io_out_ch_id_1  = 6;
  // coupler id-2 : 9채널의 출력 모듈
  static constexpr uint32_t max_io_out_ch_id_2  = 0;
  // 출력 채널에서 사용되는 bit cnt
  static constexpr uint32_t def_ch_out_cnt = 16;
  // 출력으로 사용되는 실제 최대 bit cnt
  static constexpr uint32_t max_cnt_io_out = (max_io_out_ch_id_1 + max_io_out_ch_id_2) * def_ch_out_cnt;


  //couplerID 1의 최대 출력 채널 byte cnt
  static constexpr auto max_cnt_ch_out_id_1 = max_io_out_ch_id_1 * def_ch_byte_cnt;
  //couplerID 2의 최대 출력 채널 byte cnt
  static constexpr auto max_cnt_ch_out_id_2 = max_io_out_ch_id_2 * def_ch_byte_cnt;
  //최대 출력 채널 byte cnt
  //static constexpr auto max_cnt_ch_out = max_cnt_ch_out_id_1 + max_cnt_ch_out_id_2;

  // coupler ID 1 - 4byte의 dword로 관리하는 출력 register 의 사이즈
  static constexpr auto def_out_reg_size_id_1  = ((max_cnt_ch_out_id_1) + ((max_cnt_ch_out_id_1) % sizeof(udword_t))) / sizeof(udword_t);
  // coupler ID 2 - 4byte의 dword로 관리하는 출력 register 의 사이즈
  static constexpr auto def_out_reg_size_id_2  = ((max_cnt_ch_out_id_2) + ((max_cnt_ch_out_id_2) % sizeof(udword_t))) / sizeof(udword_t);
   // 사이즈 4byte(dword)로 관리하는 출력 register 의 사이즈 (m_deviceReg.outReg)
  static constexpr auto def_out_reg_size = def_out_reg_size_id_1 + def_out_reg_size_id_2;

  // address offset
  static constexpr uint16_t def_data_offset_start_out = 0;
  // address offset
  static constexpr uint16_t def_data_offset_start_in  = 34;


  static constexpr bool def_is_abs_encoder = true;
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

  public:
     
    //pn - Parameter Number
    enum param_t : uint32_t
    {
      pn_CommandedPosition          = 1,    // DOUBLE            R             Commanded position
      pn_SWLimitPosF                = 2,    // DOUBLE            R/W           Positive Software limit switch position
      pn_SWLimitNegF                = 3,    // DOUBLE            R/W           Negative Software limit switch position
      pn_EnableLimit_Pos            = 4,    // Bool              R/W           Enable positive software limit switch
      pn_EnableLimit_Neg            = 5,    // Bool              R/W           Enable negative software limit switch
      pn_EnablePosLagMonitoring     = 6,    // Bool              R/W           Enable monitoring of position lag
      pn_MaxPosition_Lag            = 7,    // DOUBLE            R/W           Maximal position lag
      pn_MaxAllowVelocitySystem     = 8,    // DOUBLE            R             Maximal allowed velocity of the axis in the motion system
      pn_MaxAllowVelocityAppl       = 9,    // DOUBLE            R/W           Maximal allowed velocity of the axis in the application
      pn_ActualVelocity             = 10,   // DOUBLE            R             Actual velocity
      pn_CommandedVelocity          = 11,   // DOUBLE            R             Commanded velocity
      pn_MaxAccelerationSysMotion   = 12,   // DOUBLE            R             Maximal allowed acceleration of the axis in the motion system
      pn_MaxAccelerationAppl        = 13,   // DOUBLE            R/W           Maximal allowed acceleration of the axis in the application
      pn_MaxDecelerationSysMotion   = 14,   // DOUBLE            R             Maximal allowed deceleration of the axis in the motion system
      pn_MaxDecelerationAppl        = 15,   // DOUBLE            R/W           Maximal allowed deceleration of the axis in the application
      pn_MaxJerkSystemMotion        = 16,   // DOUBLE            R             Maximum allowed jerk of the axis in the motion system
      pn_MaxAllowJerkAppl           = 17,   // DOUBLE            R/W           Maximum allowed jerk of the axis in the application
      pn_ActualPostion              = 1000, // DOUBLE            R             Actual Position
      pn_CommandedAccel             = 1001, // DOUBLE            R             Commanded Acceleration
      pn_ActualAccel                = 1002, // DOUBLE            R             Actual Acceleration
      pn_CommandedJerk              = 1003, // DOUBLE            R             Commanded Jerk
      pn_ActualJerk                 = 1004, // DOUBLE            R             Actual Jerk
      pn_TotalBufferCount           = 1010, // UINT32            R             Total Motion Buffer Count
      pn_AvailableBufferCount       = 1011, // UINT32            R             Available Motion Buffer Count
      pn_AxisType                   = 2002, // UINT32            R             Axis Type
      pn_ModuloAxis                 = 2003, // Bool              R             Modulo Axis
      pn_ModuloValue                = 2004, // DOUBLE            R             Modulo Value
      pn_EnableHWLimitPo            = 2010, // Bool              R             Pos. Limit Switch Enable
      pn_HWLimitPosInputNum         = 2011, // UINT32            R             Pos. Limit Switch Input Num
      pn_HWLimitPosActLevel         = 2012, // Bool              R             Pos. Limit Switch Active Level
      pn_EnableHWLimitNeg           = 2013, // Bool              R             Neg. Limit Switch Enable
      pn_HWLimitNegInputNum         = 2014, // UINT32            R             Neg. Limit Switch Input Num
      pn_HWLimitNegActLevel         = 2015, // Bool              R             Neg. Limit Switch Active Level
      pn_HomeInputNum               = 2016, // UINT32            R             Home Sensor Input Num
      pn_HomeActLevel               = 2017, // Bool              R             Home Sensor Active Level
      pn_MarkerInputNum             = 2018, // UINT32            R             Z-Phase Input Num
      pn_MarkerActLevel             = 2019, // Bool              R             Z-Phase Input Active Level
      pn_InputActLevel              = 2020, // UINT32            R             Input Active Level Setting
      pn_EnableLimitPos             = 2030, // Bool              R/W           Software Position Limit Pos. Enable
      pn_SWLimitPos                 = 2031, // DOUBLE            R/W           Software Position Limit Pos.
      pn_EnableLimitNeg             = 2032, // Bool              R/W           Software Position Limit Neg. Enable
      pn_SWLimitNeg                 = 2033, // DOUBLE            R/W           Software Position Limit Neg.
      pn_MaxVelocityAppl            = 2034, // DOUBLE            R/W           Max. Velocity Application
      pn_MaxAccelAppl               = 2035, // DOUBLE            R/W           Max. Accel Application
      pn_MaxDecelAppl               = 2036, // DOUBLE            R/W           Max. Decel Application
      pn_MaxJerkAppl                = 2037, // DOUBLE            R/W           Max. Jerk Application
      pn_MaxVelocitySystem          = 2038, // DOUBLE            R             Max. Velocity System
      pn_MaxAccelerationSystem      = 2039, // DOUBLE            R             Max. Accel System
      pn_MaxDecelerationSystem      = 2040, // DOUBLE            R             Max. Decel System
      pn_MaxJerkSystem              = 2041, // DOUBLE            R             Max. Jerk System
      pn_EStopType                  = 2060, // UINT32            R/W           E-Stop Type
      pn_EStopDecel                 = 2061, // DOUBLE            R/W           E-Stop Decel
      pn_EStopJerk                  = 2062, // DOUBLE            R/W           E-Stop Jerk
      pn_InvertCmdDir               = 2070, // Bool              R             Invert Command Direction
      pn_CmdScaleFactor             = 2071, // DOUBLE            R             Command Data Unit Scale Factor
      pn_FeedbackMode               = 2072, // UINT32            R             Feedback Mode
      pn_InvertFeedbackDir          = 2073, // Bool              R             Invert Position Feedback Direction
      pn_FeedbackScaleFactor        = 2074, // DOUBLE            R             Feedback Data Unit Scale Factor
      pn_PositionFeedbackFilter     = 2075, // DOUBLE            R/W           Feedback Filter Time Position
      pn_VelocityFeedbackFilter     = 2076, // DOUBLE            R/W           Feedback Filter Time Velocity
      pn_AccelerationFeedbackFilter = 2077, // DOUBLE            R/W           Feedback Filter Time Accel
      pn_StartVelocityOffset        = 2078, // DOUBLE            R/W           Start Velocity Offset
      pn_StopVelocityOffset         = 2079, // DOUBLE            R/W           Stop Velocity Offset
      pn_InPositionCheckType        = 2080, // UINT32            R/W           In-Position Check Type
      pn_InPositionWindowSize       = 2081, // DOUBLE            R/W           In-Position Window Size
      pn_InVelocityWindowSize       = 2082, // DOUBLE            R/W           In-Velocity Window Size
      pn_EnablePositionLagMonit     = 2083, // Bool              R/W           Enable Position Lag Monitoring
      pn_MaxPositionLag             = 2084, // DOUBLE            R/W           Max Position Lag
      pn_PositionLagCalMethod       = 2085, // UINT32            R             Position Lag Calculation
      pn_EnableVelocityLagMonit     = 2086, // Bool              R/W           Enable Velocity Lag Monitoring
      pn_MaxVelocityLag             = 2087, // DOUBLE            R/W           Max Velocity Lag
      pn_VelocityLagCalMethod       = 2088, // UINT32            R             Velocity Lag Calculation
      pn_HomingType                 = 2100, // UINT32            R             Homing Type
      pn_HomingDir                  = 2101, // UINT32            R             Homing Direction
      pn_HomingVelocity             = 2102, // DOUBLE            R/W           Homing Velocity
      pn_HomingAcceleration         = 2103, // DOUBLE            R/W           Homing Accel
      pn_HomingDeceleration         = 2104, // DOUBLE            R/W           Homing Decel
      pn_HomingJerk                 = 2105, // DOUBLE            R/W           Homing Jerk
      pn_HomingCreepVelocity        = 2106, // DOUBLE            R/W           Homing Creep Vel
      pn_HomePositionOffset         = 2107, // DOUBLE            R/W           Home Position Offset Value
      pn_HomeCompleteFlagHandle     = 2108  // UINT32            R/W           Home Complete Handling Mode
    };

    // struct param_hash
    // {
    //   static uint32_t operator()(param_t p)
    //   {
    //     return static_cast<uint32_t>(p);
    //   }
    // };


    enum direction_t
    {
      // 0:Positive Direction, 1:Shortest_way, 2:Negative_Direction, 3:Current_Direction
      
      positive_direction = 0,
      shortest_way,
      negative_direction,
      current_import_redirection,
    };

    enum buffer_mode_t
    {
      //0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
      //6:BufferedBlendingLow, 7:BufferedBlendingPrevious, 8:BufferedBlendingNext, 9:BufferedBlendingHigh

      buffMode_Aborting = 0,
      buffMode_Buffered,
      buffMode_BlendingLow,
      buffMode_BlendingPrevious,
      buffMode_BlendingNext,
      buffMode_BlendingHigh,
      buffMode_BufferedBlendingLow,
      buffMode_BufferedBlendingPrevious,
      buffMode_BufferedBlendingNext,
      buffMode_BufferedBlendingHigh,
    };

    enum execution_mode_t
    {
      exe_mode_Immediately = 0,
      exe_mode_Queued,
    };

    union axis_Info_t
    {
      uint32_t axis_info{};

      struct
      {
        uint32_t HomeAbsSwitch : 1, // = 0x00000001;
          LimitSwitchPos       : 1, // = 0x00000002;
          LimitSwitchNeg       : 1, // = 0x00000004;
          Reserved1            : 1, // = 0x00000008;
          Reserved2            : 1, // = 0x00000010;
          ReadyForPowerOn      : 1, // = 0x00000020;
          PowerOn              : 1, // = 0x00000040;
          IsHomed              : 1, // = 0x00000080;
          AxisWarining         : 1, // = 0x00000100;
          MotionComplete       : 1, // = 0x00000200;
          Gearing              : 1, // = 0x00000400;
          GroupMotion          : 1, // = 0x00000800;
          BufferFull           : 1, // = 0x00001000;
          Reseved              : 19;
      };

      uint32_t operator=(const uint32_t &data)
      {
        axis_info = data;
        return axis_info;
      }
    };
    // end of union Axis_Info_t

    union axis_status_t
    {
      uint32_t status{};

      struct
      {
        uint32_t ErrorStop      : 1,
          Disabled              : 1,
          Stopping              : 1,
          StandStill            : 1,
          DiscreteMotion        : 1,
          ContinuousMotion      : 1,
          SynchronizedMotion    : 1,
          Homing                : 1,
          SWLimitSwitchNegEvent : 1,
          SWLimitSwitchPosEvent : 1,
          ConstantVelocity      : 1,
          Accelerating          : 1,
          Decelerating          : 1,
          DirectionPositive     : 1,
          DirectionNegative     : 1,
          LimitSwitchNegative   : 1,
          LimitSwitchPositive   : 1,
          HomeAbsSwitch         : 1,
          LimitSwitchPosEvent   : 1,
          LimitSwitchNegEvent   : 1,
          DriveFault            : 1,
          Reserved1             : 1,
          ReadyForPowerOn       : 1,
          PowerOn               : 1,
          IsHomed               : 1,
          AxisWarning           : 1,
          MotionComplete        : 1,
          Gearing               : 1,
          GroupMotion           : 1,
          BufferFull            : 1,
          Reserved              : 2;
      };

      uint32_t operator=(const uint32_t &data)
      {
        status = data;
        return status;
      }

      bool operator()(uint8_t bit_idx)
      {
        if (bit_idx > 31)
        {
          std::cerr << "bit_idx out of range" << std::endl;
          return false;
        }
        return (status & (1 << (size_t)bit_idx)) != 0;
      }
    };
    // end of union Axis_status_t
    
    union motion_status_t
    {
      uint32_t status{};

      struct
      {
        uint32_t ConstantVelocity : 1, // = 0x01;
          Accelerating            : 1, // = 0x02;
          Decelerating            : 1, // = 0x04;
          DirectionPositive       : 1, // = 0x08;
          DirectionNegative       : 1, // = 0x08;
          reserved                : 27;
      };

      uint32_t operator=(const uint32_t &data)
      {
        status = data;
        return status;
      }     
    };

    // end of union motion_status_t


    union status_t
    {
      uint32_t status{};

      struct
      {
        uint32_t ErrorStop   : 1, // = 0x01;
          Disabled           : 1, // = 0x02;
          Stopping           : 1, // = 0x04;
          StandStill         : 1, // = 0x08;
          DiscreteMotion     : 1, // = 0x10;
          ContinuousMotion   : 1, // = 0x20;
          SynchronizedMotion : 1, // = 0x40;
          Homing             : 1, // = 0x80;
          reserved           : 24;
      };

      uint32_t operator=(const uint32_t &data)
      {
        status = data;
        return status;
      }

      std::string to_string(uint32_t idx) const
      {
        enum
        {
          ErrorStop,
          Disabled,
          Stopping,
          StandStill,
          DiscreteMotion,
          ContinuousMotion,
          SynchronizedMotion,
          Homing
        };

        switch (idx)
        {
          case ErrorStop:
            return std::string("ErrorStop");
          case Disabled:
            return std::string("Disabled");
          case Stopping:
            return std::string("Stopping");
          case StandStill:
            return std::string("StandStill");
          case DiscreteMotion:
            return std::string("DiscreteMotion");
          case ContinuousMotion:
            return std::string("ContinuousMotion");
          case SynchronizedMotion:
            return std::string("SynchronizedMotion");
          case Homing:
            return std::string("Homing");
          default:
            break;
        }
        return std::string("Unknown");
      }
      };
      // end of union status_t


      struct reg_st
      {
        std::array<udword_t, def_out_reg_size> outReg{};
        std::array<udword_t, def_in_reg_size>  inReg{};
      } m_deviceReg{};

      struct cfg_t
      {
        uint16_t master_idx{};

        uint32_t         begin_addr_in{};
        uint32_t         begin_addr_out{};
        hw_io::io_type_e io_type{};

        cfg_t()  = default;
        ~cfg_t() = default;

        cfg_t(const cfg_t &other)            = default; // copy constructor
        cfg_t &operator=(const cfg_t &other) = default; // copy assignment
        cfg_t(cfg_t &&other)                 = default; // move constructor
        cfg_t &operator=(cfg_t &&other)      = default; // move assignment
      } m_cfg{};

      bool m_isInit{};
      bool m_IsConnected{};

      std::thread       m_trd{};
      std::atomic<bool> m_stopThread{};
      std::mutex        m_mutex{};


      bool m_isMasterRun{}; // master board가 동작 중이면 true;
      using device_ids_t = std::vector<uint16_t>;
      using flag_t       = std::array<bool, max_master_board_cnt>;

      struct info_state_t
      {
        enum
        {
          MasterStop,
          MasterStop_Ready,
          MasterRun,
          AllDeviceChangeOP_Ready,
          RunCplt = 10
        };

        device_ids_t                          _ids{};                                    //MasterMap : 설치된 master 보드 ID List
        std::vector<device_ids_t>             _has_slave_ids{};                          // master bd의 slave 보드 ID List
        std::vector<uint8_t>                  _curr_modes{};                             // master bd의 현재 모드
        flag_t                                _OP_flag{true, true, true, true};          //전체 Device OP Check Flag
        flag_t                                _cplt_flag{false, false, false, false};    //Board 별 Run 시퀀스 완료 Flag
        std::vector<int>                      _step{};                                   // 현재 실행 중인 단계
        int                                   _end_cnt{};                                // 모든 마스터 보드를 동작 시키기 위한 카운터
        std::array<int, max_master_board_cnt> _timeout_cnt{};                            // timeout 카운터
        flag_t                                _is_masterRun{false, false, false, false}; // master board run flag
      } m_ethcatDevicesInfo{};

      struct master_state_t
      {
        enum
        {
          MasterStop,
          MasterStop_Ready,
          MasterRun,
          AllDeviceChangeOP_Ready,
          RunCplt = 10
        };

        uint16_t     _id{};                //MasterMap : 설치된 master 보드 ID List
        device_ids_t _has_slave_ids{};     // master bd의 slave 보드 ID List
        uint8_t      _curr_modes{};        // master bd의 현재 모드
        bool         _OP_flag{true};       //전체 Device OP Check Flag
        bool         _cplt_flag{false};    //Board 별 Run 시퀀스 완료 Flag
        int          _step{};              // 현재 실행 중인 단계
        int          _timeout_cnt{};       // timeout 카운터
        bool         _is_masterRun{false}; // master board run flag
      }m_infoDriver{};


      apcr::Timer<10> m_timers{};
    public:
      // constructor
      mmc_eCatLib(const apcr::attribute_st &f);
      mmc_eCatLib() = default;
      // destructor
      ~mmc_eCatLib();

    public:
    public:
      /****************************************************
     *  overriding
     ****************************************************/
      bool write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...) override;

    public:
      /****************************************************
     *  IO interface  overriding
     ****************************************************/
      bool    IsOn(uint32_t addr) override;
      bool    IsOff(uint32_t addr) override;
      errno_t OutputOn(uint32_t addr) override;
      errno_t OutputOff(uint32_t addr) override;
      errno_t OutputToggle(uint32_t addr) override;
      errno_t GetData(void) override;
      errno_t GetSetIO(void);

      // MARK:Initialize
      /****************************************************
     *	func
     ****************************************************/

    public:
      errno_t Init(const cfg_t &cfg);

    private:

      errno_t get_master_info(void);
      errno_t search_devices(void);
      errno_t start_init_process(void);
      errno_t end_init_procecss(void);

      void    threadStop(void);
      void    thread_masterInit(void);
      void    thread_masterInitJob(void);
      void    threadRun(void);      
      void    threadJob(void);
      void    threadJob_masterRun(void);
      errno_t get_current_io_reg(void);
      errno_t getInputReg(void);
      errno_t getOutReg(void);
      void    getOutput(void);
      errno_t setOutputReg(uint32_t value);


    private:
      void close(void);
      bool reconnect(void);
      bool connect(void);

    public:
      bool             IsConnected(void);
      bool             Disconnect(void);
      void             GetOutput(void);
      void             GetInput(void);
      uint32_t         GetBeginAddrIN(void) const;
      uint32_t         GetBeginAddrOUT(void) const;
      hw_io::io_type_e GetIOType(void) const;
      uint32_t         get_in_tail_addr(void);
      uint32_t         get_out_tail_addr(void);
      std::string      get_info(void);

      //MARK::io
      /****************************************************
     *	func
     ****************************************************/

      // MARK:master Amp
      /****************************************************
     *	func - system API
     ****************************************************/

      errno_t InitAmp(sub_idx dev_idx);
      bool    isAmpFault(sub_idx dev_idx);
      errno_t ResetAmpFault(sub_idx dev_idx);
      bool    isMasterRunMode(void);
      bool checkNetwork(void);

      // MARK:axis status
      errno_t ReadStatus(sub_idx axis_idx);
      errno_t checkSubNetwork(sub_idx axis_idx);

      // MARK:axis func
      /****************************************************
     *	func
     ****************************************************/
    errno_t AmpOnOff(sub_idx axis_idx, bool enable);
    errno_t AmpReset(sub_idx axis_idx);

      errno_t EStop(double dec, sub_idx axis_idx);
      errno_t ResetEStop(double dec, sub_idx axis_idx);
      errno_t ServoOn(sub_idx axis_idx);
      errno_t ServoOff(sub_idx axis_idx);
      errno_t zeroset_absEncoder(sub_idx axis_idx);      
      // position 관련 Act position, Target position, Current Position   모두 셋팅 값으로 변경한다. 
      errno_t SetPosition(sub_idx axis_idx, double pos,bool is_relative, execution_mode_t exe_mode = execution_mode_t::exe_mode_Immediately); 
      bool    isHomeSensorDectected(sub_idx axis_idx);
      bool    isPosiLimitSensorDectected(sub_idx axis_idx);
      bool    isNegaLimitSensorDectected(sub_idx axis_idx);
      bool    isErrorStop(sub_idx axis_idx);
      // get_orgPriority(sub_idx axis_idx);
      bool        isServoOn(sub_idx axis_idx);
      errno_t     checkAxisExistArgument(sub_idx axis_idx);
      errno_t     checkAxisState(sub_idx axis_idx);
      std::string statusDescription(int err_code); //eMMC::MC_STATUS

      double getAxisActPosition(sub_idx axis_idx, double &ret_pos);
      double getAxisCurrPosition(sub_idx axis_idx);
      double getAxisCommandedPoition(sub_idx axis_idx, double &ret_pos);
      double getAxisCommandedVelocity(sub_idx axis_idx, double &ret_vel);
      double getAxisActVelocity(sub_idx axis_idx);
      errno_t getStatus(sub_idx axis_idx, uint32_t &ret_status);
      errno_t getMotionStatus(sub_idx axis_idx, uint32_t &ret_status);
      // errno_t getAxisInfo(sub_idx axis_idx, uint32_t &ret_info);
      errno_t getAxisStatus(sub_idx axis_idx, uint32_t &ret_status);
      errno_t moveAbsolute(sub_idx axis_idx, double pos, double vel, double acc, double dec, double jerk, direction_t dir, buffer_mode_t buffer_mode = buffer_mode_t::buffMode_Aborting);
      errno_t moveRelative(sub_idx axis_idx, double dist, double vel, double acc, double dec, double jerk, buffer_mode_t buffer_mode = buffer_mode_t::buffMode_Aborting);
      errno_t moveVelocity(sub_idx axis_idx, double vel, double acc, double dec, double jerk, direction_t dir, buffer_mode_t buffer_mode = buffer_mode_t::buffMode_Aborting);
      errno_t moveHome(sub_idx axis_idx, double pos, buffer_mode_t buffer_mode = buffer_mode_t::buffMode_Aborting);
      errno_t moveStop(sub_idx axis_idx, bool is_execute, double dec, double jerk);
      errno_t halt(sub_idx axis_idx, double dec, double jerk, buffer_mode_t buffer_mode = buffer_mode_t::buffMode_Aborting);

      errno_t getAxisDigitalInput(sub_idx axis_idx, uint32_t &value_bit);


    
      template<typename ParamValueType>
				requires mcc_param_type_c<ParamValueType>
      errno_t getParameter(sub_idx axis_idx, param_t param_idx, ParamValueType &value);



      template<typename ParamValueType>
        requires mcc_param_type_c<ParamValueType>
      errno_t setParameter(sub_idx axis_idx, param_t param_idx, ParamValueType value);








///////////////////////////////////////////////////////////////////////////////////////
#ifdef _USE_HW_CLI
    /// @brief test_callback
    /// @param argc
    /// @param argv
    /// @return
    int testCB_func(int argc, char *argv[]);
#endif
    };

    
#endif 
//////////////////////////////////////

}
// end of namespace hw_lib



#endif //_LIB_MMC_LIB_HPP_


       //MARK:API list
/*

//============================================================================
//                              > APIs <
//----------------------------------------------------------------------------

MC_Init(void);
MC_MasterInit(UINT16 MasterID);
MC_MasterRUN(UINT16 MasterID);
MC_MasterSTOP(UINT16 MasterID);
MC_GetSWVersion(UINT16 Type, UINT16 *Major, UINT16 *Minor);
MC_GetErrorMessage(UINT32 ErrorCode, UINT32 Size, char *ErrorMessage);
FileDownloadCB(
    UINT16 BoardID,
    LPVOID lpCallbackFunc, // Callback Function
    int    nFileType,
    char  *sNvsFileName,
    char  *sBinFileName);

//===========================================================================
// Master Utility APIs
//---------------------------------------------------------------------------
//
FileMasterFWDownloadCB(
    UINT16 BoardID,
    LPVOID lpCallbackFunc,
    int    nFileType,
    char  *sNvsFileName,
    char  *sBinFileName);
FileDownload(
    UINT16 BoardID,
    int    nFileType,
    char  *sNvsFileName,
    char  *sBinFileName,
    int    nMsgSize,
    char  *szMsg);
MC_GetMasterMap(
    UINT16  MasterMap[MAX_BOARD_CNT],
    UINT16 *MasterCount);
MC_GetMasterCount(UINT16 *MasterCount);
MC_GetBoardScanNo(UINT16  BoardID, UINT16 *BoardScanNo);
MasterGetSDOData(
    UINT16  BoardID,
    UINT16  SlaveNo,
    UINT16  SDOIndex,
    UINT8   SubIndex,
    UINT32  DataSize,
    UINT32 *respDataSize,
    UINT8  *bDataArray);
MasterGetSDODataEcatAddr(
    UINT16  BoardID,
    UINT16  EcatAddr,
    UINT16  SDOIndex,
    UINT8   SubIndex,
    UINT32  DataSize,
    UINT32 *respDataSize,
    UINT8  *bDataArray);
MasterSetSDOData(
    UINT16  BoardID,
    UINT16  SlaveNo,
    UINT16  SDOIndex,
    UINT8   SubIndex,
    UINT32  DataSize,
    UINT32 *respDataSize,
    UINT8  *bDataArray);
MasterSetSDODataEcatAddr(
    UINT16  BoardID,
    UINT16  EcatAddr,
    UINT16  SDOIndex,
    UINT8   SubIndex,
    UINT32  DataSize,
    UINT32 *respDataSize,
    UINT8  *bDataArray);
MasterGetCurMode(UINT16 BoardID, UINT8 *MasterMode);
MasterGetBoardID(UINT16 MasterScanNo, UINT16 *MasterID);
MasterGetOSRevision(UINT16 BoardID, UINT8 *Major, UINT8 *Minor);
MasterGetMCRevision(UINT16 BoardID, UINT8 *Major, UINT8 *Minor);
MC_Power(
    UINT16 BoardID, // BoardID
    UINT16 AxisID,  //Axis number
    bool   Enable   // Axis enable : 0:Disable, 1:Enable
  );
MC_MoveAbsolute(
    UINT16         BoardID,   // BoardID
    UINT16         AxisID,    //Axis number
    DOUBLE         Position,  //Target Position
    DOUBLE         Velocity,  // Max Velocity
    DOUBLE         Accel,     // Max Acceleration
    DOUBLE         Decel,     // Max Deceleration
    DOUBLE         Jerk,      //Max Jerk
    MC_DIRECTION   Dir,       // 0:Positive Direction, 1:Shortest_way, 2:Negative_Direction, 3:Current_Direction
    MC_BUFFER_MODE BufferMode //Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
  );
MC_MoveRelative(
    UINT16         BoardID,   // BoardID
    UINT16         AxisID,    //Axis number
    DOUBLE         Distance,  //Distance
    DOUBLE         Velocity,  // Max Velocity
    DOUBLE         Accel,     // Max Acceleration
    DOUBLE         Decel,     // Max Deceleration
    DOUBLE         Jerk,      //Max Jerk
    MC_BUFFER_MODE BufferMode //Buffer Mode : 0:Aborting, 1:Buffered, 2:BlendingLow, 3:BlendingPrevious, 4:BlendingNext, 5:BlendingHigh
  );
MC_MoveVelocity(
    UINT16         BoardID,   // BoardID
    UINT16         AxisID,    //Axis number
    DOUBLE         Velocity,  // Max Velocity
    DOUBLE         Accel,     // Max Acceleration
    DOUBLE         Decel,     // Max Deceleration
    DOUBLE         Jerk,      //Max Jerk
    MC_DIRECTION   Dir,       // 1: Positive_Direction, 3:Negative_Direction, 4:Current_Direction
    MC_BUFFER_MODE BufferMode //Buffer Mode : 0:Aborting, 1:Buffered
  );
MC_Home(
    UINT16         BoardID,  // BoardID
    UINT16         AxisID,   // Axis number
    DOUBLE         Position, //
    MC_BUFFER_MODE BufferMode);
MC_Stop(
    UINT16 BoardID, // BoardID
    UINT16 AxisID,  //Axis number
    bool   Execute,
    DOUBLE Decel,   // Max Deceleration
    DOUBLE Jerk     //Max Jerk
  );
MC_ReadStatus(
    UINT16  BoardID, // BoardID
    UINT16  AxisID,  //Axis number
    UINT32 *pStatus  //MC_Status
  );
MC_ReadAxisError(
    UINT16  BoardID, // BoardID
    UINT16  AxisID,  //Axis number
    UINT16 *pErrorID,
    UINT16 *pErrorInfo,
    UINT16 *pErrorInfoExt);
MC_Reset(
    UINT16 BoardID, // BoardID
    UINT16 AxisID   //Axis number
  );
MC_ReadParameter(
    UINT16  BoardID,      // BoardID
    UINT16  AxisID,       //Axis number,
    UINT32  ParameterNum, //PN
    DOUBLE *pValue);
MC_ReadBoolParameter(
    UINT16 BoardID,      // BoardID
    UINT16 AxisID,       //Axis number,
    UINT32 ParameterNum, //PN
    bool  *pValue);
MC_ReadIntParameter(
    UINT16  BoardID,      // BoardID
    UINT16  AxisID,       //Axis number,
    UINT32  ParameterNum, //PN
    UINT32 *pValue);
MC_WriteParameter(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 ParameterNum,
    DOUBLE dValue);
MC_WriteBoolParameter(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 ParameterNum,
    bool   Value);
MC_WriteIntParameter(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 ParameterNum,
    UINT32 dwValue);
MC_GearIn(
    UINT16         BoardID,
    UINT16         MasterAxisID,
    UINT16         SlaveAxisID,
    UINT32         RatioNumerator,
    UINT32         RatioDenominator,
    MC_SOURCE      MasterValueSource,
    DOUBLE         Acceleration,
    DOUBLE         Deceleration,
    DOUBLE         Jerk,
    MC_BUFFER_MODE BufferMode);
MC_GearOut(
    UINT16 BoardID,
    UINT16 SlaveAxisID);
MC_TouchProbe(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 TriggerInput,
    bool   WindowOnly,
    DOUBLE FirstPosition,
    DOUBLE LastPosition);
MC_AbortTrigger(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 TriggerInput);
MC_ReadDigitalInput(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 InputNumber,
    bool  *pValue);
MC_ReadDigitalOutput(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 OutputNumber,
    bool  *pValue);
MC_WriteDigitalOutput(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT32 OutputNumber,
    bool   Value);
MC_SetPosition(
    UINT16            BoardID, //BoardID
    UINT16            AxisID,  //Axis number
    DOUBLE            Position,
    bool              Relative,
    MC_EXECUTION_MODE Mode);
MC_ReadActualVelocity(
    UINT16  BoardID,  // BoardID
    UINT16  AxisID,   //Axis number
    DOUBLE *pVelocity ); // Velocity Acutal Value 
MC_Halt(
    UINT16         BoardID,
    UINT16         AxisID,
    DOUBLE         Deceleration,
    DOUBLE         Jerk,
    MC_BUFFER_MODE BufferMode);
MC_TriggerMonitor(
    UINT16  BoardID,
    UINT16  AxisID,
    UINT32  TriggerInput,
    bool   *pDone,
    DOUBLE *pRecordedPosition,
    bool   *pProbeActive);
MC_ReadMotionState(
    UINT16  BoardID,     // BoardID
    UINT16  AxisID,      //Axis number
    UINT32 *pMotionState); // MC_MOTIONSTATE  
MC_ReadAxisInfo(
    UINT16  BoardID,  // BoardID
    UINT16  AxisID,   //Axis number
    UINT32 *pAxisInfo  );//MC_AXISINFO 
MC_ReadAxisStatus(
    UINT16  BoardID,    // BoardID
    UINT16  AxisID,     //Axis number
    UINT32 *pAxisStatus );//MC_AXISSTATUS 
MC_GearMonitor(
    UINT16  BoardID,
    UINT16  AxisID,
    UINT16 *pStatus);
MC_ReadProfileData(
    UINT16  BoardID,
    UINT16  AxisID,
    UINT32 *TickCount,
    DOUBLE *Position,
    DOUBLE *Velocity,
    DOUBLE *Accel,
    DOUBLE *Jerk,
    DOUBLE *ActPos,
    DOUBLE *ActVel);

//MARK:Group motion
//===========================================================================
//GROUP Motion
//---------------------------------------------------------------------------   
MC_AddAxisToGroup(
    UINT16 BoardID,
    UINT16 AxisID,
    UINT16 AxesGroupNo,
    UINT16 IDInGroup);
MC_RemoveAxisFromGroup(
    UINT16 BoardID,
    UINT16 AxesGroupNo,
    UINT16 IDInGroup);
MC_UngroupAllAxes(
    UINT16 BoardID,
    UINT16 AxesGroupNo);
MC_GroupReadConfiguration(
    UINT16         BoardID,
    UINT16         AxesGroupNo,
    UINT16         IDInGroup,
    MC_COORDSYSTEM CoordSystem,
    UINT16        *AxisNo);
MC_GroupEnable(
    UINT16 BoardID,
    UINT16 AxesGroupNo);
MC_GroupDisable(
    UINT16 BoardID,
    UINT16 AxesGroupNo);
MC_MoveLinearAbsolute(
    UINT16             BoardID,
    UINT16             AxesGroupNo,
    UINT16             PositionCount,
    DOUBLE            *PositionData,
    DOUBLE             Velocity,
    DOUBLE             Acceleration,
    DOUBLE             Deceleration,
    DOUBLE             Jerk,
    MC_COORDSYSTEM     CoordSystem,              //Coordination System �� �����Ѵ�. //ACS, MCS, PCS
    MC_BUFFER_MODE     BufferMode,               //Buffer Mode �� �����Ѵ�.
    MC_TRANSITION_MODE TransitionMode,           //Transition Mode �� �����Ѵ�.
    UINT16             TransitionParameterCount, //TransitionParameter �� Count �� �����Ѵ�.
    DOUBLE            *TransitionParameter       //TransitionParameterCount �� ���õ� ũ�� ��ŭ�� Data �� �Է��Ѵ�.
  );
MC_GroupHalt(
    UINT16         BoardID,
    UINT16         AxesGroupNo,
    DOUBLE         Deceleration,
    DOUBLE         Jerk,
    MC_BUFFER_MODE BufferMode //Buffer Mode �� �����Ѵ�.
  );
MC_GroupStop(
    UINT16 BoardID,
    UINT16 AxesGroupNo,
    bool   Execute,
    DOUBLE Deceleration,
    DOUBLE Jerk);
MC_MoveCircularAbsolute2D(
    UINT16             BoardID,
    UINT16             AxesGroupNo,
    MC_CIRC_MODE       CircMode,
    MC_CIRC_PATHCHOICE PathChoice,
    DOUBLE             AuxPoint[2],
    DOUBLE             EndPoint[2],
    DOUBLE             Angle,
    DOUBLE             Velocity,
    DOUBLE             Acceleration,
    DOUBLE             Deceleration,
    DOUBLE             Jerk,
    MC_COORDSYSTEM     CordSystem,
    MC_BUFFER_MODE     BufferMode,
    MC_TRANSITION_MODE TransitionMode,
    UINT16             TransitionParamCount,
    DOUBLE            *TransitionParameter);
MC_GroupReadStatus(
    UINT16  BoardID,
    UINT16  AxesGroupNo,
    UINT32 *pGroupStatus);
MC_GroupReadError(
    UINT16  BoardID,
    UINT16  AxesGroupNo,
    UINT16 *pErrorID,
    UINT16 *pErrorInfo0,
    UINT16 *pErrorInfo1);
MC_GroupReset(
    UINT16 BoardID,
    UINT16 AxesGroupNo);
MC_GroupReadProfileData(
    UINT16  BoardID,
    UINT16  AxesGroupNo,
    UINT16 *AxisCount,
    UINT32 *TimeTick,                                                   //msec
    DOUBLE  ProfileDataArray[MAX_AXES_IN_GROUP][MAX_PROFILE_ITEM_COUNT] //[0]Pos,Vel,Acc,Jerk,ActPos,ActVel,[1]Pos,Vel,Acc,Jerk,ActPos,ActVel ...
  );
MC_GroupReadInfo(
    UINT16  BoardID,
    UINT16  AxesGroupNo,
    UINT16 *LastIdentNum,
    UINT16  IdentAxisNumList[MAX_AXES_IN_GROUP]);
MC_ReadAllStatus(
    UINT16 BoardID,
    UINT32 StatusData[MAX_ALL_STATUS_SIZE]);
MC_GroupSetRawDataMode(
    UINT16           BoardID,
    UINT16           AxesGroupNo,
    MC_RAW_DATA_MODE Mode,
    bool             Enable,        // Raw data mode enable : 0:Disable, 1:Enable
    UINT8            ReservedZero1, // MC_COORDSYSTEM CoordSystem,
    UINT8            ReservedZero2  // MC_BUFFER_MODE BufferMode
  );
MC_GroupReadRawDataStatus(
    UINT16                     BoardID,
    UINT16                     AxesGroupNo,
    MC_AxesGroupRawDataStatus *AxesGroupRawDataStatus);
MC_GroupClearRawData(
    UINT16 BoardID,
    UINT16 AxesGroupNo,
    UINT16 ReservedZero1,
    UINT16 ReservedZero2);
MC_GroupSetRawData(
    UINT16       BoardID,
    UINT16       AxesGroupNo,
    MC_SAVE_MODE SaveMode,
    UINT16       IndexNum,
    UINT16       RawDataSize,
    UINT16       RawDataCount,
    UINT8       *RawDataArray,
    UINT16      *StoredNum);
MC_ReadRemainBuffer(
    UINT16  BoardID,
    UINT16  AxesNo,
    UINT32 *BufferSize);
MC_GroupReadRemainBuffer(
    UINT16  BoardID,
    UINT16  AxesGroupNo,
    UINT32 *BufferSize);

//MARK: motion
//===========================================================================
// Motion APIs Add
//---------------------------------------------------------------------------

MC_MoveAbsoluteMultiAxis(
    UINT16        BoardID,        //Board ID
    UINT16        AxisCount,      //Number of Axis
    UINT16       *AxisArray,      //Array of Axis ID
    DOUBLE       *PositionArray,  //Target Position Array
    DOUBLE        Velocity,       //Max Velocity
    DOUBLE        Acceleration,   //Max Acceleration
    DOUBLE        Deceleration,   //Max Deceleration
    DOUBLE        Jerk,           //Max Jerk
    MC_DIRECTION *DirectionArray, //Array of Moving Direction for each Axis(0,1,2,3)
    UINT8         ErrorStopMode   //Mode of multi-axes Motion Stop when Error occurred in one(or more) of multi-axes.
  );                              //0:Remainder axes do not Stop (Continue their Motion), 1:Remainder axes also Stop with E-Stop Parameters.

MC_MoveRelativeMultiAxis(
    UINT16  BoardID,       //Board ID
    UINT16  AxisCount,     //Number of Axis
    UINT16 *AxisArray,     //Array of Axis ID
    DOUBLE *PositionArray, //Target Position Array
    DOUBLE  Velocity,      //Max Velocity
    DOUBLE  Acceleration,  //Max Acceleration
    DOUBLE  Deceleration,  //Max Deceleration
    DOUBLE  Jerk,          //Max Jerk
    UINT8   ErrorStopMode  //Mode of multi-axes Motion Stop when Error occurred in one(or more) of multi-axes.
  );                       //0:Remainder axes do not Stop (Continue their Motion), 1:Remainder axes also Stop with E-Stop Parameters.

MC_HaltMultiAxis(
    UINT16  BoardID,      //Board ID
    UINT16  AxisCount,    //Number of Axis
    UINT16 *AxisArray,    //Array of Axis ID
    UINT8   ErrorStopMode //Mode of multi-axes Motion Stop when Error occurred in one(or more) of multi-axes.
  );                      //0:Remainder axes do not Stop (Continue their Motion), 1:Remainder axes also Stop with E-Stop Parameters.

MC_SetHomeFlag(
    UINT16 BoardID, //BoardID
    UINT16 AxisID,  //Axis number
    UINT16 EcatAddr //Ethercat Address
  );

MC_GetHomeFlag(
    UINT16  BoardID,  //BoardID
    UINT16  AxisID,   //Axis number
    UINT32 *pHomeFlag //Home Flag
  );

MC_ModeChange(
    UINT16 BoardID,     //BoardID
    UINT16 AxisID,      //Axis number
    UINT8  Mode,        //Mode(6:hm,8:csp)
    UINT8  ReservedZero //MC_BUFFER_MODE BufferMode (Reserved)
  );

MC_SlaveHomeSet(
    UINT16 BoardID,      //BoardID
    UINT16 EcatAddr,     //Ethercat Address
    INT32  Offset,       //Home Offset
    INT8   Method,       //Method
    UINT32 SpeedSwitch,  //Speed during search for switch
    UINT32 SpeedZero,    //Speed during search for zero
    UINT32 Acceleration, //Acceleration
    UINT8  ReservedZero  //MC_BUFFER_MODE BufferMode (Reserved)
  );

MC_SlaveHome(
    UINT16 BoardID,     //BoardID
    UINT16 AxisID,      //Axis number
    UINT8  Start,       //0:Stop,1:Start
    UINT8  ReservedZero //MC_BUFFER_MODE BufferMode (Reserved)
  );
MC_SlaveHomeHalt(
    UINT16 BoardID,     //BoardID
    UINT16 AxisID,      //Axis number
    UINT8  ReservedZero //MC_BUFFER_MODE BufferMode (Reserved)
  );
MC_ReadSlaveHomeStatus(
    UINT16  BoardID,   //BoardID
    UINT16  AxisID,    //Axis number
    UINT32 *HomeStatus //Response Data(Bit0:Homing Complete, Bit1:Homing Error)
  );
MC_ReadSlaveModeStatus(
    UINT16 BoardID,   //BoardID
    UINT16 AxisID,    //Axis number
    UINT8 *ModeStatus //Response Data(6:hm,8:csp)
  );
MC_ReadMultiAxisStatus(
    UINT16  BoardID,   //Board ID
    UINT16  AxisCount, //Number of Axis
    UINT16 *AxisArray, //Array of Axis ID
    UINT32 *Status     //Axis Status
  );

MC_ReadCommandedPosition(
    UINT16  BoardID,  //BoardID
    UINT16  AxisID,   //Axis number
    DOUBLE *pPosition //Commmanded Position Value
  );
MC_ReadCommandedVelocity(
    UINT16  BoardID,  //BoardID
    UINT16  AxisID,   //Axis number
    DOUBLE *pVelocity //Commmanded Velocity Value
  );
//===========================================================================


//MARK: IO Operating
//---------------------------------------------------------------------------
// Device IO APIs
//---------------------------------------------------------------------------
//MC_GetIOState

MC_GetIOState(
    UINT16  BoardID,
    UINT32  type,
    UINT32 *State);

MC_IO_RAW_WRITE(
    UINT16 BoardID,
    UINT32 Offset,
    UINT32 Size,
    UINT8 *DataArray);
MC_IO_WRITE(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT32 Offset,
    UINT32 Size,
    UINT8 *DataArray);
MC_IO_WRITE_BIT(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT32 Offset,
    UINT8  bitOffset,
    bool   data);
MC_IO_RAW_WRITE_BIT(
    UINT16 BoardID,
    UINT32 Offset,
    UINT8  bitOffset,
    bool   data);
MC_IO_WRITE_BYTE(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT32 Offset,
    BYTE   data);
MC_IO_WRITE_WORD(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT32 Offset,
    WORD   data);
MC_IO_WRITE_DWORD(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT32 Offset,
    DWORD  data);
MC_IO_READ(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT16 BufferInOut,
    UINT32 Offset,
    UINT32 Size,
    UINT8 *DataArray);
MC_IO_RAW_READ(
    UINT16 BoardID,
    UINT16 BufferInOut,
    UINT32 Offset,
    UINT32 Size,
    UINT8 *DataArray);
MC_IO_READ_BIT(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT16 BufferInOut,
    UINT32 Offset,
    UINT8  BitOffset,
    bool  *data);
MC_IO_RAW_READ_BIT(
    UINT16 BoardID,
    UINT16 BufferInOut,
    UINT32 Offset,
    UINT8  BitOffset,
    bool  *data);
MC_IO_READ_BYTE(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT16 BufferInOut,
    UINT32 Offset,
    BYTE  *data);
MC_IO_READ_WORD(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT16 BufferInOut,
    UINT32 Offset,
    WORD  *data);
MC_IO_READ_DWORD(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT16 BufferInOut,
    UINT32 Offset,
    DWORD *data);
NMCReadErrorInfo(
    UINT16  BoardID,
    UINT32 *pSequenceNo,
    UINT32 *pErrorCode,
    UINT8   ExtErrorInfo[6],
    UINT32 *RemainErrorCount);
MasterGetLastError(
    UINT16  BoardID,
    UINT32 *pSequenceNo,
    UINT32 *pErrorCode,
    UINT8   ExtErrorInfo[6]);
MasterClearError(
    UINT16 BoardID);
MasterGetAxesCount(
    UINT16  BoardID,
    UINT32 *TotalAxisCount);
MasterGetAxesID(
    UINT16 BoardID,
    UINT16 AxisID[MAX_LOGICAL_AXIS_COUNT]);
MasterGetDeviceCount(
    UINT16  BoardID,
    UINT32 *TotalDeviceCount);
MasterGetDeviceID(
    UINT16 BoardID,
    UINT16 DeviceID[MAX_LOGICAL_DEVICE_COUNT]);
SlaveGetAliasNo(
    UINT16  BoardID,
    UINT16  EcatAddr,
    UINT16 *AliasID);
SlaveSetAliasNo(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT16 AliasID);
SlaveGetCurState(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT8 *data);
MasterECatDirectAccess(
    UINT16  BoardID,
    UINT8   EcatCmd,
    UINT16  Adp,
    UINT16  Ado,
    UINT16  ReqDataSize,
    UINT8  *bReqDataArray,
    UINT16 *RespDataSize,
    UINT8  *bRespDataArray,
    UINT16 *WC);
ReadET1100EEPROMWORD(
    UINT16  BoardID,
    UINT16  EcatAddr,
    UINT32  Addr,
    UINT16 *wData,
    UINT32  TIMEOUT);
WriteET1100EEPROMWORD(
    UINT16 BoardID,
    UINT16 EcatAddr,
    UINT32 Addr,
    UINT16 wData,
    UINT32 TIMEOUT);

//===========================================================================
//---------------------------------------------------------------------------
// System Performance APIs
//---------------------------------------------------------------------------

     MC_SetSystemPerformance(UINT16 Level);


*/


//---------------------------------------------------------------------------
// Parameters
//---------------------------------------------------------------------------
/*

PN    Name                      Datatype          R/W           Comments
1     CommandedPosition         DOUBLE            R             Commanded position
2     SWLimitPos                DOUBLE            R/W           Positive Software limit switch position
3     SWLimitNeg                DOUBLE            R/W           Negative Software limit switch position
4     EnableLimitPos            Bool              R/W           Enable positive software limit switch
5     EnableLimitNeg            Bool              R/W           Enable negative software limit switch
6     EnablePosLagMonitoring    Bool              R/W           Enable monitoring of position lag
7     MaxPositionLag            DOUBLE            R/W           Maximal position lag
8     MaxVelocitySystem         DOUBLE            R             Maximal allowed velocity of the axis in the motion system
9     MaxVelocityAppl           DOUBLE            R/W           Maximal allowed velocity of the axis in the application
10    ActualVelocity            DOUBLE            R             Actual velocity
11    CommandedVelocity         DOUBLE            R             Commanded velocity
12    MaxAccelerationSystem     DOUBLE            R             Maximal allowed acceleration of the axis in the motion system
13    MaxAccelerationAppl       DOUBLE            R/W           Maximal allowed acceleration of the axis in the application
14    MaxDecelerationSystem     DOUBLE            R             Maximal allowed deceleration of the axis in the motion system
15    MaxDecelerationAppl       DOUBLE            R/W           Maximal allowed deceleration of the axis in the application
16    MaxJerkSystem             DOUBLE            R             Maximum allowed jerk of the axis in the motion system
17    MaxJerkAppl               DOUBLE            R/W           Maximum allowed jerk of the axis in the application
1000  ActualPostion             DOUBLE            R             Actual Position
1001  CommandedAccel            DOUBLE            R             Commanded Acceleration
1002  ActualAccel               DOUBLE            R             Actual Acceleration
1003  CommandedJerk             DOUBLE            R             Commanded Jerk
1004  ActualJerk                DOUBLE            R             Actual Jerk
1010  TotalBufferCount          UINT32            R             Total Motion Buffer Count
1011  AvailableBufferCount      UINT32            R             Available Motion Buffer Count
2002  AxisType                  UINT32            R             Axis Type
2003  ModuloAxis                Bool              R             Modulo Axis
2004  ModuloValue               DOUBLE            R             Modulo Value
2010  EnableHWLimitPo           Bool              R             Pos. Limit Switch Enable
2011  HWLimitPosInputNum        UINT32            R             Pos. Limit Switch Input Num
2012  HWLimitPosActLevel        Bool              R             Pos. Limit Switch Active Level
2013  EnableHWLimitNeg          Bool              R             Neg. Limit Switch Enable
2014  HWLimitNegInputNum        UINT32            R             Neg. Limit Switch Input Num
2015  HWLimitNegActLevel        Bool              R             Neg. Limit Switch Active Level
2016  HomeInputNum              UINT32            R             Home Sensor Input Num
2017  HomeActLevel              Bool              R             Home Sensor Active Level
2018  MarkerInputNum            UINT32            R             Z-Phase Input Num
2019  MarkerActLevel            Bool              R             Z-Phase Input Active Level
2020  InputActLevel             UINT32            R             Input Active Level Setting
2030  EnableLimitPos            Bool              R/W           Software Position Limit Pos. Enable
2031  SWLimitPos                DOUBLE            R/W           Software Position Limit Pos.
2032  EnableLimitNeg            Bool              R/W           Software Position Limit Neg. Enable
2033  SWLimitNeg                DOUBLE            R/W           Software Position Limit Neg.
2034  MaxVelocityAppl           DOUBLE            R/W           Max. Velocity Application
2035  MaxAccelAppl              DOUBLE            R/W           Max. Accel Application
2036  MaxDecelAppl              DOUBLE            R/W           Max. Decel Application
2037  MaxJerkAppl               DOUBLE            R/W           Max. Jerk Application
2038  MaxVelocitySystem         DOUBLE            R             Max. Velocity System
2039  MaxAccelerationSystem     DOUBLE            R             Max. Accel System
2040  MaxDecelerationSystem     DOUBLE            R             Max. Decel System
2041  MaxJerkSystem             DOUBLE            R             Max. Jerk System
2060  EStopType                 UINT32            R/W           E-Stop Type
2061  EStopDecel                DOUBLE            R/W           E-Stop Decel
2062  EStopJerk                 DOUBLE            R/W           E-Stop Jerk
2070  InvertCmdDir              Bool              R             Invert Command Direction
2071  CmdScaleFactor            DOUBLE            R             Command Data Unit Scale Factor
2072  FeedbackMode              UINT32            R             Feedback Mode
2073  InvertFeedbackDir         Bool              R             Invert Position Feedback Direction
2074  FeedbackScaleFactor       DOUBLE            R             Feedback Data Unit Scale Factor
2075  PositionFeedbackFilter    DOUBLE            R/W           Feedback Filter Time Position
2076  VelocityFeedbackFilter    DOUBLE            R/W           Feedback Filter Time Velocity
2077  AccelerationFeedbackFilterDOUBLE            R/W           Feedback Filter Time Accel
2078  StartVelocityOffset       DOUBLE            R/W           Start Velocity Offset
2079  StopVelocityOffset        DOUBLE            R/W           Stop Velocity Offset
2080  InPositionCheckType       UINT32            R/W           In-Position Check Type
2081  InPositionWindowSize      DOUBLE            R/W           In-Position Window Size
2082  InVelocityWindowSize      DOUBLE            R/W           In-Velocity Window Size
2083  EnablePositionLagMonit    Bool              R/W           Enable Position Lag Monitoring
2084  MaxPositionLag            DOUBLE            R/W           Max Position Lag
2085  PositionLagCalMethod      UINT32            R             Position Lag Calculation
2086  EnableVelocityLagMonit    Bool              R/W           Enable Velocity Lag Monitoring
2087  MaxVelocityLag            DOUBLE            R/W           Max Velocity Lag
2088  VelocityLagCalMethod      UINT32            R             Velocity Lag Calculation
2100  HomingType                UINT32            R             Homing Type
2101  HomingDir                 UINT32            R             Homing Direction
2102  HomingVelocity            DOUBLE            R/W           Homing Velocity
2103  HomingAcceleration        DOUBLE            R/W           Homing Accel
2104  HomingDeceleration        DOUBLE            R/W           Homing Decel
2105  HomingJerk                DOUBLE            R/W           Homing Jerk
2106  HomingCreepVelocity       DOUBLE            R/W           Homing Creep Vel
2107  HomePositionOffset        DOUBLE            R/W           Home Position Offset Value
2108  HomeCompleteFlagHandle    UINT32            R/W           Home Complete Handling Mode




*/

//#ifdef __TEMP__USE_HW_LIB_MMC_ETHERCAT