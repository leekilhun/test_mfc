#pragma once
#include "afxdialogex.h"

// Cui_popErrMsg 대화 상자

class Cui_popErrMsg : public CDialogEx
{

	/** 배경색을 칠할 브러쉬 변수 */
	CBrush				m_brBckColor{};

	CBtnEnh m_btnOK{};
	CBtnEnh m_btnInfo{};
	CBtnEnh m_btnBuzzerOff{};

	CBtnEnh m_prcText{};
	CBtnEnh m_unitText{};
	CBtnEnh m_msgText{};
	CBtnEnh m_infoText{};
	CBtnEnh m_infoTitle{};


	uint32_t m_objID{};
	uint32_t m_instanNo{};
	uint32_t m_errCode{};
	int m_lineNo{};
	CString m_fileInfo{};
	CString m_funcInfo{};
	CString m_errMsg{};
	UINT_PTR m_TimerID{};
public:
	void SetInfo(pop_alarm_t& data);


	DECLARE_DYNAMIC(Cui_popErrMsg)

public:
	Cui_popErrMsg(pop_alarm_t* err, CWnd* pParent = nullptr);

	Cui_popErrMsg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Cui_popErrMsg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POP_TASK_ERR_MSG	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	void ClickBuzzerOff();
	void ClickOk();
	void ClickInfo();


private:
	void viewDefault();
	void viewInfo();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
