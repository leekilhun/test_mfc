// on_imgui.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "on_imgui.h"

#include <tchar.h>
#include "on_imguiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#ifdef _UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif
#include <iostream>


#pragma comment(lib, "d3d11.lib")

ConimguiApp theApp;

// ConimguiApp construction

ConimguiApp::ConimguiApp()
{
  // TODO: add construction code here,

}

BEGIN_MESSAGE_MAP(ConimguiApp, CWinApp)
END_MESSAGE_MAP()  // 🔹 메시지 맵 정의 추가

BOOL ConimguiApp::InitInstance() {
  CWinApp::InitInstance();

  ConimguiDlg dlg;
  m_pMainWnd = &dlg;

  INT_PTR nResponse = dlg.DoModal();

  if (nResponse == IDOK) {
    // OK 처리
  }
  else if (nResponse == IDCANCEL) {
    // Cancel 처리
  }


  return FALSE;
}

