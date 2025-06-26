// ui_popGetNumber.cpp: 구현 파일
//

#include "pch.h"

#include "afxdialogex.h"

#include "ui_def.hpp"

#include "ui_popGetNumber.h"

// Cui_popGetNumber 대화 상자

IMPLEMENT_DYNAMIC(Cui_popGetNumber, CDialogEx)


Cui_popGetNumber::Cui_popGetNumber(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GET_NUMBER, pParent)
{
	
}

Cui_popGetNumber::Cui_popGetNumber(int max_num, HWND hItem, LPCTSTR title, COLORREF color, CWnd* pParent)
	: CDialogEx(IDD_GET_NUMBER, pParent)
{

	m_hWndControl = hItem;
	m_intMaxChar = max_num;
	m_changedColor = color;

	constexpr auto max_length = 64;
	std::vector<wchar_t> buffer(max_length);
	::GetWindowText(m_hWndControl, buffer.data(), max_length);

	m_strWndText = CString{ title };
	m_strGetNum = CString{ buffer.data() };
	m_orgNum = m_strGetNum;
	m_isInitNumber = (cfu::get_number_type(m_strGetNum.GetString()) == cfu::nt_INTEGER);

}

Cui_popGetNumber::Cui_popGetNumber(int max_num, CString& value, LPCTSTR title, CWnd* pParent)
	: CDialogEx(IDD_GET_NUMBER, pParent)
{
	m_intMaxChar = max_num;
	m_strWndText = CString{ title };
	m_strGetNum = value;


	m_isInitNumber = (cfu::get_number_type(m_strGetNum.GetString()) == cfu::nt_INTEGER);
}

Cui_popGetNumber::~Cui_popGetNumber()
{
}


void Cui_popGetNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRESENT, m_ctrlBtnSetValue);
	DDX_Control(pDX, IDC_MODIFY, m_ctrlBtnDispNum);
	DDX_Control(pDX, IDB_BTN_DOT, m_btnDot);
}


BEGIN_MESSAGE_MAP(Cui_popGetNumber, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// Cui_popGetNumber 메시지 처리기





BEGIN_EVENTSINK_MAP(Cui_popGetNumber, CDialogEx)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_0, DISPID_CLICK, Cui_popGetNumber::ClickBtn0, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_1, DISPID_CLICK, Cui_popGetNumber::ClickBtn1, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_2, DISPID_CLICK, Cui_popGetNumber::ClickBtn2, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_3, DISPID_CLICK, Cui_popGetNumber::ClickBtn3, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_4, DISPID_CLICK, Cui_popGetNumber::ClickBtn4, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_5, DISPID_CLICK, Cui_popGetNumber::ClickBtn5, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_6, DISPID_CLICK, Cui_popGetNumber::ClickBtn6, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_7, DISPID_CLICK, Cui_popGetNumber::ClickBtn7, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_8, DISPID_CLICK, Cui_popGetNumber::ClickBtn8, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDC_BTN_9, DISPID_CLICK, Cui_popGetNumber::ClickBtn9, VTS_NONE)

	ON_EVENT(Cui_popGetNumber, IDB_BTN_DOT, DISPID_CLICK, Cui_popGetNumber::ClickBtnDot, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_SIGN, DISPID_CLICK, Cui_popGetNumber::ClickBtnSign, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_ESC, DISPID_CLICK, Cui_popGetNumber::ClickBtnEsc, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_OK, DISPID_CLICK, Cui_popGetNumber::ClickBtnOk, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_CLEAR, DISPID_CLICK, Cui_popGetNumber::ClickBtnClear, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_BACK, DISPID_CLICK, Cui_popGetNumber::ClickBtnBack, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_PLUS, DISPID_CLICK, Cui_popGetNumber::ClickBtnPlus, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_MINUS, DISPID_CLICK, Cui_popGetNumber::ClickBtnMinus, VTS_NONE)
	ON_EVENT(Cui_popGetNumber, IDB_BTN_EQUAL, DISPID_CLICK, Cui_popGetNumber::ClickBtnEqual, VTS_NONE)
END_EVENTSINK_MAP()



BOOL Cui_popGetNumber::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowText(m_strWndText);
	if (m_isInitNumber)
	{
		m_btnDot.SetEnabled(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



BOOL Cui_popGetNumber::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_NUMPAD0:
		case 0x30:
			ClickBtn0();
			break;
		case VK_NUMPAD1:
		case 0x31:
			ClickBtn1();
			break;
		case VK_NUMPAD2:
		case 0x32:
			ClickBtn2();
			break;
		case VK_NUMPAD3:
		case 0x33:
			ClickBtn3();
			break;
		case VK_NUMPAD4:
		case 0x34:
			ClickBtn4();
			break;
		case VK_NUMPAD5:
		case 0x35:
			ClickBtn5();
			break;
		case VK_NUMPAD6:
		case 0x36:
			ClickBtn6();
			break;
		case VK_NUMPAD7:
		case 0x37:
			ClickBtn7();
			break;
		case VK_NUMPAD8:
		case 0x38:
			ClickBtn8();
			break;
		case VK_NUMPAD9:
		case 0x39:
			ClickBtn9();
			break;
		case VK_BACK:
			ClickBtnBack();
			break;
		case VK_DECIMAL:
			ClickBtnDot();
			break;

		default: break;
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


void Cui_popGetNumber::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	UpdateDisplay();
}


void Cui_popGetNumber::SetWindowTitle(CString strTitle)
{

	m_strWndText = strTitle;
}


void Cui_popGetNumber::UpdateDisplay(COLORREF bkcolor)
{
	UNUSED_ALWAYS(bkcolor);

	//if (m_strGetNum != m_strOrg)
	//{

	//	m_ctrlBtnDispNum.SetForeColor(cdu::BLACK);
	//	m_ctrlBtnDispNum.SetBackColor(cdu::CYAN);
	//}


	m_ctrlBtnDispNum.SetCaption(m_strGetNum);
}


BOOL Cui_popGetNumber::VerifyMaxChar()
{
	if (m_strGetNum.GetLength() >= m_intMaxChar)
		return FALSE;
	else
		return TRUE;
}

//void Cui_popGetNumber::InitStaticDispWnd()
//{
//	CStatic* pRect = (CStatic*)GetDlgItem(IDC_MODIFY);
//	pRect->GetWindowRect(&m_rect);
//	pRect->DestroyWindow();
//	ScreenToClient(&m_rect);
//
//	TEXTMETRIC tm;
//	CClientDC dc(this);
//	dc.GetTextMetrics(&tm);
//	m_cxChar = tm.tmAveCharWidth;
//	m_cyChar = tm.tmHeight - tm.tmDescent;
//}


void Cui_popGetNumber::ClickBtnPlus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_firstFlag)
		return;

	m_isCal = true;
	m_isCalPluse = true;

	m_firstFlag = true;

	m_strOrg = m_strGetNum;
	m_strGetNum = _T("");

	m_ctrlBtnSetValue.SetCaption(m_strOrg);
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtnMinus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_firstFlag) return;

	m_isCal = true;
	m_isCalPluse = false;

	m_firstFlag = true;

	m_strOrg = m_strGetNum;
	m_strGetNum = _T("");

	m_ctrlBtnSetValue.SetCaption(m_strOrg);
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtnEqual()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	double fi, se, rel;
	CString strBuf;

	if (!m_isCal) return;

	fi = _tstof(m_strOrg);
	se = _tstof(m_strGetNum);

	if (m_isCalPluse)
		rel = (fi)+(se);
	else
		rel = (fi)-(se);

	if (m_isInitNumber)
		strBuf.Format(_T("%.0f"), rel);
	else
		strBuf.Format(_T("%.6f"), rel);

	m_strGetNum = strBuf;
	UpdateDisplay();

	m_firstFlag = true;
	m_isCal = FALSE;
}


void Cui_popGetNumber::ClickBtnDot()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_isPassNumber)
		return;

	if (m_isInitNumber)
		return;

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T(".");
	UpdateDisplay();
}

void Cui_popGetNumber::ClickBtnSign()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_isPassNumber)
		return;

	if (!m_strGetNum.GetLength())
		return;

	if (TRUE == m_numFlag)
	{
		m_strGetNum.Insert(0, _T('-'));
		m_numFlag = FALSE;
	}
	else
	{
		m_strGetNum.Remove(_T('-'));
		m_numFlag = true;
	}

	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtnBack()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_firstFlag = false;

	int len = m_strGetNum.GetLength();
	if (len)
	{
		m_strGetNum.SetAt(len - 1, _T(' '));
		m_strGetNum.TrimRight();
		UpdateDisplay();
	}
}


void Cui_popGetNumber::ClickBtnOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// //m_isInitNumber = (cfu::get_number_type(m_strGetNum.GetString()) == cfu::nt_INTEGER);
	if (m_isInitNumber)
	{
		if ((cfu::get_number_type(m_strGetNum.GetString()) == cfu::nt_INTEGER))
		{
			::SetWindowText(m_hWndControl, m_strGetNum);
		}
	}
	else
	{
		if ((cfu::get_number_type(m_strGetNum.GetString()) == cfu::nt_FLOAT))
		{
			::SetWindowText(m_hWndControl, m_strGetNum);
		}
		else
		{
			// cfu::get_number_type이 nt_FLOAT가 아닐 때 처리
			try
			{
				// 문자열을 float로 변환
				float number = std::stof(m_strGetNum.GetString());

				// 변환된 float 값을 문자열로 다시 변환
				CString strFormattedNum;
				strFormattedNum.Format(_T("%.2f"), number); // 소수점 둘째 자리까지 표현

				// 변환된 문자열을 설정
				::SetWindowText(m_hWndControl, strFormattedNum);
			}
			catch (const std::exception& e)
			{
				// 예외 정보 출력
				CString errorMsg;
				errorMsg.Format(_T("Invalid number format. Exception: %S"), e.what()); // e.what()은 char* 반환
				AfxMessageBox(errorMsg);
			}
		}
	}

	
	if (m_changedColor 
		&& (m_strGetNum != m_orgNum))
	{	
		// 컨트롤러 백그라운드 색상을 변경한다.
		
		CWnd* pMainControl = CWnd::FromHandle(m_hWndControl);
		if (pMainControl)
		{
			// 메인 다이얼로그의 컨트롤에 배경색 적용
			pMainControl->SetProperty(0x2d, VT_I4, (unsigned long)m_changedColor);
			pMainControl->Invalidate();  // 컨트롤 다시 그리기
		}
	}

	
	if (m_isPassNumber)
		DestroyWindow();
	else
		CDialog::OnOK();
}

void Cui_popGetNumber::ClickBtnEsc()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_isPassNumber)
		DestroyWindow();
	else
		CDialog::OnCancel();
}

void Cui_popGetNumber::ClickBtnClear()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_strGetNum = _T("");
	m_numFlag = true;
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn0()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;


	m_strGetNum += _T("0");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("1");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("2");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("3");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn4()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("4");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn5()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("5");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn6()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("6");
	UpdateDisplay();
}



void Cui_popGetNumber::ClickBtn7()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("7");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn8()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("8");
	UpdateDisplay();
}


void Cui_popGetNumber::ClickBtn9()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_firstFlag)
	{
		m_firstFlag = false;
		m_numFlag = true;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar())
		return;

	m_strGetNum += _T("9");
	UpdateDisplay();
}


HBRUSH Cui_popGetNumber::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
