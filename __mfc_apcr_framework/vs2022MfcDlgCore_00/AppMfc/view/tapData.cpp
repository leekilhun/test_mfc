// tapData.cpp: 구현 파일
//

#include "pch.h"
#include "ui_def.hpp"
#include "commonUI.h"
#include "afxdialogex.h"
#include "tapData.h"


// CtapData 대화 상자

IMPLEMENT_DYNAMIC(CtapData, CDialogEx)


CtapData::CtapData(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAP_DATA, pParent)
{
	m_ptrApi = apcr::get_api();
	m_ptrTeachData = apcr::get_teach_data();
}

CtapData::~CtapData()
{
}

void CtapData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	using namespace cui;
	int idx = tdi_ch1_standby;
	DDX_Control(pDX, IDB_TEACH_POS_0, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_0, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_0, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_0, m_datas[idx].view_description);


	idx = tdi_ch1_start;
	DDX_Control(pDX, IDB_TEACH_POS_1, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_1, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_1, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_1, m_datas[idx].view_description);


	idx = tdi_ch1_move1;
	DDX_Control(pDX, IDB_TEACH_POS_2, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_2, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_2, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_2, m_datas[idx].view_description);


	idx = tdi_ch1_press;
	DDX_Control(pDX, IDB_TEACH_POS_3, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_3, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_3, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_3, m_datas[idx].view_description);


	idx = tdi_ch1_check;
	DDX_Control(pDX, IDB_TEACH_POS_4, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_4, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_4, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_4, m_datas[idx].view_description);


	idx = tdi_ch2_standby;
	DDX_Control(pDX, IDB_TEACH_POS_5, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_5, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_5, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_5, m_datas[idx].view_description);


	idx = tdi_ch2_start;
	DDX_Control(pDX, IDB_TEACH_POS_6, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_6, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_6, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_6, m_datas[idx].view_description);


	idx = tdi_ch2_move1;
	DDX_Control(pDX, IDB_TEACH_POS_7, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_7, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_7, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_7, m_datas[idx].view_description);


	idx = tdi_ch2_press;
	DDX_Control(pDX, IDB_TEACH_POS_8, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_8, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_8, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_8, m_datas[idx].view_description);


	idx = tdi_ch2_check;
	DDX_Control(pDX, IDB_TEACH_POS_9, m_datas[idx].view_name);
	DDX_Control(pDX, IDC_TEACH_POS_DATA_9, m_datas[idx].view_dist);
	DDX_Control(pDX, IDC_TEACH_VEL_DATA_9, m_datas[idx].view_velocity);
	DDX_Control(pDX, IDC_TEACH_DESC_DATA_9, m_datas[idx].view_description);


}


BEGIN_MESSAGE_MAP(CtapData, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CtapData 메시지 처리기


HBRUSH CtapData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CtapData::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CtapData::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CtapData::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//key event
	CString msg_str;
	//int value;

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
		{
			//if (pMsg->hwnd == GetDlgItem(IDC_ORGCMD_SEARCHSPEED2)->m_hWnd)
			//{

			//	value = GetDlgItemInt(IDC_ORGCMD_SEARCHSPEED2);
			//	msg_str.Format(L"변경된 값 [%d](Origin Search Speed)을 저장하시겠습니까?", value);
			//	if (IDYES == AfxMessageBox(msg_str, MB_YESNO))
			//	{
			//		writeMcuData(static_cast<uint8_t>(fm_parameter::param::org_search_speed), value);
			//	}
			//}

			//if (pMsg->hwnd == GetDlgItem(IDC_ORGCMD_SPEED2)->m_hWnd)
			//{

			//	value = GetDlgItemInt(IDC_ORGCMD_SPEED2);
			//	msg_str.Format(L"변경된 값 [%d](Origin Speed)을 저장하시겠습니까?", value);
			//	if (IDYES == AfxMessageBox(msg_str, MB_YESNO))
			//	{
			//		writeMcuData(static_cast<uint8_t>(fm_parameter::param::org_speed), value);
			//	}
			//}

			//if (pMsg->hwnd == GetDlgItem(IDC_ORGCMD_ACCELDECEL)->m_hWnd)
			//{

			//	value = GetDlgItemInt(IDC_ORGCMD_ACCELDECEL);
			//	msg_str.Format(L"변경된 값 [%d](Origin Acc Dec Time)을 저장하시겠습니까?", value);
			//	if (IDYES == AfxMessageBox(msg_str, MB_YESNO))
			//	{
			//		writeMcuData(static_cast<uint8_t>(fm_parameter::param::org_acc_dec_time), value);
			//	}
			//}

		}
		break;
		default:
			break;
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CtapData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//-------------------------------------------------------
	// 
	//-------------------------------------------------------


	m_colorChanged = RGB(255, 255, 50);  // 노란색 배경 지정
	m_colorDefault = RGB(240, 240, 240);

	loadTeachingData();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



BEGIN_EVENTSINK_MAP(CtapData, CDialogEx)
	ON_EVENT(CtapData, IDB_DATA_SAVE, DISPID_CLICK, CtapData::ClickDataSave, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_0, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_0, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_0, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_1, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_1, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_1, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_2, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_2, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_2, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_3, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_3, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_3, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_4, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_4, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_4, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_5, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_5, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_5, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_6, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_6, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_6, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_7, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_7, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_7, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_8, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_8, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_8, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)

	ON_EVENT(CtapData, IDC_TEACH_POS_DATA_9, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_VEL_DATA_9, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)
	ON_EVENT(CtapData, IDC_TEACH_DESC_DATA_9, DISPID_CLICK, CtapData::ClickDataTblEdit, VTS_NONE)


END_EVENTSINK_MAP()

void CtapData::ClickDataSave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	using idx_t = ap_data::teachDat::data_reg_idx;
	ap_data::teachDat::pos_dat_t&  datas = m_ptrTeachData->m_posDatas[static_cast<int>(idx_t::teaching_data)];
	int idx = 0;
	for (auto& elm : m_datas)
	{
		CString str{};
		auto& obj = datas.elms[idx];
		obj._name = ptrWcha2cha(elm.view_name.GetCaption().GetBuffer());
		obj._pose = str_To_double(elm.view_dist.GetCaption().GetBuffer());
		obj._velocity = str_To_double(elm.view_velocity.GetCaption().GetBuffer());
		obj._desc = ptrWcha2cha(elm.view_description.GetCaption().GetBuffer());
		idx++;

	}
	if (MessageBox(L"변경된 데이터를 저장하겠습니까  ?", L"확인", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		m_ptrTeachData->SaveToFile();
		loadTeachingData();
	}
	

}

void CtapData::loadTeachingData(void)
{
	using idx_t = ap_data::teachDat::data_reg_idx;
	ap_data::teachDat::pos_dat_t  datas{ m_ptrTeachData->m_posDatas[static_cast<int>(idx_t::teaching_data)] };

	using namespace cui;

	auto set_data = [&](view_t& elm, teach_data_idx idx) ->void
		{
			elm.description = datas.elms[idx]._desc;
			elm.distance_mm = datas.elms[idx]._pose;
			elm.speed_rpm = datas.elms[idx]._velocity;
			elm.view_name.SetCaption(cha2Cstr(datas.elms[idx]._name.c_str()));
			elm.view_name.SetBackColor(m_colorDefault);
			std::wstring w_desc = string2Wstring(datas.elms[idx]._desc);
			CString desc{ w_desc.c_str()};
			elm.view_description.SetCaption(desc);
			elm.view_description.SetBackColor(m_colorDefault);
			elm.view_dist.SetCaption(double2str<std::wstring>(datas.elms[idx]._pose).c_str());
			elm.view_dist.SetBackColor(m_colorDefault);
			elm.view_velocity.SetCaption(double2str<std::wstring>(datas.elms[idx]._velocity).c_str());
			elm.view_velocity.SetBackColor(m_colorDefault);

		};


	for (int i = 0; i < datas.elms.size(); i++)
	{
		if (i < teach_data_idx::tdi_max)
		{
			//view_t ret{ };
			set_data(m_datas[i], (teach_data_idx)i);
			//m_datas.emplace_back(ret);
		}
	}
	

}
void CtapData::ClickDataTblEdit()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	CWnd* pActiveXControl = GetFocus();  // 현재 포커스를 가진 ActiveX 컨트롤 얻기
	if (pActiveXControl)
	{
		constexpr const wchar_t* desc_pose = L"모터의 위치 데이터를 변경합니다.";
		constexpr const wchar_t* desc_speed = L"모터의 이동 속도 데이터를 변경합니다.";
		constexpr const wchar_t* desc_descript = L"모터의 설명을 변경합니다.";
		UINT nCtrlID = pActiveXControl->GetDlgCtrlID();  // 컨트롤 ID 가져오기		
		switch (nCtrlID)
		{

			/////////////////   name
			// dist
		case IDC_TEACH_POS_DATA_0:__fallthrough;
		case IDC_TEACH_POS_DATA_1:__fallthrough;
		case IDC_TEACH_POS_DATA_2:__fallthrough;
		case IDC_TEACH_POS_DATA_3:__fallthrough;
		case IDC_TEACH_POS_DATA_4:__fallthrough;
		case IDC_TEACH_POS_DATA_5:__fallthrough;
		case IDC_TEACH_POS_DATA_6:__fallthrough;
		case IDC_TEACH_POS_DATA_7:__fallthrough;
		case IDC_TEACH_POS_DATA_8:__fallthrough;
		case IDC_TEACH_POS_DATA_9:
		{
			constexpr int limit_value = 30;
			GetNumberWin(limit_value, GetDlgItem(nCtrlID)->GetSafeHwnd(), desc_pose, m_colorChanged);
		}
		break;
		/////////////////   name
    // velocity
		case IDC_TEACH_VEL_DATA_0:__fallthrough;
		case IDC_TEACH_VEL_DATA_1:__fallthrough;
		case IDC_TEACH_VEL_DATA_2:__fallthrough;
		case IDC_TEACH_VEL_DATA_3:__fallthrough;
		case IDC_TEACH_VEL_DATA_4:__fallthrough;
		case IDC_TEACH_VEL_DATA_5:__fallthrough;
		case IDC_TEACH_VEL_DATA_6:__fallthrough;
		case IDC_TEACH_VEL_DATA_7:__fallthrough;
		case IDC_TEACH_VEL_DATA_8:__fallthrough;
		case IDC_TEACH_VEL_DATA_9:
		{
			constexpr int limit_value = 100;
			GetNumberWin(limit_value, GetDlgItem(nCtrlID)->GetSafeHwnd(), desc_pose, m_colorChanged);
		}
		break;
		default: AfxMessageBox(L"indivial ID"); break;
		}
		// end of switch
	}
}
