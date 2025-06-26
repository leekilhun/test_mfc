#pragma once
#include "afxdialogex.h"


// Cui_popInfo 대화 상자

class Cui_popInfo : public CDialogEx
{
	CBtnEnh m_info{};
	DECLARE_DYNAMIC(Cui_popInfo)

public:
	Cui_popInfo(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Cui_popInfo();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INFO_BOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
};
