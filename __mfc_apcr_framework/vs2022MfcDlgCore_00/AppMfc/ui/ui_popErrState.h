#pragma once

#include "afxdialogex.h"


// Cui_popErrState 대화 상자입니다.

class Cui_popErrState : public CDialog
{
	DECLARE_DYNAMIC(Cui_popErrState)

public:
	Cui_popErrState(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~Cui_popErrState();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_POP_VIEW_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

public:
	void SetMessage(CString strMsg);

private:
	CBtnEnh	m_Msg;
	CBtnEnh	m_Icon;

	CBrush m_brBckColor;
};
