/**
 * @file ap.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief
 * @version 0.1
 * @date 2024-06-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#ifndef AP_AP_HPP_
#define AP_AP_HPP_

#include "ap_def.hpp"

#include "ap_dat.hpp"
#include "ap_reg.hpp"
//-----------------------------------
//MARK: interface
//-----------------------------------
#include "_inc/interface/IComm.hpp"
#include "_inc/interface/IIO.hpp"
#include "_inc/interface/ILock.hpp"

//-----------------------------------
//MARK: hw module
//-----------------------------------
#include "socket.hpp"
#include "uart.hpp"
namespace apcr
{
  using net_tcp_server_t = apcr::NetComm<apcr::TcpServer>;
  using nets_t           = std::array<std::shared_ptr<apcr::net_tcp_server_t>, INST_IDX::_SOCKET::net_max>;

  using net_udp_client_t = apcr::NetComm<apcr::UdpClient>;
  using ucnets_t  = std::array<std::shared_ptr<apcr::net_udp_client_t>, INST_IDX::_UCSOCKET::ucnet_max>;
  
  std::shared_ptr<apcr::net_tcp_server_t> get_net(INST_IDX::_SOCKET idx);
  std::shared_ptr<apcr::net_udp_client_t> get_ucnet(INST_IDX::_UCSOCKET idx);

  using uart_t = apcr::serial_comm;
  using uarts_t = std::array<std::shared_ptr<apcr::uart_t>, INST_IDX::_SERIAL::uart_max>;
  std::shared_ptr<uart_t> get_uart(INST_IDX::_SERIAL idx);

  
  ap_data::appConf *get_cfg(void);

  using app_data_t = ap_mcuData<mcudat_t>;
}
// end of namespace

#include "ap_io.hpp"
#include "ap_vlog.hpp"
//-----------------------------------
//MARK: control module
//-----------------------------------


//-----------------------------------
//MARK: api
//-----------------------------------
#include "xprotocol_cmd.hpp"
#include "api_cmd.hpp"

namespace apcr
{
  errno_t init(void);
  void main(void);
  void exit(void);


  using pc_api_t = std::shared_ptr<apcr::api_cmd>;
  pc_api_t get_api();

  using vlog_t = std::shared_ptr<apcr::vLog>;
  vlog_t get_vlog();

  udword_t &get_ioY();
}

// end of namespace

#endif /* AP_AP_HPP_ */