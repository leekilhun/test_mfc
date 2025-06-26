// CimgCn.h
#pragma once

#include <afxwin.h>

class CimgCn : public CStatic
{
  DECLARE_DYNAMIC(CimgCn)

public:
  CimgCn();
  virtual ~CimgCn();

  void SetImage(LPCTSTR strImagePath); // 이미지 설정 함수
  void SetText(CString strText);        // 텍스트 설정 함수

protected:
  virtual void OnPaint();  // 그리기 처리  

  //afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); // 호출되지 않아 paint으로 그리는 것으로 변경함

  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();
  afx_msg void PreSubclassWindow();

  DECLARE_MESSAGE_MAP()
private:
  Gdiplus::Image* m_pImage{};   // 이미지 객체
  CString m_strText{};          // 텍스트 객체
  BOOL m_mouseOver{};       // 마우스 오버 상태

  void RemoveWhiteBackground(Gdiplus::Bitmap* pBitmap);

};
