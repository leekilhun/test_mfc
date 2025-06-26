/*****************************************************************//**
 * \file   commonUI_def.h
 * \brief  
 * 
 * \author gns2.lee (gns2.lee@samsung.com)
 * \date   August 2024
 * 
 * 
 * @copyright Copyright (c) 2024
 * 
 *********************************************************************/
#pragma once


//common define UI 
namespace cdu
{
  constexpr COLORREF LINE_W = RGB(255, 255, 254);
  constexpr COLORREF LINE_Y = RGB(255, 255, 224);
	constexpr COLORREF LINE_G = RGB(252, 246, 245);
	constexpr COLORREF LINE_B = RGB(231, 244, 255);

  const COLORREF BACKGROUND_COLOR = RGB(230, 230, 230);
  const COLORREF CLOUDBLUE = RGB(128, 184, 223);
  const COLORREF WHITE = RGB(255, 255, 255);
  const COLORREF BLACK = RGB(1, 1, 1);
  const COLORREF DKGRAY = RGB(128, 128, 128);
  const COLORREF LTGRAY = RGB(192, 192, 192);
  const COLORREF YELLOW = RGB(255, 255, 0);
  const COLORREF DKYELLOW = RGB(128, 128, 0);
  const COLORREF RED = RGB(255, 0, 0);
  const COLORREF DKRED = RGB(128, 0, 0);
  const COLORREF BLUE = RGB(0, 0, 255);
  const COLORREF DKBLUE = RGB(0, 0, 128);
  const COLORREF CYAN = RGB(0, 255, 255);
  const COLORREF DKCYAN = RGB(0, 128, 128);
  const COLORREF GREEN = RGB(0, 255, 0);
  const COLORREF DKGREEN = RGB(0, 128, 0);
  const COLORREF MAGENTA = RGB(255, 0, 255);
  const COLORREF DKMAGENTA = RGB(128, 0, 128);
  const COLORREF BASIC = RGB(212, 208, 200);
  const COLORREF LABEL_BACK = RGB(237, 236, 218);

  const COLORREF GREENYELLOW = RGB(0xad, 0xff, 0x2f);
  const COLORREF GOLDENROD = RGB(0xda, 0xa5, 0x20);
  const COLORREF ORANGE = RGB(0xff, 0xa5, 0x00);
  const COLORREF TOMATO = RGB(0xff, 0x63, 0x47);
  const COLORREF GOLD = RGB(0xff, 0xd7, 0x00);
  const COLORREF ORCHID = RGB(0xda, 0x70, 0xd6);
  const COLORREF MEDIUMVIOLETRED = RGB(0xc7, 0x15, 0x85);
  const COLORREF SALMON = RGB(0xfa, 0x80, 0x72);
  const COLORREF DARKORANGE = RGB(0xff, 0x8c, 0x00);
  const COLORREF CHARTREUSE = RGB(0x7f, 0xff, 0x00);
}
// end of namespace cdu
// 
//common function in UI
namespace cfu
{

  enum number_t 
  {
    nt_INTEGER,
    nt_FLOAT,
    nt_INVALID
  };

  inline static number_t get_number_type(const wchar_t* str)
  {
    bool has_decimal_point = false;

    while (*str) 
    {
      if (*str == L'.') 
      {
        if (has_decimal_point)
          return nt_INVALID; // 소수점이 두 번 이상 나오면 INVALID
        has_decimal_point = true;
      }
      else if (!iswdigit(*str)) 
        return nt_INVALID; // 숫자 또는 소수점 이외의 문자가 나오면 INVALID

      str++;
    }

    return has_decimal_point ? nt_FLOAT : nt_INTEGER;
  }

  inline static double str_to_number(const wchar_t* str)
  {
    switch (get_number_type(str)) 
    {
    case nt_INTEGER:
      return static_cast<double>(_wtoi(str)); // 정수로 변환 후 double로 캐스팅
    case nt_FLOAT:
      return _wtof(str); // 실수로 변환
    case nt_INVALID:
    default:
      return 0.0; // 변환할 수 없으면 0.0 반환
    }
  }


}

int alarmBox(LPCTSTR msg, LPCTSTR ccaption, UINT type = (MB_OK | MB_ICONINFORMATION), HWND hWnd = nullptr);
int Message_Box(LPCTSTR text, LPCTSTR title = NULL, UINT type = (MB_OK | MB_ICONINFORMATION));

bool GetNumberWin(int max_num, CString &get_value, const wchar_t* title = L"Insert Number Only.");
bool GetNumberWin(int max_num, HWND hItem, const wchar_t* title = L"Insert Number Only.", COLORREF color = 0);

bool GetPasswordWin(CString* ptr_value, const wchar_t* title = L"Insert Password Char.");
bool GetTextWin(CString* ptr_value, const wchar_t* title = L"Insert Character.");
bool GetTextWin( HWND hItem, const wchar_t* title = L"Insert Character.");

struct pop_alarm_t
{
	void* w_parm{};
	void* l_parm{};
};

