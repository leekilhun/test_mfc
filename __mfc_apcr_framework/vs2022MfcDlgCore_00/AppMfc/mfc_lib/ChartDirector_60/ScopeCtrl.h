#pragma once

#define WM_SCOPE_CTRL_MESSAGE					(WM_USER	+ 10010)

// Scope Range 데이터 클래스
class MScopeRange
{
public:
	MScopeRange(){ Clear(); };
	~MScopeRange(){ Clear(); };
	void Clear()
	{
		m_ptRange = CPoint(0, 100);
		m_iLabelSpacing = 50;
		m_strText = _T("");
		m_crNormal = RGB(255, 0, 0);
		m_crBack = RGB(255, 128, 128);
		m_bUseRange = TRUE;
		m_bUseGridLabel = TRUE;

		m_bUseLabelChar = FALSE;
		m_strLabelChar = _T("");

		m_dLabelScale = 1.0;
		m_bHighlight_Zero = FALSE;
	};
	void SetData(CPoint ptRange,
		int iLabelSpacing,
		CString strText,
		COLORREF crNormal,
		COLORREF crBack,
		BOOL bUseRange = TRUE, 
		BOOL bUseGridLabel = TRUE)
	{
		m_ptRange = ptRange;
		m_iLabelSpacing = iLabelSpacing;
		m_strText = strText;
		m_crNormal = crNormal;
		m_crBack = crBack;
		m_bUseRange = bUseRange;
		m_bUseGridLabel = bUseGridLabel;
	};
	int GetRange(){ return m_ptRange.y - m_ptRange.x; };

public:
	CPoint		m_ptRange;			// 범위
	int			m_iLabelSpacing;	// Label간 간격
	CString		m_strText;			// 단위
	COLORREF	m_crNormal;			// 단위, 그래프 색상
	COLORREF	m_crBack;			// 내려올 때 색상
	BOOL		m_bUseRange;		// 사용 여부
	BOOL		m_bUseGridLabel;	// 그리드에 단위 글자 표시여부

	BOOL		m_bUseLabelChar;
	CString		m_strLabelChar;		// 그리드에 표시할 1개 문자열

	double		m_dLabelScale;		// 라벨 숫자에 곱할 단위
	BOOL		m_bHighlight_Zero;	// 0점 라인을 밝게 표시할지 여부
};

/**
* 오실로스코프 형태의 표시를 위한 사용자 지정 컨트롤
*/

// CScopeCtrl

class CScopeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CScopeCtrl)

public:
	/** 생성자 */
	CScopeCtrl();
	/** 소멸자 */
	virtual ~CScopeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnPaint();

// 내부 사용 함수
private:
	/**
	 * 멤버 변수를 초기화 한다. 
	 */
	void clearVariable();

// 외부 제공 함수
public:
	/**
	* 그래프를 생성
	*
	* @param	&rect				: 컨트롤 생성 영역
	* @param	*pParentWnd			: 부모 윈도우 핸들
	* @param	uID					: 리소스 ID
	* @param	dataRange_X			: X 범위 데이터
	* @param	dataRange_Y1		: Y1 범위 데이터
	* @param	dataRange_Y1_2nd	: Y1 2nd 범위 데이터
	* @param	dataRange_Y2		: Y2 범위 데이터
	* @return	BOOL				: 성공시 TRUE, 그 외 FALSE
	*/
	BOOL Create(const RECT &rect, 
				CWnd *pParentWnd, 
				UINT uID,
				MScopeRange dataRange_X,
				MScopeRange dataRange_Y1,
				MScopeRange dataRange_Y1_2nd,
				MScopeRange dataRange_Y2);

	/**
	 * 컨트롤을 소멸시킨다.
	 *
	 * @return	BOOL			: 성공시 TRUE, 그 외 FALSE
	 */
	BOOL Destroy();

	/**
	 * 그래프를 초기화 한다. (리스트 초기화, 배경 다시그리기)
	 */
	void Clear();

	/**
	* Point를 추가한다. (Range 범위를 벗어나는 데이터는 FALSE 처리)
	*
	* @param	ptNewPos		: 추가할 Point 데이터
	* @param	bReverse		: 역방향 그래프 여부 (기본값 = FALSE)
	* @return	BOOL			: 성공시 TRUE, 그 외 FALSE
	*/
	BOOL AddPoint_Y1(CPoint ptNewPos, BOOL bReverse = FALSE);
	BOOL AddPoint_Y1_2nd(CPoint ptNewPos, BOOL bReverse = FALSE);
	BOOL AddPoint_Y2(CPoint ptNewPos, BOOL bReverse = FALSE);	

	void SetOffset_Y1(int iOffset, int iOffset_Reverse);
	void SetOffset_Y2(int iOffset, int iOffset_Reverse);

	/**
	* 지정된 위치에 문자를 출력한다.
	*
	* @param	ptPos			: 문자를 출력할 위치
	* @param	strText			: 출력할 문자열
	*/
	void DrawText_Graph(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT);
	void DrawText_Graph_Slim(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT);

	void DrawText_Result(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT);
	void DrawText_Result_Slim(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT);

	void DrawText_PortNo(CPoint ptPos, int iPortNo);

	void DrawText_Scale(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT);

	void DrawText_FontSize(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT, int iFontHeight = 10, int iFontWidth = 0, CString strFontName = _T("MS Shell Dlg"));

	/**
	* 리스트에 있는 좌표 데이터로 라인을 그린다. (돌아오는 그래프 포함)
	*/
	void DrawLine();

	void DrawLine_Dash(CPoint ptStart, CPoint ptEnd, COLORREF crColor);
	void DrawLine_Scale(CPoint ptStart, CPoint ptEnd, COLORREF crColor);
	void DrawLine_Cross(CPoint ptPos, int iSize, int iWidth, COLORREF crColor);
	void DrawLine_Rect_Dash(CPoint ptStart, CPoint ptEnd, COLORREF crColor);

	void Draw_Linearity_Range(CPoint ptStart, CPoint ptEnd, CPoint ptMax);
	void Draw_Hysteresis_Max(CPoint ptMax);
	void Draw_OIS_Stroke(CPoint ptLeft, CPoint ptRight, CPoint ptUp, CPoint ptDown);
	void Draw_OISShift_Spec(CPoint ptStart, CPoint ptEnd);

// 내부 사용 함수
private:
	/**
	 * 화면에 가로, 세로 라인과 단위 문자를 그린다.
	 */
	void UpdateLabels();

	/**
	* 컨트롤 화면을 전부 지운다.
	*/
	BOOL InvalidateCtrl();

	void clearList();

	void drawText_Label(CPoint ptPos, CString strText, COLORREF crText, int iAlign = TA_LEFT);

private:
	/**
	* Point 값을 그래프 픽셀 값으로 변환한다.
	*
	* @param	ptPos			: 그래프 데이터 Point 값
	* @return	CPoint			: Pixel로 변환된 값
	*/
	CPoint convertPointToPixel_Y1(const CPoint ptPos);
	CPoint convertPointToPixel_Y2(const CPoint ptPos);

	/**
	* 픽셀에 해당하는 Point 값으로 변환한다.
	* 
	* @param	ptPos			: Pixel 데이터 값
	* @return	CPoint			: Point 데이터로 변환된 값
	*/
	CPoint convertPixelToPoint_Y1(const CPoint ptPos);
	CPoint convertPixelToPoint_Y2(const CPoint ptPos);

public:
	/**
	* 지정된 Range를 저장하고 화면에 표시하는데 필요한 값(Scale 등)을 계산한다.
	*
	* @param	dataRange		: Range 데이터 클래스
	*/
	void setRange_X(const MScopeRange dataRange);
	void setRange_Y1(const MScopeRange dataRange);
	void setRange_Y1_2nd(const MScopeRange dataRange);
	void setRange_Y2(const MScopeRange dataRange);

// 내부 사용 변수
private:
	BOOL					m_bCreate;
	CDC*					m_pMemDC;			// 그림 그릴 DC

	// List of points for drawing
	CList<CPoint, CPoint&> *m_pList_Y1;			// 포인트를 저장할 리스트
	CList<CPoint, CPoint&> *m_pListBack_Y1;		// 포인트를 저장할 리스트 (돌아오는 그래프)

	CList<CPoint, CPoint&> *m_pList_Y1_2nd;		// 포인트를 저장할 리스트
	CList<CPoint, CPoint&> *m_pListBack_Y1_2nd;	// 포인트를 저장할 리스트 (돌아오는 그래프)

	CList<CPoint, CPoint&> *m_pList_Y2;			// 포인트를 저장할 리스트
	CList<CPoint, CPoint&> *m_pListBack_Y2;		// 포인트를 저장할 리스트 (돌아오는 그래프)

private:
	MScopeRange				m_dataRange_X;		// X 영역 데이터
	MScopeRange				m_dataRange_Y1;		// Y1 영역 데이터
	MScopeRange				m_dataRange_Y1_2nd;	// Y1 2nd 영역 데이터
	MScopeRange				m_dataRange_Y2;		// Y2 영역 데이터

	// Client Window size, width and height
	CRect					m_rcClient;			// 그래프 영역 
	int						m_iClientWidth;		// 가로 영역 크기
	int						m_iClientHeight;	// 세로 영역 크기

	double					m_fScale_X;			// X 영역 / Range X 의 Scale 값
	double					m_fScale_Y1;		// Y1 영역 / Range Y1 의 Scale 값
	double					m_fScale_Y2;		// Y2 영역 / Range Y2 의 Scale 값

	int						m_iOffset_Y1;
	int						m_iOffset_Y2;
	int						m_iOffset_Y1_Reverse;
	int						m_iOffset_Y2_Reverse;

	int						m_iTextSpacing_X;

private:
	// 색상 설정
	COLORREF				m_crBackGround;		// 배경 색상
	COLORREF				m_crGrids;			// 그리드 라인 색상
	COLORREF				m_crText;			// 텍스트 색상

	COLORREF				m_crRange_X;		// X 영역 색상 (텍스트)
	COLORREF				m_crRange_Y1;		// Y1 영역 색상 (그래프, 텍스트)
	COLORREF				m_crRange_Y2;		// Y2 영역 색상 (그래프, 텍스트)
	COLORREF				m_crRangeBack_Y1;	// Y1 영역 역그래프 색상
	COLORREF				m_crRangeBack_Y2;	// Y2 영역 역그래프 색상

private:
	CPen					m_DrawPen_Y1;
	CPen					m_DrawPen_Y1_Back;
	CPen					m_DrawPen_Y1_2nd;
	CPen					m_DrawPen_Y1_2nd_Back;
	CPen					m_DrawPen_Y2;
	CPen					m_DrawPen_Y2_Back;
	int						m_iDrawPenWidth;

	CFont					m_TextFont;
	CFont					m_TextFont_Slim;
	CFont					m_TextFont_Result;	
	CFont					m_TextFont_Result_Slim;

	CPen					m_DrawPen_Y1_Dash;
	CPen					m_DrawPen_Y1_Cross;

	CWnd*					m_pParentWnd;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


