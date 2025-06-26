// ui_popErrMsg.cpp: 구현 파일
//

#include "pch.h"


#include "ui_def.hpp"


#include "ui_popErrMsg.h"


// Cui_popErrMsg 대화 상자

IMPLEMENT_DYNAMIC(Cui_popErrMsg, CDialogEx)

static bool is_default_view{};

Cui_popErrMsg::Cui_popErrMsg(pop_alarm_t* err, CWnd* pParent)
	: CDialogEx(IDD_POP_TASK_ERR_MSG, pParent)
{
	alarm_datas_t receive_agvs{ *((alarm_datas_t*)err->l_parm) };

	m_objID = receive_agvs.object_idx;
	m_instanNo = receive_agvs.instance_idx;
	m_errCode = receive_agvs.err_no;
	m_lineNo = receive_agvs.line;

	m_fileInfo = cha2Cstr(receive_agvs.file);
	m_funcInfo = cha2Cstr(receive_agvs.func);
	m_errMsg = cha2Cstr(receive_agvs.msg);


	m_brBckColor.CreateSolidBrush(RGB(255, 255, 255));
	return;
}

Cui_popErrMsg::Cui_popErrMsg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POP_TASK_ERR_MSG, pParent)
{
	//apcr::auto_t ptr_auto = apcr::get_auto();


	m_brBckColor.CreateSolidBrush(RGB(255, 255, 255));
	return;

}

Cui_popErrMsg::~Cui_popErrMsg()
{	
	// 브러쉬 삭제
	m_brBckColor.DeleteObject();
}

void Cui_popErrMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDB_INFO, m_btnInfo);
	DDX_Control(pDX, IDB_OK, m_btnOK);
	DDX_Control(pDX, IDS_MESSAGE, m_msgText);
	DDX_Control(pDX, IDS_PROCESS_OBJ_ID, m_prcText);
	DDX_Control(pDX, IDS_UNIT_OBJ_ID, m_unitText);
	DDX_Control(pDX, IDS_ERROR_INFO_TITLE, m_infoTitle);
	DDX_Control(pDX, IDS_MESSAGE_ERR_INFO, m_infoText);
	DDX_Control(pDX, IDB_BUZZER_OFF, m_btnBuzzerOff);
}



BEGIN_MESSAGE_MAP(Cui_popErrMsg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
ON_WM_SIZE()
ON_WM_CREATE()
END_MESSAGE_MAP()



BEGIN_EVENTSINK_MAP(Cui_popErrMsg, CDialogEx)
	ON_EVENT(Cui_popErrMsg, IDB_BUZZER_OFF, DISPID_CLICK, Cui_popErrMsg::ClickBuzzerOff, VTS_NONE)
	ON_EVENT(Cui_popErrMsg, IDB_OK, DISPID_CLICK, Cui_popErrMsg::ClickOk, VTS_NONE)
	ON_EVENT(Cui_popErrMsg, IDB_INFO, DISPID_CLICK, Cui_popErrMsg::ClickInfo, VTS_NONE)
END_EVENTSINK_MAP()



// cpopErr 메시지 처리기
BOOL Cui_popErrMsg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (WM_KEYDOWN == pMsg->message)
	{
		if (13 == pMsg->wParam)	// 13은 VK_RETURN 임
		{
			pMsg->message = 0;
			pMsg->wParam = 0;
		}

		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Cui_popErrMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	/*
	m_objID = r
	m_instanNo 
	m_errCode =
	m_lineNo = 	
	*/
	using namespace apcr;
	using data_t = attribute_st;
	data_t objinfo = get_object_data<data_t>(m_objID + m_instanNo);
	
	CString process_str = m_fileInfo + L" (" + m_funcInfo + L")[" + CString{ std::to_wstring(m_lineNo).c_str()} + L"]";
	m_prcText.SetCaption(process_str.GetBuffer());
	m_unitText.SetCaption(ptrCha2wcha(objinfo.obj_name.c_str()));
	m_msgText.SetCaption(m_errMsg);
	viewDefault();

	// Error 메시지 화면이 항상 위에 표시되어 있도록 추가 - 2013.05.02 최석원
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void Cui_popErrMsg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void Cui_popErrMsg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	CRect rectArea;
	GetClientRect(&rectArea);
	dc.FillRect(rectArea.operator LPRECT(), &m_brBckColor);

	// Error 메시지 화면이 항상 위에 표시되어 있도록 추가 - 2013.05.02 최석원
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

}


void Cui_popErrMsg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	if (bShow == TRUE)
	{
		m_TimerID = SetTimer(UPDATE_TIMER_POP_ALARM_DLG, 50, NULL);
		is_default_view = false;
		//m_prcText.SetCaption(L"test process");
		//m_unitText.SetCaption(L"unit infor");
		//m_msgText.SetCaption(m_errMsg);
	//viewDefault();

	}
	else
	{
		if (m_TimerID != 0)
		{
			KillTimer(m_TimerID);
			m_TimerID = 0;
		}
	}

}


void Cui_popErrMsg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}


void Cui_popErrMsg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void Cui_popErrMsg::ClickBuzzerOff()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//using namespace apcr;
	//apcr::op_t op = apcr::get_op();
	//op->SetBuzzer(false);
}


void Cui_popErrMsg::ClickOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//using namespace apcr;
	//apcr::auto_t p_auto = apcr::get_auto();
	//p_auto->ResetSw();
	CDialogEx::OnCancel();
}


void Cui_popErrMsg::ClickInfo()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_infoTitle.IsWindowVisible() == TRUE)
		return;

	viewInfo();
}

void Cui_popErrMsg::SetInfo(pop_alarm_t& data)
{
	alarm_datas_t receive_agvs{ *((alarm_datas_t*)data.l_parm) };

	m_objID = receive_agvs.object_idx;
	m_instanNo = receive_agvs.instance_idx;
	m_errCode = receive_agvs.err_no;
	m_lineNo = receive_agvs.line;

	std::vector<std::string> v_strs{};
	std::string str{ receive_agvs.file };
	if (trans::string_split(receive_agvs.file, v_strs, "\\") > 0)
		str = v_strs.back();
	m_fileInfo = cha2Cstr(str.c_str());

	v_strs.clear();
	if (trans::string_split(receive_agvs.func, v_strs, "::") > 0)
		str = v_strs.back();
	m_funcInfo = cha2Cstr(str.c_str());
	m_errMsg = cha2Cstr(receive_agvs.msg);


	//m_prcText.SetCaption(L"test process");
	//m_unitText.SetCaption(L"unit infor");
	//m_msgText.SetCaption(m_errMsg);
	//viewDefault();

}



void Cui_popErrMsg::viewDefault()
{
	if (is_default_view == false)
	{
		is_default_view = true;
		// Window 축소
		CRect	win_rect;
		CPoint	left_top;
		GetWindowRect(&win_rect);


		CRect  info_rect;
		m_infoTitle.GetWindowRect(&info_rect);
		int win_decreasing_height = info_rect.Height();

		left_top = win_rect.TopLeft();
		MoveWindow(left_top.x, left_top.y, win_rect.Width(), win_rect.Height() - win_decreasing_height);

		//버튼 위로 보내기 	
		auto move_btn = [&](LIB_3D_BUTTON::CBtnEnh& obj, int delta_y)
			{
				CRect  btn_rect;
				CPoint btn_left_top;

				obj.GetWindowRect(&btn_rect);
				btn_left_top = btn_rect.TopLeft();
				int x = (btn_left_top.x - (left_top.x)) - 10;
				int y = btn_left_top.y - left_top.y - delta_y;
				obj.MoveWindow(x, y, btn_rect.Width(), btn_rect.Height());

			};

		win_decreasing_height = win_decreasing_height + 40; // 실제 측정한 값
		move_btn(m_btnBuzzerOff, win_decreasing_height);
		move_btn(m_btnOK, win_decreasing_height);
		move_btn(m_btnInfo, win_decreasing_height);

		m_infoTitle.ShowWindow(SW_HIDE);
		m_infoText.ShowWindow(SW_HIDE);

	}
	
}

void Cui_popErrMsg::viewInfo()
{
	is_default_view = false;
	if (m_infoTitle.IsWindowVisible() == TRUE)
		return;

	CRect	win_rect;
	CPoint	left_top;
	GetWindowRect(&win_rect);

	CRect  info_rect;
	m_infoTitle.GetWindowRect(&info_rect);
	int win_increasing_height = info_rect.Height();

	left_top = win_rect.TopLeft();
	MoveWindow(left_top.x, left_top.y, win_rect.Width(), win_rect.Height() + win_increasing_height);

	//버튼 아래로 보내기 	
	auto move_btn = [&](LIB_3D_BUTTON::CBtnEnh& obj, int delta_y)
		{
			CRect  btn_rect;
			CPoint btn_left_top;

			obj.GetWindowRect(&btn_rect);
			btn_left_top = btn_rect.TopLeft();
			int x = (btn_left_top.x - (left_top.x)) ;
			int y = btn_left_top.y - left_top.y + delta_y;
			obj.MoveWindow(x, y, btn_rect.Width(), btn_rect.Height());

		};

	win_increasing_height = win_increasing_height - 25; // 실제 측정한 값
	move_btn(m_btnBuzzerOff, win_increasing_height);
	move_btn(m_btnOK, win_increasing_height);
	move_btn(m_btnInfo, win_increasing_height);

	m_infoTitle.ShowWindow(SW_SHOW);
	m_infoText.ShowWindow(SW_SHOW);
}


int Cui_popErrMsg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
