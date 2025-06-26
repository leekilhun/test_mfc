// tapIO.cpp: 구현 파일
//

#include "pch.h"
#include "ui_def.hpp"
#include "commonUI.h"

#include "afxdialogex.h"
#include "tapIO.h"



// CtapIO 대화 상자

IMPLEMENT_DYNAMIC(CtapIO, CDialogEx)

CtapIO::CtapIO(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAP_IO_REG, pParent)
{
	m_ptrApi = apcr::get_api();
}

CtapIO::~CtapIO()
{
}

void CtapIO::DoDataExchange(CDataExchange* pDX)
{
	using namespace apcr::ap_reg;
	auto assign_control = [&](CDataExchange* pDX, int startID, size_t count, std::vector<CToggleBtn>& array)
		{
			for (int i = 0; i < (int)count; ++i)
			{
				DDX_Control(pDX, startID + i, array[i]);
			}
		};

	CDialogEx::DoDataExchange(pDX);

	/* 컨트롤 ID가 연속되지 않았다면 아래처럼 명시적으로 연결할 것.*/
	int idx{};
	DDX_Control(pDX, IDC_CHECK_REG_145, m_regs[idx++]); //SYSIO_00
	DDX_Control(pDX, IDC_CHECK_REG_146, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_147, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_148, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_149, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_150, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_151, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_152, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_153, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_154, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_155, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_156, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_157, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_158, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_159, m_regs[idx++]);
	DDX_Control(pDX, IDC_CHECK_REG_160, m_regs[idx++]);

	/* 반드시 컨트롤 ID가 연속되어 있어야 한다.*/
	assign_control(pDX, IDC_CHECK_REG_1, m_status.size(), m_status);// ~ STATUS_17(16개 컨트롤러 인덱스)
	assign_control(pDX, IDC_CHECK_REG_17, m_option.size(), m_option); // ~ OPTION_17(16개 컨트롤러 인덱스)
	assign_control(pDX, IDC_CHECK_REG_49, m_alarm.size(), m_alarm); // ~ ALARM_37(32개 컨트롤러 인덱스)
	assign_control(pDX, IDC_CHECK_REG_81, m_regX.size(), m_regX); // ~ ALARM_37(32개 컨트롤러 인덱스)
	assign_control(pDX, IDC_CHECK_REG_113, m_regY.size(), m_regY); // ~ ALARM_37(32개 컨트롤러 인덱스)	

}

BEGIN_MESSAGE_MAP(CtapIO, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_REG_17, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_18, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_19, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_20, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_21, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_22, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_23, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_24, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_25, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_26, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_27, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_28, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_29, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_30, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_31, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_32, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_33, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_34, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_35, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_36, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_37, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_38, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_39, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_40, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_41, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_42, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_43, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_44, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_45, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_46, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_47, &CtapIO::OnBnClickedCheckRegOptionWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_48, &CtapIO::OnBnClickedCheckRegOptionWrite)

	//113 ~144
	ON_BN_CLICKED(IDC_CHECK_REG_113, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_114, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_115, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_116, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_117, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_118, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_119, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_120, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_121, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_122, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_123, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_124, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_125, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_126, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_127, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_128, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_129, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_130, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_131, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_132, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_133, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_134, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_135, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_136, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_137, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_138, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_139, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_140, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_141, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_142, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_143, &CtapIO::OnBnClickedCheckRegOutWrite)
	ON_BN_CLICKED(IDC_CHECK_REG_144, &CtapIO::OnBnClickedCheckRegOutWrite)
END_MESSAGE_MAP()


// CtapIO 메시지 처리기


void CtapIO::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		m_TimerID = SetTimer(UPDATE_TIMER_TAP_REG_DLG, 50, NULL);
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


void CtapIO::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	update();
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CtapIO::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	auto btn_init = [&](std::vector<CToggleBtn>& array, const wchar_t* desc)
		{
			for (int i = 0; i < array.size(); ++i)
			{
				array[i].SetBackgroundColor(RGB(255, 255, 255));
				array[i].SetTextColor(RGB(0, 0, 255));
				array[i].LoadImages(L"res\\3_circle_state.png");     // 이미지 설정3_circle_state

				CString str = CString{ desc } + CString{ std::to_wstring(i).c_str() };
				array[i].SetWindowText(str);
			}
		};
	btn_init(m_regs, L"sys_");
	btn_init(m_status, L"sta_");
	btn_init(m_option, L"opt_");
	btn_init(m_alarm, L"err_");
	btn_init(m_regX, L"ioX_");
	btn_init(m_regY, L"ioY_");

	for (int i = 0; i < m_regs.size(); ++i)
		m_regs[i].SetWindowText(cha2Cstr(apcr::ap_reg::sysio_str[i].c_str()));

	for (int i = 0; i < m_status.size(); ++i)
		m_status[i].SetWindowText(cha2Cstr(apcr::ap_reg::status_str[i].c_str()));

	for (int i = 0; i < m_option.size(); ++i)
		m_option[i].SetWindowText(cha2Cstr(apcr::ap_reg::option_str[i].c_str()));

	for (int i = 0; i < m_alarm.size(); ++i)
		m_alarm[i].SetWindowText(cha2Cstr(apcr::ap_reg::alarm_str[i].c_str()));

	for (int i = 0; i < m_regX.size(); ++i)
		m_regX[i].SetWindowText(cha2Cstr(apcr::ap_reg::IOX_str[i].c_str()));

	for (int i = 0; i < m_regY.size(); ++i)
		m_regY[i].SetWindowText(cha2Cstr(apcr::ap_reg::IOY_str[i].c_str()));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CtapIO::update(void)
{
	mcudat_t mcd_data = m_ptrApi->m_cfg.ptr_data->get_data();

	//mcd_data.reg_state = 534;

	auto set_bit = [&](std::vector<CToggleBtn>& array, udword_t data)
		{
			for (size_t i = 0; i < array.size(); ++i)
			{
				if ((BOOL)data((uint8_t)i) != array[i].GetCheck())
				{
					array[i].SetCheck((BOOL)data((uint8_t)i));
					//Invalidate();
				}
			}
		};

	;
	set_bit(m_status, udword_t{ mcd_data.reg_state });
	set_bit(m_option, udword_t{ mcd_data.reg_opt });
	set_bit(m_alarm, udword_t{ mcd_data.reg_err });
	//mcd_data.io_X[0] = 1295;
	set_bit(m_regX, udword_t{ mcd_data.io_X[0] });
	//udword_t out_reg = apcr::get_ioY();
	//set_bit(m_regY, out_reg);
	set_bit(m_regY, udword_t{ mcd_data.io_Y[0] });



}


void CtapIO::OnBnClickedCheckRegOptionWrite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	uint32_t dataToSend = 0;
	for (int i = 0; i < m_option.size(); ++i)
		if (m_option[i].GetCheck())
			dataToSend |= (1 << i);

	m_ptrApi->do_write_reg_option(dataToSend);
}


void CtapIO::OnBnClickedCheckRegOutWrite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	uint32_t dataToSend = 0;
	for (int i = 0; i < m_regY.size(); ++i)
		if (m_regY[i].GetCheck())
			dataToSend |= (1 << i);
	udword_t& out_reg = apcr::get_ioY();
	out_reg = dataToSend;
	//m_ptrApi->do_write_out_reg(dataToSend);
}
