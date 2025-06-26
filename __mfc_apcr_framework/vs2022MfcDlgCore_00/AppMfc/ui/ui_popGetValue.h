#pragma once
#include "afxdialogex.h"


// Cui_popGetValue 대화 상자

class Cui_popGetValue : public CDialogEx
{

	HWND          m_hWndControl{};
	CString* m_retHwd{};

private:
	CBtnEnh			m_dispValue;
	CBtnEnh			m_dot;
	CBtnEnh			m_equal;
	CBtnEnh			m_colon;
	CBtnEnh			m_won;

public:
	CString			m_strGetVal;

private:
	BOOL			m_bpwd{};
	BOOL			m_bbtndisable{};
	int				m_intMaxChar{};

	CString			m_strWndText{};

	int				m_cxChar{};
	int				m_cyChar{};
	CRect			m_rect{};

	DECLARE_DYNAMIC(Cui_popGetValue)

public:

	Cui_popGetValue(CString *get_value, int iMaxchar = 10,
		CString strTitle = _T("Input the Value."),
		bool bPwd = false, CWnd* pParent = nullptr);   // 표준 생성자입니다.

	Cui_popGetValue(HWND hItem, int iMaxchar = 10,
		CString strTitle = _T("Input the Value."),
		bool bPwd = false, CWnd* pParent = nullptr);   // 표준 생성자입니다.

	Cui_popGetValue(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Cui_popGetValue();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GET_VALUE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();


public:
	DECLARE_EVENTSINK_MAP()
	void ClickKeyOk();
	void ClickKeyEsc();
	void ClickKeyBack();
	void ClickKeyClear();
	void ClickKey1();
	void ClickKey2();
	void ClickKey3();
	void ClickKey4();
	void ClickKey5();
	void ClickKey6();
	void ClickKey7();
	void ClickKey8();
	void ClickKey9();
	void ClickKey0();
	void ClickKeyQ();
	void ClickKeyW();
	void ClickKeyE();
	void ClickKeyR();
	void ClickKeyT();
	void ClickKeyY();
	void ClickKeyU();
	void ClickKeyI();
	void ClickKeyO();
	void ClickKeyP();
	void ClickKeyA();
	void ClickKeyS();
	void ClickKeyD();
	void ClickKeyF();
	void ClickKeyG();
	void ClickKeyH();
	void ClickKeyJ();
	void ClickKeyK();
	void ClickKeyL();
	void ClickKeyZ();
	void ClickKeyX();
	void ClickKeyC();
	void ClickKeyV();
	void ClickKeyB();
	void ClickKeyN();
	void ClickKeyM();
	void ClickKeyEqual();
	void ClickKeyWon();
	void ClickKeyColon();
	void ClickKeyDot();
	void ClickKeyDash();
	void ClickKeyUnderBar();

private:

	void btnDisable();

	BOOL VerifyMaxChar();

	void UpdateDisplay(COLORREF bkcolor = RGB(255, 255, 255));

public:
	void InitStaticDispWnd();

	CString GetStringValue();
};
