
#include "pch.h"

#include "cbtn.h"

//constexpr int IMAGE_WIDTH = 22;  // 개별 이미지 너비
//constexpr int IMAGE_HEIGHT = 22; // 개별 이미지 높이


BEGIN_MESSAGE_MAP(cbtn, CButton)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



cbtn::cbtn()
{
  m_bgColor = RGB(250, 250, 250); // 기본 배경색 흰색
  m_textColor = (RGB(0, 0, 0));      // 기본 글자색 검정
  m_mouseOver = FALSE;
}

cbtn::~cbtn() {
  if (m_image.IsDIBSection())
  {
    m_image.Destroy(); // 이미지 정리
  }
}

void cbtn::SetBackgroundColor(COLORREF color)
{
  m_bgColor = color;
  Invalidate(); // 버튼 다시 그리기
}

void cbtn::SetTextColor(COLORREF color)
{
  m_textColor = color;
  Invalidate(); // 버튼 다시 그리기
}

void cbtn::SetImageList(UINT nImageID, int cx, COLORREF crMask)
{
  CBitmap bmp;
  bmp.LoadBitmap(nImageID); // 리소스에서 비트맵 로드

  m_imageList.DeleteImageList(); // 기존 이미지 리스트 제거
  m_imageList.Create(cx, cx, ILC_COLOR24 | ILC_MASK, 5, 1); // 이미지 리스트 생성 (22x22 크기)
  m_imageList.Add(&bmp, crMask); // 비트맵 추가
}

//void cbtn::SetButtonState(int state)
//{
//  if (state >= 0 && state < 5)
//  {
//    m_currentState = state;
//    Invalidate(); // 버튼을 다시 그리도록 요청
//  }
//}

//void cbtn::SetImage(LPCTSTR imagePath)
//{
//  m_image.Destroy(); // 기존 이미지 해제
//  m_image.Load(imagePath); // 이미지 로드
//  Invalidate(); // 버튼 다시 그리기
//}

void cbtn::LoadImages(LPCTSTR lpszFilePath, CSize size)
{
  if (!m_image.IsNull())
    m_image.Destroy();
  m_imageSize = size;
  HRESULT hr = m_image.Load(lpszFilePath);
  if (FAILED(hr))
  {
    AfxMessageBox(_T("Failed to load image."));
  }
}

BOOL cbtn::SetCheck(BOOL checked)
{
  m_isChecked = checked;
  Invalidate(); // 버튼 다시 그리기
  return m_isChecked;
}

BOOL cbtn::GetCheck() const
{
  return m_isChecked;
}


void cbtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

  // 버튼 영역 가져오기
  CRect rect = lpDrawItemStruct->rcItem;

#if 0
  // 버튼 상태에 따른 색상 정의 (텍스트 색상)
  COLORREF textColor = RGB(0, 0, 0);

  // 상태별로 이미지 선택
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    m_currentState = 4; // 비활성화 상태
  else if (lpDrawItemStruct->itemState & ODS_SELECTED)
    m_currentState = 2; // 클릭 상태
  else if (m_mouseOver)
    m_currentState = 1; // 마우스 오버 상태
  else
    m_currentState = 0; // 기본 상태

  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    textColor = RGB(150, 150, 150); // 비활성화 텍스트 색

  // 둥근 테두리 설정
  int cornerRadius = 10; // 모서리 둥글기 반경
  CRect innerRect(rect);
  innerRect.DeflateRect(2, 2); // 3D 테두리와 내용 간의 여백

  // 배경 이미지 그리기
  if (!m_image.IsNull())
  {
    // 이미지 크기
    int imageHeight = m_image.GetHeight() / 5; // 상태당 높이
    int imageWidth = m_image.GetWidth();      // 전체 이미지의 폭

    // 상태별로 선택된 이미지 부분 계산
    int offsetY = m_currentState * imageHeight;
    CRect srcRect(0, offsetY, imageWidth, offsetY + imageHeight);

    // 이미지를 버튼 크기로 확대하여 출력
    m_image.Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(),
      srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
  }
  else
  {
    // 기본 배경색으로 채우기 (이미지가 없는 경우)
    CBrush backgroundBrush(RGB(240, 240, 240));
    CBrush* pOldBrush = pDC->SelectObject(&backgroundBrush);
    pDC->RoundRect(&innerRect, CPoint(cornerRadius, cornerRadius));
    pDC->SelectObject(pOldBrush);
  }

  // 둥근 테두리 그리기
  CPen borderPen(PS_SOLID, 1, RGB(200, 200, 200));
  CPen* pOldPen = pDC->SelectObject(&borderPen);
  pDC->RoundRect(&rect, CPoint(cornerRadius, cornerRadius));
  pDC->SelectObject(pOldPen);

  // 텍스트 그리기
  CString text;
  GetWindowText(text);
  CRect textRect(innerRect);
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(textColor);
  pDC->DrawText(text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


#endif

#if 1
  // 버튼의 상태에 따른 색상 정의
  COLORREF borderColor = RGB(200, 200, 200); // 기본 테두리 색상
  COLORREF backgroundColor = RGB(240, 240, 240); // 기본 배경색
  COLORREF textColor = RGB(0, 0, 0); // 기본 텍스트 색상

  if (lpDrawItemStruct->itemState & ODS_SELECTED)
  {
    borderColor = RGB(150, 150, 150);   // 클릭 상태 테두리 색
    backgroundColor = RGB(220, 220, 220); // 클릭 상태 배경색
  }
  else if (m_mouseOver)
  {
    borderColor = RGB(100, 100, 255);   // 마우스 오버 테두리 색
    backgroundColor = RGB(220, 220, 255); // 마우스 오버 배경색
  }
  else if (lpDrawItemStruct->itemState & ODS_DISABLED)
  {
    borderColor = RGB(180, 180, 180);   // 비활성화 테두리 색
    textColor = RGB(150, 150, 150);     // 비활성화 텍스트 색
  }

  // 둥근 테두리 그리기
  CBrush brushBackground(backgroundColor);
  CPen penBorder(PS_SOLID, 1, borderColor);
  CBrush* pOldBrush = pDC->SelectObject(&brushBackground);
  CPen* pOldPen = pDC->SelectObject(&penBorder);

  int cornerRadius = 10; // 모서리 반경
  pDC->RoundRect(&rect, CPoint(cornerRadius, cornerRadius));

  // 이전 객체 복원
  pDC->SelectObject(pOldBrush);
  pDC->SelectObject(pOldPen);

  // 내부 영역 설정 (테두리 내부로 조금 좁힘)
  rect.DeflateRect(5, 5);

  // 텍스트와 이미지 처리
  CString text;
  GetWindowText(text);
  CSize textSize = pDC->GetTextExtent(text);


  //int subImageHeight = m_image.GetHeight() / 5; // 각 이미지 높이
  int imageTop = 0; // 기본 이미지: 두 번째
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
  {
		imageTop = m_imageSize.cy * 3; // 비활성화 상태 : 네 번째 이미지
  }
  else if (lpDrawItemStruct->itemState & ODS_SELECTED)
  {
    imageTop = m_imageSize.cy * 2; // 눌림 상태: 세 번째 이미지
  }
  else if (m_mouseOver)
  {
    imageTop = m_imageSize.cy;
  }

  int imageWidth = 0;
  if (!m_image.IsNull())
  {
    // 이미지 처리
    int buttonHeight = rect.Height();
    int imageHeight = static_cast<int>(buttonHeight * 0.8);
    imageWidth = static_cast<int>((m_imageSize.cx * imageHeight) / m_imageSize.cy);

    int imageX = rect.left + 5; // 좌측 정렬
    int imageY = rect.top + (buttonHeight - imageHeight) / 2;
    int yOffset = imageTop;//m_currentState * IMAGE_HEIGHT;
    CRect srcRect(0, yOffset, m_imageSize.cx, yOffset + m_imageSize.cy);

    m_image.Draw(pDC->m_hDC, imageX, imageY, imageWidth, imageHeight,
      srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
  }

  // 텍스트 처리
  CRect textRect(rect);
  textRect.left += imageWidth + 10; // 이미지 오른쪽으로 간격 추가
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(textColor);
  pDC->DrawText(text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

#endif


#if 0
  // 3D 버튼 스타일을 위한 프레임 그리기
  UINT edgeType = (lpDrawItemStruct->itemState & ODS_SELECTED) ? EDGE_SUNKEN : EDGE_RAISED;
  pDC->DrawEdge(&rect, edgeType, BF_RECT);

  // 상태별로 이미지 선택
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    m_currentState = 4; // 비활성화 상태
  else if (lpDrawItemStruct->itemState & ODS_SELECTED)
    m_currentState = 2; // 클릭 상태
  else if (m_mouseOver)
    m_currentState = 1; // 마우스 오버 상태
  else
    m_currentState = 0; // 기본 상태

  // 내부 영역 설정 (3D 효과를 제외한 영역)
  rect.DeflateRect(2, 2);

  // 버튼 텍스트를 계산
  CString text;
  GetWindowText(text);
  CSize textSize = pDC->GetTextExtent(text);
  int textHeight = textSize.cy;
  int textWidth = textSize.cx;

  // 이미지가 유효한 경우에만 처리
  int imageWidth = 0;
  if (!m_image.IsNull())
  {
    // 버튼 높이의 80%에 해당하는 이미지 크기를 계산
    int buttonHeight = rect.Height();
    int imageHeight = static_cast<int>(buttonHeight * 0.8);
    imageWidth = static_cast<int>((IMAGE_WIDTH * imageHeight) / IMAGE_HEIGHT);

    // 이미지 위치 계산 (좌측 정렬)
    int imageX = rect.left + 5; // 좌측에서 5px 간격
    int imageY = rect.top + (buttonHeight - imageHeight) / 2;

    // 상태별 Y 오프셋 계산
    int yOffset = m_currentState * IMAGE_HEIGHT;
    CRect srcRect(0, yOffset, IMAGE_WIDTH, yOffset + IMAGE_HEIGHT);

    // 이미지 그리기
    m_image.Draw(pDC->m_hDC, imageX, imageY, imageWidth, imageHeight,
      srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
  }

  // 텍스트 그리기 (중앙 정렬)
  CRect textRect(rect);
  textRect.left += imageWidth + 10; // 이미지 오른쪽에서 10px 간격
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(RGB(0, 0, 0)); // 검은색 텍스트
  pDC->DrawText(text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
#endif




#if 0
  //// 배경 색깔 채우기
CBrush brush(m_bgColor);
pDC->FillRect(rect, &brush);

// 버튼 상태에 따른 처리 (클릭 시 테두리 변경 등)
if (lpDrawItemStruct->itemState & ODS_SELECTED)
{
  pDC->DrawEdge(rect, EDGE_SUNKEN, BF_RECT);
}
else
{
  pDC->DrawEdge(rect, EDGE_RAISED, BF_RECT);
}
      // 상태별로 이미지 선택
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    m_currentState = 4; // 비활성화 상태
  else if (lpDrawItemStruct->itemState & ODS_SELECTED)
    m_currentState = 2; // 클릭 상태
  else if (m_mouseOver)
    m_currentState = 1; // 마우스 오버 상태
  else
    m_currentState = 0; // 기본 상태

  // 이미지 그리기
  if (!m_image.IsNull())
  {
    CSize imageSize(m_image.GetWidth(), m_image.GetHeight());
    CPoint imagePos(
      rect.left + (rect.Width() - imageSize.cx) / 2, // 중앙 정렬
      rect.top + 5                                  // 텍스트 위쪽에 배치
    );
    m_image.Draw(pDC->GetSafeHdc(), imagePos.x, imagePos.y);
  }

  // 텍스트 그리기
  CString text;
  GetWindowText(text); // 버튼의 텍스트 가져오기
  pDC->SetTextColor(m_textColor);
  pDC->SetBkMode(TRANSPARENT);
  pDC->DrawText(text, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
#endif


  // 버튼 배경색 채우기
  //CBrush brushBackground(RGB(240, 240, 240)); // 연한 회색 배경
  //pDC->FillRect(&rect, &brushBackground);


#if 0
// 상태별로 이미지 선택
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    m_currentState = 4; // 비활성화 상태
  else if (lpDrawItemStruct->itemState & ODS_SELECTED)
    m_currentState = 2; // 클릭 상태
  else if (m_mouseOver)
    m_currentState = 1; // 마우스 오버 상태
  else
    m_currentState = 0; // 기본 상태

  // 버튼 배경색 채우기
  CBrush brushBackground(RGB(240, 240, 240)); // 연한 회색 배경
  pDC->FillRect(&rect, &brushBackground);

  // 버튼 텍스트를 계산
  CString text;
  GetWindowText(text);
  CSize textSize = pDC->GetTextExtent(text);
  int textHeight = textSize.cy;
  int textWidth = textSize.cx;

  // 이미지가 유효한 경우에만 처리
  if (!m_image.IsNull())
  {
    // 버튼 높이의 80%에 해당하는 이미지 크기를 계산
    int buttonHeight = rect.Height();
    int imageHeight = static_cast<int>(buttonHeight * 0.8);
    int imageWidth = static_cast<int>((IMAGE_WIDTH * imageHeight) / IMAGE_HEIGHT);

    // 이미지 위치 계산 (텍스트 오른쪽)
    int imageX = rect.left + (rect.Width() - textWidth - imageWidth) / 2 + textWidth + 5; // 텍스트 오른쪽 5px 간격
    int imageY = rect.top + (buttonHeight - imageHeight) / 2;

    // 상태별 Y 오프셋 계산
    int yOffset = m_currentState * IMAGE_HEIGHT;
    CRect srcRect(0, yOffset, IMAGE_WIDTH, yOffset + IMAGE_HEIGHT);

    // 이미지 그리기
    m_image.Draw(pDC->m_hDC, imageX, imageY, imageWidth, imageHeight,
      srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
  }

  // 텍스트 그리기
  int textX = rect.left + (rect.Width() - textWidth - IMAGE_WIDTH) / 2; // 이미지 포함 텍스트 중앙 정렬
  int textY = rect.top + (rect.Height() - textHeight) / 2;
  CRect textRect(textX, textY, textX + textWidth, textY + textHeight);

  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(RGB(0, 0, 0)); // 검은색 텍스트
  pDC->DrawText(text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

#endif



#if 0

  // 버튼 상태에 따른 색상 정의 (텍스트 색상)
  COLORREF textColor = RGB(0, 0, 0);

  // 상태별로 이미지 선택
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    m_currentState = 4; // 비활성화 상태
  else if (lpDrawItemStruct->itemState & ODS_SELECTED)
    m_currentState = 2; // 클릭 상태
  else if (m_mouseOver)
    m_currentState = 1; // 마우스 오버 상태
  else
    m_currentState = 0; // 기본 상태

  if (lpDrawItemStruct->itemState & ODS_DISABLED)
    textColor = RGB(150, 150, 150); // 비활성화 텍스트 색

  // 둥근 테두리 설정
  int cornerRadius = 10; // 모서리 둥글기 반경
  CRect innerRect(rect);
  innerRect.DeflateRect(2, 2); // 3D 테두리와 내용 간의 여백



  // 상태에 맞는 이미지 부분 잘라 그리기
  if (!m_image.IsNull()) // 유효한 비트맵인지 확인
  {
    int yOffset = m_currentState * IMAGE_HEIGHT; // 상태별 Y 오프셋
    CRect srcRect(0, yOffset, IMAGE_WIDTH, yOffset + IMAGE_HEIGHT);
    m_image.Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(),
      srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
  }
  else
  {
    // 기본 배경색으로 채우기 (이미지가 없는 경우)
    CBrush backgroundBrush(RGB(240, 240, 240));
    CBrush* pOldBrush = pDC->SelectObject(&backgroundBrush);
    pDC->RoundRect(&innerRect, CPoint(cornerRadius, cornerRadius));
    pDC->SelectObject(pOldBrush);
  }

  // 둥근 테두리 그리기
  //CPen borderPen(PS_SOLID, 1, RGB(200, 200, 200));
  //CPen* pOldPen = pDC->SelectObject(&borderPen);
  //pDC->RoundRect(&rect, CPoint(cornerRadius, cornerRadius));
  //pDC->SelectObject(pOldPen);

  // 텍스트 그리기
  CString text;
  GetWindowText(text);
  CRect textRect(innerRect);
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(textColor);
  pDC->DrawText(text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
#endif




}

void cbtn::OnMouseMove(UINT nFlags, CPoint point)
{
  if (!m_mouseOver)
  {
    m_mouseOver = TRUE;
    TRACKMOUSEEVENT tme{};
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = GetSafeHwnd();
    TrackMouseEvent(&tme);

    Invalidate(); // 다시 그리기 요청
  }

  CMFCButton::OnMouseMove(nFlags, point);
}

void cbtn::OnLButtonDown(UINT nFlags, CPoint point)
{
  Invalidate();
  CMFCButton::OnLButtonDown(nFlags, point);
}

void cbtn::OnLButtonUp(UINT nFlags, CPoint point)
{
  int change_state = (GetCheck() == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
  SetCheck(change_state);
  Invalidate();

  CMFCButton::OnLButtonUp(nFlags, point);
}

void cbtn::OnMouseLeave()
{
  m_mouseOver = FALSE;

  Invalidate();
  CMFCButton::OnMouseLeave();
}


void cbtn::PreSubclassWindow()
{
  // 기존 스타일에서 BS_CHECKBOX 제거, BS_OWNERDRAW 추가
  //ModifyStyle(BS_CHECKBOX, BS_OWNERDRAW);

  // 부모 클래스의 PreSubclassWindow 호출
  CMFCButton::PreSubclassWindow();
}




