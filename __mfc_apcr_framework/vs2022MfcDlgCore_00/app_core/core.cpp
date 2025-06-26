/**
 * @file core.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-04-01
 *  간소화 형태의 framework
 * 
 * @copyright Copyright (c) 2025
 * 
 */
/*****************************************************************
 *  appication tiny framework 프로젝트 구조 (이길훈 '25.04 V1.0)
 * 
 * 프로젝트 폴더/
 * ├─ .vscode/                       >> vscode 설정 파일들 모음
 * │    ├─ c_cpp_properties.json
 * │    ├── settings.json
 * │    ├── launch.json
 * │    └── tasks.json
 * │
 * ├─ Data/                          >> 데이터 파일들 모음
 * │
 * │
 * └─ ap/  >>> application core code (계층적인 구조로 구성됨)
 * │    ├── _inc/ -> application header 파일들 모음
 * │    │   │   ├── hw/ -> hw header
 * │    │   │   └── interface/ -> interface header
 * │    │   ├── cn/ -> control layer source 파일들 모음
 * │    │   └── en/ -> engine layer source 파일들 모음
 * │    └─ ap_def.hpp
 * │       ap.cpp
 * │       ap.hpp
 * │       def_io.hpp
 * │       def_obj.hpp
 * │       def_sys.hpp
 * │       def_type.hpp
 * │
 * ├── lib                     >> external library
 * │
 * ├── mcc_lib                 >> mcc(machine control cpp programers) 라이브러리 
 * │
 * ├── util
 * │
 * └── core.cpp                >> 프로젝트 메인 파일 (진입점)
 * │   core.hpp
 * │
 * └─ .clang-format                 >> clang format 설정 파일 (코딩 스타일)
 *     CMakeLists.txt               >> cmake 설정 파일
 *     CMakePrsets.json             >> cmake preset 설정 파일
 * 
 *
 *********************************************************************/



/*
(이길훈 '24.12 V1.0)
application core structure (namespace apcr)

        ┌───────────────────────────┐ 
        │   GUI layer               │
        │   (MFC, C#, Pyside, Qt)   │
        └───────────────────────────┘
                    ▼  ▼ 
core                ▼  ▼  
        ┌───────────────────────────┐ 
        │  ┌─────────────────────┐  │ 
        │  │ application         │  │
        │  │ ┌────────────┐      │  │
        │  │ │ api(API)   │  ▼   │  │ 
        │  │ └────────────┘      │  │
        │  │ ┌────────────┐      │  │
        │  │ │ cn(control)│  ▼   │  │
        │  │ └────────────┘      │  │
        │  │ ┌────────────┐      │  │  
        │  │ │ en(engine) │  ▼   │  │
        │  │ └────────────┘      │  │        
        │  └─────────────────────┘  │            
        │  ┌────────────┐    ▼      │       ┌────────────┐
        │  │ hradware   │           │       │ common     │
        │  │ layer      │           │  ▶▶▶  │ interface  │            
        │  └────────────┘           │  ▶▶▶  │ hw-header  │ 
        │  ┌────────────┐    ▼      │       │ util       │   
        │  │ app system │           │       └────────────┘ 
        │  │ layer      │           │             ▲
        │  └────────────┘           │             ▲
        └───────────────────────────┘             ▲ 
                ▼▼▼                               ▲
        ┌───────────────────────────────────────────────────────     
lib     │  ┌────────────┐  ┌────────────┐ ┌────────────┐
        │  │ hardware   │  │ maker제공  │ │ extern     │
        │  │ library    │  │ library    │ │ libray     │
        │  └────────────┘  └────────────┘ └────────────┘   .....
        └───────────────────────────────────────────────────────   
                ▼▼▼
mcc_lib ┌────────────┐ 
        │ mcc        │ 
        │ libray     │ 
        └────────────┘ 





*/
#include "core.hpp"

/* UI (mfc) framework 이벤트 처리를 위한 네임스페이스*/
namespace ext_api
{
  static std::function<int(void *, void *)> mfc_cb{};

  void registerMfcCallback(std::function<int(void *, void *)> cb)
  {
    mfc_cb = cb;
  }

  void ext_alarm(alarm_datas_t &&alarm_msg)
  {
    /*
    (alarm_datas_t*) w_parm

    (const char*) l_parm
    */
    if (mfc_cb)
    {
      mfc_cb(&alarm_msg, nullptr);
    }
  }
}