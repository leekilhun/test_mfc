// ScopeCtrl.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ScopeCtrl.h"


// CScopeCtrl

IMPLEMENT_DYNAMIC(CScopeCtrl, CWnd)

CScopeCtrl::CScopeCtrl()
{
	clearVariable();
}

CScopeCtrl::~CScopeCtrl()
{
	if (m_pMemDC) {
		delete m_pMemDC;
		m_pMemDC = NULL;
	}

	if (m_pList_Y1) {
		m_pList_Y1->RemoveAll();
		delete m_pList_Y1;
		m_pList_Y1 = NULL;
	}

	if (m_pListBack_Y1) {
		m_pListBack_Y1->RemoveAll();
		delete m_pListBack_Y1;
		m_pListBack_Y1 = NULL;
	}

	if (m_pList_Y1_2nd) {
		m_pList_Y1_2nd->RemoveAll();
		delete m_pList_Y1_2nd;
		m_pList_Y1_2nd = NULL;
	}

	if (m_pListBack_Y1_2nd) {
		m_pListBack_Y1_2nd->RemoveAll();
		delete m_pListBack_Y1_2nd;
		m_pListBack_Y1_2nd = NULL;
	}

	if (m_pList_Y2) {
		m_pList_Y2->RemoveAll();
		delete m_pList_Y2;
		m_pList_Y2 = NULL;
	}

	if (m_pListBack_Y2) {
		m_pListBack_Y2->RemoveAll();
		delete m_pListBack_Y2;
		m_pListBack_Y2 = NULL;
	}

	if (m_TextFont.m_hObject != NULL) {
		m_TextFont.DeleteObject();
	}

	if (m_TextFont_Slim.m_hObject != NULL) {
		m_TextFont_Slim.DeleteObject();
	}

	if (m_TextFont_Result.m_hObject != NULL) {
		m_TextFont_Result.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CScopeCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CScopeCtrl 메시지 처리기입니다.




void CScopeCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 그림 그린 DC를 화면에 그린다.
	if (m_pMemDC->GetSafeHdc())
		dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), m_pMemDC, 0, 0, SRCCOPY);
}

void CScopeCtrl::clearVariable()
{
	m_bCreate = FALSE;
	m_pMemDC = NULL;

	m_pList_Y1 = NULL;
	m_pListBack_Y1 = NULL;
	m_pList_Y1_2nd = NULL;
	m_pListBack_Y1_2nd = NULL;
	m_pList_Y2 = NULL;
	m_pListBack_Y2 = NULL;

	m_dataRange_X.Clear();
	m_dataRange_Y1.Clear();
	m_dataRange_Y1_2nd.Clear();
	m_dataRange_Y2.Clear();

	m_iClientWidth = 0;
	m_iClientHeight = 0;

	m_fScale_X = 0;
	m_fScale_Y1 = 0;
	m_fScale_Y2 = 0;

	m_iOffset_Y1 = 0;
	m_iOffset_Y2 = 0;
	m_iOffset_Y1_Reverse = 0;
	m_iOffset_Y2_Reverse = 0;

	m_iTextSpacing_X = 10;

	m_crBackGround = RGB(0, 0, 0);	// 배경 색상
	m_crGrids = RGB(64, 64, 64);	// 그리드 라인 색상
	m_crText = RGB(255, 255, 255);	// 텍스트 색상

	m_crRange_X = RGB(255, 255, 255);
	m_crRange_Y1 = RGB(0, 255, 0);
	m_crRange_Y2 = RGB(255, 0, 0);
	m_crRangeBack_Y1 = RGB(128, 255, 128);
	m_crRangeBack_Y2 = RGB(255, 128, 128);

	m_iDrawPenWidth = 1;

	m_pParentWnd = NULL;
}

BOOL CScopeCtrl::Create(const RECT &rect,
						CWnd *pParentWnd,
						UINT uID,
						MScopeRange dataRange_X,
						MScopeRange dataRange_Y1,
						MScopeRange dataRange_Y1_2nd,
						MScopeRange dataRange_Y2)
{
	// 이미 생성되어 있으면 다시 생성하지 않는다.
	if (m_bCreate == TRUE) {
		return TRUE;
	}

	m_pParentWnd = pParentWnd;

	// 주어진 rect에 window를 생성한다. (성공시 TRUE, 실패시 FALSE 리턴)
	BOOL bRet = CWnd::CreateEx(WS_EX_CLIENTEDGE,
							AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW),
							NULL,
							WS_VISIBLE | WS_CHILD,//| WS_HSCROLL,
							rect.left,
							rect.top,
							rect.right - rect.left,
							rect.bottom - rect.top,
							pParentWnd->GetSafeHwnd(),
							(HMENU)(DWORD_PTR)uID);

	// 실패시 리턴
	if (!bRet) {
		return FALSE;
	}

	// 그래프를 그릴 DC를 생성한다.
	m_pMemDC = new CDC;
	if (!m_pMemDC) {
		return FALSE;
	}

	// 포인트를 저장할 리스트를 생성한다.
	m_pList_Y1 = new CList<CPoint, CPoint&>;
	if (!m_pList_Y1) {
		return FALSE;
	}

	// 돌아오는 포인트를 저장할 리스트를 생성한다.
	m_pListBack_Y1 = new CList<CPoint, CPoint&>;
	if (!m_pListBack_Y1) {
		return FALSE;
	}

	// 포인트를 저장할 리스트를 생성한다.
	m_pList_Y1_2nd = new CList<CPoint, CPoint&>;
	if (!m_pList_Y1_2nd) {
		return FALSE;
	}

	// 돌아오는 포인트를 저장할 리스트를 생성한다.
	m_pListBack_Y1_2nd = new CList<CPoint, CPoint&>;
	if (!m_pListBack_Y1_2nd) {
		return FALSE;
	}

	// 포인트를 저장할 리스트를 생성한다.
	m_pList_Y2 = new CList<CPoint, CPoint&>;
	if (!m_pList_Y2) {
		return FALSE;
	}

	// 돌아오는 포인트를 저장할 리스트를 생성한다.
	m_pListBack_Y2 = new CList<CPoint, CPoint&>;
	if (!m_pListBack_Y2) {
		return FALSE;
	}

	// 현재 컨트롤 영역을 저장한다.
	GetClientRect(m_rcClient);
	m_iClientWidth = m_rcClient.Width();
	m_iClientHeight = m_rcClient.Height();

	// 컨트롤 영역 전체를 지운다.
	if (!InvalidateCtrl()) {
		return FALSE;
	}

	// 폰트 설정
	LOGFONT lf;

	ZeroMemory(&lf, sizeof(LOGFONT));	
	lstrcpy(lf.lfFaceName, _T("MS Shell Dlg"));
	lf.lfHeight = 12;
	m_TextFont.CreateFontIndirect(&lf);

	ZeroMemory(&lf, sizeof(LOGFONT));
	lstrcpy(lf.lfFaceName, _T("Arial"));
	lf.lfHeight = 35;
	lf.lfWeight = 10;
	m_TextFont_Result.CreateFontIndirect(&lf);

	ZeroMemory(&lf, sizeof(LOGFONT));
	lstrcpy(lf.lfFaceName, _T("Arial"));
	lf.lfHeight = 28;
	lf.lfWeight = 10;
	m_TextFont_Result_Slim.CreateFontIndirect(&lf);

	m_TextFont_Slim.CreateFont(12,					// 글자 높이
							   0,					// 글자 너비
							   0,					// 출력 각도
							   0,					// 기준 선에서의 각도
							   FW_NORMAL,				// 글자 굵기
							   FALSE,				// Italic 적용 여부
							   FALSE,				// 밑줄 적용 여부
							   FALSE,				// 취소선 적용 여부
							   DEFAULT_CHARSET,		// 문자셋 종류
							   OUT_DEFAULT_PRECIS,	// 출력 정밀도
							   CLIP_DEFAULT_PRECIS,	// 클리핑 정밀도
							   DEFAULT_QUALITY,		// 출력 문자 품질
							   DEFAULT_PITCH,		// 글꼴 Pitch
							   _T("Arial")			// 글꼴
							   );

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont);
	UNUSED_ALWAYS(pOldFont);
	m_pMemDC->SetTextColor(m_crText);

	// Range를 설정한다.
	setRange_X(dataRange_X);
	setRange_Y1(dataRange_Y1);
	setRange_Y1_2nd(dataRange_Y1_2nd);
	setRange_Y2(dataRange_Y2);

	// 펜 만들기
	m_iDrawPenWidth = 1;
	m_DrawPen_Y1.DeleteObject();
	m_DrawPen_Y1.CreatePen(PS_SOLID, m_iDrawPenWidth, m_dataRange_Y1.m_crNormal);

	m_DrawPen_Y1_Back.DeleteObject();
	m_DrawPen_Y1_Back.CreatePen(PS_SOLID, m_iDrawPenWidth, m_dataRange_Y1.m_crBack);

	m_DrawPen_Y1_2nd.DeleteObject();
	m_DrawPen_Y1_2nd.CreatePen(PS_SOLID, m_iDrawPenWidth, m_dataRange_Y1_2nd.m_crNormal);

	m_DrawPen_Y1_2nd_Back.DeleteObject();
	m_DrawPen_Y1_2nd_Back.CreatePen(PS_SOLID, m_iDrawPenWidth, m_dataRange_Y1_2nd.m_crBack);

	m_DrawPen_Y2.DeleteObject();
	m_DrawPen_Y2.CreatePen(PS_SOLID, m_iDrawPenWidth, m_dataRange_Y2.m_crNormal);

	m_DrawPen_Y2_Back.DeleteObject();
	m_DrawPen_Y2_Back.CreatePen(PS_SOLID, m_iDrawPenWidth, m_dataRange_Y2.m_crBack);	

	// 점선 펜 만들기
	m_DrawPen_Y1_Dash.DeleteObject();
	m_DrawPen_Y1_Dash.CreatePen(PS_DOT, m_iDrawPenWidth, RGB(255, 255, 255));

	m_DrawPen_Y1_Cross.DeleteObject();
	m_DrawPen_Y1_Cross.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

	// 가로, 세로 그리드 및 라벨을 새로 그린다.
	UpdateLabels();

	return TRUE;
}

BOOL CScopeCtrl::Destroy()
{
	return (CWnd::DestroyWindow());
}

void CScopeCtrl::Clear()
{
	// 리스트 초기화
	clearList();

	// 컨트롤 영역 전체를 지운다.
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}
	//Set the background color of the control
	CBrush bkBrush;
	if (!bkBrush.CreateSolidBrush(m_crBackGround)) {
		return;
	}
	m_pMemDC->FillRect(m_rcClient, &bkBrush);

	InvalidateRect(m_rcClient);

	// 가로, 세로 그리드 및 라벨을 새로 그린다.
	UpdateLabels();
}

BOOL CScopeCtrl::AddPoint_Y1(CPoint ptNewPos, BOOL bReverse /*= FALSE*/)
{
	// Range 영역을 벗어나는 데이터일 경우 최대 값으로 설정
	if (ptNewPos.x < m_dataRange_X.m_ptRange.x) {	// Range X = Min
		ptNewPos.x = m_dataRange_X.m_ptRange.x;
	}
	if (ptNewPos.x > m_dataRange_X.m_ptRange.y) {	// Rnage Y = Max
		ptNewPos.x = m_dataRange_X.m_ptRange.y;
	}

	// Offset 값을 적용한다.
	if (bReverse == TRUE) {
		ptNewPos.y += m_iOffset_Y1_Reverse;
	}
	else {
		ptNewPos.y += m_iOffset_Y1;
	}

	if (ptNewPos.y < m_dataRange_Y1.m_ptRange.x) {
		ptNewPos.y = m_dataRange_Y1.m_ptRange.x;
	}
	if (ptNewPos.y > m_dataRange_Y1.m_ptRange.y) {
		ptNewPos.y = m_dataRange_Y1.m_ptRange.y;
	}
	
	// 정상 그래프일 경우
	if (bReverse == FALSE) {
		if (m_pList_Y1) {
			m_pList_Y1->AddTail(ptNewPos);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else {
		if (m_pListBack_Y1) {
			m_pListBack_Y1->AddTail(ptNewPos);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CScopeCtrl::AddPoint_Y1_2nd(CPoint ptNewPos, BOOL bReverse /*= FALSE*/)
{
	// Range 영역을 벗어나는 데이터일 경우 최대 값으로 설정
	if (ptNewPos.x < m_dataRange_X.m_ptRange.x) {	// Range X = Min
		ptNewPos.x = m_dataRange_X.m_ptRange.x;
	}
	if (ptNewPos.x > m_dataRange_X.m_ptRange.y) {	// Rnage Y = Max
		ptNewPos.x = m_dataRange_X.m_ptRange.y;
	}

	// Offset 값을 적용한다.
	if (bReverse == TRUE) {
		ptNewPos.y += m_iOffset_Y1_Reverse;
	}
	else {
		ptNewPos.y += m_iOffset_Y1;
	}

	if (ptNewPos.y < m_dataRange_Y1_2nd.m_ptRange.x) {
		ptNewPos.y = m_dataRange_Y1_2nd.m_ptRange.x;
	}
	if (ptNewPos.y > m_dataRange_Y1_2nd.m_ptRange.y) {
		ptNewPos.y = m_dataRange_Y1_2nd.m_ptRange.y;
	}

	// 정상 그래프일 경우
	if (bReverse == FALSE) {
		if (m_pList_Y1_2nd) {
			m_pList_Y1_2nd->AddTail(ptNewPos);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else {
		if (m_pListBack_Y1_2nd) {
			m_pListBack_Y1_2nd->AddTail(ptNewPos);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CScopeCtrl::AddPoint_Y2(CPoint ptNewPos, BOOL bReverse /*= FALSE*/)
{
	// Range 영역을 벗어나는 데이터일 경우 최대 값으로 설정
	if (ptNewPos.x < m_dataRange_X.m_ptRange.x) {
		ptNewPos.x = m_dataRange_X.m_ptRange.x;
	}
	if (ptNewPos.x > m_dataRange_X.m_ptRange.y) {
		ptNewPos.x = m_dataRange_X.m_ptRange.y;
	}

	// Offset 값을 적용한다.
	if (bReverse == TRUE) {
		ptNewPos.y += m_iOffset_Y2_Reverse;
	}
	else {
		ptNewPos.y += m_iOffset_Y2;
	}

	if (ptNewPos.y < m_dataRange_Y2.m_ptRange.x) {
		ptNewPos.y = m_dataRange_Y2.m_ptRange.x;
	}
	if (ptNewPos.y > m_dataRange_Y2.m_ptRange.y) {
		ptNewPos.y = m_dataRange_Y2.m_ptRange.y;
	}

	// 정상 그래프일 경우
	if (bReverse == FALSE) {
		if (m_pList_Y2) {
			m_pList_Y2->AddTail(ptNewPos);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else {
		if (m_pListBack_Y2) {
			m_pListBack_Y2->AddTail(ptNewPos);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return FALSE;
}

void CScopeCtrl::SetOffset_Y1(int iOffset, int iOffset_Reverse)
{
	m_iOffset_Y1 = iOffset;
	m_iOffset_Y1_Reverse = iOffset_Reverse;
}

void CScopeCtrl::SetOffset_Y2(int iOffset, int iOffset_Reverse)
{
	m_iOffset_Y2 = iOffset;
	m_iOffset_Y2_Reverse = iOffset_Reverse;
}

void CScopeCtrl::DrawText_Graph(CPoint ptPos, CString strText, COLORREF crText, int iAlign/* = TA_LEFT*/)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont);
	UNUSED_ALWAYS(pOldFont);

	m_pMemDC->SetTextColor(m_crText);

	m_pMemDC->SetTextAlign(iAlign);
	m_pMemDC->SetTextColor(crText);
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strText);

	m_pMemDC->SelectObject(pOldFont);
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawText_Graph_Slim(CPoint ptPos, CString strText, COLORREF crText, int iAlign/* = TA_LEFT*/)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont_Slim);
	UNUSED_ALWAYS(pOldFont);

	m_pMemDC->SetTextColor(m_crText);

	m_pMemDC->SetTextAlign(iAlign);
	m_pMemDC->SetTextColor(crText);
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strText);

	m_pMemDC->SelectObject(pOldFont);
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawText_Result(CPoint ptPos, CString strText, COLORREF crText, int iAlign /*= TA_LEFT*/)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont_Result);
	UNUSED_ALWAYS(pOldFont);
	m_pMemDC->SetTextColor(m_crText);

	m_pMemDC->SetTextAlign(iAlign);
	m_pMemDC->SetTextColor(crText);
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strText);

	m_pMemDC->SelectObject(pOldFont);
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawText_Result_Slim(CPoint ptPos, CString strText, COLORREF crText, int iAlign /*= TA_LEFT*/)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont_Result_Slim);
	UNUSED_ALWAYS(pOldFont);
	m_pMemDC->SetTextColor(m_crText);

	m_pMemDC->SetTextAlign(iAlign);
	m_pMemDC->SetTextColor(crText);
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strText);

	m_pMemDC->SelectObject(pOldFont);
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawText_PortNo(CPoint ptPos, int iPortNo)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont_Result);
	UNUSED_ALWAYS(pOldFont);
	m_pMemDC->SetTextColor(m_crText);

	CString strData;
	strData.Format(_T("%d"), iPortNo);

	m_pMemDC->SetTextAlign(TA_CENTER);
	m_pMemDC->SetTextColor(RGB(255, 255, 255));
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strData);

	m_pMemDC->SelectObject(pOldFont);
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawText_Scale(CPoint ptPos, CString strText, COLORREF crText, int iAlign /*= TA_LEFT*/)
{
	CPoint ptConvert = convertPointToPixel_Y1(ptPos);

	DrawText_Graph_Slim(ptConvert, strText, crText, iAlign);
}

void CScopeCtrl::DrawText_FontSize(CPoint ptPos, CString strText, COLORREF crText, int iAlign /*= TA_LEFT*/, int iFontHeight /*= 10*/, int iFontWidth /*= 0*/, CString strFontName /*= _T("MS Shell Dlg")*/)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	CFont font;
	font.CreateFont(iFontHeight,		// 글자 높이
					iFontWidth,			// 글자 너비
					0,					// 출력 각도
					0,					// 기준 선에서의 각도
					FW_NORMAL,			// 글자 굵기
					FALSE,				// Italic 적용 여부
					FALSE,				// 밑줄 적용 여부
					FALSE,				// 취소선 적용 여부
					DEFAULT_CHARSET,	// 문자셋 종류
					OUT_DEFAULT_PRECIS,	// 출력 정밀도
					CLIP_DEFAULT_PRECIS,// 클리핑 정밀도
					DEFAULT_QUALITY,	// 출력 문자 품질
					DEFAULT_PITCH,		// 글꼴 Pitch
					_T("MS Shell Dlg")	// 글꼴
					);

	CFont *pOldFont = m_pMemDC->SelectObject(&font);
	UNUSED_ALWAYS(pOldFont);
	m_pMemDC->SetTextColor(m_crText);

	m_pMemDC->SetTextAlign(iAlign);
	m_pMemDC->SetTextColor(crText);
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strText);

	m_pMemDC->SelectObject(pOldFont);
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawLine()
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	POSITION pos = NULL;
	CPoint pt1, pt2;
	CPoint ptStart, ptEnd;

	// 정방향 Y1 라인을 그린다.
	if (m_pList_Y1 != NULL && m_pList_Y1->GetCount() > 1) {
		m_pMemDC->SelectObject(m_DrawPen_Y1);

		// go to first point to draw from 
		pos = m_pList_Y1->GetHeadPosition();
		while (pos) {
			pt1 = m_pList_Y1->GetNext(pos);
			ptStart = convertPointToPixel_Y1(pt1);

			if (pos != NULL) {
				pt2 = m_pList_Y1->GetAt(pos);
				ptEnd = convertPointToPixel_Y1(pt2);

				m_pMemDC->MoveTo(ptStart);
				m_pMemDC->LineTo(ptEnd);
			}
		}
	}

	// 정방향 Y1 2nd 라인을 그린다.
	if (m_pList_Y1_2nd != NULL && m_pList_Y1_2nd->GetCount() > 1) {
		m_pMemDC->SelectObject(m_DrawPen_Y1_2nd);

		// go to first point to draw from 
		pos = m_pList_Y1_2nd->GetHeadPosition();
		while (pos) {
			pt1 = m_pList_Y1_2nd->GetNext(pos);
			ptStart = convertPointToPixel_Y1(pt1);

			if (pos != NULL) {
				pt2 = m_pList_Y1_2nd->GetAt(pos);
				ptEnd = convertPointToPixel_Y1(pt2);

				m_pMemDC->MoveTo(ptStart);
				m_pMemDC->LineTo(ptEnd);
			}
		}
	}

	// 역방향 Y1 라인을 그린다.
	if (m_pListBack_Y1 != NULL && m_pListBack_Y1->GetCount() > 1) {
		m_pMemDC->SelectObject(m_DrawPen_Y1_Back);

		// go to first point to draw from 
		pos = m_pListBack_Y1->GetHeadPosition();
		while (pos) {
			pt1 = m_pListBack_Y1->GetNext(pos);
			ptStart = convertPointToPixel_Y1(pt1);

			if (pos != NULL) {
				pt2 = m_pListBack_Y1->GetAt(pos);
				ptEnd = convertPointToPixel_Y1(pt2);

				m_pMemDC->MoveTo(ptStart);
				m_pMemDC->LineTo(ptEnd);
			}
		}
	}

	// 역방향 Y1 2nd 라인을 그린다.
	if (m_pListBack_Y1_2nd != NULL && m_pListBack_Y1_2nd->GetCount() > 1) {
		m_pMemDC->SelectObject(m_DrawPen_Y1_2nd_Back);

		// go to first point to draw from 
		pos = m_pListBack_Y1_2nd->GetHeadPosition();
		while (pos) {
			pt1 = m_pListBack_Y1_2nd->GetNext(pos);
			ptStart = convertPointToPixel_Y1(pt1);

			if (pos != NULL) {
				pt2 = m_pListBack_Y1_2nd->GetAt(pos);
				ptEnd = convertPointToPixel_Y1(pt2);

				m_pMemDC->MoveTo(ptStart);
				m_pMemDC->LineTo(ptEnd);
			}
		}
	}

	// 정방향 Y2 라인을 그린다.
	if (m_pList_Y2 != NULL && m_pList_Y2->GetCount() > 1) {
		m_pMemDC->SelectObject(m_DrawPen_Y2);

		// go to first point to draw from 
		pos = m_pList_Y2->GetHeadPosition();
		while (pos) {
			pt1 = m_pList_Y2->GetNext(pos);
			ptStart = convertPointToPixel_Y2(pt1);

			if (pos != NULL) {
				pt2 = m_pList_Y2->GetAt(pos);
				ptEnd = convertPointToPixel_Y2(pt2);

				m_pMemDC->MoveTo(ptStart);
				m_pMemDC->LineTo(ptEnd);
			}
		}
	}

	// 역방향 Y2 라인을 그린다.
	if (m_pListBack_Y2 != NULL && m_pListBack_Y2->GetCount() > 1) {
		m_pMemDC->SelectObject(m_DrawPen_Y2_Back);

		// go to first point to draw from 
		pos = m_pListBack_Y2->GetHeadPosition();
		while (pos) {
			pt1 = m_pListBack_Y2->GetNext(pos);
			ptStart = convertPointToPixel_Y2(pt1);

			if (pos != NULL) {
				pt2 = m_pListBack_Y2->GetAt(pos);
				ptEnd = convertPointToPixel_Y2(pt2);

				m_pMemDC->MoveTo(ptStart);
				m_pMemDC->LineTo(ptEnd);
			}
		}
	}
	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawLine_Dash(CPoint ptStart, CPoint ptEnd, COLORREF crColor)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	m_DrawPen_Y1_Dash.DeleteObject();
	m_DrawPen_Y1_Dash.CreatePen(PS_DOT, m_iDrawPenWidth, crColor);

	m_pMemDC->SelectObject(m_DrawPen_Y1_Dash);

	CPoint ptStart_1, ptEnd_1;

	ptStart_1 = convertPointToPixel_Y1(ptStart);
	ptEnd_1 = convertPointToPixel_Y1(ptEnd);

	m_pMemDC->MoveTo(ptStart_1);
	m_pMemDC->LineTo(ptEnd_1);

	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawLine_Scale(CPoint ptStart, CPoint ptEnd, COLORREF crColor)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	m_DrawPen_Y1_Dash.DeleteObject();
	m_DrawPen_Y1_Dash.CreatePen(PS_SOLID, m_iDrawPenWidth, crColor);

	m_pMemDC->SelectObject(m_DrawPen_Y1_Dash);

	CPoint ptStart_1, ptEnd_1;

	ptStart_1 = convertPointToPixel_Y1(ptStart);
	ptEnd_1 = convertPointToPixel_Y1(ptEnd);

	m_pMemDC->MoveTo(ptStart_1);
	m_pMemDC->LineTo(ptEnd_1);

	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawLine_Cross(CPoint ptPos, int iSize, int iWidth, COLORREF crColor)
{
	if (!m_pMemDC->GetSafeHdc())
		return;

	m_DrawPen_Y1_Cross.DeleteObject();
	m_DrawPen_Y1_Cross.CreatePen(PS_SOLID, iWidth, crColor);

	m_pMemDC->SelectObject(m_DrawPen_Y1_Cross);

	CPoint ptStart, ptEnd;
	CPoint ptStart_1, ptEnd_1;

	ptStart.x = ptPos.x - (iSize / 2);
	ptStart.y = ptPos.y;// - iSize;

	ptEnd.x = ptPos.x + (iSize / 2);
	ptEnd.y = ptPos.y;// + iSize;

	ptStart_1 = convertPointToPixel_Y1(ptStart);
	ptEnd_1 = convertPointToPixel_Y1(ptEnd);

	m_pMemDC->MoveTo(ptStart_1);
	m_pMemDC->LineTo(ptEnd_1);

	///////////////////////////////////////////////

	ptStart.x = ptPos.x;// - iSize;
	ptStart.y = ptPos.y + iSize;

	ptEnd.x = ptPos.x;// + iSize;
	ptEnd.y = ptPos.y - iSize;

	ptStart_1 = convertPointToPixel_Y1(ptStart);
	ptEnd_1 = convertPointToPixel_Y1(ptEnd);

	m_pMemDC->MoveTo(ptStart_1);
	m_pMemDC->LineTo(ptEnd_1);

	InvalidateRect(m_rcClient);
}

void CScopeCtrl::DrawLine_Rect_Dash(CPoint ptStart, CPoint ptEnd, COLORREF crColor)
{
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	m_DrawPen_Y1_Dash.DeleteObject();
	m_DrawPen_Y1_Dash.CreatePen(PS_DOT, m_iDrawPenWidth, crColor);

	m_pMemDC->SelectObject(m_DrawPen_Y1_Dash);

	CPoint ptStart_1, ptEnd_1;

	ptStart_1 = convertPointToPixel_Y1(ptStart);
	ptEnd_1 = convertPointToPixel_Y1(ptEnd);

	m_pMemDC->MoveTo(ptStart_1);
	m_pMemDC->LineTo(CPoint(ptEnd_1.x, ptStart_1.y));

	m_pMemDC->MoveTo(CPoint(ptEnd_1.x, ptStart_1.y));
	m_pMemDC->LineTo(CPoint(ptEnd_1.x, ptEnd_1.y));

	m_pMemDC->MoveTo(CPoint(ptEnd_1.x, ptEnd_1.y));
	m_pMemDC->LineTo(CPoint(ptStart_1.x, ptEnd_1.y));

	m_pMemDC->MoveTo(CPoint(ptStart_1.x, ptEnd_1.y));
	m_pMemDC->LineTo(ptStart_1);

	InvalidateRect(m_rcClient);
}

void CScopeCtrl::Draw_Linearity_Range(CPoint ptStart, CPoint ptEnd, CPoint ptMax)
{
	DrawLine_Dash(ptStart, ptEnd, RGB(255, 255, 0));

	CPoint ptStart_1;
	CPoint ptStart_2;

	ptStart_1 = ptStart;
	ptStart_1.y -= 150;

	if (ptStart_1.y < m_dataRange_Y1.m_ptRange.x) {
		ptStart_1.y = m_dataRange_Y1.m_ptRange.x;
	}
	if (ptStart_1.y > m_dataRange_Y1.m_ptRange.y) {
		ptStart_1.y = m_dataRange_Y1.m_ptRange.y;
	}

	ptStart_2 = ptStart;
	ptStart_2.y += 150;

	if (ptStart_2.y < m_dataRange_Y1.m_ptRange.x) {
		ptStart_2.y = m_dataRange_Y1.m_ptRange.x;
	}
	if (ptStart_2.y > m_dataRange_Y1.m_ptRange.y) {
		ptStart_2.y = m_dataRange_Y1.m_ptRange.y;
	}

	DrawLine_Dash(ptStart_1, ptStart_2, RGB(255, 255, 0) );

	ptStart_1 = ptEnd;
	ptStart_1.y -= 150;

	if (ptStart_1.y < m_dataRange_Y1.m_ptRange.x) {
		ptStart_1.y = m_dataRange_Y1.m_ptRange.x;
	}
	if (ptStart_1.y > m_dataRange_Y1.m_ptRange.y) {
		ptStart_1.y = m_dataRange_Y1.m_ptRange.y;
	}

	ptStart_2 = ptEnd;
	ptStart_2.y += 150;

	if (ptStart_2.y < m_dataRange_Y1.m_ptRange.x) {
		ptStart_2.y = m_dataRange_Y1.m_ptRange.x;
	}
	if (ptStart_2.y > m_dataRange_Y1.m_ptRange.y) {
		ptStart_2.y = m_dataRange_Y1.m_ptRange.y;
	}

	DrawLine_Dash(ptStart_1, ptStart_2, RGB(255, 255, 0));

	// X 그리기
	DrawLine_Cross(ptMax, 10, 2, RGB(255, 0, 0));
}

void CScopeCtrl::Draw_Hysteresis_Max(CPoint ptMax)
{
	// X 그리기
	DrawLine_Cross(ptMax, 20, 2, RGB(255, 255, 255));
}

void CScopeCtrl::Draw_OIS_Stroke(CPoint ptLeft, CPoint ptRight, CPoint ptUp, CPoint ptDown)
{
	// Stroke X
	DrawLine_Dash(ptLeft, CPoint(ptRight.x, ptLeft.y), RGB(255, 255, 0));
	DrawLine_Dash(ptRight, CPoint(ptRight.x, ptLeft.y), RGB(255, 255, 0));

	// Stroke Y
	DrawLine_Dash(ptUp, CPoint(ptUp.x, ptDown.y), RGB(0, 0, 255));
	DrawLine_Dash(ptDown, CPoint(ptUp.x, ptDown.y), RGB(0, 0, 255));
}

void CScopeCtrl::Draw_OISShift_Spec(CPoint ptStart, CPoint ptEnd)
{
	DrawLine_Scale(CPoint(m_dataRange_X.m_ptRange.x, 0), CPoint(m_dataRange_X.m_ptRange.y, 0), RGB(255, 255, 255));
	DrawLine_Scale(CPoint(0, m_dataRange_Y1.m_ptRange.x), CPoint(0, m_dataRange_Y1.m_ptRange.y), RGB(255, 255, 255));

	// -
	DrawLine_Dash(CPoint(ptStart.x, ptStart.y), CPoint(ptEnd.x, ptStart.y), RGB(255, 255, 0));
	// |
	DrawLine_Dash(CPoint(ptEnd.x, ptStart.y), CPoint(ptEnd.x, ptEnd.y), RGB(255, 255, 0));
	// -
	DrawLine_Dash(CPoint(ptEnd.x, ptEnd.y), CPoint(ptStart.x, ptEnd.y), RGB(255, 255, 0));
	// |
	DrawLine_Dash(CPoint(ptStart.x, ptEnd.y), CPoint(ptStart.x, ptStart.y), RGB(255, 255, 0));
}

void CScopeCtrl::UpdateLabels()
{
	// Draw Grids and Labels
	if (!m_pMemDC->GetSafeHdc()) {
		return;
	}

	//Select a specified pen to the device context to draw background lines
	CPen bkLinesPen;
	if (!bkLinesPen.CreatePen(PS_SOLID, 1, m_crGrids)) {
		return;
	}
	if (!m_pMemDC->SelectObject(bkLinesPen)) {
		return;
	}

	CPoint ptGrid;
	CPoint ptLabel_X;
	CPoint ptLabel_Y1;
	CPoint ptLabel_Y2;

	CString strLabel;

	// Vertical Lines, Horizontal Labels
	double dSpacing_X = m_dataRange_X.GetRange() / double(m_dataRange_X.m_iLabelSpacing);

	int iVerticalPixel = int(m_iClientWidth / dSpacing_X);
	for (int i = iVerticalPixel; i < m_iClientWidth; i += iVerticalPixel)
	{
		m_pMemDC->MoveTo(i, m_rcClient.top);
		m_pMemDC->LineTo(i, m_rcClient.bottom);
	}

	// Horizontal Lines, Vertical Labels
	double dSpacing_Y1 = m_dataRange_Y1.GetRange() / double(m_dataRange_Y1.m_iLabelSpacing);

	int iHorizonPixel = int(m_iClientHeight / dSpacing_Y1);
	for (int i = m_iClientHeight; i > 0; i -= iHorizonPixel)
	{
		m_pMemDC->MoveTo(m_rcClient.left, i);
		m_pMemDC->LineTo(m_rcClient.right, i);
	}

	if (m_dataRange_X.m_bUseLabelChar == FALSE)
	{
		// X 간격 출력
		for (int i = 1; i <= int(dSpacing_X); i++)
		{
			if (m_dataRange_X.m_bUseGridLabel == TRUE)
			{
				if (m_dataRange_X.m_dLabelScale == 1.0)
					strLabel.Format(_T("%d"), m_dataRange_X.m_ptRange.x + (m_dataRange_X.m_iLabelSpacing * i));
				else
					strLabel.Format(_T("%.1f"), (m_dataRange_X.m_ptRange.x + (m_dataRange_X.m_iLabelSpacing * i)) * m_dataRange_X.m_dLabelScale);

				ptLabel_X.x = (iVerticalPixel * i) - 5;
				ptLabel_X.y = m_iClientHeight - 15;
				drawText_Label(ptLabel_X, strLabel, m_dataRange_X.m_crNormal, TA_RIGHT);
			}
		}
	}
	else
	{
		CString strTemp = m_dataRange_X.m_strLabelChar;
		int iPos = 0;
		CString strTok;

		// X 간격 출력
		for (int i = 0; i <= int(dSpacing_X); i++)
		{
			if (iPos <= strTemp.GetLength())
			{
				strTok = strTemp.Tokenize(_T("|"), iPos);
				strTok.TrimLeft();
				strTok.TrimRight();

				strLabel.Format(_T("%s"), strTok);
				ptLabel_X.x = (iVerticalPixel * i) - 5;
				ptLabel_X.y = m_iClientHeight - 15;
				drawText_Label(ptLabel_X, strLabel, m_dataRange_X.m_crNormal, TA_RIGHT);
			}
		}
	}

	
	// 단위 출력
	ptLabel_X.x = int(m_iClientWidth / 2);
	
	if (m_dataRange_X.m_bUseGridLabel == TRUE)
		ptLabel_X.y = m_iClientHeight - 35;
	else
		ptLabel_X.y = m_iClientHeight - 15;
	drawText_Label(ptLabel_X, m_dataRange_X.m_strText, m_dataRange_X.m_crNormal, TA_CENTER);

	// Y1 간격 출력	
	for (int i = 1; i <= int(dSpacing_Y1); i++)
	{
		if (m_dataRange_Y1.m_bUseGridLabel == TRUE)
		{
			ptLabel_Y1.y = ptLabel_Y2.y = m_iClientHeight - (iHorizonPixel * i) - 5;

			if (m_dataRange_Y1.m_dLabelScale == 1.0)
			{
				if (i == 1)
					strLabel.Format(_T("%d %s"), m_dataRange_Y1.m_ptRange.x + (m_dataRange_Y1.m_iLabelSpacing * i),
					m_dataRange_Y1.m_strText);
				else
					strLabel.Format(_T("%d"), m_dataRange_Y1.m_ptRange.x + (m_dataRange_Y1.m_iLabelSpacing * i));
			}
			else
			{
				if (i == 1)
					strLabel.Format(_T("%.1f %s"), (m_dataRange_Y1.m_ptRange.x + (m_dataRange_Y1.m_iLabelSpacing * i)) * m_dataRange_Y1.m_dLabelScale,
					m_dataRange_Y1.m_strText);
				else
					strLabel.Format(_T("%.1f"), (m_dataRange_Y1.m_ptRange.x + (m_dataRange_Y1.m_iLabelSpacing * i)) * m_dataRange_Y1.m_dLabelScale);
			}			

			ptLabel_Y1.x = 2;
			drawText_Label(ptLabel_Y1, strLabel, m_dataRange_Y1.m_crNormal, TA_LEFT);
		}

		if (m_dataRange_Y2.m_bUseGridLabel == TRUE)
		{
			if (i == 1)
				strLabel.Format(_T("%s %d"), m_dataRange_Y2.m_strText,
				m_dataRange_Y2.m_ptRange.x + (m_dataRange_Y2.m_iLabelSpacing * i));
			else
				strLabel.Format(_T("%d"), m_dataRange_Y2.m_ptRange.x + (m_dataRange_Y2.m_iLabelSpacing * i));

			ptLabel_Y2.x = m_iClientWidth - 5;
			drawText_Label(ptLabel_Y2, strLabel, m_dataRange_Y2.m_crNormal, TA_RIGHT);
		}
	}
}

BOOL CScopeCtrl::InvalidateCtrl()
{
	CClientDC dc(this);

	if (m_pMemDC->GetSafeHdc())
		return FALSE;

	if (!m_pMemDC->CreateCompatibleDC(&dc))
		return FALSE;
	
	m_pMemDC->SetBkColor(m_crBackGround);

	CBitmap bmp;
	if (!bmp.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height()))
		return FALSE;

	if (!m_pMemDC->SelectObject(bmp))
		return FALSE;

	//Set the background color of the control
	CBrush bkBrush;
	if (!bkBrush.CreateSolidBrush(m_crBackGround))
		return FALSE;

	m_pMemDC->FillRect(m_rcClient, &bkBrush);

	//Select a specified pen to the device context to draw background lines
	CPen bkLinesPen;
	if (!bkLinesPen.CreatePen(PS_SOLID, 1, m_crGrids))
		return FALSE;

	if (!m_pMemDC->SelectObject(bkLinesPen))
		return FALSE;

	InvalidateRect(m_rcClient);
	return TRUE;
}

void CScopeCtrl::clearList()
{
	if (m_pList_Y1) {
		m_pList_Y1->RemoveAll();
	}
	if (m_pListBack_Y1) {
		m_pListBack_Y1->RemoveAll();
	}

	if (m_pList_Y1_2nd) {
		m_pList_Y1_2nd->RemoveAll();
	}
	if (m_pListBack_Y1_2nd) {
		m_pListBack_Y1_2nd->RemoveAll();
	}

	if (m_pList_Y2) {
		m_pList_Y2->RemoveAll();
	}
	if (m_pListBack_Y2) {
		m_pListBack_Y2->RemoveAll();
	}
}

void CScopeCtrl::drawText_Label(CPoint ptPos, CString strText, COLORREF crText, int iAlign /*= TA_LEFT*/)
{
	if (!m_pMemDC->GetSafeHdc())
		return;

	CFont *pOldFont = m_pMemDC->SelectObject(&m_TextFont);
	UNUSED_ALWAYS(pOldFont);

	m_pMemDC->SetTextColor(m_crText);

	m_pMemDC->SetTextAlign(iAlign);
	m_pMemDC->SetTextColor(crText);
	m_pMemDC->TextOut(ptPos.x, ptPos.y, strText);
	InvalidateRect(m_rcClient);
}

CPoint CScopeCtrl::convertPointToPixel_Y1(const CPoint ptPos)
{
	CPoint retPos;
	retPos.x = int(m_fScale_X * (ptPos.x - m_dataRange_X.m_ptRange.x));
	retPos.y = int(m_fScale_Y1 * (ptPos.y - m_dataRange_Y1.m_ptRange.x));
	retPos.y = m_iClientHeight - retPos.y;

	return retPos;
}

CPoint CScopeCtrl::convertPointToPixel_Y2(const CPoint ptPos)
{
	CPoint retPos;
	retPos.x = int(m_fScale_X * (ptPos.x - m_dataRange_X.m_ptRange.x));
	retPos.y = int(m_fScale_Y2 * (ptPos.y - m_dataRange_Y2.m_ptRange.x));
	retPos.y = m_iClientHeight - retPos.y;

	return retPos;
}

CPoint CScopeCtrl::convertPixelToPoint_Y1(const CPoint ptPos)
{
	CPoint retPos;
	retPos.x = int(ptPos.x / m_fScale_X) + m_dataRange_X.m_ptRange.x;
	retPos.y = int(ptPos.y / m_fScale_Y1) + m_dataRange_Y1.m_ptRange.x;

	return retPos;
}

CPoint CScopeCtrl::convertPixelToPoint_Y2(const CPoint ptPos)
{
	CPoint retPos;
	retPos.x = int(ptPos.x / m_fScale_X) + m_dataRange_X.m_ptRange.x;
	retPos.y = int(ptPos.y / m_fScale_Y2) + m_dataRange_Y2.m_ptRange.x;

	return retPos;
}

void CScopeCtrl::setRange_X(const MScopeRange dataRange)
{
	m_dataRange_X = dataRange;

	m_fScale_X = double(m_iClientWidth) / double(m_dataRange_X.m_ptRange.y - m_dataRange_X.m_ptRange.x);
}

void CScopeCtrl::setRange_Y1(const MScopeRange dataRange)
{
	m_dataRange_Y1 = dataRange;

	m_fScale_Y1 = double(m_iClientHeight) / double(m_dataRange_Y1.m_ptRange.y - m_dataRange_Y1.m_ptRange.x);
}

void CScopeCtrl::setRange_Y1_2nd(const MScopeRange dataRange)
{
	m_dataRange_Y1_2nd = dataRange;
}


void CScopeCtrl::setRange_Y2(const MScopeRange dataRange)
{
	m_dataRange_Y2 = dataRange;

	m_fScale_Y2 = double(m_iClientHeight) / double(m_dataRange_Y2.m_ptRange.y - m_dataRange_Y2.m_ptRange.x);
}


void CScopeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// 더블 클릭 이벤트는 안 먹네;;;; - 2014.03.02
	m_pParentWnd->SendMessage(WM_SCOPE_CTRL_MESSAGE, (WPARAM)this, 0);

	CWnd::OnLButtonDblClk(nFlags, point);
}


void CScopeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_pParentWnd->SendMessage(WM_SCOPE_CTRL_MESSAGE, (WPARAM)this, 0);

	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CScopeCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}