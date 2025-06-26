
// AppMfc.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "AppMfc.h"
#include "AppMfcDlg.h"

#include "3dabmManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifndef _viEw_ProMt
#define _viEw_ProMt
#endif

// CAppMfcApp

BEGIN_MESSAGE_MAP(CAppMfcApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


#ifdef _DEBUG

#include <crtdbg.h>
#include <windows.h>
#include <dbghelp.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "dbghelp.lib")

void WriteStackTrace(std::ofstream& logFile)
{
	void* stack[62];
	USHORT frames = CaptureStackBackTrace(0, 62, stack, nullptr);

	SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256);
	if (!symbol) return;

	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, TRUE);

	logFile << "Call Stack:" << std::endl;
	for (USHORT i = 0; i < frames; ++i) {
		if (SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol)) {
			logFile << "  [" << i << "] " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << std::endl;
		}
	}

	SymCleanup(process);
	free(symbol);
}

int reportHook(int reportType, char* message, int* returnValue)
{
	//누적되지 않도록 함 , 최종 로그만 남김
	std::ofstream logFile("assertion_log.txt", std::ios::out); // 'app' 플래그 제거
	if (logFile.is_open()) {
		logFile << "CRT Report: " << message << std::endl;
		WriteStackTrace(logFile);
		logFile << "----------------------------------------\n";
		logFile.close();
	}

	return FALSE; // 기본 동작 유지 (메시지 박스도 출력됨)
}
#endif


// CAppMfcApp construction

CAppMfcApp::CAppMfcApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAppMfcApp object

CAppMfcApp theApp;

#ifdef _viEw_ProMt
bool TinyC::sequence_terminal::is_enabled = false;
#endif

#ifdef USE_IMAGE_GDI_PLUSE
ULONG_PTR gdiplusToken; // gdi_pluse 토큰
#endif


// CAppMfcApp initialization

BOOL CAppMfcApp::InitInstance()
{
	CWinApp::InitInstance();

#ifdef _DEBUG
	_CrtSetReportHook(reportHook);
#endif

#ifdef USE_IMAGE_GDI_PLUSE
	//초기화 입력
	GdiplusStartupInput gdiplusStartupInput;
	if (::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok) {
		AfxMessageBox(_T("GDI+ 라이브러리의 초기화에 실패하였습니다!"));
		return FALSE;
	}
#endif


	AfxEnableControlContainer();

	//-----------------------------------------------------------------
	// 3D 버튼 라이브러리 사용 시 OCX 설치 상태 체크
	//----------------------------------------------------------------
	CString strRegName = _T("tinyfwkCAConnector");

	// Ctrl 키를 누르고 실행하면 기존 저장된 위치 및 크기를 초기화 한다.
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		CString strKey;
		strKey.Format(_T("SOFTWARE\\%s\\%s\\Workspace\\WindowPlacement"), strRegName.GetBuffer(), strRegName.GetBuffer());
		LONG Reg_Ret = RegDeleteKey(HKEY_CURRENT_USER, strKey);
	}

	// 3D 버튼 DLL이 설치되어 있는지 확인한다.
	M3dabmManager ocxManager;
	if (ocxManager.CheckDllRegister() == FALSE)
	{
		if (AfxMessageBox(_T("3dabm ocx not installed\nInstall ocx?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return FALSE;
		}

		E3dabm_Error eError = ocxManager.RegisterOCX();
		if (eError != e3dabm_Error_Success)
		{
			CString strMsg = ocxManager.GetErrorString(eError);
			AfxMessageBox(strMsg, MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}
	//-----------------------------------------------------------------
	//-----------------------------------------------------------------



	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// create console window
#ifdef _viEw_ProMt
	// consol out korean
	SetConsoleOutputCP(CP_UTF8);

	auto& vt = TinyC::sequence_terminal::get_instance();
#endif

	//-----------------------------------------------------------------
//-----------------------------------------------------------------

	apcr::init();
	apcr::main();

	//-----------------------------------------------------------------
	//-----------------------------------------------------------------



	//CAppMfcDlg* dlg = new CAppMfcDlg();
	CAppMfcDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	apcr::exit();

#ifdef _viEw_ProMt
	TinyC::sequence_terminal::disable_sequence_terminal_processing();
#endif


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


int CAppMfcApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
#ifdef USE_IMAGE_GDI_PLUSE
	::GdiplusShutdown(gdiplusToken);
#endif


	return CWinApp::ExitInstance();
}

ULONG_PTR CAppMfcApp::get_gdiplusToken(void)
{
#ifdef USE_IMAGE_GDI_PLUSE
	return gdiplusToken; // gdi_pluse 토큰
#endif
}