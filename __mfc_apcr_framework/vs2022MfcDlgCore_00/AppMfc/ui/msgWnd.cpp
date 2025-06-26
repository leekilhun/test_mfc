#include "pch.h"
#include "msgWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define IDC_MYMESSAGEWND		20

/////////////////////////////////////////////////////////////////////////////
// CmsgWnd

CmsgWnd::CmsgWnd(bool pos/*=true*/)
{
	m_MostTopFlag = false;
	m_hbrush = NULL;
	m_pos = pos;
	m_CancelFlag = false;
	m_OkFlag = false;
	CommonConstruct();
	m_strTitle = "Message";
	Create(NULL);
}

CmsgWnd::CmsgWnd(CWnd* pParent, bool pos/*=true*/)
{
	m_hbrush = NULL;
	m_pos = pos;
	m_CancelFlag = false;
	m_OkFlag = false;
	CommonConstruct();
	m_strTitle = "Message";
	Create(pParent);
}

void CmsgWnd::CommonConstruct()
{
	BkColor = RGB(125, 125, 255);
	m_strTitle = _T("Message");
}

CmsgWnd::~CmsgWnd()
{
	if (m_hbrush) ::DeleteObject(m_hbrush);

	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CmsgWnd, CWnd)
	//{{AFX_MSG_MAP(CmsgWnd)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CmsgWnd message LASERs

//bool CmsgWnd::Create(CWnd* pParent, LPCTSTR pszTitle)
BOOL CmsgWnd::Create(CWnd* pParent)
{
	// TODO: Add your specialized code here and/or call the base class
	bool bSuccess;

	// Register window class
	CString csClassName = AfxRegisterWndClass(CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		::LoadCursor(NULL, IDC_WAIT),
		CBrush(::GetSysColor(COLOR_BTNFACE)));


	// If no parent supplied then try and get a pointer to it anyway
	if (!pParent) pParent = AfxGetMainWnd();

	// Create popup window
	bSuccess = CreateEx(WS_EX_DLGMODALFRAME,            // Extended style
		csClassName,                    // Classname
		//                      pszTitle,                       // Title
		m_strTitle,
		WS_POPUP | WS_BORDER | WS_CAPTION,  // style
		0, 0,                            // position - updated soon.
		390, 130,                        // Size - updated soon
		pParent->GetSafeHwnd(),         // handle to parent
		0,                              // No menu
		NULL);
	if (!bSuccess) return false;

	// Now create the controls
	CRect TempRect(0, 0, 10, 10);

	bSuccess = m_Message.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOPREFIX | SS_LEFTNOWORDWRAP,
		TempRect, this, IDC_MYMESSAGEWND);

	if (!bSuccess) return false;

	GetWindowRect(TempRect);
	TempRect.left = 120;
	TempRect.top = 320;
	TempRect.right = 520;
	TempRect.bottom = 450;
	MoveWindow(TempRect);

	TempRect.SetRect(10, 15, 385, 85);
	m_Message.MoveWindow(TempRect);

	// Center and show window
	int sys_width = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int sys_height = ::GetSystemMetrics(SM_CYFULLSCREEN);

	if (m_pos) CenterWindow();
	else
	{
		GetWindowRect(&TempRect);
		int x = (sys_width - (TempRect.right - TempRect.left)) / 2;
		int width = TempRect.right - TempRect.left;
		int y = TempRect.bottom - TempRect.top;

		TempRect.left = x;
		TempRect.right = x + width;
		TempRect.bottom = sys_height - 20;
		TempRect.top = TempRect.bottom - y;

		MoveWindow(TempRect);
	}

	Show();

	if (m_MostTopFlag)
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return true;
}

void CmsgWnd::DoGradientFill(CDC* pDC, CRect* pRect)
{
	int i;
	CBrush* pBrush[64];

	for (i = 0; i < 64; i++)
		pBrush[i] = new CBrush(RGB(0, 255 - (i * 4), 255 - (i * 4)));

	int nWidth = pRect->Width();
	int nHeight = pRect->Height();
	CRect rect;

	for (i = 0; i < nHeight; i++)
	{
		rect.SetRect(0, i, nWidth, i + 1);
		pDC->FillRect(&rect, pBrush[(i * 63) / nHeight]);
	}

	for (i = 0; i < 64; i++)
		delete pBrush[i];
}

void CmsgWnd::UpdateDisplay(COLORREF bkcolor)
{
	CRect mrect;
	m_Message.GetWindowRect(&mrect);
	mrect.InflateRect(2, 2);
	ScreenToClient(&mrect);
	CClientDC dc(this);
	dc.SetBkColor(bkcolor);
	dc.DrawEdge(mrect, EDGE_SUNKEN, BF_RECT);
}

void CmsgWnd::Show()
{
	if (!::IsWindow(GetSafeHwnd()))
		return;

	if ((::GetWindowLong(GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE) == 0)
	{
		ModifyStyle(0, WS_VISIBLE);
		ShowWindow(SW_SHOWNA);
		RedrawWindow(NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
	//	SetFocus();
}

void CmsgWnd::DoSolidFill(CDC* pDC, CRect* pRect)
{
	CBrush pBrush(BkColor);
	pDC->FillRect(pRect, &pBrush);
}

BOOL CmsgWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message LASER code here and/or call default
	CBrush backBrush(GetSysColor(COLOR_BTNFACE));
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);     // Erase the area needed
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	backBrush.DeleteObject();

	//	DoGradientFill(pDC, &rect);
	DoSolidFill(pDC, &rect);

	return true;
}

void CmsgWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message LASER code here
	UpdateDisplay(RGB(255, 155, 255));
	// Do not call CDialog::OnPaint() for painting messages
}

HBRUSH CmsgWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	HWND hwnd = pWnd->GetSafeHwnd();
	if (hwnd == GetDlgItem(IDC_MYMESSAGEWND)->m_hWnd)
	{
		COLORREF textColor = RGB(0, 0, 0);		// TEXT Color
		COLORREF backColor = RGB(255, 255, 255);	// Background Color

		if (m_hbrush) ::DeleteObject(m_hbrush);

		pDC->SetTextColor(textColor);
		pDC->SetBkColor(backColor);
		pDC->SetBkMode(TRANSPARENT);			// 글자를 쓸때 배경이 변경 안되게..

		m_hbrush = ::CreateSolidBrush(backColor);	// Control의 바탕색

		return m_hbrush;
	}

	return hbr; // TODO: Return a different brush if the default is not desired
}

void CmsgWnd::SetMessage(LPCTSTR fmt, ...)
{
	if (!::IsWindow(GetSafeHwnd())) return;

	va_list args;

	va_start(args, fmt);
	_vstprintf_s(m_cMessage, _countof(m_cMessage), fmt, args);
	va_end(args);

	m_Message.SetWindowText(m_cMessage);
}

void CmsgWnd::PeekAndPump()
{
	MSG msg;

	//	SetFocus();

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (msg.message == WM_CHAR)
		{
			switch (msg.wParam)
			{
			case VK_ESCAPE: m_CancelFlag = true; break;
			case VK_BACK:
			case VK_F12: m_OkFlag = true; break;
			default: break;
			}
		}

		if (!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return;
		}
	}
}

void CmsgWnd::ResetFlag()
{
	m_CancelFlag = false;
	m_OkFlag = false;
}

#if 0
UINT CmsgWnd::ShowMsgBox(CString strMessage, CString strTitle, UINT type, CString strYesBut, CString strNoBut)
{


	CCommonMsgDlg dlg;



	if (type & M_ICONINFORMATION) dlg.m_uiIcontype = M_ICONINFORMATION;
	else if (type & M_ICONQUESTION) dlg.m_uiIcontype = M_ICONQUESTION;
	else if (type & M_ICONSTOP) dlg.m_uiIcontype = M_ICONSTOP;
	else if (type & M_ICONERROR) dlg.m_uiIcontype = M_ICONERROR;
	else dlg.m_uiIcontype = M_ICONINFORMATION;

	if (type & M_OK) dlg.m_uiType = M_OK;
	else if (type & M_OKCANCEL) dlg.m_uiType = M_OKCANCEL;
	else if (type & M_YESNO) dlg.m_uiType = M_YESNO;
	else if (type & M_ABORTRETRYIGNORE) dlg.m_uiType = M_ABORTRETRYIGNORE;
	else if (type & M_RETRYCANCEL) dlg.m_uiType = M_RETRYCANCEL;
	else dlg.m_uiType = M_OK;

	dlg.m_strMessage = strMessage;
	dlg.m_strTitle = strTitle;
	dlg.m_strYes = strYesBut;
	dlg.m_strNo = strNoBut;

	return (UINT(dlg.DoModal()));
}

#endif