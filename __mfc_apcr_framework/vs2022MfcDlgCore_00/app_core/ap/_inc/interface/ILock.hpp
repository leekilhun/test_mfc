/**
 * @file ILock.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief interlock
 * @version 0.1
 * @date 2024-09-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#ifndef _IF_INTERLOCK_HPP
#define _IF_INTERLOCK_HPP

#include "def_obj.hpp"

/*
## naming rule
jsi_rx_cplt_req_scan_bcr
jsi_동작_상태_구분_참조하고있는 스텝

"__ "붙여서 끝 단락에참조하는 스텝 명시 
    ex) TODO 스텝에서 참조 -> jsi_state_rxbox_cplt__todo

# 동작
- info : information 정보 제공
- wait : 다른 객체가 참조할 idle 상태
- do   : 내부 스텝진입을 위한 진입 시 셋, 완료 후 리렛
- state : 내부 스텝에서 참조하기 위한 상태 정보
_ key : key, 참조하고 있는 스텝의 진입을 결정, 진입 시 해제


# 구분
- req : request, 참조하고 있는 스텝의 진입을 결정
  ex) jsi_bcr_cplt_req__boxout
- ref : reference, 다른 객체 스텝에서 참조하여 내부 조건 결정
  ex) jsi_wait_rxbox_ref__taping_outstart
- set : setting, 다른 객체가 접근하여 값을 셋하고, 내부에서 조건을 만족하여 리셋
  ex) jsi_wait_rxbox_set__taping_outend
- lock : 다른 객체의 접근을 차단

*/
class ILock
{
  public:
  virtual udword_t *get_job_status() = 0;
};



#endif //_IF_INTERLOCK_HPP