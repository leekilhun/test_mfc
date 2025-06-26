// ui_popGetValue.cpp: 구현 파일
//

#include "pch.h"

#include "afxdialogex.h"

#include "ui_def.hpp"
#include "ui_popGetValue.h"


// Cui_popGetValue 대화 상자

IMPLEMENT_DYNAMIC(Cui_popGetValue, CDialogEx)

Cui_popGetValue::Cui_popGetValue(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GET_VALUE, pParent)
{

}

Cui_popGetValue::Cui_popGetValue(CString* get_value, int iMaxchar, CString strTitle, bool bPwd, CWnd* pParent)
	: CDialogEx(IDD_GET_VALUE, pParent)
{
	m_intMaxChar = iMaxchar;
	m_bpwd = bPwd;
	m_strWndText = strTitle;

	m_retHwd = get_value;

	m_strGetVal = *get_value;

}

Cui_popGetValue::Cui_popGetValue(HWND hItem, int iMaxchar, CString strTitle, bool bPwd, CWnd* pParent)
	: CDialogEx(IDD_GET_VALUE, pParent)
{
	m_hWndControl = hItem;

	m_intMaxChar = iMaxchar;
	m_bpwd = bPwd;
	m_strWndText = strTitle;

	constexpr auto max_length = 512;
	std::vector<wchar_t> buffer(max_length);
	::GetWindowText(m_hWndControl, buffer.data(), max_length);
	m_strGetVal = CString{ buffer.data() };

}



Cui_popGetValue::~Cui_popGetValue()
{
}

void Cui_popGetValue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISP_VAL, m_dispValue);
	DDX_Control(pDX, IDB_KEY_DOT, m_dot);
	DDX_Control(pDX, IDB_KEY_EQUAL, m_equal);
	DDX_Control(pDX, IDB_KEY_COLON, m_colon);
	DDX_Control(pDX, IDB_KEY_WON, m_won);
}


BEGIN_MESSAGE_MAP(Cui_popGetValue, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(Cui_popGetValue, CDialogEx)
	ON_EVENT(Cui_popGetValue, IDB_KEY_OK, DISPID_CLICK, Cui_popGetValue::ClickKeyOk, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_ESC, DISPID_CLICK, Cui_popGetValue::ClickKeyEsc, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_BACK, DISPID_CLICK, Cui_popGetValue::ClickKeyBack, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_CLEAR, DISPID_CLICK, Cui_popGetValue::ClickKeyClear, VTS_NONE)

	ON_EVENT(Cui_popGetValue, IDB_KEY_1, DISPID_CLICK, Cui_popGetValue::ClickKey1, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_2, DISPID_CLICK, Cui_popGetValue::ClickKey2, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_3, DISPID_CLICK, Cui_popGetValue::ClickKey3, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_4, DISPID_CLICK, Cui_popGetValue::ClickKey4, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_5, DISPID_CLICK, Cui_popGetValue::ClickKey5, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_6, DISPID_CLICK, Cui_popGetValue::ClickKey6, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_7, DISPID_CLICK, Cui_popGetValue::ClickKey7, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_8, DISPID_CLICK, Cui_popGetValue::ClickKey8, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_9, DISPID_CLICK, Cui_popGetValue::ClickKey9, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_0, DISPID_CLICK, Cui_popGetValue::ClickKey0, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_Q, DISPID_CLICK, Cui_popGetValue::ClickKeyQ, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_W, DISPID_CLICK, Cui_popGetValue::ClickKeyW, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_E, DISPID_CLICK, Cui_popGetValue::ClickKeyE, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_R, DISPID_CLICK, Cui_popGetValue::ClickKeyR, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_T, DISPID_CLICK, Cui_popGetValue::ClickKeyT, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_Y, DISPID_CLICK, Cui_popGetValue::ClickKeyY, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_U, DISPID_CLICK, Cui_popGetValue::ClickKeyU, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_I, DISPID_CLICK, Cui_popGetValue::ClickKeyI, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_O, DISPID_CLICK, Cui_popGetValue::ClickKeyO, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_P, DISPID_CLICK, Cui_popGetValue::ClickKeyP, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_A, DISPID_CLICK, Cui_popGetValue::ClickKeyA, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_S, DISPID_CLICK, Cui_popGetValue::ClickKeyS, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_D, DISPID_CLICK, Cui_popGetValue::ClickKeyD, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_F, DISPID_CLICK, Cui_popGetValue::ClickKeyF, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_G, DISPID_CLICK, Cui_popGetValue::ClickKeyG, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_H, DISPID_CLICK, Cui_popGetValue::ClickKeyH, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_J, DISPID_CLICK, Cui_popGetValue::ClickKeyJ, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_K, DISPID_CLICK, Cui_popGetValue::ClickKeyK, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_L, DISPID_CLICK, Cui_popGetValue::ClickKeyL, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_Z, DISPID_CLICK, Cui_popGetValue::ClickKeyZ, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_X, DISPID_CLICK, Cui_popGetValue::ClickKeyX, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_C, DISPID_CLICK, Cui_popGetValue::ClickKeyC, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_V, DISPID_CLICK, Cui_popGetValue::ClickKeyV, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_B, DISPID_CLICK, Cui_popGetValue::ClickKeyB, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_N, DISPID_CLICK, Cui_popGetValue::ClickKeyN, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_M, DISPID_CLICK, Cui_popGetValue::ClickKeyM, VTS_NONE)

	ON_EVENT(Cui_popGetValue, IDB_KEY_EQUAL, DISPID_CLICK, Cui_popGetValue::ClickKeyEqual, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_WON, DISPID_CLICK, Cui_popGetValue::ClickKeyWon, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_COLON, DISPID_CLICK, Cui_popGetValue::ClickKeyColon, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_DOT, DISPID_CLICK, Cui_popGetValue::ClickKeyDot, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_DASH, DISPID_CLICK, Cui_popGetValue::ClickKeyDash, VTS_NONE)
	ON_EVENT(Cui_popGetValue, IDB_KEY_UNDER_BAR, DISPID_CLICK, Cui_popGetValue::ClickKeyUnderBar, VTS_NONE)
	
END_EVENTSINK_MAP()
// Cui_popGetValue 메시지 처리기


void Cui_popGetValue::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	UpdateDisplay();
}

BOOL Cui_popGetValue::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_intMaxChar <= 0)
		m_intMaxChar = 1;

	if (m_bbtndisable)
		btnDisable();

	SetWindowText(m_strWndText);

	m_dispValue.SetCaption(m_strGetVal);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL Cui_popGetValue::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_NUMPAD0:
		case 0x30:
			ClickKey0();
			break;
		case VK_NUMPAD1:
		case 0x31:
			ClickKey1();
			break;
		case VK_NUMPAD2:
		case 0x32:
			ClickKey2();
			break;
		case VK_NUMPAD3:
		case 0x33:
			ClickKey3();
			break;
		case VK_NUMPAD4:
		case 0x34:
			ClickKey4();
			break;
		case VK_NUMPAD5:
		case 0x35:
			ClickKey5();
			break;
		case VK_NUMPAD6:
		case 0x36:
			ClickKey6();
			break;
		case VK_NUMPAD7:
		case 0x37:
			ClickKey7();
			break;
		case VK_NUMPAD8:
		case 0x38:
			ClickKey8();
			break;
		case VK_NUMPAD9:
		case 0x39:
			ClickKey9();
			break;
		case VK_BACK:
			ClickKeyBack();
			break;
		case VK_DECIMAL:
			ClickKeyDot();
			break;
		case 0x41:
			ClickKeyA();
			break;
		case 0x42:
			ClickKeyB();
			break;
		case 0x43:
			ClickKeyC();
			break;
		case 0x44:
			ClickKeyD();
			break;
		case 0x45:
			ClickKeyE();
			break;
		case 0x46:
			ClickKeyF();
			break;
		case 0x47:
			ClickKeyG();
			break;
		case 0x48:
			ClickKeyH();
			break;
		case 0x49:
			ClickKeyI();
			break;
		case 0x4A:
			ClickKeyJ();
			break;
		case 0x4B:
			ClickKeyK();
			break;
		case 0x4C:
			ClickKeyL();
			break;
		case 0x4D:
			ClickKeyM();
			break;
		case 0x4E:
			ClickKeyN();
			break;
		case 0x4F:
			ClickKeyO();
			break;
		case 0x50:
			ClickKeyP();
			break;
		case 0x51:
			ClickKeyQ();
			break;
		case 0x52:
			ClickKeyR();
			break;
		case 0x53:
			ClickKeyS();
			break;
		case 0x54:
			ClickKeyT();
			break;
		case 0x55:
			ClickKeyU();
			break;
		case 0x56:
			ClickKeyV();
			break;
		case 0x57:
			ClickKeyW();
			break;
		case 0x58:
			ClickKeyX();
			break;
		case 0x59:
			ClickKeyY();
			break;
		case 0x5A:
			ClickKeyZ();
			break;
		case 0x5F:
			ClickKeyUnderBar();
			break;
		case VK_RETURN:
			ClickKeyOk();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void Cui_popGetValue::btnDisable()
{
	// TODO: Add your control notification handler code here
	m_dot.EnableWindow(FALSE);
	m_colon.EnableWindow(FALSE);
	m_won.EnableWindow(FALSE);
	m_equal.EnableWindow(FALSE);
}

void Cui_popGetValue::ClickKeyOk()
{
	if (m_hWndControl)
	{
		::SetWindowText(m_hWndControl, m_strGetVal);

	}
	else if(m_retHwd)
	{
		m_retHwd->SetString(m_strGetVal.GetString());
	}

	CDialog::OnOK();
}

void Cui_popGetValue::ClickKeyEsc()
{
	CDialog::OnCancel();
}

void Cui_popGetValue::ClickKeyBack()
{
	int len = m_strGetVal.GetLength();
	if (len)
	{
		m_strGetVal.SetAt(len - 1, _T(' '));
		m_strGetVal.TrimRight();
		UpdateDisplay();
	}
}

void Cui_popGetValue::ClickKeyClear()
{
	m_strGetVal = _T("");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey1()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("1");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey2()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("2");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey3()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("3");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey4()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("4");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey5()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("5");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey6()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("6");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey7()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("7");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey8()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("8");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey9()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("9");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKey0()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("0");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyQ()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("Q");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyW()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("W");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyE()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("E");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyR()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("R");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyT()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("T");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyY()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("Y");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyU()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("U");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyI()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("I");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyO()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("O");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyP()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("P");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyA()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("A");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyS()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("S");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyD()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("D");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyF()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("F");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyG()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("G");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyH()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("H");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyJ()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("J");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyK()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("K");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyL()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("L");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyZ()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("Z");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyX()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("X");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyC()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("C");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyV()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("V");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyB()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("B");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyN()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("N");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyM()
{
	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("M");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyEqual()
{
	if (m_bpwd)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("=");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyWon()
{
	if (m_bpwd)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("\\");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyColon()
{
	if (m_bpwd)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T(":");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyDot()
{
	if (m_bpwd)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T(".");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyDash()
{
	if (m_bpwd)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("-");
	UpdateDisplay();
}

void Cui_popGetValue::ClickKeyUnderBar()
{
	if (m_bpwd)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetVal += _T("_");
	UpdateDisplay();
}

BOOL Cui_popGetValue::VerifyMaxChar()
{
	if (m_strGetVal.GetLength() >= m_intMaxChar)
		return FALSE;
	else
		return TRUE;
}

void Cui_popGetValue::UpdateDisplay(COLORREF bkcolor)
{
	UNUSED_ALWAYS(bkcolor);

	int len, i;
	CString bufstr;

	if (m_bpwd)
	{
		len = m_strGetVal.GetLength();
		if (len <= 0)
		{
			bufstr = _T("");
			m_dispValue.SetCaption(bufstr);
			return;
		}

		for (i = 0; i < len; i++)
			bufstr += _T("*");

		m_dispValue.SetCaption(bufstr);
	}
	else
	{
		m_dispValue.SetCaption(m_strGetVal);
	}
}

void Cui_popGetValue::InitStaticDispWnd()
{
	CStatic* pRect = (CStatic*)GetDlgItem(IDC_DISP_VAL);
	pRect->GetWindowRect(&m_rect);
	pRect->DestroyWindow();
	ScreenToClient(&m_rect);

	TEXTMETRIC tm;
	CClientDC dc(this);
	dc.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;
	m_cyChar = tm.tmHeight - tm.tmDescent;
}

CString Cui_popGetValue::GetStringValue()
{
	return m_strGetVal;
}

