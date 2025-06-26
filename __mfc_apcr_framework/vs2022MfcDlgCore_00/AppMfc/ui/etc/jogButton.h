#pragma once
#include "afxdialogex.h"



#define UM_JOG_BTN_DOWN		WM_USER + 1001
#define UM_JOG_BTN_UP		WM_USER + 1002

// CJogButton window
class CJogButton : public CButton
{
public:


public:
	CJogButton();

public:

private:
	BOOL	m_bChecked{FALSE};

public:


public:
	virtual ~CJogButton();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
};
