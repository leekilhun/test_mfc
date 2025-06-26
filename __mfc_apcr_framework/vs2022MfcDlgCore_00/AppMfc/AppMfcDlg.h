
// AppMfcDlg.h : header file
//

#pragma once
// 프로젝트 공용 include
#include "commonUI.h"

// ui 작업을 위한 etc 클래스
#include "cbtn.h"
#include "btn.h"
#include "toggle_btn.h"
#include "imgCn.h"
#include "toolTip.h"
#include "jogButton.h"
#include "fontStatic.h"
#include "jog3DBtn.h"

#include "hereRect.h"


// CAppMfcDlg dialog
class CAppMfcDlg : public CDialogEx
{
// Construction
public:
	CAppMfcDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
