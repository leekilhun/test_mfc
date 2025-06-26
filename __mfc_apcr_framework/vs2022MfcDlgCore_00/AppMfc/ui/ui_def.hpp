/*****************************************************************//**
 * \file   ui_def.hpp
 * \brief  
 * 
 * 
 * \author gns2.lee (gns2.lee@samsung.com)
 * \date   April 2024
 * 
 * 
 * @copyright Copyright (c) 2024
 * 
 *********************************************************************/

#pragma once
#ifndef _UI_UI_DEF_HPP
#define _UI_UI_DEF_HPP


#include "AppMfc.h"
#include "AppMfcDlg.h"

 /*
 (檜望�� '24.12 V1.0)
 application core structure (namespace apcr)


 MFC SDI framework


         Щ煎薛お ィ渦/
         戍式 ui
         弛    戍式 etc/
         弛    戍式式 view/body formview ...
         弛    戍式式 common ui ...
         弛    戌式式 pop-up dialog...
         弛
         戍式 Data/                          >> 等檜攪 だ橾菟 賅擠
         弛
         戌式 core                           >> core 囀萄 だ橾菟 賅擠
         弛    戍式式 ap/                      >>> application core code (啗類瞳檣 掘褻煎 掘撩脾)
         弛    弛   戍式式 _inc/
         弛    弛   戍式式 api/
         弛    弛   戍式式 cn/
         弛    弛   戌式式 en/
         弛    弛
         弛    戍式式式 common/
         弛    弛      戍式式 inc_hw/
         弛    弛      戍式式 interface/
         弛    弛      戌式式 util/
         弛    弛
         弛    戍式式 hw/
         弛    弛   戌式式 drive/
         弛    弛
         弛    戍式式 sys/
         弛    弛
         弛    弛
         弛    戌式式 core.hpp, core.cpp      >>
         弛
         戍式式 mfc_lib                      >> mfc 塭檜粽楝葬
         弛
         戍式式 lib                          >> external library
         弛
         戍式式 mcc_lib                      >> mcc(machine control cpp programers) 塭檜粽楝葬
         弛
         戌式式 project晦獄 儅撩 ィ渦
             framework.h,  pch.h, MainFrm.h, resource.h...   >>  project 儅撩だ橾
             project(name)戲煎 儅撩腎朝 ④渦 塽 模蝶だ橾, Щ煎薛お だ橾


         忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式  GUI layer  式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
         弛  忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
         弛  弛 CFormView view class                                                          弛
         弛  弛 忙式式式式式式式式式式式式式忖  忙式式式式式式式式式式式式式式 pop-up dialog 式式式式式式式式式忖                     弛
         弛  弛 弛 top form    弛  弛 io_control, axis_org, unit_init  ... 弛                     弛
         弛  弛 戌式式式式式式式式式式式式式戎  戌式式式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式戎                     弛
         弛  弛 忙式式式式式式式式式式式式式忖  忙式式式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式忖                     弛
         弛  弛 弛 body form   弛--弛 auto, manaul, teach, data, log ...   弛                     弛
         弛  弛 戌式式式式式式式式式式式式式戎  戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎                     弛
         弛  弛 忙式式式式式式式式式式式式式忖                                                               弛
         弛  弛 弛 bottom form 弛   body form曖 view蒂 滲唳ж朝 檜漸お 幗が                     弛
         弛  弛 戌式式式式式式式式式式式式式戎                                                               弛
         弛  弛 忙式式式式式式式式式式式式式式式式式式式式式式式式式 common ui 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖  弛
         弛  弛 弛 msgWnd, progressWnd, GetValue(number,str), ErrMsg, popMsg ,popAlarm ...  弛  弛
         弛  弛 戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎  弛
         弛  戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
         弛                          ∪  ∪
         弛  忙式式式式式式式式式式式式式忖    忙式式式式式式式式式式式式忖
         弛  弛 MainFrm.cpp 弛 Ⅱ  弛 MainFrm.h  弛   include - mfc Lib, commonUI.h, view/formView headers, pop-up dialogs,
         弛  弛             弛    弛            弛   define gui timer id, WM_USER_MESSAGE ids, etc.
         弛  戌式式式式式式式式式式式式式戎    戌式式式式式式式式式式式式戎
         弛   single doc. top/middle/btm戲煎 碟йж堅 檜漸お縑 評塭憮 view蒂 滲唳
         弛       ∪  ∪
         弛  忙式式式式式式式式式式式式式忖    忙式式式式式式式式式式式式忖
         弛  弛 project.cpp 弛 Ⅱ  弛 project.h  弛   ⅡⅡⅡ ap.hpp
         弛  弛             弛    弛            弛      (namespace apcr)
         弛  戌式式式式式式式式式式式式式戎    戌式式式式式式式式式式式式戎
         弛    ～
         弛    framwork.h afwinappex.h afxdialogex.h        framwork.h -> 3d幗が dependency 頂辨 塽 п渦 蹺陛
         弛    project.h .....                              project.h -> include namepace::apcr
         弛    MainFrm.h  projecDoct.h  projectView.h
         弛
         戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式



         忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
         弛   GUI layer               弛
         弛   (MFC, C#, Pyside, Qt)   弛
         戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
                     ∪  ∪
 core                ∪  ∪
         忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
         弛  忙式式式式式式式式式式式式式式式式式式式式式忖  弛
         弛  弛 application         弛  弛
         弛  弛 忙式式式式式式式式式式式式忖      弛  弛
         弛  弛 弛 api(API)   弛  ∪   弛  弛
         弛  弛 戌式式式式式式式式式式式式戎      弛  弛
         弛  弛 忙式式式式式式式式式式式式忖      弛  弛
         弛  弛 弛 cn(control)弛  ∪   弛  弛
         弛  弛 戌式式式式式式式式式式式式戎      弛  弛
         弛  弛 忙式式式式式式式式式式式式忖      弛  弛
         弛  弛 弛 en(engine) 弛  ∪   弛  弛
         弛  弛 戌式式式式式式式式式式式式戎      弛  弛
         弛  戌式式式式式式式式式式式式式式式式式式式式式戎  弛
         弛  忙式式式式式式式式式式式式忖    ∪      弛       忙式式式式式式式式式式式式忖
         弛  弛 hradware   弛           弛       弛 common     弛
         弛  弛 layer      弛           弛  ⅡⅡⅡ  弛 interface  弛
         弛  戌式式式式式式式式式式式式戎           弛  ⅡⅡⅡ  弛 hw-header  弛
         弛  忙式式式式式式式式式式式式忖    ∪      弛       弛 util       弛
         弛  弛 app system 弛           弛       戌式式式式式式式式式式式式戎
         弛  弛 layer      弛           弛             ～
         弛  戌式式式式式式式式式式式式戎           弛             ～
         戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎             ～
                 ∪∪∪                               ～
         忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
 lib     弛  忙式式式式式式式式式式式式忖  忙式式式式式式式式式式式式忖 忙式式式式式式式式式式式式忖
         弛  弛 hardware   弛  弛 maker薯奢  弛 弛 extern     弛
         弛  弛 library    弛  弛 library    弛 弛 libray     弛
         弛  戌式式式式式式式式式式式式戎  戌式式式式式式式式式式式式戎 戌式式式式式式式式式式式式戎   .....
         戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
                 ∪∪∪
 mcc_lib 忙式式式式式式式式式式式式忖
         弛 mcc        弛
         弛 libray     弛
         戌式式式式式式式式式式式式戎





 */



#endif //_UI_UI_DEF_HPP
