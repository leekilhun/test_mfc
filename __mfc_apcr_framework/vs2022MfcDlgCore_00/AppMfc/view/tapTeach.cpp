// tapMain.cpp: 구현 파일
//

#include "pch.h"
#include "ui_def.hpp"
#include "commonUI.h"

#include "afxdialogex.h"
#include "tapTeach.h"


// CtapTeach 대화 상자
CtapTeach* ptr_this{};
static int rxCallback(void* w_parm, void* l_parm);

int rxCallback(void* w_parm, void* l_parm)
{

	if (ptr_this->m_TimerID == 0)
		return 0;

	apcr::xprotocol_cmd::packet_st data = *(apcr::xprotocol_cmd::packet_st*)l_parm;

	CString str = cha2Cstr(tim::TimeStr().c_str()) + L"[PC << DRIVER] ";
	for (int i = 0; i < data.buffer_idx; i++)
	{
		str.AppendFormat(L"%02X ", data.buffer[i]);
	}

	str.AppendFormat(L" response ms: %04d", data.resp_ms);

	ptr_this->addRxLog(str);

	return 0;
}

IMPLEMENT_DYNAMIC(CtapTeach, CDialogEx)

CtapTeach::CtapTeach(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAP_TEACH, pParent)
{
	m_ptrApi = apcr::get_api();

	ptr_this = this;
}

CtapTeach::~CtapTeach()
{
	if (m_image)
		m_image.Destroy();

	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = nullptr;
	}
}
//
void CtapTeach::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_LIST_RX, m_listRxLog);
	DDX_Control(pDX, IDC_MAIN_LOG_LST_STATUS, m_listErrLog);
	DDX_Control(pDX, IDC_MAIN_PICTURE_CONTROL, m_pictureControl); // Picture Control
	DDX_Control(pDX, IDC_MIAN_BTN_NJOG, m_btnNJog);
	DDX_Control(pDX, IDC_MIAN_BTN_PJOG, m_btnPJog);

	DDX_Control(pDX, IDC_MAIN_LOG_LST_STATUS2, m_listStepLog);

}

void CtapTeach::addRxLog(CString str)
{
	// TODO: 여기에 구현 코드 추가.
	if (m_listRxLog.GetCount() == 5)
		m_listRxLog.ResetContent();
	m_listRxLog.AddString(str);
}

void CtapTeach::update(void)
{
	if (m_ptrApi->m_logQ.Available())
	{
		log_dat log_data;
		m_ptrApi->m_logQ.Get(&log_data);

		ERR_PRINT(log_data.log_buff.log.data());
		CString err_type = log_data.log_buff.head.header == log_dat::header_info ? L"[INF]" : log_data.log_buff.head.header == log_dat::header_warning ? L"[WAR]" : L"[ERR]";
		CString str = cha2Cstr(tim::TimeStr().c_str()) + err_type + L":";
		str.AppendFormat(L"[err_no : %d, obj_idx : %d, step_no : %d] [msg] : %s",
			log_data.log_buff.head.error_no, log_data.log_buff.head.obj_idx, log_data.log_buff.head.step_no, cha2Cstr(log_data.log_buff.log.data()).GetBuffer());

		if (m_listErrLog.GetCount() == 5)
			m_listErrLog.ResetContent();
		m_listErrLog.AddString(str);
	}

	{
		using mot_status = mcudat_t::mt_status_t;
		using mot_pulse = mcudat_t::mt_pulse_t;
		mot_status status = m_ptrApi->m_cfg.ptr_data->m_data.mt_stats;
		mot_pulse enc_pulse = m_ptrApi->m_cfg.ptr_data->m_data.mt_poses;
		uint8_t  mot_x = (uint8_t)m_ptrApi->m_cfg.ptr_data->m_data.datas[0];

		SetDlgItemText(IDC_MAIN_EDT_ACTPOS, CString{ std::to_wstring(enc_pulse[0]).c_str() });
		CString statusStr;
		statusStr.Format(L"0x%08X", status[0]);
		SetDlgItemText(IDC_MAIN_EDT_AXISSTATUS, statusStr);

		// status 정보를 IDC_MAIN_STATE_CHK_1 체크박스에 적용
		using dsp_st = apcr::mot_reg::dsp_status;
		dsp_st check_st{ (uint16_t)status[0] };
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_1))->SetCheck((mot_x >> 1) & 1);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_2))->SetCheck((mot_x >> 0) & 1);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_3))->SetCheck((mot_x >> 2) & 1);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_4))->SetCheck(check_st.Drive_Fault);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_5))->SetCheck(check_st.Alarm_present);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_6))->SetCheck(check_st.In_Position);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_7))->SetCheck(check_st.Motor_Enabled);
		((CButton*)GetDlgItem(IDC_MAIN_STATE_CHK_8))->SetCheck(check_st.Homing);

		uint16_t dsp_status = (uint16_t)status[0];
		for (int i = 0; i < 16; i++)
		{
			CButton* pCheckBox = (CButton*)GetDlgItem(IDC_MAIN_MOT_ST_CHK_1 + i);
			if (pCheckBox)
			{
				if ((dsp_status >> i) & 0x01) // 예시로 status의 첫 번째 비트를 체크 조건으로 사용
				{
					pCheckBox->SetCheck(BST_CHECKED);
				}
				else
				{
					pCheckBox->SetCheck(BST_UNCHECKED);
				}
			}
		}
	}
}

void CtapTeach::doRunStep()
{
	using namespace apcr::ap_reg;

	enum steps
	{
		STEP_INIT,
		STEP_TODO,
		STEP_STATE_UPDATE,
		STEP_TIMEOUT,

		STEP_DO_STANDBY,
		STEP_DO_STANDBY_START,
		STEP_DO_STANDBY_WAIT,
		STEP_DO_STANDBY_END,

		STEP_DO_PICK,
		STEP_DO_PICK_START,
		STEP_DO_PICK_WAIT,
		STEP_DO_PICK_END,

		STEP_DO_PLACE,
		STEP_DO_PLACE_START,
		STEP_DO_PLACE_WAIT,
		STEP_DO_PLACE_END,

		STEP_MAX
	};


	auto set_log = [&](CString str)
		{
			if (m_listStepLog.GetCount() == 5)
				m_listStepLog.ResetContent();
			m_listStepLog.AddString(str);
		};


	udword_t ioX_reg = m_ptrApi->m_cfg.ptr_data->get_ioX();
	udword_t opt_reg = m_ptrApi->m_cfg.ptr_data->get_opt();
	// write out register를 위한 전역 변수
	udword_t& ioY_reg = apcr::get_ioY();

	auto reset_task_timer = [&]() ->void
		{
			m_timers.Reset(tm_idx_0);
			m_timers.Reset(tm_idx_1);
			m_timers.Reset(tm_idx_2);
			m_timers.Reset(tm_idx_3);
			m_timers.Reset(tm_idx_4);
		};

	auto write_out = [&](IOY_idx idx, bool onoff) ->void
		{
			ioY_reg(idx, onoff);
		};

	auto move_act = [&](int steps, uint16_t rpm) ->errno_t
		{
			return m_ptrApi->do_motor_move(steps, rpm);
		};

	auto set_out_bits = [&](auto... bits) ->void
		{
			((ioY_reg.u32D |= (1UL << bits)), ...);
		};

	auto reset_out_bits = [&](auto... bits) ->void
		{
			((ioY_reg.u32D &= ~(1UL << bits)), ...);
		};

	static int set_pose = 0;

	switch (m_step.GetStep())
	{
	case STEP_INIT:
	{
		m_isStandby = false;
		m_step.sub_step = 0;
		m_step.retry_cnt = 0;
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_POSITION_1))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_VELOCITY_1))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_POSITION_2))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_VELOCITY_2))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_DELAY_1))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_DELAY_2))->EnableWindow(TRUE);
		m_step.SetStep(STEP_TODO);
	}
	break;

	case STEP_TODO:
	{
		m_step.SetStep(STEP_STATE_UPDATE);
	}
	break;

	case STEP_STATE_UPDATE:
	{
		if (m_step.LessThan(50))
			break;

		if (m_startflag)
		{
			if (m_isStandby)
				m_step.SetStep(STEP_DO_PICK);
			else
				m_step.SetStep(STEP_DO_STANDBY);

			break;
		}

		m_startflag = false;
		m_isStandby = false;
		m_step.SetStep(STEP_TODO);
	}
	break;

	case STEP_TIMEOUT:
	{
		if (m_step.LessThan(50))
			break;

		m_step.SetStep(STEP_TODO);
	}
	break;

	case STEP_DO_STANDBY:
	{
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_POSITION_1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_VELOCITY_1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_POSITION_2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_VELOCITY_2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_DELAY_1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_DELAY_2))->EnableWindow(FALSE);

		m_step.sub_step = 0;
		m_step.retry_cnt = 0;
		m_step.SetStep(STEP_DO_STANDBY_START);
	}
	break;

	case STEP_DO_STANDBY_START:
	{
		m_step.SetStep(STEP_DO_STANDBY_WAIT);
	}
	break;

	case STEP_DO_STANDBY_WAIT:
	{
		if (is_motor_ready())
			m_isStandby = true;

		m_step.SetStep(STEP_DO_STANDBY_END);
	}
	break;

	case STEP_DO_STANDBY_END:
	{
		m_step.SetStep(STEP_TODO);
	}
	break;

	case STEP_DO_PICK:
	{
		int wait_time = GetDlgItemInt(IDC_MAIN_EDT_AT_DELAY_1);
		if (m_step.LessThan(wait_time))
			break;

		m_step.sub_step = 0;
		m_step.retry_cnt = 0;
		m_step.SetStep(STEP_DO_PICK_START);
	}
	break;

	case STEP_DO_PICK_START:
	{

		enum
		{
			step_run,
			step_wait,
			step_is_run,
		};

		switch (m_step.sub_step)
		{
		case step_run:
		{
			set_pose = (int)GetDlgItemInt(IDC_MAIN_EDT_AT_POSITION_1);
			uint16_t rpm = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_AT_VELOCITY_1);
			//m_ptrApi->do_motor_move(set_pose, rpm);

			set_log(L"pick run");
			m_step.sub_step = step_wait;
			m_step.SetStep(STEP_DO_PICK_START);
			break;
		}
		break;

		case step_wait:
		{
			if (m_step.LessThan(200 * 1))
				break;
			if (is_motor_inpose(set_pose))
			{
				set_log(L"pick inpose");
				m_step.SetStep(STEP_DO_PICK_WAIT);
				break;
			}

			m_step.sub_step = step_is_run;
			m_step.SetStep(STEP_DO_PICK_START);
			break;
		}
		break;
		case step_is_run:
		{
			if (is_motor_stop())
			{
				if (m_step.retry_cnt++ < 3)
				{
					m_step.sub_step = step_run;
					m_step.SetStep(STEP_DO_PICK_START);
					break;
				}
				else
				{
					m_startflag = false;
					m_step.SetStep(STEP_INIT);
					CString msg{};
					msg.Format(L"motor move run error %d", set_pose);
					AfxMessageBox(msg);
				}
				break;
			}

			set_log(L"pick run ok");
			m_step.SetStep(STEP_DO_PICK_WAIT);
		}
		break;
		default:			break;
		}
	}
	break;

	case STEP_DO_PICK_WAIT:
	{
		if (is_motor_inpose(set_pose))
		{
			m_step.SetStep(STEP_DO_PICK_END);
		}
		else
		{
			if (m_step.MoreThan(5'000))// move timeout
			{
				m_startflag = false;
				m_step.SetStep(STEP_INIT);
				CString msg{};
				msg.Format(L"motor move pick error %d", set_pose);
				AfxMessageBox(msg);
			}
			break;
		}
	}
	break;

	case STEP_DO_PICK_END:
	{
		if (m_startflag)
			m_step.SetStep(STEP_DO_PLACE);
		else
			m_step.SetStep(STEP_TODO);
	}
	break;

	case STEP_DO_PLACE:
	{
		int wait_time = GetDlgItemInt(IDC_MAIN_EDT_AT_DELAY_2);
		if (m_step.LessThan(wait_time))
			break;
		m_step.sub_step = 0;
		m_step.retry_cnt = 0;
		m_step.SetStep(STEP_DO_PLACE_START);
	}
	break;

	case STEP_DO_PLACE_START:
	{
		enum
		{
			step_run,
			step_wait,
			step_is_run,
		};

		switch (m_step.sub_step)
		{
		case step_run:
		{
			set_pose = (int)GetDlgItemInt(IDC_MAIN_EDT_AT_POSITION_2);
			uint16_t rpm = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_AT_VELOCITY_2);
			move_act(set_pose, rpm);

			set_log(L"place run");
			m_step.sub_step = step_wait;
			m_step.SetStep(STEP_DO_PLACE_START);
			break;
		}
		break;

		case step_wait:
		{
			if (m_step.LessThan(200 * 1))
				break;

			if (is_motor_inpose(set_pose))
			{
				set_log(L"pick inpose");
				m_step.SetStep(STEP_DO_PLACE_WAIT);
				break;
			}
			m_step.sub_step = step_is_run;
			m_step.SetStep(STEP_DO_PLACE_START);
			break;
		}
		break;
		case step_is_run:
		{
			if (is_motor_stop())
			{
				if (m_step.retry_cnt++ < 3)
				{
					m_step.sub_step = step_run;
					m_step.SetStep(STEP_DO_PLACE_START);
					break;
				}
				else
				{
					m_startflag = false;
					m_step.SetStep(STEP_INIT);
					CString msg{};
					msg.Format(L"motor move run error %d", set_pose);
					AfxMessageBox(msg);
				}
			}

			set_log(L"place run ok");
			m_step.SetStep(STEP_DO_PLACE_WAIT);
		}
		break;
		default:			break;
		}
	}
	break;

	case STEP_DO_PLACE_WAIT:
	{
		if (is_motor_inpose(set_pose))
		{
			m_step.SetStep(STEP_DO_PLACE_END);
		}
		else
		{
			if (m_step.MoreThan(5'000))// move timeout
			{
				m_startflag = false;
				m_step.SetStep(STEP_INIT);
				CString msg{};
				msg.Format(L"motor move place error %d", set_pose);
				AfxMessageBox(msg);
			}
			break;
		}
	}
	break;

	case STEP_DO_PLACE_END:
	{
		if (m_startflag)
			m_step.SetStep(STEP_DO_PICK);
		else
			m_step.SetStep(STEP_TODO);
	}
	break;

	default: break;
	}

}


BEGIN_MESSAGE_MAP(CtapTeach, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_MAIN_BTN_POWER_ON, &CtapTeach::OnBnClickedMainBtnPowerOn)
	ON_BN_CLICKED(IDC_MAIN_BTN_POWER_OFF, &CtapTeach::OnBnClickedMainBtnPowerOff)
	ON_BN_CLICKED(IDC_MAIN_BTN_RESET, &CtapTeach::OnBnClickedMainBtnReset)
	ON_BN_CLICKED(IDC_MAIN_BTN_CLEAR, &CtapTeach::OnBnClickedMainBtnClear)
	ON_BN_CLICKED(IDC_MIAN_BTN_ABS_MOVE, &CtapTeach::OnBnClickedMianBtnAbsMove)
	ON_BN_CLICKED(IDC_MIAN_BTN_REL_MOVE, &CtapTeach::OnBnClickedMianBtnRelMove)
	ON_BN_CLICKED(IDC_MIAN_BTN_HOME, &CtapTeach::OnBnClickedMianBtnHome)
	ON_BN_CLICKED(IDC_MIAN_BTN_HALT, &CtapTeach::OnBnClickedMianBtnHalt)

	// jog button message 
	ON_MESSAGE(UM_JOG_BTN_DOWN, OnJogButtonDown)
	ON_MESSAGE(UM_JOG_BTN_UP, OnJogButtonUp)
	ON_BN_CLICKED(IDC_MIAN_BTN_ATUOTEST_START, &CtapTeach::OnBnClickedMianBtnAutoTestStart)
	ON_BN_CLICKED(IDC_MIAN_BTN_ATUOTEST_STOP, &CtapTeach::OnBnClickedMianBtnAutoTestStop)
END_MESSAGE_MAP()


// CtapTeach 메시지 처리기


BOOL CtapTeach::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	/*
	+GDI+를 사용하여 이미지를 로드하고 Picture Control에 표시하는 방법

	*/
	auto RemoveWhiteBackground = [&](Gdiplus::Bitmap* pBitmap)
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
		};


	//DrawItem 함수를 사용하기 위해 Picture Control을 Owner Draw 속성으로 변경했으나 호출이 안되어 주석처리
	//CStatic* pPictureControl = (CStatic*)GetDlgItem(IDC_MAIN_PICTURE_CONTROL);
	//if (pPictureControl)
	//{
	//	pPictureControl->ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY);
	//	pPictureControl->Invalidate(); // 스타일 변경 후 강제 갱신
	//	pPictureControl->UpdateWindow();
	//}
	m_pictureControl.SetImage(L"res\\pnp_profile_main.jpg");
	m_pictureControl.SetText(_T("HRT-pXY-200-G"));

	m_pictureControl.ModifyStyle(0, SS_OWNERDRAW);


	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_1, L"motor on");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_2, L"sampling");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_3, L"drive err");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_4, L"in pose");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_5, L"moving");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_6, L"jogging");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_7, L"stopping");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_8, L"wait. in");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_9, L"saving");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_10, L"alarm-pres");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_11, L"homing");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_12, L"wait time");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_13, L"wizard run");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_14, L"checkin enc.");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_15, L"Qprg run");
	SetDlgItemText(IDC_MAIN_MOT_ST_CHK_16, L"initializing");


	SetDlgItemInt(IDC_MAIN_EDT_POSITION, 4096 * 2);
	SetDlgItemInt(IDC_MAIN_EDT_VELOCITY, 100);
	SetDlgItemInt(IDC_MAIN_EDT_ACCEL, 100);
	SetDlgItemInt(IDC_MAIN_EDT_DECEL, 100);

	SetDlgItemInt(IDC_MAIN_EDT_AT_POSITION_1, 25000);
	SetDlgItemInt(IDC_MAIN_EDT_AT_POSITION_2, 5000);
	SetDlgItemInt(IDC_MAIN_EDT_AT_VELOCITY_1, 200);
	SetDlgItemInt(IDC_MAIN_EDT_AT_VELOCITY_2, 200);

	SetDlgItemInt(IDC_MAIN_EDT_AT_DELAY_1, 500);
	SetDlgItemInt(IDC_MAIN_EDT_AT_DELAY_2, 500);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CtapTeach::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		registCallback();
		m_TimerID = SetTimer(UPDATE_TIMER_TAP_AUTO_DLG, 50, NULL);
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


void CtapTeach::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	update();
	doRunStep();
	CDialogEx::OnTimer(nIDEvent);
}



BOOL CtapTeach::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 휠 이벤트가 발생했을 때 호출되는 함수입니다.
	// zDelta 값이 양수이면 휠이 위로, 음수이면 휠이 아래로 이동한 것입니다.

	//if (m_isTeachable)
	{
		if (zDelta > 0)
		{
			// 휠이 위로 이동했을 때 처리할 코드
			// 예: teach_up 명령을 전송
			//m_ptrApi->sendCmd(apcr::xprotocol_cmd::cmd_e::teach_up);
			std::cout << "teach up :" << std::to_string(zDelta) << std::endl;
		}
		else
		{
			// 휠이 아래로 이동했을 때 처리할 코드
			// 예: teach_down 명령을 전송
			//m_ptrApi->sendCmd(apcr::xprotocol_cmd::cmd_e::teach_down);
			std::cout << "teach down :" << std::to_string(zDelta) << std::endl;
		}
	}

	// 기본 휠 이벤트 처리를 호출합니다.
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}



void CtapTeach::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	if (m_pImage)
	{
		Gdiplus::Graphics graphics(dc.m_hDC);

		// 다이얼로그 클라이언트 영역
		CRect rect;
		GetDlgItem(IDC_MAIN_PICTURE_CONTROL)->GetWindowRect(&rect);
		ScreenToClient(&rect);

		// 이미지 크기와 클라이언트 영역에 맞게 출력
		UINT imgWidth = m_pImage->GetWidth();
		UINT imgHeight = m_pImage->GetHeight();
		graphics.DrawImage(m_pImage, rect.left, rect.top, rect.Width(), rect.Height());
	}
	else
	{
		CDialogEx::OnPaint(); // 기본 그리기 처리
	}
}


void CtapTeach::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDCtl == IDC_MAIN_PICTURE_CONTROL)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);

		if (m_pImage == nullptr)
			return;

		// 부드러운 렌더링 설정
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		// Picture Control의 클라이언트 영역 가져오기
		CRect rect = lpDrawItemStruct->rcItem;

		// 둥근 모서리를 위한 GraphicsPath 생성
		const int cornerRadius = 20; // 모서리 반경
		Gdiplus::GraphicsPath path;
		path.AddArc(rect.left, rect.top, cornerRadius, cornerRadius, 180, 90);
		path.AddArc(rect.right - cornerRadius, rect.top, cornerRadius, cornerRadius, 270, 90);
		path.AddArc(rect.right - cornerRadius, rect.bottom - cornerRadius, cornerRadius, cornerRadius, 0, 90);
		path.AddArc(rect.left, rect.bottom - cornerRadius, cornerRadius, cornerRadius, 90, 90);
		path.CloseFigure();

		// 클리핑 설정
		graphics.SetClip(&path);

		// 이미지 출력
		graphics.DrawImage(m_pImage, rect.left, rect.top, rect.Width(), rect.Height());

		// 클리핑 해제
		graphics.ResetClip();

		// 테두리(선) 그리기
		Gdiplus::Pen borderPen(Gdiplus::Color(255, 0, 0, 0), 2); // 검은색 테두리
		graphics.DrawPath(&borderPen, &path);
	}
	else
	{
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct); // 기본 처리
	}

}

LRESULT CtapTeach::OnJogButtonDown(WPARAM wParam, LPARAM lParam)
{
	enum { _ccw, _cw };
	errno_t result{};
	if (wParam == IDC_MIAN_BTN_NJOG)
	{
		//result = ptrAxiss[axisID_idx]->move_velocity(dVelocity, dAccel, dDecel, dJerk,
			//apcr::enAxis::cat_t::direction_t::positive_direction, apcr::enAxis::cat_t::buffMode_Aborting);
		m_ptrApi->do_motor_jog(_cw);
	}
	else if (wParam == IDC_MIAN_BTN_PJOG)
	{
		//result = ptrAxiss[axisID_idx]->move_velocity(dVelocity, dAccel, dDecel, dJerk,
			//apcr::enAxis::cat_t::direction_t::negative_direction, apcr::enAxis::cat_t::buffMode_Aborting);
		m_ptrApi->do_motor_jog(_ccw);
	}

	return LRESULT();
}

LRESULT CtapTeach::OnJogButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_ptrApi->do_motor_stop();
	return LRESULT();
}


void CtapTeach::OnBnClickedMainBtnPowerOn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ptrApi->do_enable();
}


void CtapTeach::OnBnClickedMainBtnPowerOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ptrApi->do_disable();
}


void CtapTeach::OnBnClickedMainBtnReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ptrApi->do_motor_alarm_reset();
}


void CtapTeach::OnBnClickedMainBtnClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ptrApi->do_motor_enc_reset();
}


void CtapTeach::OnBnClickedMianBtnAbsMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strPos;
	GetDlgItemText(IDC_MAIN_EDT_POSITION, strPos);

	bool is_numbers = isConvertibleToNumber<int>(std::wstring{ strPos.GetBuffer() });
	if (is_numbers)
	{
		int steps = _ttoi(strPos);
		uint16_t vel = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_VELOCITY);
		uint16_t acc = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_ACCEL);
		uint16_t dec = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_DECEL);
		m_ptrApi->do_motor_move(steps, vel, acc, dec);
	}
	else
	{
		SetDlgItemText(IDC_MAIN_EDT_POSITION, L"");
	}
}


void CtapTeach::OnBnClickedMianBtnRelMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strPos;
	GetDlgItemText(IDC_MAIN_EDT_POSITION, strPos);
	// 숫자인지 확인하는 코드
	bool is_numbers = isConvertibleToNumber<int>(std::wstring{ strPos.GetBuffer() });
	if (is_numbers)
	{
		int steps = _ttoi(strPos);
		uint16_t vel = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_VELOCITY);
		uint16_t acc = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_ACCEL);
		uint16_t dec = (uint16_t)GetDlgItemInt(IDC_MAIN_EDT_DECEL);
		m_ptrApi->do_motor_move_rel(steps, vel, acc, dec);
	}
	else
	{
		SetDlgItemText(IDC_MAIN_EDT_POSITION, L"");
	}
}


void CtapTeach::OnBnClickedMianBtnHome()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ptrApi->do_motor_org();
}


void CtapTeach::OnBnClickedMianBtnHalt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ptrApi->do_motor_stop();
}

bool CtapTeach::is_motor_ready()
{
	using mot_status = mcudat_t::mt_status_t;
	using mot_pulse = mcudat_t::mt_pulse_t;
	mot_status status = m_ptrApi->m_cfg.ptr_data->m_data.mt_stats;//->motor_status;
	mot_pulse enc_pulse = m_ptrApi->m_cfg.ptr_data->m_data.mt_poses;//motor_pulse;

	using dsp_st = apcr::mot_reg::dsp_status;
	dsp_st check_st{ (uint16_t)status[0] };

	return (check_st.In_Position && check_st.Motor_Enabled);
}

bool CtapTeach::is_motor_stop()
{
	using mot_status = mcudat_t::mt_status_t;
	using mot_pulse = mcudat_t::mt_pulse_t;
	mot_status status = m_ptrApi->m_cfg.ptr_data->m_data.mt_stats;//->motor_status;
	mot_pulse enc_pulse = m_ptrApi->m_cfg.ptr_data->m_data.mt_poses;//motor_pulse;

	using dsp_st = apcr::mot_reg::dsp_status;
	dsp_st check_st{ (uint16_t)status[0] };

	return !(check_st.Moving);
}

bool CtapTeach::is_motor_inpose(int32_t pos)
{
	using mot_status = mcudat_t::mt_status_t;
	using mot_pulse = mcudat_t::mt_pulse_t;
	mot_status status = m_ptrApi->m_cfg.ptr_data->m_data.mt_stats;//->motor_status;
	mot_pulse enc_pulse = m_ptrApi->m_cfg.ptr_data->m_data.mt_poses;//motor_pulse;
	using dsp_st = apcr::mot_reg::dsp_status;
	dsp_st check_st{ (uint16_t)status[0] };

	bool in_inpose = std::abs(pos - (int32_t)enc_pulse[0]) < 10;

	return in_inpose;
}



void CtapTeach::OnBnClickedMianBtnAutoTestStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_startflag = true;
}


void CtapTeach::OnBnClickedMianBtnAutoTestStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_startflag = false;


	((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_POSITION_1))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_VELOCITY_1))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_POSITION_2))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_VELOCITY_2))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_DELAY_1))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_MAIN_EDT_AT_DELAY_2))->EnableWindow(TRUE);
}

void CtapTeach::registCallback()
{
	m_ptrApi->registerCallback(rxCallback);
}
