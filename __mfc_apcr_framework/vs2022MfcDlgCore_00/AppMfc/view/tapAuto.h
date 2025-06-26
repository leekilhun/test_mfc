#pragma once
#include "afxdialogex.h"

#include "etc/imgCn.h"
#include "etc/jogButton.h"

// CtapAuto 대화 상자

class CtapAuto : public CDialogEx
{

	enum job_status_idx : uint8_t
	{
		jsi_do_ready,
		jsi_do_start,
		jsi_2,
		jsi_3,
		jsi_4,
		jsi_5,
		jsi_6,
		jsi_7,

		jsi_8,
		jsi_9,
		jsi_10,
		jsi_11,
		jsi_12,
		jsi_13,
		jsi_14,
		jsi_15,

		jsi_16,
		jsi_17,
		jsi_18,
		jsi_19,
		jsi_20,
		jsi_21,
		jsi_22,
		jsi_state_cycle_doing,

		jsi_24,
		jsi_25,
		jsi_26,
		jsi_27,
		jsi_28,
		jsi_29,
		jsi_30,
		jsi_31,

		jsi_max
	};
	udword_t m_jobflag{};


	enum task_reg_idx : uint8_t
	{
		tri_0,
		tri_1,
		tri_2,
		tri_3,
		tri_4,
		tri_5,
		tri_6,
		tri_7,

		tri_sys_init_prc_cpltd, // 시스템 초기화 완료된 상태
		tri_9,
		tri_10,
		tri_11,
		tri_12,
		tri_13,
		tri_14,
		tri_15,
		tri_doing_sys_init_prc, // 시스템 초기화가 진행 중

		tri_result_fail_ch1,
		tri_result_fail_ch2,
		tri_result_pass_ch1,
		tri_result_pass_ch2,
		tri_21,
		tri_22,
		tri_23,

		tri_24,
		tri_25,
		tri_26,
		tri_27,
		tri_28,
		tri_29,
		tri_30,
		tri_31,

		tri_max
	};
	udword_t m_taskReg{};

	apcr::pc_api_t m_ptrApi{};
	ap_data::teachDat* m_ptrTeachData{};

	using op_t = apcr::ap_reg::inner_ope_reg;
	op_t m_opReg{};

	enum tm_t : uint8_t
	{
		tm_idx_0,
		tm_idx_1,
		tm_idx_2,
		tm_idx_3,
		tm_idx_4,
		tm_idx_5,
		tm_idx_6,
		tm_idx_7,
		tm_idx_max,
	};
	apcr::Timer<tm_idx_max> m_timers{};
	using step_t = apcr::machine_step_t;
	step_t m_step{};
	std::thread                        m_trd{};
	std::atomic<bool>                  m_stopThread{};
	std::function<int(void*, void*)> m_cb{};
	std::mutex m_mutex{};
	void threadStop();
	void threadRun();
	void threadJob();

	void ThreadJob();
	errno_t StopThread(); 
	errno_t StartThread(); 

	void auto_start();
	
	enum state_lamp_idx
	{
		sli_green,
		sli_yellow,
		sli_red,
		sli_max
	};

	enum state_idx
	{
		si_ready,
		si_start,
		si_move1,
		si_press,
		si_check,
		si_return,
		si_max
	};

	enum data_pose_idx
	{
		dpi_move1,
		dpi_press,
		dpi_check,
		dpi_max
	};
	struct view_elm_t
	{
		std::vector<CBtnEnh> _lamps{sli_max };
		std::vector<CBtnEnh> _state_labels{ si_max };
		std::vector<CBtnEnh> _data_dist{ dpi_max };
		std::vector<CBtnEnh> _data_rpm{ dpi_max };
		CBtnEnh _result{};
		CBtnEnh _curr_pos{};
	};

	view_elm_t m_viewElmsCh1{};

	DECLARE_DYNAMIC(CtapAuto)

public:
	CtapAuto(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CtapAuto();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAP_AUTO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	UINT_PTR	m_TimerID{};
	CListBox m_listRxLog;
	CListBox m_listErrLog{};

	void addRxLog(CString str);
	void update(void);
	void doRunStep();
	CListBox m_listStepLog{};
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	void registCallback();
};
