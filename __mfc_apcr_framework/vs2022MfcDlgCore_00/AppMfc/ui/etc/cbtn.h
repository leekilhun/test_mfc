#pragma once
#include <afxdialogex.h>

/*

load image (수직 상태 변화 이미지 .png 파일)
    ┌─┐
    ├─┤
    ├─┤
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


class cbtn : public CMFCButton
{
public:
  cbtn();
  virtual ~cbtn();

  void SetBackgroundColor(COLORREF color);  // 배경색 설정
  void SetTextColor(COLORREF color);        // 글자색 설정
  void LoadImages(LPCTSTR lpszFileName, CSize size = CSize{ 22,22 }); //   // 이미지 로드

  void SetImageList(UINT nImageID, int cx, COLORREF crMask = RGB(255, 0, 255)); // 이미지 리스트 설정
  //void SetButtonState(int state); // 버튼 상태 설정

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
  COLORREF m_bgColor;    // 배경색
  COLORREF m_textColor;  // 글자색
  CImage m_image;        // 버튼에 표시할 이미지
  CImageList m_imageList; // 이미지 리스트
  BOOL m_mouseOver{};       // 마우스 오버 상태
  BOOL m_isChecked{};
  CSize m_imageSize{};
};

/*

1. 리소스 편집기를 사용하여 다이얼로그에 버튼을 추가한 뒤, 
    해당 버튼에 CCustomButton 클래스를 연결합니다.

2. 멤버 변수에 CCustomButton을 추가한다.

3. 리소스 편집기 생성 다이얼로그와 연결 및 CCustomButton 초기화

      BOOL CMyDialog::OnInitDialog()
      {
          CDialogEx::OnInitDialog();
      
          // 커스텀 버튼 초기화
          m_customButton.SetBackgroundColor(RGB(200, 200, 255)); // 배경색 설정 (연한 파랑)
          m_customButton.SetTextColor(RGB(0, 0, 255));           // 텍스트 색 설정 (파랑)
          m_customButton.LoadImages(L"res\\button_image.png");     // 이미지 설정
      
          return TRUE;
      }

*/