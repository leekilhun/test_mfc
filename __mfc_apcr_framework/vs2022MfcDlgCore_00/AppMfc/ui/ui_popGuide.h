#pragma once
#include "afxdialogex.h"


// Cui_popGuide 대화 상자

class Cui_popGuide : public CDialogEx
{
  DECLARE_DYNAMIC(Cui_popGuide)

public:
  Cui_popGuide(CWnd* pParent = nullptr);   // 표준 생성자입니다.
  virtual ~Cui_popGuide();

  // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_TRANSPARENT_GUIDE_DLG  };
#endif

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

  DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
