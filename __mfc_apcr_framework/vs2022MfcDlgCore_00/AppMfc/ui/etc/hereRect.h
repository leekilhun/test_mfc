#pragma once
#ifndef __ETC_UI_HERE_RECT_H___
#define __ETC_UI_HERE_RECT_H___		


#include "afxdialogex.h"


class ChereRect : public CStatic
{
public:
  //static const int RECT_WIDTH = 500;   // 사각형 너비
 // static const int RECT_HEIGHT = 60;   // 사각형 높이
  static const int BORDER_RADIUS = 10; // 모서리 둥근 정도
  static const int ANIMATION_STEPS = 10;
  static const int BORDER_THICKNESS = 4; // 테두리 두께

  ChereRect();
  virtual ~ChereRect();


  BOOL Create(CWnd* pParent, CRect rect);
  void MoveToPosition(CRect targetRect);
  void MoveToPosition(int x, int y);
  void SetTransparency(int alpha);  // 투명도 설정
  void ShowHighlight(bool show);    // 강조 박스 보이기/숨기기


protected:
  CRect m_currentRect;
  CRect m_targetRect;
  int m_step;
  bool m_animating;
  int m_alpha;  // 투명도 (0=완전 투명, 255=불투명)

  afx_msg void OnPaint();
  afx_msg void OnTimer(UINT_PTR nIDEvent);

  DECLARE_MESSAGE_MAP()
};

#endif //__ETC_UI_HERE_RECT_H___//
