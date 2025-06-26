#pragma once

//*----- For MyMessageBox --------*//
#define  M_ICONINFORMATION    0x01
#define  M_ICONQUESTION       0x02 
#define  M_ICONSTOP           0x04 
#define  M_ICONERROR          0x08 	

#define  M_OK                 0x10
#define  M_OKCANCEL           0x20
#define  M_YESNO              0x40
#define  M_ABORTRETRYIGNORE   0x80
#define  M_RETRYCANCEL        0x100



class CmsgWnd :   public CWnd
{
// Construction
public:
	HBRUSH m_hbrush;

	CmsgWnd(bool pos = true);
	CmsgWnd(CWnd* pParent, bool pos = true);
	BOOL Create(CWnd* pParent);

	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMyMsgWnd)
		//}}AFX_VIRTUAL

	// Implementation
public:
	bool m_MostTopFlag;
	void ResetFlag();
	bool m_OkFlag;
	bool m_CancelFlag;
	void PeekAndPump();

	void UpdateDisplay(COLORREF bkcolor);
	void SetMessage(LPCTSTR fmt, ...);
	void Show();
	virtual ~CmsgWnd();

	// Generated message map functions
protected:
	bool m_pos;
	COLORREF BkColor;
	CString m_strTitle;
	void CommonConstruct();
	CStatic m_Message;
	//{{AFX_MSG(CMyMsgWnd)
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	TCHAR m_cMessage[512];
	void DoSolidFill(CDC* pDC, CRect* pRect);
	void DoGradientFill(CDC*, CRect*);

public:
	UINT ShowMsgBox(CString strMessage, CString strTitle, UINT type, CString strYesBut, CString strNoBut);

};

