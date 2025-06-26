#pragma once
#include "afxdialogex.h"


// Cui_popAlarm 대화 상자

class Cui_popAlarm : public CDialogEx
{
	DECLARE_DYNAMIC(Cui_popAlarm)

public:
	Cui_popAlarm(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Cui_popAlarm();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ERROR_MSG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedErrMsgOk();
private:
	CMFCButton m_btnOK;
	CMFCButton m_btnBuzzerOff;
	CMFCButton m_btnInfo;
public:
	afx_msg void OnBnClickedErrMsgShow();
	afx_msg void OnBnClickedErrMsgBuzzerOff();
private:
	void viewDefault();
	CEdit m_editViewObj;
	CEdit m_editViewUnit;
	CEdit m_editViewContent;
	CEdit m_editViewInfor;
	CStatic m_staErrInfor;
	void viewInfor();
};
