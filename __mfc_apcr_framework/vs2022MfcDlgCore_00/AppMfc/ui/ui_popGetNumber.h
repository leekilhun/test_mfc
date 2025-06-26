#pragma once
#include "afxdialogex.h"


// Cui_popGetNumber 대화 상자

class Cui_popGetNumber : public CDialogEx
{
	HWND          m_hWndControl{};
	int						m_cyChar{};
	int						m_cxChar{};
	bool					m_numFlag{};
	CRect					m_rect{};

public:
	CString					m_strGetNum{};

protected:
	//int						m_nPointNum{};
	bool					m_isCal{};
	bool          m_isInitNumber{};
	bool					m_isCalPluse{};
	bool					m_firstFlag{true};
	bool					m_isPassNumber{};

	CString					m_strOrg{};
	CString					m_orgNum{}; // 가져온 숫자와 다른지 확인
	int						m_intMaxChar{};
	CString					m_strWndText{};

	// 컨트롤 변수
private:
	CBtnEnh					m_ctrlBtnSetValue;
	CBtnEnh					m_ctrlBtnDispNum;
	CBtnEnh					m_btnDot;
	COLORREF        m_changedColor{};

	DECLARE_DYNAMIC(Cui_popGetNumber)

public:
	Cui_popGetNumber(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	Cui_popGetNumber(int max_num, HWND hItem, LPCTSTR title = L"Insert Number Only.", COLORREF color = 0, CWnd* pParent = nullptr);
	Cui_popGetNumber(int max_num, CString& value, LPCTSTR title = L"Insert Number Only.", CWnd* pParent = nullptr);
	virtual ~Cui_popGetNumber();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GET_NUMBER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	void UpdateDisplay(COLORREF bkcolor = RGB(255, 255, 255));

protected:
	BOOL VerifyMaxChar();
	//void InitStaticDispWnd();

public:
	void SetWindowTitle(CString strTitle);

public:

	DECLARE_EVENTSINK_MAP()
	void ClickBtn0();
	void ClickBtn1();
	void ClickBtn2();
	void ClickBtn3();
	void ClickBtn4();
	void ClickBtn5();
	void ClickBtn6();
	void ClickBtn7();
	void ClickBtn8();
	void ClickBtn9();

	void ClickBtnDot();
	void ClickBtnSign();
	void ClickBtnEsc();
	void ClickBtnOk();
	void ClickBtnClear();
	void ClickBtnBack();
	void ClickBtnPlus();
	void ClickBtnMinus();
	void ClickBtnEqual();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
