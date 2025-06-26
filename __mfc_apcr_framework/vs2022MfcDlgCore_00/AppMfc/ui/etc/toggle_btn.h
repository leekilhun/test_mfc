#pragma once
#include <afxdialogex.h>

/*

load image (수직 상태 변화 이미지 .png 파일)
    ┌─┐
    ├─┤
    ├─┤
    └─┘
 수직 인덱스에 따라 이미지 변경 사용
    ┌─┐
    └─┘
  
 버튼 상태 : 기본        첫번째 이미지 (idx 0),
              hover 상태 두번째 이미지 (idx 1),
              클릭 상태  세번째 이미지 (idx 2),

*/

class CToggleBtn : public CButton
{
  enum btn_state :int
  {
    bs_normal = 0,
    bs_hover,
    bs_pressed,
    bs_checked,
    bs_disabled,
    bs_max

  };


public:
  CToggleBtn();
  virtual ~CToggleBtn();

  void SetBackgroundColor(COLORREF color);  //
  void SetTextColor(COLORREF color);        //
  void LoadImages(LPCTSTR lpszFileName, CSize size = CSize{38,38}); // 
  BOOL SetCheck(BOOL checked);
  BOOL GetCheck() const;

protected:
  afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();
  afx_msg void PreSubclassWindow();

  DECLARE_MESSAGE_MAP()

private:

  COLORREF m_bgColor{};    // 배경색
  COLORREF m_textColor{};  // 글자색
  CImage m_image{};        // 이미지
  btn_state m_currentState{};     // 현재 버튼 상태
  BOOL m_mouseOver{};       // 마우스 오버 상태
  BOOL m_isChecked{};
  CSize m_imageSize{};
};