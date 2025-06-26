#pragma once
#include "afxdialogex.h"


// CtapData 대화 상자

class CtapData : public CDialogEx
{
	apcr::pc_api_t m_ptrApi{};
	ap_data::teachDat* m_ptrTeachData{};
	UINT_PTR	m_TimerID{};

	struct view_t
	{
		CBtnEnh view_name{};
		CBtnEnh view_dist{};
		CBtnEnh view_velocity{};
		CBtnEnh view_description{};
		double distance_mm{};
		double speed_rpm{};
		std::string description{};
	};

	COLORREF m_colorChanged{};
	COLORREF m_colorDefault{};
	std::vector <view_t> m_datas{ cui::tdi_max };


	void loadTeachingData(void);

	DECLARE_DYNAMIC(CtapData)

public:
	CtapData(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CtapData();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAP_DATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickDataSave();
	void ClickDataTblEdit();
};
