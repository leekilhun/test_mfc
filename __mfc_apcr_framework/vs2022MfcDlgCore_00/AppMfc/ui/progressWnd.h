#pragma once
//#include <afxwin.h>


#define PERCENT			-1
#define TIME_UNLIMITED	0
#define TIME_LIMITED	1


class CprogressWnd :   public CWnd
{

  // Construction/Destruction
public:
  HBRUSH m_hbrush;

  CprogressWnd(bool bCenter = true);
  CprogressWnd(CWnd* pParent, LPCTSTR pszTitle, bool bPos = true, bool bSmooth = true);
  virtual ~CprogressWnd();

  BOOL Create(CWnd* pParent, LPCTSTR pszTitle, bool bSmooth = true);

protected:
  void CommonConstruct();

  // Operations
public:
  bool m_MostTopFlag;
  bool m_bCenter;
  void SetRange(int nLower, int nUpper, int nStep = 1);
  // Set range and step size
  int OffsetPos(int nPos);                        // Same as CProgressCtrl
  int StepIt();                                   //    "
  int SetStep(int nStep);                         //    "
  int SetPos(int nPos);                           //    "

  void SetText(LPCTSTR fmt, ...);                 // Set text in text area

  void Clear();                                   // Clear text, reset bar
  void Hide();                                    // Hide window
  void Show();                                    // Show window

  void SetWindowSize(int nNumTextLines, int nWindowWidth = 390);
 

  void SetDisplayType(int iType);
  void PeekAndPump();
  // Implementation
protected:
  int  m_nPrevPos, m_nPrevPercent;
  int  m_nStep;
  int  m_nMaxValue, m_nMinValue;
  int  m_nNumTextLines;
  int m_iDisplayType;

  CStatic       m_Text;
  CProgressCtrl m_wndProgress;
  CButton       m_CancelButton;
  CString       m_strTitle;
  CFont         m_font;

  // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(CMyProgressWnd)
      //}}AFX_VIRTUAL

  // Generated message map functions
protected:
  //{{AFX_MSG(CMyProgressWnd)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  //}}AFX_MSG
  afx_msg void OnCancel();
  DECLARE_MESSAGE_MAP()


};

