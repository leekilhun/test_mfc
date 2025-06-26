#pragma once
#ifndef __ETC_UI_JOG_3D_BTN_H___
#define __ETC_UI_JOG_3D_BTN_H___		


#include "afxdialogex.h"


#include "btnenh.h"


using namespace LIB_3D_BUTTON;
// CJogButton window
static constexpr int def_um_jog_btn_down = WM_USER + 1101;
static constexpr int def_um_jog_btn_up = WM_USER + 1102;
class CjogBtn : public CBtnEnh
{
public:


public:
	CjogBtn();

public:

private:
	BOOL	m_bChecked{ FALSE };

public:


public:
	virtual ~CjogBtn();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
};


#endif // !__ETC_UI_JOG_3D_BTN_H___	
