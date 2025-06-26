#include "pch.h"

#include "toolTip.h"



CCtoolTip::CCtoolTip()
  : m_dwStyle(0)
{
}

CCtoolTip::~CCtoolTip()
{
}

BOOL CCtoolTip::Initialize(CWnd* pParentWnd)
{
  if (!Create(pParentWnd, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOPREFIX))
  {
    TRACE("Failed to create tooltip control.\n");
    return FALSE;
  }

  CToolTipCtrl::Activate(TRUE); // 툴팁 활성화
  return TRUE;
}

void CCtoolTip::AddTool(CWnd* pWnd, const CString& text)
{
  ASSERT(pWnd != nullptr);

  CRect rect{};
  pWnd->GetClientRect(&rect);
  CToolTipCtrl::AddTool(pWnd, text, &rect, pWnd->GetDlgCtrlID());
}

void CCtoolTip::AddTitle(const CString& title, int iconType)
{
  ASSERT(m_hWnd != nullptr);


  CToolTipCtrl::SendMessage(TTM_SETTITLE, iconType, (LPARAM)(LPCTSTR)title);
}

void CCtoolTip::SetCustomStyle(DWORD dwStyle)
{
  m_dwStyle = dwStyle;
  CToolTipCtrl::ModifyStyle(0, dwStyle); // 새로운 스타일 적용
}

void CCtoolTip::RelayTooltipEvent(MSG* pMsg)
{
  if (m_hWnd != nullptr)
  {
    CToolTipCtrl::RelayEvent(pMsg);
  }
}

void CCtoolTip::SetTooltipDelays(int initial, int autoPop, int reshow)
{
  CToolTipCtrl::SetDelayTime(TTDT_INITIAL, initial);  // 툴팁 표시 전 지연
  CToolTipCtrl::SetDelayTime(TTDT_AUTOPOP, autoPop); // 툴팁 유지 시간
  CToolTipCtrl::SetDelayTime(TTDT_RESHOW, reshow);   // 툴팁 다시 표시 전 지연
}

void CCtoolTip::EnableMultiLine(int maxWidth)
{
  CToolTipCtrl::SetMaxTipWidth(maxWidth);
}
