// ui_popAlarm.cpp: 구현 파일
//

#include "pch.h"

#include "ui_def.hpp"

#include "afxdialogex.h"
#include "ui_popAlarm.h"


// Cui_popAlarm 대화 상자

IMPLEMENT_DYNAMIC(Cui_popAlarm, CDialogEx)

Cui_popAlarm::Cui_popAlarm(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ERROR_MSG, pParent)
{

}

Cui_popAlarm::~Cui_popAlarm()
{
}

void Cui_popAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ERR_MSG_OK, m_btnOK);
	DDX_Control(pDX, IDC_ERR_MSG_BUZZER_OFF, m_btnBuzzerOff);
	DDX_Control(pDX, IDC_ERR_MSG_SHOW, m_btnInfo);
	DDX_Control(pDX, IDC_ERR_MSG_EDIT_OBJ, m_editViewObj);
	DDX_Control(pDX, IDC_ERR_MSG_EDIT_UNIT, m_editViewUnit);
	DDX_Control(pDX, IDC_ERR_MSG_EDIT_CONTENT, m_editViewContent);
	DDX_Control(pDX, IDC_ERR_MSG_EDIT_INFOR, m_editViewInfor);
	DDX_Control(pDX, IDC_ERR_MSG_STA_INFOR, m_staErrInfor);
}


BEGIN_MESSAGE_MAP(Cui_popAlarm, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ERR_MSG_OK, &Cui_popAlarm::OnBnClickedErrMsgOk)
	ON_BN_CLICKED(IDC_ERR_MSG_SHOW, &Cui_popAlarm::OnBnClickedErrMsgShow)
	ON_BN_CLICKED(IDC_ERR_MSG_BUZZER_OFF, &Cui_popAlarm::OnBnClickedErrMsgBuzzerOff)
END_MESSAGE_MAP()


// Cui_popAlarm 메시지 처리기


BOOL Cui_popAlarm::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	viewDefault();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void Cui_popAlarm::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void Cui_popAlarm::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void Cui_popAlarm::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}


void Cui_popAlarm::OnBnClickedErrMsgOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
	return;
}


void Cui_popAlarm::OnBnClickedErrMsgShow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_staErrInfor.IsWindowVisible() == TRUE)
		return;
	viewInfor();
	return;
}


void Cui_popAlarm::OnBnClickedErrMsgBuzzerOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//apcr::op_t ptr_op = apcr::get_op();
	//ptr_op->SetBuzzer(false);
	return;
}



void Cui_popAlarm::viewDefault()
{
	// TODO: 여기에 구현 코드 추가.
	// Window 축소
	CRect	DialogRect;
	CPoint	LeftTop;
	GetWindowRect(&DialogRect);

	LeftTop = DialogRect.TopLeft();
	MoveWindow(LeftTop.x, LeftTop.y, DialogRect.Width(), DialogRect.Height() - 70);

	// OK 버튼 위로 보내기 
	CRect  ButtonRect;
	CPoint ButtonLeftTop;

	// Buzzeroff 버튼 위로 보내기 
	m_btnBuzzerOff.GetWindowRect(&ButtonRect);
	ButtonLeftTop = ButtonRect.TopLeft();
	m_btnBuzzerOff.MoveWindow(ButtonLeftTop.x - (LeftTop.x + 5), ButtonLeftTop.y - LeftTop.y - 100,
		ButtonRect.Width(), ButtonRect.Height());


	// OK 버튼 위로 보내기 
	m_btnOK.GetWindowRect(&ButtonRect);
	ButtonLeftTop = ButtonRect.TopLeft();
	m_btnOK.MoveWindow(ButtonLeftTop.x - (LeftTop.x + 0), ButtonLeftTop.y - LeftTop.y - 100,
		ButtonRect.Width(), ButtonRect.Height());



	// Info 버튼 위로 보내기
	m_btnInfo.GetWindowRect(&ButtonRect);
	ButtonLeftTop = ButtonRect.TopLeft();
	m_btnInfo.MoveWindow(ButtonLeftTop.x - (LeftTop.x + 0), ButtonLeftTop.y - LeftTop.y - 100,
		ButtonRect.Width(), ButtonRect.Height());

	m_staErrInfor.ShowWindow(SW_HIDE);
	m_editViewInfor.ShowWindow(SW_HIDE);

}


void Cui_popAlarm::viewInfor()
{
	// TODO: 여기에 구현 코드 추가.

	if (m_staErrInfor.IsWindowVisible() == TRUE)
		return;

	CRect	DialogRect;
	CPoint	LeftTop;
	GetWindowRect(&DialogRect);

	LeftTop = DialogRect.TopLeft();
	MoveWindow(LeftTop.x, LeftTop.y, DialogRect.Width(), DialogRect.Height() + 70);

	// OK 버튼 위로 보내기 
	CRect  ButtonRect;
	CPoint ButtonLeftTop;

	// Buzzeroff 버튼 위로 보내기 
	m_btnBuzzerOff.GetWindowRect(&ButtonRect);
	ButtonLeftTop = ButtonRect.TopLeft();
	m_btnBuzzerOff.MoveWindow(ButtonLeftTop.x - (LeftTop.x + 5), ButtonLeftTop.y - LeftTop.y + 40,
		ButtonRect.Width(), ButtonRect.Height());


	// OK 버튼 위로 보내기 
	m_btnOK.GetWindowRect(&ButtonRect);
	ButtonLeftTop = ButtonRect.TopLeft();
	m_btnOK.MoveWindow(ButtonLeftTop.x - (LeftTop.x + 10), ButtonLeftTop.y - LeftTop.y + 40,
		ButtonRect.Width(), ButtonRect.Height());



	// Info 버튼 위로 보내기
	m_btnInfo.GetWindowRect(&ButtonRect);
	ButtonLeftTop = ButtonRect.TopLeft();
	m_btnInfo.MoveWindow(ButtonLeftTop.x - (LeftTop.x + 15), ButtonLeftTop.y - LeftTop.y + 40,
		ButtonRect.Width(), ButtonRect.Height());

	m_staErrInfor.ShowWindow(SW_SHOW);
	m_editViewInfor.ShowWindow(SW_SHOW);

}
