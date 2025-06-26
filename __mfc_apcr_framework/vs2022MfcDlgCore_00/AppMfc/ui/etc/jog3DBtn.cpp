#include "pch.h"

#include "jog3DBtn.h"


/*

  사용할 mfc ui 클래스에서 아래와 같이 사용
	0. 도구상자에서 버튼을 만든다
  1. 버튼을  CButton -> CJogButton 클래스로 랩핑한다.
		CJogButton	m_btnNJog{};
	  CJogButton	m_btnPJog{};
	
	2. 메세지 이벤트를 추가하고  메세지 idx를 추가한다.
		BEGIN_MESSAGE_MAP(CmmcTestDlg, CDialogEx)
			ON_MESSAGE(UM_JOG_BTN_DOWN, OnJogButtonDown)
			ON_MESSAGE(UM_JOG_BTN_UP, OnJogButtonUp)

	3. OnJogButtonDown 함수 등록

	4. OnJogButtonUp 함수 등록

*/

CjogBtn::CjogBtn()
{
	m_bChecked = FALSE;
}

CjogBtn::~CjogBtn()
{
}

BEGIN_MESSAGE_MAP(CjogBtn, CBtnEnh)

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()

END_MESSAGE_MAP()

void CjogBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bChecked == FALSE)
	{
		GetParent()->SendMessage(def_um_jog_btn_down, this->GetDlgCtrlID(), 0);
		m_bChecked = TRUE;
	}

	CBtnEnh::OnLButtonDown(nFlags, point);
}

void CjogBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bChecked == TRUE)
	{
		GetParent()->SendMessage(def_um_jog_btn_up, this->GetDlgCtrlID(), 0);
		m_bChecked = FALSE;
	}
	CBtnEnh::OnLButtonUp(nFlags, point);
}

void CjogBtn::OnKillFocus(CWnd* pNewWnd)
{
	CBtnEnh::OnKillFocus(pNewWnd);

	if (m_bChecked == TRUE)
	{
		GetParent()->SendMessage(def_um_jog_btn_up, this->GetDlgCtrlID(), 0);
		m_bChecked = FALSE;
	}
}
