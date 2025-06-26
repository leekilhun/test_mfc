#pragma once
#include "afxdialogex.h"

#include "etc/toggle_btn.h"

// CtapIO 대화 상자

class CtapIO : public CDialogEx
{
	UINT_PTR	m_TimerID{};
	apcr::pc_api_t m_ptrApi{};
	DECLARE_DYNAMIC(CtapIO)

public:
	CtapIO(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CtapIO();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAP_IO_REG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();


	void update(void);
	std::vector<CToggleBtn> m_regs{ 16 };

	std::vector<CToggleBtn> m_status{ 16 };
	std::vector<CToggleBtn> m_option{ 32 };
	std::vector<CToggleBtn> m_alarm{ 32 };
	std::vector<CToggleBtn> m_regX{ 32 };
	std::vector<CToggleBtn> m_regY{ 32 };
	afx_msg void OnBnClickedCheckRegOptionWrite();
	afx_msg void OnBnClickedCheckRegOutWrite();
};
