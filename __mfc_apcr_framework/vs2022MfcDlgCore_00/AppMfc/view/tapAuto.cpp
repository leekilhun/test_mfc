// tapAuto.cpp: 구현 파일
//

#include "pch.h"
#include "ui_def.hpp"
#include "commonUI.h"

#include "afxdialogex.h"
#include "tapAuto.h"


// CtapAuto 대화 상자

using namespace apcr::ap_obj;

using namespace apcr::ap_reg;

CtapAuto* ptr_this{};
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

IMPLEMENT_DYNAMIC(CtapAuto, CDialogEx)



CtapAuto::CtapAuto(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAP_AUTO, pParent)
{
	m_ptrApi = apcr::get_api();
	m_ptrApi->registerCallback(rxCallback);
	m_ptrTeachData = apcr::get_teach_data();

	ptr_this = this;
}

CtapAuto::~CtapAuto()
{
	threadStop();
}


//
void CtapAuto::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_LIST_RX, m_listRxLog);
	DDX_Control(pDX, IDC_MAIN_LOG_LST_STATUS, m_listErrLog);
	DDX_Control(pDX, IDC_MAIN_LOG_LST_STATUS2, m_listStepLog);

	//ch1
	DDX_Control(pDX, IDC_AUTO_PRC_NAME_CH1_01, m_viewElmsCh1._state_labels.at(si_ready));
	DDX_Control(pDX, IDC_AUTO_PRC_NAME_CH1_2, m_viewElmsCh1._state_labels.at(si_start));
	DDX_Control(pDX, IDC_AUTO_PRC_NAME_CH1_3, m_viewElmsCh1._state_labels.at(si_move1));
	DDX_Control(pDX, IDC_AUTO_PRC_NAME_CH1_4, m_viewElmsCh1._state_labels.at(si_press));
	DDX_Control(pDX, IDC_AUTO_PRC_NAME_CH1_5, m_viewElmsCh1._state_labels.at(si_check));
	DDX_Control(pDX, IDC_AUTO_PRC_NAME_CH1_6, m_viewElmsCh1._state_labels.at(si_return));
	DDX_Control(pDX, IDC_AUTO_RESET_1, m_viewElmsCh1._result);
	DDX_Control(pDX, IDC_AUTO_FLOW_MOVE_POS_VALUE_1, m_viewElmsCh1._data_dist.at(dpi_move1));
	DDX_Control(pDX, IDC_AUTO_FLOW_MOVE_POS_RPM_1, m_viewElmsCh1._data_rpm.at(dpi_move1));
	DDX_Control(pDX, IDC_AUTO_FLOW_MOVE_POS_VALUE_2, m_viewElmsCh1._data_dist.at(dpi_press));
	DDX_Control(pDX, IDC_AUTO_FLOW_MOVE_POS_RPM_2, m_viewElmsCh1._data_rpm.at(dpi_press));
	DDX_Control(pDX, IDC_AUTO_FLOW_MOVE_POS_VALUE_5, m_viewElmsCh1._data_dist.at(dpi_check));
	DDX_Control(pDX, IDC_AUTO_FLOW_MOVE_POS_RPM_5, m_viewElmsCh1._data_rpm.at(dpi_check));
	DDX_Control(pDX, IDC_AUTO_CURR_POS_DATA_0, m_viewElmsCh1._curr_pos);

	DDX_Control(pDX, IDC_AUTO_LAMP_GREEN_0, m_viewElmsCh1._lamps.at(sli_green));
	DDX_Control(pDX, IDC_AUTO_LAMP_YELLOW_0, m_viewElmsCh1._lamps.at(sli_yellow));
	DDX_Control(pDX, IDC_AUTO_LAMP_RED_0, m_viewElmsCh1._lamps.at(sli_red));


}

void CtapAuto::threadStop()
{
	m_stopThread = true;
	if (m_trd.joinable())
		m_trd.join();
}

void CtapAuto::threadRun()
{
	REPORT_OUT("[prc] Thread Start!");
	while (!m_stopThread)
	{
		threadJob();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	REPORT_OUT("Thread Terminatet!");
}

void CtapAuto::threadJob()
{
	//---------------------------------
  //MARK:job
	





	using namespace cdu;
	if (m_taskReg(tri_result_fail_ch1))
	{
		if (m_viewElmsCh1._result.GetBackColor() != ORANGE)
		{
			m_viewElmsCh1._result.SetBackColor(ORANGE);
			m_viewElmsCh1._result.SetCaption(L"FAIL");
		}
	}
	else if (m_taskReg(tri_result_pass_ch1))
	{
		if (m_viewElmsCh1._result.GetBackColor() != GREENYELLOW)
		{
			m_viewElmsCh1._result.SetBackColor(GREENYELLOW);
			m_viewElmsCh1._result.SetCaption(L"PASS");
		}
	}
	else
	{
		m_viewElmsCh1._result.SetCaption(L"-");
	}



	 //-------------------------------------------
	//MARK: auto state
	//-------------------------------------------

	mode_t op_mode = m_opReg.GetMode();
	status_t op_status = m_opReg.GetStatus();
	
	bool is_auto_task{ false };


	if (op_status == status_t::INIT)
	{
	}
	else if (op_status == status_t::RUN)
	{
		if (op_mode == mode_t::AUTORUN)
		{
			is_auto_task = true;
		}
		else if (op_mode == mode_t::DRY_RUN)
		{
			is_auto_task = true;
		}
	}
	else if (op_status == status_t::STEP_STOP)
	{
		if (op_mode == mode_t::MANUAL)
		{
		}
	}
	else if (op_status == status_t::ERR_STOP)
	{
	}

	if (is_auto_task)
		doRunStep();
}

void CtapAuto::ThreadJob()
{
	threadJob();
}

errno_t CtapAuto::StopThread()
{
	threadStop();
	return 0;
}

errno_t CtapAuto::StartThread()
{
	m_trd = std::thread(&CtapAuto::threadRun, this);
	return 0;
}

void CtapAuto::auto_start()
{
}


void CtapAuto::addRxLog(CString str)
{
	// TODO: 여기에 구현 코드 추가.
	if (m_listRxLog.GetCount() == 5)
		m_listRxLog.ResetContent();
	m_listRxLog.AddString(str);
}

void CtapAuto::update(void)
{
	//log
	if (m_ptrApi->m_logQ.Available())
	{
		log_dat log_data;
		m_ptrApi->m_logQ.Get(&log_data);

		ERR_PRINT(log_data.log_buff.log.data());
		CString err_type = (log_data.log_buff.head.header == log_dat::header_info) ? L"[INF]" : ((log_data.log_buff.head.header == log_dat::header_warning) ? L"[WAR]" : L"[ERR]");
		CString str = cha2Cstr(tim::TimeStr().c_str()) + err_type + L":";
		str.AppendFormat(L"[err_no : %d, obj_idx : %d, step_no : %d] [msg] : %s",
			log_data.log_buff.head.error_no, log_data.log_buff.head.obj_idx, log_data.log_buff.head.step_no, cha2Cstr(log_data.log_buff.log.data()).GetBuffer());

		if (m_listErrLog.GetCount() == 5)
			m_listErrLog.ResetContent();
		m_listErrLog.AddString(str);
	}



	//
	{
		using idx_t = ap_data::teachDat::data_reg_idx;
		ap_data::teachDat::pos_dat_t  datas{ m_ptrTeachData->m_posDatas[static_cast<int>(idx_t::teaching_data)] };

		using namespace cui;
		//CString str{};
		//str = CString{ double2str<std::wstring>(datas.elms.at(tdi_ch1_move1)._pose).c_str()};
		//m_viewElmsCh1._data_dist.at(dpi_move1).SetCaption(str);
		//str = CString{ double2str<std::wstring>(datas.elms.at(tdi_ch1_move1)._velocity).c_str() };
		//m_viewElmsCh1._data_rpm.at(dpi_move1).SetCaption(str);

		auto set_teach_data = [&](view_elm_t& obj, data_pose_idx pidx, teach_data_idx tdi) ->void
			{
				CString str{};
				str = CString{ double2str<std::wstring>(datas.elms.at(tdi)._pose).c_str() };
				obj._data_dist.at(pidx).SetCaption(str);
				str = CString{ double2str<std::wstring>(datas.elms.at(tdi)._velocity).c_str() };
				obj._data_rpm.at(pidx).SetCaption(str);
			};

		set_teach_data(m_viewElmsCh1, dpi_move1, tdi_ch1_move1);
		set_teach_data(m_viewElmsCh1, dpi_press, tdi_ch1_press);
		set_teach_data(m_viewElmsCh1, dpi_check, tdi_ch1_check);
	}
}

void CtapAuto::doRunStep()
{

	enum steps
	{
		STEP_INIT,
		STEP_TODO,
		STEP_TIMEOUT,

		STEP_DO_STANDBY,
		STEP_DO_STANDBY_START,
		STEP_DO_STANDBY_WAIT,
		STEP_DO_STANDBY_END, 

		STEP_DO_WORK,
		STEP_DO_WORK_START,
		STEP_DO_WORK_WAIT,
		STEP_DO_WORK_END,
	};

	auto set_log = [&](CString str)
		{
			CString msg = cha2Cstr(tim::TimeStr().c_str()) + L": " + str;
			if (m_listStepLog.GetCount() == 5)
				m_listStepLog.ResetContent();
			m_listStepLog.AddString(msg);
		};



	udword_t ioX_reg = m_ptrApi->m_cfg.ptr_data->get_ioX();
	udword_t ioY_reg = m_ptrApi->m_cfg.ptr_data->get_ioY();
	udword_t opt_reg = m_ptrApi->m_cfg.ptr_data->get_opt();

	auto write_out = [&](IOY_idx idx, bool onoff) ->errno_t
		{
			ioY_reg(idx, onoff);
			return m_ptrApi->do_write_out_reg(ioY_reg.get());
		};

	auto set_ioY = [&](uint32_t data) ->errno_t
		{
			return m_ptrApi->do_write_out_reg(data);
		};

	auto set_out_bits = [&](auto... bits) ->errno_t
		{
			uint32_t data{ ioY_reg.get() };
			((data |= (1UL << bits)), ...);
			return m_ptrApi->do_write_out_reg(data);
		};

	auto reset_out_bits = [&](auto... bits) ->errno_t
		{
			uint32_t data{ ioY_reg.get() };
			((data &= ~(1UL << bits)), ...);
			return m_ptrApi->do_write_out_reg(data);
		};


	using idx_t = ap_data::teachDat::data_reg_idx;
	ap_data::teachDat::pos_dat_t  datas{ m_ptrTeachData->m_posDatas[static_cast<int>(idx_t::teaching_data)] };
	using namespace cui;

	double move1_pose_ch1 = datas.elms.at(tdi_ch1_move1)._pose;
	double move1_rpm_ch1 = datas.elms.at(tdi_ch1_move1)._velocity;
	double move1_pose_ch2 = datas.elms.at(tdi_ch2_move1)._pose;
	double move1_rpm_ch2 = datas.elms.at(tdi_ch2_move1)._velocity;

	double press_pose_ch1 = datas.elms.at(tdi_ch1_press)._pose;
	double press_rpm_ch1 = datas.elms.at(tdi_ch1_press)._velocity;
	double press_pose_ch2 = datas.elms.at(tdi_ch2_press)._pose;
	double press_rpm_ch2 = datas.elms.at(tdi_ch2_press)._velocity;


	double check_pose_ch1 = datas.elms.at(tdi_ch1_check)._pose;
	double check_rpm_ch1 = datas.elms.at(tdi_ch1_check)._velocity;
	double check_pose_ch2 = datas.elms.at(tdi_ch2_check)._pose;
	double check_rpm_ch2 = datas.elms.at(tdi_ch2_check)._velocity;

	switch (m_step.GetStep())
	{
	case STEP_INIT:
	{
		set_log(L"init");
		m_step.SetStep(STEP_TODO);
	}
	break;

	case STEP_TODO:
	{
		
	}
	break;

	case STEP_TIMEOUT:
	{

		m_step.SetStep(STEP_TODO);
	}
	break;

	//----------------------------------------------------
	//standby
	//----------------------------------------------------
	case STEP_DO_STANDBY:
	{
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

		m_step.SetStep(STEP_DO_STANDBY_END);
	}
	break;

	case STEP_DO_STANDBY_END:
	{
		m_step.SetStep(STEP_TODO);
	}
	break;

	//----------------------------------------------------
		//work 
		//----------------------------------------------------
	case STEP_DO_WORK:
	{
		m_step.sub_step = 0;
		m_step.retry_cnt = 0;
		m_step.SetStep(STEP_DO_WORK_START);
	}
	break;

	case STEP_DO_WORK_START:
	{
		enum
		{
			do_init,
			do_end,
		};
		m_jobflag(jsi_state_cycle_doing, true);
		switch (m_step.sub_step)
		{
		case do_init:
		{
		
			m_step.SetStep(STEP_DO_WORK_WAIT);
		}
		break;

		case do_end:
		{
			if (m_timers.LessThan(1'000 * 1))
				break;

			m_jobflag(jsi_state_cycle_doing, false);
			m_step.SetStep(STEP_DO_WORK_WAIT);
		}
		break;

		default:
		{
			set_log(L"invalid sub step");

			//m_step.SetStep(STEP_TODO);
		}
		break;

		};
	}
	break;

	case STEP_DO_WORK_WAIT:
	{
		enum
		{
			do_init,
			do_end,
		};

		switch (m_step.sub_step)
		{
		case do_init:
		{
			//if (m_step.LessThan(1'000 * 1))
			//	break;
			set_log(L"press init");
			m_step.SetStep(STEP_DO_WORK_END);
		}
		break;

		case do_end:
		{
			if (m_step.LessThan(1'000 * 1))
				break;

			set_log(L"press end");
			m_step.SetStep(STEP_DO_WORK_END);
		}
		break;

		default:
		{
			set_log(L"invalid sub step");

			//m_step.SetStep(STEP_TODO);
		}
		break;

		};
		// end of switch
	}
	break;

	case STEP_DO_WORK_END:
	{
		enum
		{
			do_init,
			do_end,
		};

		m_step.retry_cnt = 0;
		m_timers.Reset(tm_idx_0);
		m_timers.Reset(tm_idx_1);
		m_timers.Reset(tm_idx_2);
		switch (m_step.sub_step)
		{
		case do_init:
		{

			m_step.sub_step = do_end;
			m_step.SetStep(STEP_DO_WORK_START);
		}
		break;


		case do_end:
		{

			m_jobflag(jsi_do_start, false);
			m_jobflag(jsi_state_cycle_doing, false);
			m_step.sub_step = 0;
			m_step.SetStep(STEP_TODO);
		}
		break;
		default:
		{
			set_log(L"invalid sub step");

			//m_step.SetStep(STEP_TODO);
		}
		break;

		};
		// end of switch


	}
	break;


	//################################################################
	//################################################################

	default: break;
	};
	// end of switch


}


BEGIN_MESSAGE_MAP(CtapAuto, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CtapAuto 메시지 처리기


BOOL CtapAuto::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//double dist_datas[] = { 15.0,20.0 };
	//m_viewElmsCh1._data_dist.at(dpi_move1).SetCaption(double2str<std::wstring>(dist_datas[dpi_move1]).c_str());
	//m_viewElmsCh1._data_dist.at(dpi_move1).SetBackColor(RGB(240, 240, 240));
	//m_viewElmsCh1._data_dist.at(dpi_press).SetCaption(double2str<std::wstring>(dist_datas[dpi_press]).c_str());
	//m_viewElmsCh1._data_dist.at(dpi_press).SetBackColor(RGB(240, 240, 240));
	//m_viewElmsCh2._data_dist.at(dpi_move1).SetCaption(double2str<std::wstring>(dist_datas[dpi_move1]).c_str());
	//m_viewElmsCh2._data_dist.at(dpi_move1).SetBackColor(RGB(240, 240, 240));
	//m_viewElmsCh2._data_dist.at(dpi_press).SetCaption(double2str<std::wstring>(dist_datas[dpi_press]).c_str());
	//m_viewElmsCh2._data_dist.at(dpi_press).SetBackColor(RGB(240, 240, 240));


	//double rpm_datas[] = { 100.0, 5.0 };
	//m_viewElmsCh1._data_rpm.at(dpi_move1).SetCaption(double2str<std::wstring>(rpm_datas[dpi_move1]).c_str());
	//m_viewElmsCh1._data_rpm.at(dpi_press).SetCaption(double2str<std::wstring>(rpm_datas[dpi_press]).c_str());	
	//m_viewElmsCh1._data_rpm.at(dpi_move1).SetBackColor(RGB(240, 240, 240));
	//m_viewElmsCh1._data_rpm.at(dpi_press).SetBackColor(RGB(240, 240, 240));

	//m_viewElmsCh2._data_rpm.at(dpi_move1).SetCaption(double2str<std::wstring>(rpm_datas[dpi_move1]).c_str());
	//m_viewElmsCh2._data_rpm.at(dpi_press).SetCaption(double2str<std::wstring>(rpm_datas[dpi_press]).c_str());
	//m_viewElmsCh2._data_rpm.at(dpi_move1).SetBackColor(RGB(240, 240, 240));
	//m_viewElmsCh2._data_rpm.at(dpi_press).SetBackColor(RGB(240, 240, 240));

	StartThread();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CtapAuto::OnShowWindow(BOOL bShow, UINT nStatus)
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


void CtapAuto::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	update();
	doRunStep();
	CDialogEx::OnTimer(nIDEvent);
}


void CtapAuto::registCallback()
{
	m_ptrApi->registerCallback(rxCallback);
}

