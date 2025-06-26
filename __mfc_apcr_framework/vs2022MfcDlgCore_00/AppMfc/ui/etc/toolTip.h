#pragma once
#include "afxdialogex.h"



class CCtoolTip : public CToolTipCtrl
{
public:
  CCtoolTip();
  virtual ~CCtoolTip();

  // 초기화
  BOOL Initialize(CWnd* pParentWnd);

  // 툴팁 추가
  void AddTool(CWnd* pWnd, const CString& text);

  // 이미지 추가
  //void AddImage(int nID, const CString& title, int iconType = TTI_INFO);

  // 이미지 추가
  void AddTitle(const CString& title, int iconType = TTI_INFO);

  // 툴팁 스타일 설정
  void SetCustomStyle(DWORD dwStyle);

  // 툴팁 릴레이 이벤트
  void RelayTooltipEvent(MSG* pMsg);

  // 시간 설정
  void SetTooltipDelays(int initial, int autoPop, int reshow);

  // 텍스트 멀티라인 지원
  void EnableMultiLine(int maxWidth = 300);

protected:
  // 멤버 변수
  DWORD m_dwStyle; // 커스텀 스타일
};


/*
* 

초기화
BOOL CMyDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 툴팁 컨트롤 생성
    m_ToolTip.Create(this);

    // 컨트롤에 툴팁 추가
    m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON1), _T("This is a tooltip for Button1"));
    m_ToolTip.AddTool(GetDlgItem(IDC_EDIT1), _T("Enter text here"));

    // 툴팁 활성화
    m_ToolTip.Activate(TRUE);

    return TRUE;
}

툴팁 메시지가 제대로 전달되도록 PreTranslateMessage를 재정의합니다.
BOOL CMyDialog::PreTranslateMessage(MSG* pMsg)
{
    if (m_ToolTip.m_hWnd != NULL)
        m_ToolTip.RelayEvent(pMsg); // 툴팁 메시지 전달

    return CDialog::PreTranslateMessage(pMsg);
}


동적 툴팁 텍스트 변경
툴팁 텍스트를 동적으로 변경하고 싶을 때 UpdateTipText를 사용합니다.
void CMyDialog::OnButtonClicked()
{
    m_ToolTip.UpdateTipText(_T("New tooltip text"), GetDlgItem(IDC_BUTTON1));
}

툴팁 스타일 변경
툴팁의 스타일을 변경하여 시각적 효과를 추가할 수 있습니다.

BOOL CMyDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_BALLOON); // 스타일 추가
    m_ToolTip.SetMaxTipWidth(200); // 최대 너비 설정
    m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON1), _T("Balloon-style tooltip example"));
    m_ToolTip.Activate(TRUE);

    return TRUE;
}


툴팁 시간 설정
툴팁이 표시되는 시간과 대기 시간을 설정할 수 있습니다.

m_ToolTip.SetDelayTime(TTDT_INITIAL, 500); // 툴팁 표시 전 대기 시간 (밀리초)
m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 5000); // 툴팁이 사라지기 전 표시 시간



툴팁에 이미지 추가
툴팁에 아이콘 이미지를 포함하려면 TTM_SETTITLE 메시지를 사용합니다.

m_ToolTip.SendMessage(TTM_SETTITLE, TTI_INFO, (LPARAM)_T("Information Tooltip"));


5. 툴팁에 리치 텍스트 추가
툴팁에 HTML 스타일의 리치 텍스트를 표시하려면 CToolTipCtrl의 고급 기능을 활용해야 합니다.
m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON1), _T("This is a <b>bold</b> tooltip"));


6. 툴팁 위치 변경
툴팁의 기본 위치를 커스터마이징하려면 SetWindowPos를 사용할 수 있습니다.

CRect rect;
GetDlgItem(IDC_BUTTON1)->GetWindowRect(&rect);
m_ToolTip.SetWindowPos(NULL, rect.left, rect.top - 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);



*/





