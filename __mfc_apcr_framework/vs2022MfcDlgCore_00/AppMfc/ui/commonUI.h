#pragma once

#include "commonUI_def.h"

// mfc Grid Library
#include "GridCtrl.h"
#include "btnenh.h"

// mfc Chart Library
//#include "ChartViewer.h"
//#include "ScopeCtrl.h"


// ui 작업을 위한 etc 클래스
using namespace LIB_GRID_CTRL;
using namespace LIB_3D_BUTTON;


#define ptrCha2wcha(utf_8)   mcc::conversion::utf8_to_utf16(utf_8).c_str()
#define cha2Cstr(utf_8)      CString{mcc::conversion::cchar_to_wstring(utf_8).c_str()}
#define ptrWcha2cha(utf_16)   mcc::conversion::utf16_to_utf8(utf_16).c_str()
#define string2Wstring(str)  mcc::conversion::string_to_wstring(str)

#define str_To_int(str) (_ttoi(str))  // CString을 int로 변환
#define str_To_float(str) ((float)_ttof(str))  // CString을 float로 변환
#define str_To_double(str) (_tstof(str))  // CString을 double로 변환

#define safe_str_To_int(str, def) (str.IsEmpty() ? (def) : _ttoi(str))
#define safe_str_To_float(str, def) (str.IsEmpty() ? (def) : (float)_ttof(str))
#define safe_str_To_double(str, def) (str.IsEmpty() ? (def) : _tstof(str))


// mfc gui 
/****************************************************
	*	gui timer id define
	****************************************************/
#define  UPDATE_TIMER_MAIN_DLG                  (1)
#define  UPDATE_TIMER_TAP_AUTO_DLG						  (11)
#define  UPDATE_TIMER_TAP_TEACH_DLG             (12)
#define  UPDATE_TIMER_TAP_REG_DLG								(13)
#define  UPDATE_TIMER_TAP_DATA_DLG							(14)
#define  UPDATE_TIMER_TAP_LOG_DLG								(15)
#define  UPDATE_TIMER_POP_UNIT_IMGUI_DLG        (21)
#define  UPDATE_TIMER_POP_ALARM_DLG             (22)


#include "msgWnd.h"
#include "progressWnd.h"
#include "ui_popGetNumber.h"
#include "ui_popErrMsg.h"
#include "ui_popMsg.h"
#include "ui_popGetValue.h"
#include "ui_popAlarm.h"
#include "ui_popGuide.h"
#include "ui_popErrState.h"


#ifdef _AFX
template <typename RetType>
inline RetType get_dlg_value(CWnd* pWnd, int nID)
{
  if (!pWnd || !::IsWindow(pWnd->GetSafeHwnd()))
    return RetType{}; // 유효하지 않은 윈도우 핸들일 경우 기본값 반환

  CWnd* pControl = pWnd->GetDlgItem(nID); // nID에 해당하는 컨트롤 가져오기
  if (!pControl || !::IsWindow(pControl->GetSafeHwnd()))
    return RetType{}; // 컨트롤이 없거나 유효하지 않은 경우 기본값 반환

  if constexpr (std::is_same_v<std::remove_cvref_t<RetType>, double>)
  {
    CString strValue;
    pControl->GetWindowText(strValue); // 컨트롤의 텍스트 가져오기
    return str_To_double(strValue); // CString을 double로 변환
  }
  else if constexpr (std::is_same_v<std::remove_cvref_t<RetType>, int>)
  {
    CString strValue;
    pControl->GetWindowText(strValue);
    return str_To_int(strValue); // CString을 int로 변환
  }
  else if constexpr (std::is_same_v<std::remove_cvref_t<RetType>, CString>)
  {
    CString strValue;
    pControl->GetWindowText(strValue);
    return strValue; // CString 반환
  }
  else
  {
    return RetType{}; // 지원되지 않는 반환 유형일 경우 기본값 반환
  }
}
#endif


/**
 * 숫자로 변환이 가능한지 확인한다.
 *
 * \param str
 * \return 가능하면 true, 문자가 있으면 false
 */
template<typename NumT, typename CharT>
bool isConvertibleToNumber(const std::basic_string<CharT>& str) {
  // T가 산술 타입(정수 또는 실수)이 아니면 컴파일 오류 발생
  static_assert(std::is_arithmetic<NumT>::value, "Template argument T must be an arithmetic type (int, double, etc.).");

  // CharT에 따라 적절한 stringstream 타입을 선택
  // std::conditional은 컴파일 타임에 조건에 따라 타입을 선택합니다.
  using StringStreamType = typename std::conditional<
    std::is_same<CharT, char>::value,
    std::stringstream,
    std::wstringstream
  >::type;

  StringStreamType ss{};

  // CharT가 char인 경우와 wchar_t인 경우에 따라 스트림에 문자열을 전달
  // basic_string<CharT>의 str() 멤버 함수는 basic_string<CharT>을 반환합니다.
  ss.str(str);

  NumT value{};
  ss >> value;

  // 변환 실패 (숫자 아님) 또는 숫자 뒤에 다른 문자가 남아있는지 확인
  return !ss.fail() && ss.eof();
}


namespace uoi // UI object index
{

}
// end of ui object index

void commonUI_init();



