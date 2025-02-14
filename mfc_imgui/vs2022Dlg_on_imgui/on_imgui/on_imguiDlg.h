
// on_imguiDlg.h : header file
//

#pragma once

#define IMGUI_DEFINE_MATH_OPERTATORS
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"

#include "implot/implot.h"
#include "implot/implot_internal.h"

#include "imnodes/imnodes.h"
#include "imnodes/imnodes_internal.h"
#include "imguizmo/ImGuizmo.h"

#include <d3d11.h>

// ConimguiDlg dialog
class ConimguiDlg : public CDialogEx
{
	UINT_PTR m_TimerID{};
// Construction
public:
	ConimguiDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ON_IMGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

private:

	void InitImGui();
	void RenderImGui();
	void CleanupImGui();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
