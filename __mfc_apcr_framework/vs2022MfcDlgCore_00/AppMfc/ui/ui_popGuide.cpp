// ui_popGuide.cpp: 구현 파일
//

#include "pch.h"

#include "afxdialogex.h"

#include "ui_def.hpp"
#include "ui_popGuide.h"

// Cui_popGuide 대화 상자

IMPLEMENT_DYNAMIC(Cui_popGuide, CDialogEx)

Cui_popGuide::Cui_popGuide(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_TRANSPARENT_GUIDE_DLG, pParent)
{
}

Cui_popGuide::~Cui_popGuide()
{
}

void Cui_popGuide::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cui_popGuide, CDialogEx)
  ON_WM_ERASEBKGND()
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL Cui_popGuide::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // 투명 배경 설정
  SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);

  // 배경 투명도 설정 (50% 투명: 128)
  BYTE transparency = 255;  // 0: 완전 투명, 255: 불투명
  SetLayeredWindowAttributes(0, transparency, LWA_ALPHA);

  return TRUE;
}

BOOL Cui_popGuide::OnEraseBkgnd(CDC* pDC)
{
  // 배경 지우지 않음
  return TRUE;
}

HBRUSH Cui_popGuide::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  // 컨트롤 색상 설정
  HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

  if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
  {
    // 텍스트와 배경을 선명하게 설정
    pDC->SetBkMode(TRANSPARENT);  // 배경 투명
    pDC->SetTextColor(RGB(0, 0, 0));  // 텍스트 색상 (검정)

    // 배경을 흰색으로 설정하여 선명하게 보이도록 유지
    static CBrush whiteBrush(RGB(255, 255, 255));
    return whiteBrush;
  }

  return hBrush;
}
