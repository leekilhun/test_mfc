
// on_imgui.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// ConimguiApp:
// See on_imgui.cpp for the implementation of this class
//

class ConimguiApp : public CWinApp
{
public:
	ConimguiApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern ConimguiApp theApp;
