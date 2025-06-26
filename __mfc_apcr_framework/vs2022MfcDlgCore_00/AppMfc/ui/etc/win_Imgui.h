#pragma once
#include "afxdialogex.h"


#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends//imgui_impl_win32.h"


// Cwin_Imgui 대화 상자

class Cwin_Imgui : public CDialogEx
{
	UINT_PTR m_timerID{};
	DECLARE_DYNAMIC(Cwin_Imgui)

public:
	Cwin_Imgui(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Cwin_Imgui();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMGUI_WIND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	void InitImGui();
	void RenderImGui();
	void CleanupImGui();
};
