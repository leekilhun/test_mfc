
#include "pch.h"
#include "hereRect.h"
// Implementation

#define DEF_Timer_ID  51


ChereRect::ChereRect() : m_step(0), m_animating(false), m_alpha(128)
{
}


ChereRect::~ChereRect() {}


BOOL ChereRect::Create(CWnd* pParent, CRect rect)
{
  m_currentRect = rect;
  CRect initialRect(0, 0, m_currentRect.Width(), m_currentRect.Height());
  BOOL result = CStatic::Create(NULL, WS_CHILD | SS_OWNERDRAW, initialRect, pParent);

  // 투명한 배경 설정 (WS_EX_LAYERED 스타일 적용)
  ModifyStyleEx(0, WS_EX_LAYERED | WS_EX_TRANSPARENT);
  SetTransparency(m_alpha);

  // 초기에는 숨김 상태
  ShowWindow(SW_HIDE);

  return result;
}


// 투명도 조절 (0=완전 투명, 255=불투명)
void ChereRect::SetTransparency(int alpha)
{
  m_alpha = alpha;
  SetLayeredWindowAttributes(0, (BYTE)m_alpha, LWA_ALPHA);
}

// 강조 박스 보이기/숨기기
void ChereRect::ShowHighlight(bool show)
{
  ShowWindow(show ? SW_SHOW : SW_HIDE);
  if (show)
    RedrawWindow();
}

void ChereRect::MoveToPosition(int x, int y)
{
  ShowHighlight(true);
  m_targetRect = CRect(x, y, x + m_currentRect.Width(), y + m_currentRect.Height());
  m_currentRect = m_targetRect;
  MoveWindow(&m_targetRect);
  RedrawWindow();
}

// 애니메이션 효과를 위한 이동
void ChereRect::MoveToPosition(CRect targetRect)
{
  ShowHighlight(true);
  m_targetRect = targetRect;
  m_step = 0;
  m_animating = true;
  SetTimer(DEF_Timer_ID, 30, nullptr);  // 30ms 간격으로 애니메이션 타이머 시작
}

BEGIN_MESSAGE_MAP(ChereRect, CStatic)
  ON_WM_PAINT()
  ON_WM_TIMER()
END_MESSAGE_MAP()


void ChereRect::OnPaint()
{
  CPaintDC dc(this);
  Gdiplus::Graphics graphics(dc);

  // 안티앨리어싱 활성화
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

  // 배경 투명 처리
  Gdiplus::SolidBrush transparentBrush(Gdiplus::Color(0, 0, 0, 0));
  graphics.FillRectangle(&transparentBrush, 0, 0, m_currentRect.Width(), m_currentRect.Height());

  // 강조 테두리 및 반투명 내부 색상
  /*
   #A4193D 색상
  128 → 알파(불투명도) 값 (완전 불투명)
  164 → Red 값 (A4)
  25 → Green 값 (19)
  61 → Blue 값 (3D)
  */
  Gdiplus::Color borderColor(128, 164, 25, 61); // 붉은색 테두리 (불투명)
  Gdiplus::Color fillColor(m_alpha, 255, 255, 255); // 반투명 붉은 내부

  Gdiplus::Pen pen(borderColor, BORDER_THICKNESS);
  Gdiplus::SolidBrush fillBrush(fillColor);

  // 둥근 사각형 그리기
  Gdiplus::GraphicsPath path;
  path.AddArc(0, 0, BORDER_RADIUS * 2, BORDER_RADIUS * 2, 180, 90);
  path.AddArc(m_currentRect.Width() - BORDER_RADIUS * 2, 0, BORDER_RADIUS * 2, BORDER_RADIUS * 2, 270, 90);
  path.AddArc(m_currentRect.Width() - BORDER_RADIUS * 2, m_currentRect.Height() - BORDER_RADIUS * 2, BORDER_RADIUS * 2, BORDER_RADIUS * 2, 0, 90);
  path.AddArc(0, m_currentRect.Height() - BORDER_RADIUS * 2, BORDER_RADIUS * 2, BORDER_RADIUS * 2, 90, 90);
  path.CloseFigure();

  graphics.FillPath(&fillBrush, &path);
  graphics.DrawPath(&pen, &path);
}

void ChereRect::OnTimer(UINT_PTR nIDEvent)
{
  if (nIDEvent == DEF_Timer_ID && m_animating)
  {
    m_step++;
    int dx = (m_targetRect.left - m_currentRect.left) / ANIMATION_STEPS;
    int dy = (m_targetRect.top - m_currentRect.top) / ANIMATION_STEPS;

    m_currentRect.OffsetRect(dx, dy);
    MoveWindow(&m_currentRect);
    RedrawWindow();

    if (m_step >= ANIMATION_STEPS)
    {
      m_animating = false;
      KillTimer(DEF_Timer_ID);
      MoveWindow(&m_targetRect);
    }
  }
  CStatic::OnTimer(nIDEvent);
}
