
#include "pch.h"

#include "btn.h"


BEGIN_MESSAGE_MAP(CCustomBtn, CMFCButton)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSELEAVE()
  ON_WM_DRAWITEM()
END_MESSAGE_MAP()


CCustomBtn::CCustomBtn()
{
}

CCustomBtn::~CCustomBtn()
{
  if (m_image.IsDIBSection())
  {
    m_image.Destroy(); // 이미지 정리
  }
}

void CCustomBtn::LoadImages(LPCTSTR lpszFileName, CSize size)
{
	m_imageSize = size;


	if (FAILED(m_image.Load(lpszFileName)))
	{
		AfxMessageBox(_T("Failed to load image."));
		return;
	}

	// 이미지 크기 확인
	int imageWidth = m_image.GetWidth();
	int imageHeight = m_image.GetHeight();

	// 이미지가 3개의 세로 이미지를 포함하는지 확인
	if (imageHeight % 3 != 0)
	{
		AfxMessageBox(_T("Image height is not divisible by 3."));
		m_image.Destroy();
	}
	return;
}

void CCustomBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  CRect rect = lpDrawItemStruct->rcItem;
  // 버튼 상태 확인
  int subImageHeight = m_image.GetHeight() / 3; // 각 이미지 높이
  int imageTop = 0; // 기본 이미지: 두 번째

  if (lpDrawItemStruct->itemState & ODS_SELECTED)
  {
    imageTop = subImageHeight * 2; // 눌림 상태: 세 번째 이미지
  }
  else if (m_mouseOver)
  {
    imageTop = subImageHeight;
  }

  // 배경 그리기
  if (m_image.IsDIBSection())
  {
    // 선택된 이미지를 컨트롤 크기에 맞게 확대
    m_image.StretchBlt(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(),
      0, imageTop, m_image.GetWidth(), subImageHeight, SRCCOPY);
  }
  else
  {
    // 기본 배경 색상
    pDC->FillSolidRect(&rect, RGB(200, 200, 200));
  }
  // 텍스트 그리기

  constexpr COLORREF default_text_color = RGB(255, 255, 255);
  constexpr COLORREF checked_text_color = RGB(96, 96, 96);
  CString text;
  GetWindowText(text);
  pDC->SetTextColor(default_text_color);
  pDC->SetBkMode(TRANSPARENT);
  pDC->DrawText(text, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}


void CCustomBtn::OnMouseMove(UINT nFlags, CPoint point)
{
  if (!m_mouseOver)
  {
    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd };
    TrackMouseEvent(&tme);
    m_mouseOver = TRUE;
    m_currentState = bs_hover;
    Invalidate();
  }

  CMFCButton::OnMouseMove(nFlags, point);
}

void CCustomBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
  m_currentState = bs_pressed;
  Invalidate();

  CMFCButton::OnLButtonDown(nFlags, point);
}

void CCustomBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
  m_currentState = (m_currentState == bs_pressed) ? bs_checked : bs_normal;
  int change_state = (GetCheck() == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
  //SetCheck(change_state);

  Invalidate();

  CMFCButton::OnLButtonUp(nFlags, point);
}

void CCustomBtn::OnMouseLeave()
{
  m_mouseOver = FALSE;
  m_currentState = bs_normal;

  Invalidate();
  CMFCButton::OnMouseLeave();
}

