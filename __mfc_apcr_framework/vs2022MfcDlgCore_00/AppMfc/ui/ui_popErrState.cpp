// CommonEStopNotifyDlg.cpp : 구현 파일입니다.
//

#include "pch.h"

#include "ui_def.hpp"

#include "afxdialogex.h"
#include "ui_popErrState.h"


// Cui_popErrState 대화 상자입니다.

IMPLEMENT_DYNAMIC(Cui_popErrState, CDialog)

Cui_popErrState::Cui_popErrState(CWnd* pParent /*=NULL*/)
	: CDialog(Cui_popErrState::IDD, pParent)
{
	m_brBckColor.CreateSolidBrush(RGB(255, 255, 255));
}

Cui_popErrState::~Cui_popErrState()
{
	m_brBckColor.DeleteObject();
}

void Cui_popErrState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITLABEL, m_Msg);
	DDX_Control(pDX, IDC_PICTURE, m_Icon);
}


BEGIN_MESSAGE_MAP(Cui_popErrState, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Cui_popErrState 메시지 처리기입니다.


BOOL Cui_popErrState::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString	strTemp;
	
	CString strCurr;	
	::GetCurrentDirectory(1024, strCurr.GetBuffer(1024));
	strCurr.ReleaseBuffer();

	strTemp.Format(_T("%s\\res\\erricon.bmp"), strCurr);
	m_Icon.SetPicture(strTemp);
	m_Icon.SetPicturePosition(10);
	m_Icon.SetPictureKeepRatio(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void Cui_popErrState::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectArea;
	GetClientRect(&rectArea);
	dc.FillRect(rectArea.operator LPRECT(), &m_brBckColor);
}

void Cui_popErrState::SetMessage(CString strMsg)
{
	m_Msg.SetCaption(strMsg);
}