
#include "pch.h"

#include "toggle_btn.h"


BEGIN_MESSAGE_MAP(CToggleBtn, CButton)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


CToggleBtn::CToggleBtn()
{
	m_bgColor = RGB(250, 250, 250); // 기본 배경색 흰색
	m_textColor = (RGB(0, 0, 0));      // 기본 글자색 검정
	m_currentState = bs_normal;
	m_mouseOver = FALSE;
}

CToggleBtn::~CToggleBtn(){
  if (m_image.IsDIBSection())
  {
    m_image.Destroy(); // 이미지 정리
  }
}

void CToggleBtn::SetBackgroundColor(COLORREF color)
{
  m_bgColor = color;
  Invalidate(); // 버튼 다시 그리기
}

void CToggleBtn::SetTextColor(COLORREF color)
{
  m_textColor = color;
  Invalidate(); // 버튼 다시 그리기
}

void CToggleBtn::LoadImages(LPCTSTR lpszFileName, CSize size)
{
  if (!m_image.IsNull())
    m_image.Destroy();
  m_imageSize = size;
  HRESULT hr = m_image.Load(lpszFileName);
  if (FAILED(hr))
  {
    AfxMessageBox(_T("Failed to load image."));
  }
}

BOOL CToggleBtn::SetCheck(BOOL checked)
{
  m_isChecked = checked;
  Invalidate(); // 버튼 다시 그리기
  return m_isChecked;
}

BOOL CToggleBtn::GetCheck() const
{
  return m_isChecked;
}




afx_msg void CToggleBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  CRect rect = lpDrawItemStruct->rcItem;

  // 상태 가져오기
  UINT ctrl_state = lpDrawItemStruct->itemState;
  BOOL isChecked = (GetCheck() == BST_CHECKED);
  BOOL isDisabled = !IsWindowEnabled();
  constexpr COLORREF default_border_color = RGB(200, 200, 200);
  constexpr COLORREF default_bgcolor = RGB(240, 240, 240);
  constexpr COLORREF checked_bgcolor = RGB(214, 237, 23);
  constexpr COLORREF default_text_color = RGB(0, 0, 0);
  constexpr COLORREF checked_text_color = RGB(96, 96, 96);

  COLORREF borderColor = default_border_color; // 테두리 색상
	COLORREF bgColor = isChecked ? checked_bgcolor : default_bgcolor; // 기본 배경색
  COLORREF textColor = isChecked ? checked_text_color : default_text_color; // 기본 텍스트 색상

  // 상태별로 이미지 선택
  if (ctrl_state & ODS_DISABLED)
    m_currentState = bs_disabled; // 비활성화 상태
  else if (ctrl_state & ODS_SELECTED)
    m_currentState = bs_pressed; // 클릭 상태
  else if (m_mouseOver)
    m_currentState = bs_hover; // 마우스 오버 상태
  else
    m_currentState = bs_normal; // 기본 상태

  // std::cout << "state" << m_currentState  << std::endl;
  // 배경색 설정
  if (isDisabled)
  {
    borderColor = RGB(180, 180, 180);   // 비활성화 테두리 색
    bgColor = RGB(200, 200, 200);  // 비활성 상태 배경색 (회색)
    textColor = RGB(150, 150, 150); // 비활성 상태 글자색 (어두운 회색)
  }
  else if (ctrl_state & ODS_SELECTED)
  {
    borderColor = RGB(150, 150, 150);   // 클릭 상태 테두리 색
    bgColor = RGB(150, 150, 150); // 클릭 상태 배경색 (3D 효과)
    textColor = RGB(255, 255, 255); // 클릭 상태 글자색 
  }
  else if (m_mouseOver)
  {
    borderColor = RGB(100, 100, 255);   // 마우스 오버 테두리 색
    bgColor = RGB(220, 220, 255); // 마우스 오버 상태 배경색 
    textColor = default_text_color; // 마우스 오버 상태 글자색 (검정색)
  }

  // 둥근 테두리 그리기
  CBrush brushBackground(bgColor);
  CPen penBorder(PS_SOLID, 1, borderColor);
  CBrush* pOldBrush = pDC->SelectObject(&brushBackground);
  CPen* pOldPen = pDC->SelectObject(&penBorder);

  int cornerRadius = 5; // 모서리 반경
  pDC->RoundRect(&rect, CPoint(cornerRadius, cornerRadius));

  // 이전 객체 복원
  pDC->SelectObject(pOldBrush);
  pDC->SelectObject(pOldPen);

  // 내부 영역 설정 (테두리 내부로 조금 좁힘)
  rect.DeflateRect(2, 2);
  
  int imageWidth = 0;
  if (!m_image.IsNull())
  {
    // 이미지 처리
    int buttonHeight = rect.Height();
    constexpr double image_rate = 1.0;
    int imageHeight = static_cast<int>(buttonHeight * image_rate);
    imageWidth = static_cast<int>((m_imageSize.cx * imageHeight) / m_imageSize.cy);

    constexpr int inner_left = 2; //  좌측 들여 그리는 여백
    int imageX = rect.left + inner_left; 
    int imageY = rect.top + ((buttonHeight - imageHeight) / 2);
    int yOffset = (m_currentState) * m_imageSize.cy;

    if (isChecked)
      yOffset = bs_pressed * m_imageSize.cy;
    CRect srcRect(0, yOffset, m_imageSize.cx, yOffset + m_imageSize.cy);

    m_image.Draw(pDC->m_hDC, imageX, imageY, imageWidth, imageHeight,
      srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
  } 
  // 텍스트 처리
  CString text;
  GetWindowText(text);
  CSize textSize = pDC->GetTextExtent(text);
  CRect textRect(rect);

  constexpr int text_inner_left = 7; //  
  textRect.left += imageWidth + text_inner_left; // 이미지 오른쪽으로 간격 추가
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(textColor);
  pDC->DrawText(text, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}



void CToggleBtn::OnMouseMove(UINT nFlags, CPoint point)
{
  if (!m_mouseOver)
  {
    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd };
    TrackMouseEvent(&tme);
    m_mouseOver = TRUE;
    m_currentState = bs_hover;
    Invalidate();
  }

  CButton::OnMouseMove(nFlags, point);
}

void CToggleBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
  m_currentState = bs_pressed;
  Invalidate();

  CButton::OnLButtonDown(nFlags, point);
}

void CToggleBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
  m_currentState = (m_currentState == bs_pressed) ? bs_checked : bs_normal;
  int change_state = (GetCheck() == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
  SetCheck(change_state );
  Invalidate();

  CButton::OnLButtonUp(nFlags, point);
}

void CToggleBtn::OnMouseLeave()
{
  m_mouseOver = FALSE;
  m_currentState = bs_normal;

  Invalidate();
  CButton::OnMouseLeave();
}

void CToggleBtn::PreSubclassWindow()
{
  // 기존 스타일에서 BS_CHECKBOX 제거, BS_OWNERDRAW 추가
  ModifyStyle(BS_CHECKBOX, BS_OWNERDRAW);

  // 부모 클래스의 PreSubclassWindow 호출
  CButton::PreSubclassWindow();
}



#if 0
// 배경색 채우기
pDC->FillSolidRect(rect, bgColor);

// 이미지 크기 계산 (컨트롤러 높이의 90% 크기로 조정)
int imageWidth = 38;
int imageHeight = (int)(rect.Height() * 0.9); // 높이를 90%로 조정

// 이미지에서 사용할 Y 위치 결정 (상태에 따라)
int sourceY = 0; // 기본 상태 (Unchecked)
if (isDisabled)
{
  sourceY = 2 * imageHeight; // 비활성화 상태 이미지
}
else if (isChecked)
{
  sourceY = imageHeight; // 체크 상태 이미지
}
// 체크되지 않은 상태는 sourceY = 0 으로 두고, 체크된 상태는 sourceY = imageHeight, 비활성화 상태는 2 * imageHeight로 처리

int imageX = rect.left + 10; // 왼쪽 여백 10
int imageY = rect.top + (rect.Height() - imageHeight) / 2; // 수직 중앙 정렬

// 이미지를 그리기 위한 메모리 DC 준비
CDC memDC;
memDC.CreateCompatibleDC(pDC);

// 이미지 로드 (배경이 투명한 PNG)
if (!m_image.IsNull())
{
  // 상태에 맞는 이미지를 그리기
  CBitmap bitmap;
  HBITMAP hBitmap = m_image;  // CImage에서 HBITMAP을 가져옵니다.
  bitmap.Attach(hBitmap);     // HBITMAP을 CBitmap에 첨부

  CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

  pDC->BitBlt(
    imageX, imageY, imageWidth, imageHeight, // 대상 영역
    &memDC, 0, sourceY, SRCCOPY             // 원본 이미지에서 추출
  );

  memDC.SelectObject(pOldBitmap);
}

// 텍스트 그리기
CString text;
GetWindowText(text);

// 텍스트 영역 계산 (이미지 오른쪽에 텍스트 배치)
CRect textRect = rect;
textRect.left += 10 + (rect.Height() * 0.8) + 10; // 왼쪽 여백 + 이미지 너비 + 간격 10

pDC->SetBkMode(TRANSPARENT);
pDC->SetTextColor(textColor);
pDC->DrawText(text, textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
#endif

#if 0
// 상태별 배경색 설정
switch (m_currentState)
{
case StateNormal:
  pDC->FillSolidRect(&rect, m_bgColor);
  break;
case StateHover:
  pDC->FillSolidRect(&rect, RGB(255, 255, 0));  // 노란색 배경
  pDC->Draw3dRect(&rect, RGB(0, 0, 0), RGB(0, 0, 0));  // 테두리
  break;
case StatePressed:
  pDC->FillSolidRect(&rect, RGB(200, 200, 200));  // 클릭 효과
  break;
case StateChecked:
  pDC->FillSolidRect(&rect, m_bgColor);
  break;
case StateDisabled:
  pDC->FillSolidRect(&rect, RGB(240, 240, 240));  // 회색 배경
  break;
}

// 텍스트 색상 설정
COLORREF oldTextColor = pDC->SetTextColor(m_textColor);

// 텍스트 표시
CString strText;
GetWindowText(strText);
pDC->DrawText(strText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

// 이미지 표시
if (m_image.GetWidth() > 0 && m_image.GetHeight() > 0)
{
  int imgHeight = m_image.GetHeight() / 2;  // ON/OFF 상태 분리
  int yOffset = (m_currentState == StateChecked) ? imgHeight : 0;

  CRect imgRect(0, yOffset, m_image.GetWidth(), yOffset + imgHeight);
  m_image.BitBlt(pDC->GetSafeHdc(), rect.left + (rect.Width() - imgRect.Width()) / 2, rect.top, imgRect.Width(), imgRect.Height(), imgRect.left, imgRect.top);
}

pDC->SetTextColor(oldTextColor);  // 원래 텍스트 색상 복원
#endif
#if 0
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

// 상태별로 이미지 선택
if (lpDrawItemStruct->itemState & ODS_DISABLED)
m_currentState = 4; // 비활성화 상태
else if (lpDrawItemStruct->itemState & ODS_SELECTED)
m_currentState = 2; // 클릭 상태
else if (m_mouseOver)
m_currentState = 1; // 마우스 오버 상태
else
m_currentState = 0; // 기본 상태

int imageWidth = 0;
if (!m_image.IsNull())
{
  // 이미지 처리
  int buttonHeight = rect.Height();
  int imageHeight = static_cast<int>(buttonHeight * 0.8);
  imageWidth = static_cast<int>((IMAGE_WIDTH * imageHeight) / IMAGE_HEIGHT);

  int imageX = rect.left + 5; // 좌측 정렬
  int imageY = rect.top + (buttonHeight - imageHeight) / 2;
  int yOffset = m_currentState * IMAGE_HEIGHT;
  CRect srcRect(0, yOffset, IMAGE_WIDTH, yOffset + IMAGE_HEIGHT);

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