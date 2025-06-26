/**
 * @file ap.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-04-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*



*/

#include "ap.hpp"

/****************************************************
  0. ap static functions
 ****************************************************/
static void apISR_100ms(void *arg1, void *arg2);
static void apISR_50ms(void *arg1, void *arg2);

static void de_init();
static void ap_thread_run();
static void ap_thread_stop();

static errno_t register_sys_information();
static errno_t initHwComponents();
static errno_t initComm_socketOpen();

static errno_t createControl_apis();
static errno_t createControl_viewLoger();

enum trd_id
{
  trd_cnJob,
  trd_enAxis,
  trd_2,
  trd_3,
  trd_4,
  trd_max
};

std::array<std::thread, trd_max>       threads{};
std::array<std::atomic<bool>, trd_max> trd_stops{ };
static void trd_job();


/****************************************************
 *	log
 ****************************************************/
 // initializer static members
uint32_t TinyC::mcc_log::trace_prc_pre_tick = 0;
std::mutex TinyC::mcc_log::log_mutex{};
TinyC::Que<TinyC::mcc_log::dat_st> TinyC::mcc_log::log_table[mcc_log::lvl_max]
  = {TinyC::Que<mcc_log::dat_st>(4096), TinyC::Que<mcc_log::dat_st>(4096), TinyC::Que<mcc_log::dat_st>(4096)};


/****************************************************
 *	cli
 ****************************************************/
// initializer static memvers
using callbacks_t = std::map<std::string, std::function<int(int, char **)>>;

std::thread       TinyC::Cli::task_trd{};
std::atomic<bool> TinyC::Cli::trd_life{};
HANDLE            TinyC::Cli::hConsole = nullptr;
std::string       TinyC::Cli::c_input{};
callbacks_t       TinyC::Cli::callbacks{};

/////////////////////////////////////////////////////

/****************************************************
 * ap components
 ****************************************************/
tim::timer period_timer_100ms;
tim::timer period_timer_50ms;


//-------------------------------------------------
//  manager instance
//-------------------------------------------------
apcr::vlog_t                 ap_log{};

//-------------------------------------------------
//  Data
//-------------------------------------------------
std::shared_ptr<ap_data::appConf>  cfg_data{};

apcr::app_data_t mcu_data{};
//--------------------------------------------------------
//MARK: variable write register
//--------------------------------------------------------
static udword_t IOY_value{}; // output value array

/****************************************************
  0. hw instances
 ****************************************************/
apcr::nets_t nets{};
apcr::ucnets_t ucnets{};
apcr::uarts_t uarts{};

/****************************************************
  1. ap instances
 ****************************************************/
apcr::pc_api_t comm_pc{};

//MARK:init
/****************************************************
 ****************************************************/
errno_t apcr::init(void)
{
  REPORT_OUT("[prc] core ap initialize");
  /****************************************************
	 * initilize variable
	 ****************************************************/
    for (auto& stop : trd_stops) 
        stop.store(true);

    /****************************************************
	 *	data object
	 ****************************************************/
    cfg_data = std::make_shared<ap_data::appConf>(ap_data::check_make_app_cfg_data(ap_data::def_app_cfg_file_name));


    register_sys_information();

    initHwComponents();

    initComm_socketOpen();

    createControl_apis();

    createControl_viewLoger();

#ifdef _USE_HW_CLI
    TinyC::Cli::registerCallback("ap_log", std::bind(&apcr::vLog::testCB_func, ap_log.get(), std::placeholders::_1, std::placeholders::_2));
    TinyC::Cli::registerCallback(comm_pc->attribute.obj_name, std::bind(&apcr::api_cmd::testCB_func, comm_pc.get(), std::placeholders::_1, std::placeholders::_2));
#endif

    return ERROR_SUCCESS;
}



//MARK:init
/****************************************************
 ****************************************************/
void apcr::main(void)
{
    /****************************************************
	 *	thread start
	 ****************************************************/

  period_timer_100ms.start(std::chrono::milliseconds(100), []()
                          { apISR_100ms(nullptr, nullptr); });

  period_timer_50ms.start(std::chrono::milliseconds(50), []()
                          { apISR_50ms(nullptr, nullptr); });

  ap_log->StartThread();
  REPORT_OUT("[prc] main, thread_start");

  ap_thread_run();

}



//MARK:init
/****************************************************
 ****************************************************/
void apcr::exit(void)
{
  REPORT_OUT("[prc] exit, thread_stop");

  ap_thread_stop();
}


std::shared_ptr<apcr::uart_t> apcr::get_uart(INST_IDX::_SERIAL idx)
{
  return uarts[idx];
}

ap_data::appConf *apcr::get_cfg(void)
{
  return  cfg_data.get();
}

apcr::pc_api_t apcr::get_api()
{
  return comm_pc;
}

apcr::vlog_t apcr::get_vlog()
{
  return ap_log;
}

udword_t &apcr::get_ioY()
{
  return IOY_value;
}
std::shared_ptr<apcr::net_tcp_server_t> apcr::get_net(INST_IDX::_SOCKET idx)
{
  return nets[idx];
}

std::shared_ptr<apcr::net_udp_client_t> apcr::get_ucnet(INST_IDX::_UCSOCKET idx)
{
  return ucnets[idx];
}

void apISR_100ms(void *arg1, void *arg2)
{
  
}

void apISR_50ms(void *arg1, void *arg2)
{
  //comm_tool->send_data(apcr::xprotocol_tool::tx_t::TX_OK_RESPONSE);
}

void de_init()
{
}


//MARK: Thread run
/****************************************************
 ****************************************************/
void ap_thread_run()
{  
  threads[trd_cnJob] = std::thread(trd_job);
  if (threads[trd_cnJob].joinable()) trd_stops[trd_cnJob] = false;
  
#ifdef _USE_HW_CLI
  TinyC::Cli::threadRun();
#endif
}



//MARK: Thread stop
/****************************************************
 ****************************************************/
void ap_thread_stop()
{
  de_init();
  period_timer_50ms.stop();
  period_timer_100ms.stop();

  for (auto i = 0; i < trd_max; i++)
  {
    trd_stops[i] = true;
    if (threads[i].joinable())
      threads[i].join();
  }

#ifdef _USE_HW_CLI
  TinyC::Cli::threadStop();
#endif
}


//MARK: Thread Job
/****************************************************
 ****************************************************/
void trd_job()
{
  REPORT_OUT("[prc] app trd_job start!");
  while (trd_stops[trd_cnJob] == false)
  {
    comm_pc->ThreadJob();

#ifndef _USE_SIMULATION
    constexpr uint32_t over_concurrency_time_ms = 50;
    static uint32_t    pre_ms                   = tim::millis();
    if (tim::millis() - pre_ms > over_concurrency_time_ms)
    {
      LOG_OUT("thread loop over time : %d ", tim::millis() - pre_ms);
    }
    pre_ms = tim::millis();
#endif


    tim::delay(10);
  }

  LOG_OUT("app trd_job end!");
}

errno_t register_sys_information()
{
  using namespace apcr;

  /* object information data*/
  //MARK: object information data
  std::vector<obj_t> objs{
    {ObjType::HL_SERIAL,   HW_SERIAL_BASE,   INST_IDX::_SERIAL::uart_api,      "uart_api",    "uart/uart_api.log"     },
    {ObjType::HL_UCSOCKET, HW_UCSOCKET_BASE, INST_IDX::_UCSOCKET::ucnet_loger, "ucnet_loger", "sockek/ucnet_loger.log"},
    {ObjType::CN_TASK,     CN_TASK_BASE,     INST_IDX::_cnTASK::task_nglog,    "task_nglog",  "cn/task_nglog.log"     },
    {ObjType::API_MODULE,  API_MODULE_BASE,  INST_IDX::_API::api_cmd,          "api_cmd",     "api/api_cmd.log"       },
  };

  obj_datas = objs;


  return ERROR_SUCCESS;
}

errno_t initHwComponents()
{
  using namespace apcr;
  //---------------------------------------------------------
  //MARK: net tcp server
  //---------------------------------------------------------
  // {
  //   uint32_t id  = HW_SOCKET_BASE + INST_IDX::_SOCKET::net_nglog;
  //   info_t   ret  = get_object_data<info_t>(id);

  //   nets[INST_IDX::_SOCKET::net_nglog] = std::make_shared<net_tcp_server_t>(ret);

  //   if (apcr::regist_object(id, (IObj *)nets[INST_IDX::_SOCKET::net_nglog].get()) == ERROR_SUCCESS)
  //   {
  //     REPORT_OUT("[OK] RegisterComponent Result Success!(HW_SOCKET_BASE, net_nglog)");
  //   }
  //   else
  //   {
  //     ERR_PRINT("RegisterComponent Result Fail!(HW_SOCKET_BASE, net_nglog)");
  //   }
  // }

  //---------------------------------------------------------
  //MARK: net udp client
  //---------------------------------------------------------
  {
    uint32_t id  = HW_UCSOCKET_BASE + INST_IDX::_UCSOCKET::ucnet_loger;
    info_t   ret = get_object_data<info_t>(id);

    ucnets[INST_IDX::_UCSOCKET::ucnet_loger] = std::make_shared<net_udp_client_t>(ret);

    if (apcr::regist_object(id, (IObj *)ucnets[INST_IDX::_UCSOCKET::ucnet_loger].get()) == ERROR_SUCCESS)
    {
      REPORT_OUT("[OK] RegisterComponent Result Success!(HW_UCSOCKET_BASE, ucnet_loger)");
    }
    else
    {
      ERR_PRINT("RegisterComponent Result Fail!(HW_UCSOCKET_BASE, ucnet_loger)");
    }
  }

  //---------------------------------------------------------
  //MARK: uart
  //---------------------------------------------------------
  {
    uint32_t id  = HW_SERIAL_BASE + INST_IDX::_SERIAL::uart_api;
    info_t   ret  = get_object_data<info_t>(id);

    uarts[INST_IDX::_SERIAL::uart_api] = std::make_shared<uart_t>(ret);

    if (apcr::regist_object(id, (IObj *)uarts[INST_IDX::_SERIAL::uart_api].get()) == ERROR_SUCCESS)
    {
      REPORT_OUT("[OK] RegisterComponent Result Success!(HW_SERIAL_BASE, uart_api)");
    }
    else
    {
      ERR_PRINT("RegisterComponent Result Fail!(HW_SERIAL_BASE, uart_api)");
    }
  }

  return ERROR_SUCCESS;
}

errno_t initComm_socketOpen()
{
  using namespace apcr;

  return ERROR_SUCCESS;
}

errno_t createControl_apis()
{
  using namespace apcr;
  using dat_t = ap_data::appConf::serial_comm_t;
  dat_t::elm_t *pData   = &cfg_data->m_serial_comm.elms[dat_t::elms_module];
  apcr::xprotocol_cmd::cfg_t xcfg{};
  xcfg.uartcfg  = uart_t::cfg_t{pData->_port_name.c_str(),(uint32_t)pData->_baud_rate,false};
  xcfg.ptr_uart = uarts[INST_IDX::_SERIAL::uart_api];


  apcr::api_cmd::cfg_t cfg { };
  cfg.comm_cfg = xcfg;
  cfg.ptr_data = &mcu_data;

  uint32_t id  = API_MODULE_BASE + INST_IDX::_API::api_cmd;
  info_t   ret  = get_object_data<info_t>(id);
  comm_pc = std::make_shared<apcr::api_cmd>(ret);

  if (apcr::regist_object(id, (IObj *)comm_pc.get()) == ERROR_SUCCESS)
  {
    if (comm_pc.get()->Init(cfg) != ERROR_SUCCESS)
    {
      ERR_PRINT("init Fail!, log manager");
      //return -1;
    }
    REPORT_OUT("[OK] init Success! log manager");
  }
  else
  {
    ERR_PRINT("RegisterComponent Result Fail!(API_MODULE_BASE, comm_pc)");
  }


  return ERROR_SUCCESS;
}

errno_t createControl_viewLoger()
{
  using namespace apcr;
  using dat_t         = ap_data::appConf::socket_comm_t;
  dat_t::elm_t *pData = &cfg_data->m_socket_comm.elms[dat_t::_loger];

  vLog::cfg_t cfg{};
  cfg.ptr_client = get_ucnet(INST_IDX::_UCSOCKET::ucnet_loger).get();
  cfg.ip_str     = pData->_ip;
  cfg.port       = pData->_port;
  cfg.name       = std::string{"view loger"};
  ap_log         = std::make_shared<apcr::vLog>();
  //ap_log
  if (ap_log.get()->Init(cfg) != ERROR_SUCCESS)
  {
    ERR_PRINT("init Fail!, log manager");
    //return -1;
  }
  REPORT_OUT("[OK] init Success! log manager");


  return ERROR_SUCCESS;
}
