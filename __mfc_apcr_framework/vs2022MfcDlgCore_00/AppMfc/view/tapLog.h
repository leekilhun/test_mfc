#pragma once
#include "afxdialogex.h"


// CtapLog 대화 상자

class CtapLog : public CDialogEx
{

	apcr::pc_api_t m_ptrApi{};

	CRect m_rectGrid{};
	CGridCtrl m_logGrid{};
	CStatic m_logFrame{};
	CFont m_Font;  // 전체 그리드에 적용할 폰트
	CFont m_BoldFont; // 특정 셀에 적용할 굵은 폰트  
	CComboBox m_cboObj{};

	struct logFiles_t
	{
		std::vector<std::string> sys_build{};
		std::vector<std::string> sys_err{};
		std::vector<std::string> auto_err{};
		std::vector<std::string> etc{};
	}m_logFiles{};


private:
	void initGrid();
	void update_log();
	void init_droplist();

	UINT_PTR	m_TimerID{};

	DECLARE_DYNAMIC(CtapLog)

public:
	CtapLog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CtapLog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAP_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCboLogObj();
	afx_msg void ClickLogFuncViewErrLog();
};
