
// DataGidDlg.h: 헤더 파일
//

#pragma once

#include "GridCtrl.h"
using namespace LIB_GRID_CTRL;
// CDataGidDlg 대화 상자
class CDataGidDlg : public CDialogEx
{

	CEdit	m_TraceWnd{};
	CButton	m_CallbackButton{};
	CSpinButtonCtrl	m_RowSpin{};
	CSpinButtonCtrl	m_FixRowSpin{};
	CSpinButtonCtrl	m_FixColSpin{};
	CSpinButtonCtrl	m_ColSpin{};
	CStatic	m_GridRect{};
	int		m_nFixCols{};
	int		m_nFixRows{};
	int		m_nCols{};
	int		m_nRows{};
	BOOL	m_bEditable{};
	BOOL	m_bHorzLines{};
	BOOL	m_bListMode{};
	BOOL	m_bVertLines{};
	BOOL	m_bSelectable{};
	BOOL	m_bAllowColumnResize{};
	BOOL	m_bAllowRowResize{};
	BOOL	m_bHeaderSort{};
	BOOL	m_bItalics{};
	BOOL	m_btitleTips{};
	BOOL	m_bSingleSelMode{};
	BOOL    m_bSingleColSelMode{};
	BOOL	m_bTrackFocus{};
	BOOL	m_bFrameFocus{};
	BOOL	m_bVirtualMode{};
	BOOL	m_bCallback{};
	BOOL    m_bVertical{};
	BOOL    m_bExpandUseFixed{};
	BOOL    m_bRejectEditAttempts{};
	BOOL    m_bRejectEditChanges{};
	BOOL    m_bRow2Col2Hidden{};
	CGridCtrl m_Grid{};
	CSize m_OldSize{};
	CImageList m_ImageList{};
// 생성입니다.
public:
	CDataGidDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATAGID_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	static BOOL CALLBACK GridCallback(GV_DISPINFO* pDispInfo, LPARAM lParam);
	void OnGridLines();
	void UpdateMenuUI();
	void Trace(LPCTSTR szFmt, ...);

	afx_msg void OnUpdateEditCols();
	afx_msg void OnUpdateEditFixcols();
	afx_msg void OnUpdateEditFixrows();
	afx_msg void OnUpdateEditRows();
	afx_msg void OnCellUrl();
	afx_msg void OnCellNormal();
	afx_msg void OnCellCombo();
	afx_msg void OnCellNumeric();
	afx_msg void OnCellReadonly();
	afx_msg void OnCellCheckbox();
	afx_msg void OnCellDateTime();
	afx_msg void OnCleartrace();
	afx_msg void OnGridDblClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridRClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridStartEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridStartSelChange(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndSelChange(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridBeginDrag(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopyOrCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);

	static bool VirtualCompare(int c1, int c2);

	afx_msg void OnEditable();
	afx_msg void OnVirtualMode();    // Sets the grid mode, fills the grid
	afx_msg void OnListmode();
	afx_msg void OnItalics();
	afx_msg void OnTitletips();
	afx_msg void OnTrackfocus();
	afx_msg void OnFramefocus();
	afx_msg void OnRowResize();
	afx_msg void OnColResize();
	afx_msg void OnSingleselmode();
	afx_msg void OnSingleColMode();
	afx_msg void OnVertGridLines();
	afx_msg void OnHorzGridLines();
	afx_msg void OnAllowSelection();
	afx_msg void OnCallbackFunction();
	afx_msg void OnVertical();
	afx_msg void OnHeaderSort();
	afx_msg void OnExpandUseFixed();
	afx_msg void OnRejectEditAttempts();
	afx_msg void OnRejectEditChanges();
	afx_msg void OnExpandLast();
	afx_msg void OnSetFocus11();
	afx_msg void OnFixedRowSelection();
	afx_msg void OnFixedColSelection();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

};
