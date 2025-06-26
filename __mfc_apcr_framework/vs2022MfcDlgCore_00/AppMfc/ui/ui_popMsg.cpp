// ui_popMsg.cpp: 구현 파일
//

#include "pch.h"

#include "ui_def.hpp"

#include "afxdialogex.h"
#include "ui_popMsg.h"


// Cui_popMsg 대화 상자

IMPLEMENT_DYNAMIC(Cui_popMsg, CDialogEx)

Cui_popMsg::Cui_popMsg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MSG_BOX1, pParent), m_pParent(pParent)
	, m_form_type()
{
	m_brBckColor.CreateSolidBrush(RGB(255, 255, 255));
	m_cx = 110;
	m_cy = 16;
}

Cui_popMsg::Cui_popMsg(Set_t& set_data, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MSG_BOX1, pParent), m_pParent(nullptr), m_form_type(set_data.form_type)
	, m_title(set_data.title), m_message(set_data.message), m_cx(110), m_cy(16)
{
	m_brBckColor.CreateSolidBrush(RGB(255, 255, 255));

}

Cui_popMsg::~Cui_popMsg()
{
	m_brBckColor.DeleteObject();
	m_fontStyle.DeleteObject();
}

void Cui_popMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_EDIT_VIEW, m_editView);
	DDX_Control(pDX, IDC_MSG_BOX_YES, m_btnYes);
	DDX_Control(pDX, IDC_MSG_BOX_OK, m_btnOK);
	DDX_Control(pDX, IDC_MSG_BOX_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_MSG_BOX_RETRY, m_btnRetry);
	DDX_Control(pDX, IDC_MSG_BOX_NO, m_btnNO);
}


BEGIN_MESSAGE_MAP(Cui_popMsg, CDialogEx)
	ON_BN_CLICKED(IDC_MSG_BOX_OK, &Cui_popMsg::OnBnClickedMsgBoxOk)
	ON_BN_CLICKED(IDC_MSG_BOX_CANCEL, &Cui_popMsg::OnBnClickedMsgBoxCancel)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MSG_BOX_NO, &Cui_popMsg::OnBnClickedMsgBoxNo)
	ON_BN_CLICKED(IDC_MSG_BOX_RETRY, &Cui_popMsg::OnBnClickedMsgBoxRetry)
	ON_BN_CLICKED(IDC_MSG_BOX_YES, &Cui_popMsg::OnBnClickedMsgBoxYes)
END_MESSAGE_MAP()


// Cui_popMsg 메시지 처리기


BOOL Cui_popMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// 그림 출력에 사용하기 위해 Picture Control의 위치를 얻는다.
	GetDlgItem(IDC_FRAME_PICTURE)->GetWindowRect(m_imageRect);
	ScreenToClient(m_imageRect);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void Cui_popMsg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// Set view edit  
	setViewWindow();
	viewDefault();
	drawImage();

}



void Cui_popMsg::viewDefault()
{
	// TODO: 여기에 구현 코드 추가.
#if 0 // 화면 중앙에
	CRect rect;

	GetClientRect(&rect);
	CPoint pos;
	pos.x = GetSystemMetrics(SM_CXSCREEN) / 2.0f - rect.Width() / 2.0f;
	pos.y = GetSystemMetrics(SM_CYSCREEN) / 2.0f - rect.Height() / 2.0f;;

	SetWindowPos(NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE);
#endif

	//부모창의 위치를 받아서 정의하는 방법	 
	RECT  rectParent;
	CRect rect;
	GetWindowRect(&rectParent);// GetWindowRect는 window에서 현재 어플리케이션창의 영역을 반환한다.
	GetClientRect(&rect); // 출력할 다이얼로그의 영역을 얻는다.
	CPoint pos;

	// 시작지점 지정
	pos.x = rectParent.left + (rectParent.right - rectParent.left) / 2 - rect.Width() / 2;
	pos.y = rectParent.top + (rectParent.bottom - rectParent.top) / 2 - rect.Height() / 2;


	// 표시위치 지정
	SetWindowPos(NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE);
}


// 프레임에 이미지를 그린다
void Cui_popMsg::drawImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_image.IsNull())
	{
		m_image.Load(L"res/infoicon.bmp");
	}

	InvalidateRect(m_imageRect, FALSE);
}


void Cui_popMsg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	// 배경색 칠하기
	CRect rectArea;
	GetClientRect(&rectArea);
	dc.FillRect(rectArea.operator LPRECT(), &m_brBckColor);



	// 항상 위 처리
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (IsIconic())
	{
		/*
		  SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		  int cx_icon = GetSystemMetrics(SM_CXICON);
		  int cy_icon = GetSystemMetrics(SM_CYICON);

		  CRect rect;
		  GetClientRect(&rect);
		  int x = (rect.Width() - cx_icon + 1) / 2;
		  int y = (rect.Height() - cy_icon + 1) / 2;

		  dc.DrawIcon(x, y, m_hIcon);
		  */
	}
	else
	{
		if (!m_image.IsNull())
		{
			dc.SetStretchBltMode(COLORONCOLOR);

			m_image.Draw(dc, m_imageRect);
		}
	}


}


BOOL Cui_popMsg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//CRect rect;
	//GetClientRect(rect);
	//pDC->FillSolidRect(rect, RGB(255, 255, 0));



	return CDialogEx::OnEraseBkgnd(pDC);
}


HBRUSH Cui_popMsg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if (pWnd->GetDlgCtrlID() == IDC_MSG_EDIT_VIEW)
	{
		pDC->SetBkColor(RGB(255, 255, 255));  //글자배경 색
		hbr = ::CreateSolidBrush(RGB(255, 255, 255));  // 배경 색 
	}


	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


// 내용이 나타나는 창의 스타일 셋팅
void Cui_popMsg::setViewWindow()
{
	// TODO: 여기에 구현 코드 추가.

	switch (m_form_type & 0b00001111)
	{
	case MB_OK:
		m_btnOK.ShowWindow(SW_SHOW);
		break;

	case MB_OKCANCEL:

		m_btnOK.ShowWindow(SW_SHOW);
		m_btnCancel.ShowWindow(SW_SHOW);
		break;

	case MB_YESNO:
		m_btnYes.ShowWindow(SW_SHOW);
		m_btnNO.ShowWindow(SW_SHOW);
		break;

	case MB_ABORTRETRYIGNORE:
		break;

	case MB_RETRYCANCEL:
		m_btnRetry.ShowWindow(SW_SHOW);
		m_btnCancel.ShowWindow(SW_SHOW);
		break;

	default:
		m_btnOK.ShowWindow(SW_SHOW);
		break;
	}


	switch (m_form_type & 0b11110000)
	{
	case MB_ICONERROR://stop , error
		m_image.Load(L"res/erricon.bmp");
		break;

	case MB_ICONQUESTION:
		m_image.Load(L"res/quesicon.bmp");
		break;

	case MB_ICONWARNING:
		m_image.Load(L"res/infoicon.bmp");
		break;

	default:
		m_image.Load(L"res/stopicon.bmp");
		break;
	}



	moveButton(m_form_type & 0b00001111);

	::SetWindowText(GetSafeHwnd(), m_title);


	m_editView.SetWindowText(m_message); // for test

	/*

	CreateFont args
	문자폭, 문자높이,  기울기, 방향, 굵기, 기울기, 밑줄, 취소선, 문자셋, 출력 정확도, 클리핑 정확도, 출력의 질, 자간과 폰트

	*/

	// font set
	m_fontStyle.CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
		L"Tahoma");
	m_editView.SetFont(&m_fontStyle, 1);


	// 글자를 window 가운데 배치 되도록 한다.
	CClientDC dc(&m_editView);
	dc.SelectObject(m_editView.GetFont());
	CString cs_text; m_editView.GetWindowText(cs_text);
	CSize sz_text = dc.GetTextExtent(cs_text);
	CRect rt;
	m_editView.GetClientRect(&rt);
	CSize sz_client = rt.Size();
	rt.left += (sz_client.cx - sz_text.cx - 2) / 2;
	rt.right -= (sz_client.cx - sz_text.cx - 2) / 2;
	if (m_message.Find(L"\n") == 0)
	{
		rt.top += (sz_client.cy - sz_text.cy) / 2;
		rt.bottom -= (sz_client.cy - sz_text.cy) / 2;
		m_editView.SetRect(&rt);
	}


}


BOOL Cui_popMsg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			pMsg->message = 0;
			pMsg->wParam = 0;
		}

		if (VK_DELETE == pMsg->wParam
			|| VK_RETURN == pMsg->wParam)
		{
			clickOk();
		}

		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}






void Cui_popMsg::OnBnClickedMsgBoxOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDOK);
}


void Cui_popMsg::OnBnClickedMsgBoxCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDCANCEL);
}





void Cui_popMsg::clickOk()
{
	// TODO: 여기에 구현 코드 추가.
	CDialog::OnOK();
}


void Cui_popMsg::moveButton(UINT type)
{
	// TODO: 여기에 구현 코드 추가.
	constexpr int dialog_header_height = 30;
	CRect	dialog_rect;
	CPoint	 dialog_left_top;
	GetWindowRect(&dialog_rect);

	dialog_left_top = dialog_rect.TopLeft();

	CRect  btn_rect;
	CPoint btn_left_top;





	switch (type)
	{
	case MB_OK:
	{
		// OK  
		m_btnOK.GetWindowRect(&btn_rect);
		btn_left_top = btn_rect.TopLeft();

		int q_x, m_x, m_y = 0;
		q_x = (dialog_rect.Width() / 2);
		m_x = q_x - (btn_rect.Width() / 2);
		m_y = btn_rect.top - (dialog_rect.top + dialog_header_height);
		m_btnOK.MoveWindow(m_x, m_y, btn_rect.Width(), btn_rect.Height());

	}
	break;
	case MB_OKCANCEL:
	{
		// OK / cancel 
		m_btnOK.GetWindowRect(&btn_rect);
		btn_left_top = btn_rect.TopLeft();

		int q_x, m_x, m_y = 0;
		q_x = (dialog_rect.Width() / 4);

		m_x = q_x - (btn_rect.Width() / 2);
		m_y = btn_rect.top - (dialog_rect.top + dialog_header_height);
		m_btnOK.MoveWindow(m_x, m_y, btn_rect.Width(), btn_rect.Height());

		m_btnCancel.GetWindowRect(&btn_rect);
		m_x = (dialog_rect.Width() / 2) + (btn_rect.Width() / 2);
		m_btnCancel.MoveWindow(m_x, m_y, btn_rect.Width(), btn_rect.Height());
	}
	break;

	case MB_YESNO:
		m_btnYes.ShowWindow(SW_SHOW);
		m_btnNO.ShowWindow(SW_SHOW);
		break;

	case MB_ABORTRETRYIGNORE:
		break;

	case MB_RETRYCANCEL:
		m_btnRetry.ShowWindow(SW_SHOW);
		m_btnCancel.ShowWindow(SW_SHOW);
		break;

	default:
		m_btnOK.ShowWindow(SW_SHOW);
		break;
	}












}


void Cui_popMsg::OnBnClickedMsgBoxNo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDNO);
}


void Cui_popMsg::OnBnClickedMsgBoxRetry()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDRETRY);
}


void Cui_popMsg::OnBnClickedMsgBoxYes()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDYES);
}
