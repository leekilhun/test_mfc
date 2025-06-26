#pragma once
#include "afxdialogex.h"

#include "etc/imgCn.h"
#include "etc/jogButton.h"

// CtapTeach 대화 상자

class CtapTeach : public CDialogEx
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

		jsi_do_gathering_ch1,
		jsi_do_gathering_ch2,
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

		tri_17,
		tri_18,
		tri_19,
		tri_20,
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
		tm_idx_8,
		tm_idx_9,
		tm_idx_max,
	};
	apcr::Timer<tm_idx_max> m_timers{};

	Gdiplus::Image* m_pImage{};
	CImage m_image; // CImage 객체 추가
	CimgCn m_pictureControl; // Picture Control 객체 추가
	apcr::pc_api_t m_ptrApi{};

	CJogButton	m_btnNJog{};
	CJogButton	m_btnPJog{};

	using step_t = apcr::machine_step_t;
	step_t m_step{};
	bool m_startflag{}; //auto test start flag
	bool m_isStandby{}; // test standby flag

	DECLARE_DYNAMIC(CtapTeach)

public:
	CtapTeach(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CtapTeach();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAP_TEACH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	UINT_PTR	m_TimerID{};
	CListBox m_listRxLog;
	CListBox m_listErrLog{};
	BOOL m_isTeachable{};       // Teaching이 가능한 상태인지 여부

	void addRxLog(CString str);
	void update(void);
	void doRunStep();
	CListBox m_listStepLog{};
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	LRESULT OnJogButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnJogButtonUp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedMainBtnPowerOn();
	afx_msg void OnBnClickedMainBtnPowerOff();
	afx_msg void OnBnClickedMainBtnReset();
	afx_msg void OnBnClickedMainBtnClear();
	afx_msg void OnBnClickedMianBtnAbsMove();
	afx_msg void OnBnClickedMianBtnRelMove();
	afx_msg void OnBnClickedMianBtnHome();
	afx_msg void OnBnClickedMianBtnHalt();

	bool is_motor_ready();
	bool is_motor_stop();
	bool is_motor_inpose(int32_t pos);
	afx_msg void OnBnClickedMianBtnAutoTestStart();
	afx_msg void OnBnClickedMianBtnAutoTestStop();

	void registCallback();
};
