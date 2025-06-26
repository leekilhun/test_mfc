
// AppMfc.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAppMfcApp:
// See AppMfc.cpp for the implementation of this class
//

class CAppMfcApp : public CWinApp
{
public:
	CAppMfcApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

	ULONG_PTR get_gdiplusToken(void); // gdi_pluse ÅäÅ«
};

extern CAppMfcApp theApp;
