
// DataGidDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "DataGid.h"
#include "DataGidDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDataGidDlg 대화 상자



CDataGidDlg::CDataGidDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATAGID_DIALOG, pParent)
{
	m_OldSize = CSize(-1, -1);

	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = 8;
	m_nRows = 20;
	m_bEditable = FALSE;
	m_bHorzLines = FALSE;
	m_bVertLines = FALSE;
	m_bListMode = TRUE;
	m_bHeaderSort = FALSE;
	m_bSingleSelMode = TRUE;
	m_bSingleColSelMode = TRUE;
	m_bSelectable = FALSE;
	m_bAllowColumnResize = FALSE;
	m_bAllowRowResize = FALSE;
	m_bItalics = FALSE;
	m_btitleTips = FALSE;
	m_bTrackFocus = FALSE;
	m_bFrameFocus = FALSE;
	m_bVirtualMode = TRUE;
	m_bCallback = TRUE;
	m_bVertical = TRUE;
	m_bExpandUseFixed = FALSE;
	m_bRejectEditAttempts = TRUE;
	m_bRejectEditChanges = TRUE;
	m_bRow2Col2Hidden = FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataGidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE, m_TraceWnd);
	DDX_Control(pDX, IDC_SPIN_ROW, m_RowSpin);
	DDX_Control(pDX, IDC_SPIN_FIXROW, m_FixRowSpin);
	DDX_Control(pDX, IDC_SPIN_FIXCOL, m_FixColSpin);
	DDX_Control(pDX, IDC_SPIN_COL, m_ColSpin);
	DDX_Text(pDX, IDC_EDIT_FIXCOLS, m_nFixCols);
	DDX_Text(pDX, IDC_EDIT_FIXROWS, m_nFixRows);
	DDX_Text(pDX, IDC_EDIT_COLS, m_nCols);
	DDX_Text(pDX, IDC_EDIT_ROWS, m_nRows);
	DDX_Control(pDX, IDC_GRID, m_Grid);             // associate the grid window with a C++ object
}

BEGIN_MESSAGE_MAP(CDataGidDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()

	ON_EN_UPDATE(IDC_EDIT_COLS, OnUpdateEditCols)
	ON_EN_UPDATE(IDC_EDIT_FIXCOLS, OnUpdateEditFixcols)
	ON_EN_UPDATE(IDC_EDIT_FIXROWS, OnUpdateEditFixrows)
	ON_EN_UPDATE(IDC_EDIT_ROWS, OnUpdateEditRows)
	ON_BN_CLICKED(IDC_CELL_URL, OnCellUrl)
	ON_BN_CLICKED(IDC_CELL_NORMAL, OnCellNormal)
	ON_BN_CLICKED(IDC_CELL_COMBO, OnCellCombo)
	ON_BN_CLICKED(IDC_CELL_NUMERIC, OnCellNumeric)
	ON_BN_CLICKED(IDC_CELL_READONLY, OnCellReadonly)
	ON_BN_CLICKED(IDC_CELL_CHECK, OnCellCheckbox)
	ON_BN_CLICKED(IDC_CELL_DATETIME, OnCellDateTime)
	ON_BN_CLICKED(IDC_CLEARTRACE, OnCleartrace)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
	ON_NOTIFY(GVN_SELCHANGING, IDC_GRID, OnGridStartSelChange)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnGridEndSelChange)
	ON_NOTIFY(GVN_BEGINDRAG, IDC_GRID, OnGridBeginDrag)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopyOrCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopyOrCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)

END_MESSAGE_MAP()


// CDataGidDlg 메시지 처리기

BOOL CDataGidDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	GetClientRect(rect);
	m_OldSize = CSize(rect.Width(), rect.Height());

	// init spin controls
	m_RowSpin.SetRange(0, 999);
	m_FixRowSpin.SetRange(0, 999);
	m_FixColSpin.SetRange(0, 999);
	m_ColSpin.SetRange(0, 999);


	/////////////////////////////////////////////////////////////////////////
	// initialise grid properties
	/////////////////////////////////////////////////////////////////////////

	m_ImageList.Create(MAKEINTRESOURCE(IDB_IMAGES), 16, 1, RGB(255, 255, 255));
	m_Grid.SetImageList(&m_ImageList);

	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

	OnEditable();
	OnVirtualMode();    // Sets the grid mode, fills the grid
	OnListmode();
	OnCellReadonly();
	OnItalics();
	OnTitletips();
	OnTrackfocus();
	OnFramefocus();
	OnRowResize();
	OnColResize();
	OnSingleselmode();
	OnSingleColMode();
	OnVertGridLines();
	OnHorzGridLines();
	OnAllowSelection();
	OnCallbackFunction();
	OnVertical();
	OnHeaderSort();
	OnExpandUseFixed();
	OnRejectEditAttempts();
	OnRejectEditChanges();


	m_Grid.SetFixedColumnSelection(TRUE);
	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.EnableColumnHide();

	UpdateMenuUI();

	m_Grid.AutoSize();

	m_Grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDataGidDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDataGidDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDataGidDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CDataGidDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CDataGidDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (wParam == (WPARAM)m_Grid.GetDlgCtrlID())
	{
		*pResult = 1;
		GV_DISPINFO* pDispInfo = (GV_DISPINFO*)lParam;
		if (GVN_GETDISPINFO == pDispInfo->hdr.code)
		{
			//TRACE2("Getting Display info for cell %d,%d\n", pDispInfo->item.row, pDispInfo->item.col);
			pDispInfo->item.strText.Format(_T("Message %d,%d"), pDispInfo->item.row, pDispInfo->item.col);
			return TRUE;
		}
		else if (GVN_ODCACHEHINT == pDispInfo->hdr.code)
		{
			GV_CACHEHINT* pCacheHint = (GV_CACHEHINT*)pDispInfo;
			Trace(_T("Cache hint received for cell range %d,%d - %d,%d\n"),
				pCacheHint->range.GetMinRow(), pCacheHint->range.GetMinCol(),
				pCacheHint->range.GetMaxRow(), pCacheHint->range.GetMaxCol());
		}
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

BOOL CDataGidDlg::GridCallback(GV_DISPINFO* pDispInfo, LPARAM lParam)
{
	pDispInfo->item.strText.Format(_T("Callback %d,%d"), pDispInfo->item.row, pDispInfo->item.col);
	return TRUE;
}

void CDataGidDlg::OnGridLines()
{
	if (!m_bHorzLines && !m_bVertLines)
		m_Grid.SetGridLines(GVL_NONE);
	else if (m_bHorzLines && !m_bVertLines)
		m_Grid.SetGridLines(GVL_HORZ);
	else if (!m_bHorzLines && m_bVertLines)
		m_Grid.SetGridLines(GVL_VERT);
	else
		m_Grid.SetGridLines(GVL_BOTH);

	UpdateMenuUI();
}

void CDataGidDlg::UpdateMenuUI()
{
}

void CDataGidDlg::Trace(LPCTSTR szFmt, ...)
{
	CString str;

	// Format the message text
	va_list argptr;
	va_start(argptr, szFmt);
	str.FormatV(szFmt, argptr);
	va_end(argptr);

	str.Replace(_T("\n"), _T("\r\n"));

	CString strWndText;
	m_TraceWnd.GetWindowText(strWndText);
	strWndText += str;
	m_TraceWnd.SetWindowText(strWndText);

	//	m_TraceWnd.SetSel(str.GetLength()-1, str.GetLength()-2, FALSE);
	m_TraceWnd.LineScroll(-m_TraceWnd.GetLineCount());
	m_TraceWnd.LineScroll(m_TraceWnd.GetLineCount() - 4);
}

void CDataGidDlg::OnUpdateEditCols()
{
	if (!::IsWindow(m_Grid.m_hWnd)) return;
	UpdateData();

	int nOldNumCols = m_Grid.GetColumnCount();

	TRY{
				m_Grid.SetColumnCount(m_nCols);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		m_nCols = m_Grid.GetColumnCount();
	m_nFixCols = m_Grid.GetFixedColumnCount();
	UpdateData(FALSE);

	CString str;
	for (int i = nOldNumCols; i < m_nCols; i++)
	{
		str.Format(_T("Column %d"), i);
		m_Grid.SetItemText(0, i, str);
	}
}

void CDataGidDlg::OnUpdateEditFixcols()
{
	if (!::IsWindow(m_Grid.m_hWnd)) return;
	UpdateData();

	TRY{
				m_Grid.SetFixedColumnCount(m_nFixCols);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		m_nCols = m_Grid.GetColumnCount();
	m_nFixCols = m_Grid.GetFixedColumnCount();
	UpdateData(FALSE);
}

void CDataGidDlg::OnUpdateEditFixrows()
{
	if (!::IsWindow(m_Grid.m_hWnd)) return;
	UpdateData();

	TRY{
				m_Grid.SetFixedRowCount(m_nFixRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		m_nRows = m_Grid.GetRowCount();
	m_nFixRows = m_Grid.GetFixedRowCount();
	UpdateData(FALSE);
}

void CDataGidDlg::OnUpdateEditRows()
{
	if (!::IsWindow(m_Grid.m_hWnd)) return;
	UpdateData();

	int nOldNumRows = m_Grid.GetRowCount();

	TRY{
				m_Grid.SetRowCount(m_nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		m_nRows = m_Grid.GetRowCount();
	m_nFixRows = m_Grid.GetFixedRowCount();
	UpdateData(FALSE);

	CString str;
	for (int i = nOldNumRows; i < m_nRows; i++)
	{
		str.Format(_T("Row %d"), i);
		m_Grid.SetItemText(i, 0, str);
	}

	//m_Grid.EnsureVisible(m_nRows-1,0);
}

void CDataGidDlg::OnCellUrl()
{
	OnCellNormal();

	if (!m_Grid.SetCellType(1, 1, RUNTIME_CLASS(CGridURLCell)))
		return;
	m_Grid.SetItemText(1, 1, _T("www.codeproject.com"));
}

void CDataGidDlg::OnCellNormal()
{
	m_Grid.SetCellType(1, 1, RUNTIME_CLASS(CGridCell));
	m_Grid.SetItemText(1, 1, _T("1"));
	m_Grid.SetItemState(1, 1, m_Grid.GetItemState(1, 1) & ~GVIS_READONLY);
	m_Grid.Invalidate();
}

void CDataGidDlg::OnCellCombo()
{
	OnCellNormal();

	if (!m_Grid.SetCellType(1, 1, RUNTIME_CLASS(CGridCellCombo)))
		return;

	m_Grid.SetItemText(1, 1, _T("Option 1"));

	CStringArray options;
	options.Add(_T("Option 1"));
	options.Add(_T("Option 2"));
	options.Add(_T("Option 3"));

	CGridCellCombo* pCell = (CGridCellCombo*)m_Grid.GetCell(1, 1);
	pCell->SetOptions(options);
	pCell->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
}

void CDataGidDlg::OnCellNumeric()
{
	OnCellNormal();

	if (!m_Grid.SetCellType(1, 1, RUNTIME_CLASS(CGridCellNumeric)))
		return;

	m_Grid.SetItemText(1, 1, _T("12345"));
}

void CDataGidDlg::OnCellReadonly()
{
	OnCellNormal();

	m_Grid.SetItemState(1, 1, m_Grid.GetItemState(1, 1) | GVIS_READONLY);
}

void CDataGidDlg::OnCellCheckbox()
{
	OnCellNormal();

	m_Grid.SetCellType(1, 1, RUNTIME_CLASS(CGridCellCheck));
}

void CDataGidDlg::OnCellDateTime()
{
	OnCellNormal();

	m_Grid.SetCellType(1, 1, RUNTIME_CLASS(CGridCellDateTime));

	CGridCellDateTime* pCell = (CGridCellDateTime*)m_Grid.GetCell(1, 1);
	pCell->SetTime(CTime::GetCurrentTime());
	m_Grid.RedrawCell(1, 1);

	m_Grid.SetColumnWidth(1, 100);
}

void CDataGidDlg::OnCleartrace()
{
	m_TraceWnd.SetWindowText(_T(""));
}

void CDataGidDlg::OnGridDblClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}

void CDataGidDlg::OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}

void CDataGidDlg::OnGridRClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}

void CDataGidDlg::OnGridStartEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	*pResult = (m_bRejectEditAttempts) ? -1 : 0;
}

void CDataGidDlg::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	*pResult = (m_bRejectEditChanges) ? -1 : 0;
}

void CDataGidDlg::OnGridStartSelChange(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("Start Selection Change on row %d, col %d (%d Selected)\n"),
		pItem->iRow, pItem->iColumn, m_Grid.GetSelectedCount());
}

void CDataGidDlg::OnGridEndSelChange(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("End Selection Change on row %d, col %d (%d Selected)\n"),
		pItem->iRow, pItem->iColumn, m_Grid.GetSelectedCount());
}

void CDataGidDlg::OnGridBeginDrag(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	Trace(_T("Begin Drag row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}

void CDataGidDlg::OnEditCopy()
{
	m_Grid.OnEditCopy();
}

void CDataGidDlg::OnEditCut()
{
	m_Grid.OnEditCut();
}

void CDataGidDlg::OnUpdateEditCopyOrCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Grid.GetSelectedCount() > 0);
}

void CDataGidDlg::OnEditPaste()
{
	m_Grid.OnEditPaste();
}

void CDataGidDlg::OnUpdateEditPaste(CCmdUI* pCmdUI)
{ // Attach a COleDataObject to the clipboard see if there is any data
	COleDataObject obj;
	pCmdUI->Enable(obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT));
}

bool CDataGidDlg::VirtualCompare(int c1, int c2)
{
	return false;
}

void CDataGidDlg::OnEditable()
{
	m_bEditable = !m_bEditable;
	m_Grid.SetEditable(m_bEditable);
	UpdateMenuUI();
}

void CDataGidDlg::OnVirtualMode()
{
	UpdateData();

	m_bVirtualMode = !m_bVirtualMode;
	m_Grid.SetVirtualMode(m_bVirtualMode);
	m_Grid.SetEditable(m_bEditable);

	m_bHeaderSort = m_Grid.GetHeaderSort();

	if (m_bVirtualMode)
	{
		m_bEditable = m_Grid.IsEditable();
		m_Grid.SetCallbackFunc(m_bCallback ? GridCallback : NULL, 0);

		m_nFixCols = 1;
		m_nFixRows = 1;
		m_nCols = 100;
		m_nRows = 100000;

		TRY{
			m_Grid.SetRowCount(m_nRows);
			m_Grid.SetColumnCount(m_nCols);
			m_Grid.SetFixedRowCount(m_nFixRows);
			m_Grid.SetFixedColumnCount(m_nFixCols);
		m_Grid.AllowReorderColumn(m_bCallback != 0); // implemented now only if m_bCallback
		m_Grid.EnableDragRowMode(m_bCallback != 0);
		m_Grid.SetVirtualCompare(VirtualCompare);
		}
			CATCH(CMemoryException, e)
		{
			e->ReportError();
			return;
		}
		END_CATCH
	}
	else
	{
		m_nFixCols = 1;
		m_nFixRows = 1;
		m_nCols = 6;
		m_nRows = 16;

		m_Grid.SetAutoSizeStyle();

		TRY{
			m_Grid.SetRowCount(m_nRows);
			m_Grid.SetColumnCount(m_nCols);
			m_Grid.SetFixedRowCount(m_nFixRows);
			m_Grid.SetFixedColumnCount(m_nFixCols);
		}
			CATCH(CMemoryException, e)
		{
			e->ReportError();
			return;
		}
		END_CATCH

			// fill rows/cols with text
			for (int row = 0; row < m_Grid.GetRowCount(); row++)
			{
				for (int col = 0; col < m_Grid.GetColumnCount(); col++)
				{
					CString str;

					GV_ITEM Item;

					Item.mask = GVIF_TEXT;
					Item.row = row;
					Item.col = col;

					if (row < m_nFixRows)
						str.Format(_T("Column %d"), col);
					else if (col < m_nFixCols)
						str.Format(_T("Row %d"), row);
					else
						str.Format(_T("%d"), row * col);
					Item.strText = str;

					if (rand() % 10 == 1)
					{
						COLORREF clr = RGB(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
						Item.crBkClr = clr;             // or - m_Grid.SetItemBkColour(row, col, clr);
						Item.crFgClr = RGB(255, 0, 0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
						Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
					}

					if (col < m_Grid.GetFixedColumnCount())
					{
						Item.iImage = rand() % m_ImageList.GetImageCount();
						Item.mask |= (GVIF_IMAGE);
					}

					m_Grid.SetItem(&Item);
				}
			}
	}

	//m_Grid.GetDefaultCell(FALSE,FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
	//m_Grid.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);

	// Having weird Autosize problems with CE 2.11 - possibly just an emulation problem
#if !defined(_WIN32_WCE) || (_WIN32_WCE < 211)
	m_Grid.AutoSize();
#endif

	UpdateMenuUI();

	UpdateData(FALSE);
}

void CDataGidDlg::OnListmode()
{
	m_bListMode = !m_bListMode;
	m_Grid.SetListMode(m_bListMode);
	UpdateMenuUI();
}

void CDataGidDlg::OnItalics()
{
	m_bItalics = !m_bItalics;

	CFont* pFont = m_Grid.GetFont();
	if (!pFont)
		return;

	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfItalic = (BYTE)m_bItalics;

	m_Grid.GetDefaultCell(TRUE, FALSE)->SetFont(&lf);
	m_Grid.GetDefaultCell(FALSE, TRUE)->SetFont(&lf);
	m_Grid.GetDefaultCell(TRUE, TRUE)->SetFont(&lf);

	m_Grid.Invalidate();
	m_Grid.AutoSizeColumns(GVS_HEADER);

	UpdateMenuUI();
}

void CDataGidDlg::OnTitletips()
{
	m_btitleTips = !m_btitleTips;
	m_Grid.EnableTitleTips(m_btitleTips);
	UpdateMenuUI();
}

void CDataGidDlg::OnTrackfocus()
{
	m_bTrackFocus = !m_bTrackFocus;
	m_Grid.SetTrackFocusCell(m_bTrackFocus);
	m_Grid.Invalidate();
	UpdateMenuUI();
}

void CDataGidDlg::OnFramefocus()
{
	m_bFrameFocus = !m_bFrameFocus;
	m_Grid.SetFrameFocusCell(m_bFrameFocus);
	m_Grid.Invalidate();
	UpdateMenuUI();
}

void CDataGidDlg::OnRowResize()
{
	m_bAllowRowResize = !m_bAllowRowResize;
	m_Grid.SetRowResize(m_bAllowRowResize);
	UpdateMenuUI();
}

void CDataGidDlg::OnColResize()
{
	m_bAllowColumnResize = !m_bAllowColumnResize;
	m_Grid.SetColumnResize(m_bAllowColumnResize);
	UpdateMenuUI();
}

void CDataGidDlg::OnSingleselmode()
{
	m_bSingleSelMode = !m_bSingleSelMode;
	m_Grid.SetSingleRowSelection(m_bSingleSelMode);
	UpdateMenuUI();
}

void CDataGidDlg::OnSingleColMode()
{
	m_bSingleColSelMode = !m_bSingleColSelMode;
	m_Grid.SetSingleColSelection(m_bSingleColSelMode);
	UpdateMenuUI();
}

void CDataGidDlg::OnVertGridLines()
{
	m_bVertLines = !m_bVertLines;
	OnGridLines();
}

void CDataGidDlg::OnHorzGridLines()
{
	m_bHorzLines = !m_bHorzLines;
	OnGridLines();
}

void CDataGidDlg::OnAllowSelection()
{
	m_bSelectable = !m_bSelectable;
	m_Grid.EnableSelection(m_bSelectable);
	UpdateMenuUI();
}

void CDataGidDlg::OnCallbackFunction()
{
	m_bCallback = !m_bCallback;
	m_Grid.SetCallbackFunc(m_bCallback ? GridCallback : NULL, 0);
	m_Grid.AllowReorderColumn(m_bCallback != 0); // implemented now only if m_bCallback
	m_Grid.EnableDragRowMode(m_bCallback != 0);
	m_Grid.Refresh();
	UpdateMenuUI();
}

void CDataGidDlg::OnVertical()
{
	m_bVertical = !m_bVertical;

	// Set reasonable format
	//m_Grid.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS|DT_LEFT|DT_BOTTOM);
	//m_Grid.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS|DT_LEFT|DT_BOTTOM);

	// Get the font
	CFont* pFont = m_Grid.GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	// Make it true-type
	memcpy(lf.lfFaceName, _T("Arial"), 6);

	// Vertical or horizontal
	if (m_bVertical)
	{
		lf.lfEscapement = 900;
		lf.lfOrientation = 900;
	}
	else
	{
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
	}

	// Set the fixed row defaults as this new font
	m_Grid.GetDefaultCell(TRUE, FALSE)->SetFont(&lf);

	// Set cell 1,1 as the more normal font so URL cells look OK
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	m_Grid.SetItemFont(0, 1, &lf);


	m_Grid.AutoSize(GVS_HEADER);
	if (m_btitleTips)
	{
		m_btitleTips = !m_bVertical;
		m_Grid.EnableTitleTips(!m_bVertical);
	}

	UpdateMenuUI();
}

void CDataGidDlg::OnHeaderSort()
{
	m_bHeaderSort = !m_bHeaderSort;
	m_Grid.SetHeaderSort(m_bHeaderSort);
	UpdateMenuUI();
}

void CDataGidDlg::OnExpandUseFixed()
{
	m_bExpandUseFixed = !m_bExpandUseFixed;
	UpdateMenuUI();
}

void CDataGidDlg::OnRejectEditAttempts()
{
	m_bRejectEditAttempts = !m_bRejectEditAttempts;
	UpdateMenuUI();
}

void CDataGidDlg::OnRejectEditChanges()
{
	m_bRejectEditChanges = !m_bRejectEditChanges;
	UpdateMenuUI();
}

void CDataGidDlg::OnExpandLast()
{
	m_Grid.ExpandLastColumn();
}

void CDataGidDlg::OnSetFocus11()
{
	CCellID cell(1, 1);

	m_Grid.SetFocusCell(cell);

	if (m_Grid.GetListMode())
		m_Grid.SetSelectedRange(1, m_Grid.GetFixedColumnCount(), 1, m_Grid.GetColumnCount() - 1, TRUE);
	else
		m_Grid.SetSelectedRange(1, 1, 1, 1, TRUE);
}

void CDataGidDlg::OnFixedRowSelection()
{
	m_Grid.SetFixedRowSelection(!m_Grid.GetFixedRowSelection());
	UpdateMenuUI();
}

void CDataGidDlg::OnFixedColSelection()
{
	m_Grid.SetFixedColumnSelection(!m_Grid.GetFixedColumnSelection());
	UpdateMenuUI();
}

void CDataGidDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd->GetSafeHwnd() == m_Grid.GetSafeHwnd())
	{
		m_Grid.ScreenToClient(&point);
		CCellID cell = m_Grid.GetCellFromPt(point);
		Trace(_T("Context menu called on row %d, col %d\n"), cell.row, cell.col);
	}
}
