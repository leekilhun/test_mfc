// CimgCn.cpp
#include "pch.h"

#include "imgCn.h"

/*
#include "afxwin.h"
#include <Gdiplus.h> // 이미지 관련 GDI+
*/

// CimgCn
IMPLEMENT_DYNAMIC(CimgCn, CStatic)

CimgCn::CimgCn()
{
  m_pImage = nullptr;
}

CimgCn::~CimgCn()
{
  if (m_pImage)
  {
    delete m_pImage;
    m_pImage = nullptr;
  }
}

BEGIN_MESSAGE_MAP(CimgCn, CStatic)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

// 이미지를 설정하는 함수
void CimgCn::SetImage(LPCTSTR strImagePath)
{
  // 기존 이미지를 해제하고 새로운 이미지를 로드
  if (m_pImage)
  {
    delete m_pImage;
    m_pImage = nullptr;
  }

  m_pImage = Gdiplus::Image::FromFile(strImagePath);
  if (m_pImage == nullptr || m_pImage->GetLastStatus() != Gdiplus::Ok)
  {
    AfxMessageBox(_T("이미지를 불러오지 못했습니다."));
  }
  else
  {
    // 이미지가 Bitmap 형태인지 확인
    Gdiplus::Bitmap* pBitmap = dynamic_cast<Gdiplus::Bitmap*>(m_pImage);
    if (!pBitmap)
    {
      // Bitmap 객체로 변환
      pBitmap = new Gdiplus::Bitmap(strImagePath);//L"res\\finger_profile_1.jpg"
      if (pBitmap->GetLastStatus() != Gdiplus::Ok)
      {
        AfxMessageBox(_T("Bitmap 객체로 변환 실패"));
        delete pBitmap;
        pBitmap = nullptr;
      }
    }

    if (pBitmap)
    {
      // 배경 제거 함수 호출
      RemoveWhiteBackground(pBitmap);
      {
        // 이미지 크기 출력
        UINT width = m_pImage->GetWidth();
        UINT height = m_pImage->GetHeight();
        //TRACE(_T("Image Size: %d x %d\n"), width, height);

        INT x = 0;
        INT y = 0;
        Gdiplus::Color pixelColor;
        pBitmap->GetPixel(x, y, &pixelColor);
        //TRACE(_T("Alpha: %d, R: %d, G: %d, B: %d\n"), pixelColor.GetA(), pixelColor.GetR(), pixelColor.GetG(), pixelColor.GetB());
      }
    }
    // 기존 m_pImage 해제 후 Bitmap을 새로운 이미지로 사용
    delete m_pImage;
    m_pImage = pBitmap;
  }
  Invalidate(); // 컨트롤을 다시 그리도록 요청
}

// 텍스트를 설정하는 함수
void CimgCn::SetText(CString strText)
{
  m_strText = strText;
  Invalidate(); // 컨트롤을 다시 그리도록 요청
}

// 그림을 그리는 함수
void CimgCn::OnPaint()
{
  CPaintDC dc(this); // 디바이스 컨텍스트
  Gdiplus::Graphics graphics(dc.m_hDC);

  // 클라이언트 영역 가져오기
  CRect rect;
  GetClientRect(&rect);

  // 둥근 모서리를 위한 GraphicsPath 생성
  const int cornerRadius = 20; // 모서리 반경
  Gdiplus::GraphicsPath path;
  path.AddArc(rect.left, rect.top, cornerRadius, cornerRadius, 180, 90);
  path.AddArc(rect.right - cornerRadius, rect.top, cornerRadius, cornerRadius, 270, 90);
  path.AddArc(rect.right - cornerRadius, rect.bottom - cornerRadius, cornerRadius, cornerRadius, 0, 90);
  path.AddArc(rect.left, rect.bottom - cornerRadius, cornerRadius, cornerRadius, 90, 90);
  path.CloseFigure();

  // 투명 배경 설정
  graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
  // 배경을 흰색으로 채우기
  //Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(255, 255, 255, 255));
  //graphics.FillPath(&backgroundBrush, &path);

  // 클리핑 설정
  graphics.SetClip(&path);


  // 이미지 출력
  if (m_pImage)
  {
    graphics.DrawImage(m_pImage, rect.left, rect.top, rect.Width(), rect.Height());
  }

  // 클리핑 해제
  graphics.ResetClip();

  // 테두리(선) 그리기
  Gdiplus::Pen borderPen(Gdiplus::Color(255, 0, 0, 0), 2); // 검은색 테두리
  graphics.DrawPath(&borderPen, &path);

  // 텍스트가 있을 경우 그리기
  if (!m_strText.IsEmpty())
  {
    // 텍스트 스타일 설정
    Gdiplus::Font font(L"Consolas", 10);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(250, 250, 250)); //  텍스트
    graphics.DrawString(m_strText, -1, &font, Gdiplus::PointF((REAL)rect.left + 10, (REAL)rect.bottom - 20), &textBrush);
  }
}


//
//void CimgCn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
//{ 
//	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
//
//	Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);
//
//	// 부드러운 렌더링 설정
//	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
//
//	// Picture Control의 클라이언트 영역 가져오기
//	CRect rect = lpDrawItemStruct->rcItem;
//
//	// 둥근 모서리를 위한 GraphicsPath 생성
//	const int cornerRadius = 20; // 모서리 반경
//	Gdiplus::GraphicsPath path;
//	path.AddArc(rect.left, rect.top, cornerRadius, cornerRadius, 180, 90);
//	path.AddArc(rect.right - cornerRadius, rect.top, cornerRadius, cornerRadius, 270, 90);
//	path.AddArc(rect.right - cornerRadius, rect.bottom - cornerRadius, cornerRadius, cornerRadius, 0, 90);
//	path.AddArc(rect.left, rect.bottom - cornerRadius, cornerRadius, cornerRadius, 90, 90);
//	path.CloseFigure();
//
//	// 클리핑 설정
//	graphics.SetClip(&path);
//
//	// 이미지 출력
//	graphics.DrawImage(m_pImage, rect.left, rect.top, rect.Width(), rect.Height());
//
//	// 클리핑 해제
//	graphics.ResetClip();
//
//	// 테두리(선) 그리기
//	Gdiplus::Pen borderPen(Gdiplus::Color(255, 0, 0, 0), 2); // 검은색 테두리
//	graphics.DrawPath(&borderPen, &path);
//
//}

void CimgCn::OnMouseMove(UINT nFlags, CPoint point)
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

  CStatic::OnMouseMove(nFlags, point);
}

void CimgCn::OnLButtonDown(UINT nFlags, CPoint point)
{
  Invalidate();
  CStatic::OnLButtonDown(nFlags, point);
}

void CimgCn::OnLButtonUp(UINT nFlags, CPoint point)
{
  Invalidate();
  CStatic::OnLButtonUp(nFlags, point);
}

void CimgCn::OnMouseLeave()
{
  m_mouseOver = FALSE;

  Invalidate();
  CStatic::OnMouseLeave();
}


void CimgCn::RemoveWhiteBackground(Gdiplus::Bitmap* pBitmap)
{
  if (!pBitmap) return;

  // 이미지의 크기 가져오기
  UINT width = pBitmap->GetWidth();
  UINT height = pBitmap->GetHeight();

  // 픽셀 데이터 조작
  for (UINT y = 0; y < height; ++y)
  {
    for (UINT x = 0; x < width; ++x)
    {
      Gdiplus::Color pixelColor;
      pBitmap->GetPixel(x, y, &pixelColor);

      // 흰색 배경(혹은 유사한 밝은 색)을 투명하게 만듦
      if (pixelColor.GetR() > 240 && pixelColor.GetG() > 240 && pixelColor.GetB() > 240)
      {
        Gdiplus::Color transparentColor(0, pixelColor.GetR(), pixelColor.GetG(), pixelColor.GetB());
        pBitmap->SetPixel(x, y, transparentColor);
      }
    }
  }
}




void CimgCn::PreSubclassWindow()
{
  //ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY);

  // 부모 클래스의 PreSubclassWindow 호출
  CStatic::PreSubclassWindow();
}