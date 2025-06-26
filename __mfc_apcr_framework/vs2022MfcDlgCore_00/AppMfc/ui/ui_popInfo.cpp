// ui_popInfo.cpp: 구현 파일
//

#include "pch.h"
#include "afxdialogex.h"

#include "ui_def.hpp"

#include "ui_popInfo.h"


// Cui_popInfo 대화 상자

IMPLEMENT_DYNAMIC(Cui_popInfo, CDialogEx)

Cui_popInfo::Cui_popInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INFO_BOX, pParent)
{

}

Cui_popInfo::~Cui_popInfo()
{
}

void Cui_popInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDS_INFO_OBJ_ID, m_info);
}


BEGIN_MESSAGE_MAP(Cui_popInfo, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(Cui_popInfo, CDialogEx)
END_EVENTSINK_MAP()
// Cui_popInfo 메시지 처리기

void Cui_popInfo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL Cui_popInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_info.SetCaption(L"Door Opened !!!");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
