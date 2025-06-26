#include "pch.h"

#include "ui_def.hpp"

#include "commonUI.h"


/*

 AfxMessageBox(_T("This is a simple message box."));

 MessageBoxWithIcon()
    AfxMessageBox(_T("This message box has an exclamation icon."), MB_OK | MB_ICONEXCLAMATION);

 MessageBoxYesNo() 
   int result = AfxMessageBox(_T("Do you want to proceed?"), MB_YESNO | MB_ICONQUESTION);
   
   if (result == IDYES) {
       AfxMessageBox(_T("You chose Yes."));
   } else {
       AfxMessageBox(_T("You chose No."));
   }

MessageBoxYesNoCancel() 
    int result = AfxMessageBox(_T("Save changes before closing?"), MB_YESNOCANCEL | MB_ICONQUESTION);

    switch (result) {
        case IDYES:
            AfxMessageBox(_T("You chose Yes."));
            break;
        case IDNO:
            AfxMessageBox(_T("You chose No."));
            break;
        case IDCANCEL:
            AfxMessageBox(_T("You chose Cancel."));
            break;
    }

MessageBoxRetryCancel() 
    int result = AfxMessageBox(_T("An error occurred. Retry?"), MB_RETRYCANCEL | MB_ICONERROR);

    if (result == IDRETRY) {
        AfxMessageBox(_T("You chose Retry."));
    } else {
        AfxMessageBox(_T("You chose Cancel."));
    }

MessageBoxCustomCaption() 
    AfxMessageBox(_T("This message box has a custom caption."), MB_OK | MB_ICONINFORMATION, 0, _T("Custom Caption"));


 MessageBoxWithDefaultButton() 
    int result = AfxMessageBox(_T("Do you want to save changes?"), MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);

    switch (result) {
        case IDYES:
            AfxMessageBox(_T("You chose Yes."));
            break;
        case IDNO:
            AfxMessageBox(_T("You chose No."));
            break;
        case IDCANCEL:
            AfxMessageBox(_T("You chose Cancel."));
            break;
    }

MessageBoxWithHelp() 
    AfxMessageBox(_T("This message box has a Help button."), MB_OK | MB_HELP);
*/

/*
* 
├── caption  ──────────────────
│                             │
│                             │
│                             │
│                             │
│                             │
└────────────────────────────── 
*/

int alarmBox(LPCTSTR msg, LPCTSTR ccaption, UINT type, HWND hWnd)
{

  if (hWnd == nullptr) {
    hWnd = GetForegroundWindow();  // 현재 활성화된 창을 부모로 설정
  }

  int msgboxID = MessageBox(
    hWnd,  // 부모 윈도우 핸들
    msg,
    ccaption,
    type | MB_TOPMOST  // 메시지 박스를 최상위로 설정
  );

  // 필요시 메시지 박스를 띄운 후 창을 다시 앞으로 가져옴
  if (hWnd) {
    SetForegroundWindow(hWnd);
  }
  switch (msgboxID)
  {
  case IDCANCEL:
    // TODO: add code
    break;
  case IDTRYAGAIN:
    // TODO: add code
    break;
  case IDCONTINUE:
    // TODO: add code
    break;
  }

  return msgboxID;

	CWnd* pWnd = AfxGetMainWnd();  // Get the main window (typically)
	// Display a message box with a custom caption
	if (pWnd)
		return pWnd->MessageBox(msg, ccaption, type);

	// If pWnd is NULL, fallback to AfxMessageBox
	return AfxMessageBox(msg, type);

}

int Message_Box(LPCTSTR text, LPCTSTR title, UINT type)
{
  // TODO: 여기에 구현 코드 추가.
  Cui_popMsg::Set_t cfg;
  cfg.title = title;
  cfg.message = text;
  cfg.form_type = type;

  Cui_popMsg dlg_msg(cfg);
  INT_PTR ret = -1;
  ret = dlg_msg.DoModal();

  // Handle the return value from DoModal
  switch (ret)
  {
  case -1:
    AfxMessageBox(_T("Dialog box could not be created!"));
    break;
  case IDABORT:
    // Do something
    break;
  case IDOK:
    // Do something
    break;
  case IDCANCEL:
    // Do something
    break;
  default:
    // Do something
    break;
  };


  return  (int)ret;
}

bool GetNumberWin(int max_num, HWND hItem, const wchar_t* title , COLORREF color)
{
  bool ret = false;
  Cui_popGetNumber* p_dlg = new Cui_popGetNumber(max_num, hItem, title, color);

  if (p_dlg->DoModal() == IDOK)
  {
    ret = true;
    //lstrcpy(get_num, str); //double byte char set
  }
  //return false;

  delete p_dlg;
  p_dlg = nullptr;

  return ret;
}

bool GetNumberWin(int max_num, CString& get_value, const wchar_t* title)
{

  bool ret = false;

  Cui_popGetNumber* p_dlg = new Cui_popGetNumber(max_num, get_value, title);
  if (p_dlg->DoModal() == IDOK)
  {
    ret = true;
  }
  //return false;

  delete p_dlg;
  p_dlg = nullptr;

  return ret;


}

bool GetPasswordWin(CString* ptr_value, const wchar_t* title)
{
  bool ret = false;
  Cui_popGetValue* p_dlg = new Cui_popGetValue(ptr_value, 15, title, true);
  if (p_dlg->DoModal() == IDOK)
  {
    ret = true;
  }
  //return false;

  delete p_dlg;
  p_dlg = nullptr;

  return ret;
}

bool GetTextWin(CString* ptr_value, const wchar_t* title)
{
  bool ret = false;
  Cui_popGetValue* p_dlg = new Cui_popGetValue(ptr_value, 15, title);
  if (p_dlg->DoModal() == IDOK)
  {
    ret = true;
  }
  //return false;

  delete p_dlg;
  p_dlg = nullptr;

  return ret;
}
bool GetTextWin(HWND hItem, const wchar_t* title)
{
  bool ret = false;

  Cui_popGetValue* p_dlg = new Cui_popGetValue(hItem, 15, title);
  if (p_dlg->DoModal() == IDOK)
  {
    ret = true;
  }
  //return false;

  delete p_dlg;
  p_dlg = nullptr;

  return ret;
}

static int msgCB(void* info, void* msg);

void commonUI_init()
{
  //ext_api::registerMfcCallback(msgCB);
}

int msgCB(void* info, void* msg)
{
  /*alarm_datas_t alarm_info = *(alarm_datas_t*)info;
 
  using data_t = apcr::attribute_st;
  data_t ret{};
  if (alarm_info.object_idx == apcr::MFC_UI_BASE)
    ret.obj_name = "mfc ui";
  else
  {
    uint32_t id = alarm_info.object_idx + alarm_info.instance_idx;
    ret = apcr::get_object_data<data_t>(id);
  }

 
  std::string filename = std::filesystem::path(alarm_info.file).filename().string();
  std::vector<std::string> v_strs{};
  std::string str_func{ alarm_info.func };
  if (trans::string_split(alarm_info.func, v_strs, "::") > 0)
    str_func = v_strs.back();

  std::stringstream msg_ss{};
  msg_ss << (const char*)alarm_info.msg << "\n \n";
  msg_ss << "file : " << filename << " \n";
  msg_ss << "func : " << str_func << " \n";
  msg_ss << "line : " << std::to_string(alarm_info.line) << " \n";
  msg_ss << "err  : " << std::to_string(alarm_info.err_no);

  alarmBox(ptrCha2wcha(msg_ss.str().c_str()), ptrCha2wcha(ret.obj_name.c_str()));*/
  return 0;
}
