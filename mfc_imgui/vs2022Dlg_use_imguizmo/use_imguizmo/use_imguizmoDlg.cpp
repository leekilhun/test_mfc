
// use_imguizmoDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "use_imguizmo.h"
#include "use_imguizmoDlg.h"
#include "afxdialogex.h"

//#include "win_Implot.h"
#include "win_imguizmo.h"
#include <memory> // 스마트 포인터 사용을 위한 헤더 포함


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CuseimguizmoDlg 대화 상자


//-----------------------------------------------------------------------------
// CuseimguizmoDlg 대화 상자
//-----------------------------------------------------------------------------

#define UPDATE_TIMER_POP_UNIT_INIT_DLG 1

CuseimguizmoDlg::CuseimguizmoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USE_IMGUIZMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CuseimguizmoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CuseimguizmoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CuseimguizmoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CuseimguizmoDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CuseimguizmoDlg 메시지 처리기

BOOL CuseimguizmoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CuseimguizmoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CuseimguizmoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CuseimguizmoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//if (m_ptrImgui)
	//	delete m_ptrImgui;
	//m_ptrImgui = nullptr;

	if (m_ptrImguizmo)
		delete m_ptrImguizmo;
	m_ptrImguizmo = nullptr;
}


void CuseimguizmoDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//if (m_ptrImgui == nullptr)
	//{
	//	m_ptrImgui = new Cwin_Implot(this);
	//	m_ptrImgui->Create(IDD_IMGUI_WIND, this);
	//}
	//m_ptrImgui->ShowWindow(SW_SHOW);
}


void CuseimguizmoDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	if (m_ptrImgui == nullptr)
	{
		m_ptrImguizmo = new Cwin_imguizmo(this);
		m_ptrImguizmo->Create(IDD_IMGUI_IMGUIZMO, this);
	}
	m_ptrImguizmo->ShowWindow(SW_SHOW);
}
