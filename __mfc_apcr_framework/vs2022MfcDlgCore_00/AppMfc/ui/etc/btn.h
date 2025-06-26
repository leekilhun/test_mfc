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

class CCustomBtn : public CMFCButton
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
  CCustomBtn();
  virtual ~CCustomBtn();

  void LoadImages(LPCTSTR lpszFileName, CSize size); // 이미지 로드 함수

protected:

  DECLARE_MESSAGE_MAP()

public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
  //afx_msg virtual void PreSubclassWindow();           // 서브클래싱을 완료하고 스타일 설정

  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();

private:

  COLORREF m_bgColor{};    // 배경색
  COLORREF m_textColor{};  // 글자색
  btn_state m_currentState{};     // 현재 버튼 상태
  BOOL m_mouseOver{};       // 마우스 오버 상태
  BOOL m_isChecked{};
  CSize m_imageSize;        // 각 이미지의 크기
  CImage m_image;
};
