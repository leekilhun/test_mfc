#include "framework.h"

class CMainFrame : public CFrameWnd
{
public:
    CMainFrame()
    {
        Create(NULL, _T("MFC Application"), WS_OVERLAPPEDWINDOW, CRect(100, 100, 580, 680));
        
        // 버튼1 생성
        m_button1.Create(_T("Button 1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(10, 10, 100, 50), this, 1);
        
        // 버튼2 생성
        m_button2.Create(_T("Button 2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(10, 70, 100, 50), this, 2);
    }

    afx_msg void OnButton1Click()
    {
        AfxMessageBox(_T("Button 1 Clicked"));
    }

    afx_msg void OnButton2Click()
    {
        AfxMessageBox(_T("Button 2 Clicked"));
    }

    DECLARE_MESSAGE_MAP()

private:
    CButton m_button1;
    CButton m_button2;
};

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_BN_CLICKED(1, &CMainFrame::OnButton1Click)
    ON_BN_CLICKED(2, &CMainFrame::OnButton2Click)
END_MESSAGE_MAP()

class CMyApp : public CWinApp
{
public:
    virtual BOOL InitInstance()
    {
        CWinApp::InitInstance();

        m_pMainWnd = new CMainFrame();
        m_pMainWnd->ShowWindow(SW_SHOW);
        m_pMainWnd->UpdateWindow();
        return TRUE;
    }
};

CMyApp theApp;
