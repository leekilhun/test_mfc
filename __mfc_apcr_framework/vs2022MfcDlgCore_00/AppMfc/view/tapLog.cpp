// tapLog.cpp: 구현 파일
//

#include "pch.h"
#include "ui_def.hpp"
#include "commonUI.h"
#include "afxdialogex.h"

#include "tapLog.h"



static std::vector<TinyC::mcc_log::data_t> log_datas{};
static 	TinyC::mcc_log::group_logs grouped_logs{};
static 	CString def_default_log_dir{};
static bool is_dlg_view{};

namespace fs = std::filesystem;

static std::string extract_dateformat_from_filename(const std::string& filename);
static void sort_file_by_date(std::vector<fs::directory_entry>& files);
static int get_log_and_sort();

static void ini_group_logs();


// 파일명에서 날짜를 추출하는 함수
std::string extract_dateformat_from_filename(const std::string& filename)
{
	std::regex datePattern(R"(\[(\d{4}-\d{2}-\d{2})\])"); // [YYYY-MM-DD] 형식 추출
	std::smatch match;
	if (std::regex_search(filename, match, datePattern))
	{
		return match[1]; // 날짜 부분 반환
	}
	return ""; // 날짜가 없는 경우 빈 문자열 반환
}

// 파일 목록을 날짜 기준으로 정렬하는 함수
void sort_file_by_date(std::vector<fs::directory_entry>& files)
{
	std::sort(files.begin(), files.end(), [](const fs::directory_entry& a, const fs::directory_entry& b)
		{
			std::string dateA = extract_dateformat_from_filename(a.path().filename().string());
			std::string dateB = extract_dateformat_from_filename(b.path().filename().string());
			return dateA > dateB; // 내림차순 정렬 (최신 날짜가 먼저 오도록)
		});
}

int get_log_and_sort()
{
	std::filesystem::path cwd = std::filesystem::current_path();
	std::filesystem::path directoryPath = cwd / "Log";// 폴더 경로 지정

	// 폴더 내 파일 목록 읽기
	std::vector<fs::directory_entry> files;
	for (const auto& entry : fs::directory_iterator(directoryPath))
	{
		if (entry.is_regular_file())
		{
			std::string filename = entry.path().filename().string();
			// 파일명 형식이 [날짜]_파일명.log인지 확인
			if (std::regex_match(filename, std::regex(R"(\[\d{4}-\d{2}-\d{2}\]_.+\.log)")))
			{
				files.push_back(entry);
			}
		}
	}

	if (files.empty())
	{
		std::cout << "해당 형식의 파일이 없습니다." << std::endl;
		return 0;
	}

	// 파일 목록을 날짜 기준으로 정렬
	sort_file_by_date(files);

	// 정렬된 파일 목록 출력
	std::cout << "파일 목록 (최신 순):" << std::endl;
	for (size_t i = 0; i < files.size(); ++i)
	{
		std::cout << i + 1 << ": " << files[i].path().filename().string() << std::endl;
	}

	// 사용자에게 파일 선택 요청
	//int choice;
	//std::cout << "선택할 파일 번호를 입력하세요: ";
	//std::cin >> choice;

	//if (choice > 0 && choice <= files.size()) {
	//	std::cout << "선택된 파일: " << files[choice - 1].path().filename().string() << std::endl;
	//}
	//else {
	//	std::cout << "잘못된 선택입니다." << std::endl;
	//}

	return 0;
}

void ini_group_logs()
{
	TinyC::mcc_log::file_group(grouped_logs);

	namespace fs = std::filesystem;
	fs::path cwd = fs::current_path();
	// Log 폴더 경로 설정
	fs::path log_dir = cwd / L"Log";
	def_default_log_dir = log_dir.wstring().c_str();
}


// CtapLog 대화 상자

IMPLEMENT_DYNAMIC(CtapLog, CDialogEx)


CtapLog::CtapLog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAP_LOG, pParent)
{


	std::vector<TinyC::mcc_log::data_t> logs{};

	ini_group_logs();

}

CtapLog::~CtapLog()
{
}

void CtapLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LOG_GRID_DATA_VIEW, m_logFrame);// 
	DDX_Control(pDX, IDC_LOG_CBO_SEL_OBJ, m_cboObj);
}


BEGIN_MESSAGE_MAP(CtapLog, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_LOG_CBO_SEL_OBJ, &CtapLog::OnCbnSelchangeCboLogObj)
	ON_BN_CLICKED(IDC_TAP_LOG_ERR_VIEW, &CtapLog::ClickLogFuncViewErrLog)
END_MESSAGE_MAP()


// CtapLog 메시지 처리기
BEGIN_EVENTSINK_MAP(CtapLog, CFormView)
END_EVENTSINK_MAP()

BOOL CtapLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// 폰트 설정
	m_Font.CreatePointFont(120, _T("Arial"));   // 12pt Arial 폰트
	m_BoldFont.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Verdana")); // 굵은 Verdana 16px

	m_cboObj.SetFont(&m_BoldFont);  // 콤보박스에 폰트 적용

	init_droplist();

	GetDlgItem(IDC_LOG_GRID_DATA_VIEW)->GetWindowRect(&m_rectGrid);
	ScreenToClient(&m_rectGrid);
	m_logGrid.Create(m_rectGrid, this, IDC_LOG_GRID_DATA_VIEW, WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL);
	//m_Grid.Create(rect, this, IDC_GRID, WS_CHILD | WS_VISIBLE | WS_VSCROLL); // WS_HSCROLL 제거

	log_datas.clear();
	using data_t = TinyC::mcc_log::LogFileInfo;
	const auto& firstEntry = *grouped_logs.begin();
	// 첫 번째 요소의 날짜와 로그 리스트를 가져오기
	const std::string& firstDate = firstEntry.first;
	const std::vector<data_t>& firstLogList = firstEntry.second;
	// 첫 번째 로그 리스트의 첫 번째 로그에 접근 (존재할 경우)
	if (!firstLogList.empty())
	{
		const data_t& firstLog = firstLogList.front();
		namespace fs = std::filesystem;
		fs::path cwd = fs::current_path();
		// Log 폴더 경로 설정
		fs::path log_dir = cwd / L"Log\\";
		std::string file_path = log_dir.string() + firstLog.filename;
		TinyC::mcc_log::view_log(file_path.c_str(), log_datas);
	}


	initGrid();

	// 기본 디렉터리 설정
	// .log 파일 필터 설정
	//CString strFilter = L"Log Files (*.log)|*.log|";

	//// .log 파일 탐색 버튼 활성화
	//m_filebrowse.EnableFileBrowseButton(0, _T(" Log files (*.log)|*.log| AllFiles (*.*)|*.*|"), 0);
	//m_filebrowse.SetFont(&m_BoldFont);  // 콤보박스에 폰트 적용


	//m_filebrowse.SetWindowText(def_default_log_dir);

	is_dlg_view = true;


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CtapLog::initGrid()
{
	CRect frame_rect;
	m_logFrame.GetWindowRect(&frame_rect);
	//m_logFrame.

	double FRAME_EXCEPT_WIDTH = 0.0;
	//if (log_datas.size() > 20)
//	FRAME_EXCEPT_WIDTH = 12.0;

	int totalWidth = frame_rect.Width(); // 그리드의 전체 너비
	int  ROW_CNT_MAX = (int)(log_datas.size() + 1);

	enum cols_idx
	{
		ci_date, ci_type, ci_desc, ci_func, ci_max
	};

	constexpr int COLUMN_CNT_MAX = 4;
	int default_row_height = 35;

	double p_x = ((double)frame_rect.Width() - (double)FRAME_EXCEPT_WIDTH) / 100.0;
	double p_y = ((double)frame_rect.Height() - (double)FRAME_EXCEPT_WIDTH) / (double)ROW_CNT_MAX;

	if (-1 != default_row_height)
		m_logGrid.DeleteAllItems();

	m_logGrid.SetEditable(FALSE);
	m_logGrid.EnableDragAndDrop(FALSE);
	m_logGrid.EnableSelection(FALSE);
	m_logGrid.SetTextBkColor(cdu::LINE_B);
	m_logGrid.SetRowResize(FALSE);
	m_logGrid.SetColumnResize(FALSE);
	m_logGrid.SetListMode(FALSE);
	m_logGrid.SetSingleRowSelection(FALSE);
	m_logGrid.SetHeaderSort(FALSE);

	m_logGrid.SetColumnCount(ci_max);
	m_logGrid.SetRowCount(ROW_CNT_MAX);
	// 그리드에 기본 폰트 적용
	m_logGrid.SetFont(&m_Font);
	m_logGrid.SetHeaderSort(TRUE);

	if (-1 == default_row_height)
	{
		default_row_height = m_logGrid.GetRowHeight(0);
		auto pFont = m_logGrid.GetItemFont(0, 0);
		default_row_height = pFont->lfHeight;
	}

	for (int c = 0; c < ci_max; c++)
	{
		for (int r = 0; r < ROW_CNT_MAX; r++)
		{
			m_logGrid.SetRowHeight(r, default_row_height);
			m_logGrid.SetItemFormat(r, c, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			if (r & 0x01)
			{
				for (int cc = 0; cc < ci_max; cc++)
				{
					m_logGrid.SetItemBkColour(r, cc, cdu::LINE_G);
				}
			}
		}
	}

	m_logGrid.SetItemText(0, ci_date, L"date");
	m_logGrid.SetItemText(0, ci_type, L"type");
	m_logGrid.SetItemText(0, ci_desc, L"description");
	m_logGrid.SetItemText(0, ci_func, L"func");

	m_logGrid.SetColumnWidth(ci_date, (int)(p_x * 15.0)/*COLUMN_1_WIDTH*/);
	m_logGrid.SetColumnWidth(ci_type, (int)(p_x * 5.0)/*COLUMN_2_WIDTH*/);
	m_logGrid.SetColumnWidth(ci_desc, (int)(p_x * 58.0)/*COLUMN_3_WIDTH*/);
	m_logGrid.SetColumnWidth(ci_func, (int)(p_x * 20.0)/*COLUMN_4_WIDTH*/);

	update_log();

	m_logGrid.SetFixedRowCount(1);
	//m_logGrid.SetFixedColumnCount(1);

	m_logGrid.SetRedraw(TRUE, TRUE);

}

void CtapLog::update_log()
{
	int row_no = 1;
	enum cols_idx
	{
		ci_date, ci_type, ci_desc, ci_func, ci_max
	};

	for (size_t idx = 0; idx < log_datas.size(); ++idx)
	{
		// 역순 인덱스 계산
		size_t reverse_idx = log_datas.size() - 1 - idx;

		CGridCellBase* pCell = m_logGrid.GetCell(row_no + (int)idx, ci_date);
		pCell->SetFormat(DT_CENTER | DT_VCENTER);
		pCell->SetText(ptrCha2wcha(log_datas[reverse_idx].date.c_str()));

		pCell = m_logGrid.GetCell(row_no + (int)idx, ci_type);
		pCell->SetText(ptrCha2wcha(log_datas[reverse_idx].type.c_str()));

		pCell = m_logGrid.GetCell(row_no + (int)idx, ci_desc);
		pCell->SetFormat(DT_LEFT | DT_VCENTER);
		CString tmsg{ mcc::conversion::string_to_wstring(log_datas[reverse_idx].desc).c_str() };

		CString msg = tmsg;//cha2Cstr(log_datas[reverse_idx].desc.c_str());
		pCell->SetText(msg);

		pCell = m_logGrid.GetCell(row_no + (int)idx, ci_func);
		pCell->SetFormat(DT_LEFT | DT_VCENTER);
		pCell->SetText(ptrCha2wcha(log_datas[reverse_idx].func.c_str()));
	}
}

void CtapLog::init_droplist()
{
	m_cboObj.ResetContent();
	CString str{};

	std::vector< CString> lists{};
	m_logFiles.sys_build.clear();
	m_logFiles.sys_err.clear();
	m_logFiles.auto_err.clear();

	for (const auto& [date, logs] : grouped_logs)
	{
		CString file{};
		for (const auto& log : logs)
		{
			/*
			_.+\.log → _[^.]+\.log
			_ 이후에 .(점)을 포함하지 않는 문자열([^.]+)을 허용
			_old.log처럼 _ 이후에 .이 포함된 경우 매칭되지 않음
			정상적인 로그 파일(_sysError.log)만 매칭됨
			*/
			std::regex log_pattern(R"(\[\d{4}-\d{2}-\d{2}\]_[^_]+\.log)"); //_old.log 파일 제외
			if (std::regex_match(log.filename, log_pattern))
			{
				std::regex log_pattern(R"(^\[(\d{4}-\d{2}-\d{2})\]_(\w+)+\.log$)");//
				std::smatch match;

				if (std::regex_match(log.filename, match, log_pattern))
				{
					if (match[2] == "system")
						m_logFiles.sys_build.emplace_back(log.filename);
					else if (match[2] == "sysError")
						m_logFiles.sys_err.emplace_back(log.filename);
					else if (match[2] == "autoError")
						m_logFiles.auto_err.emplace_back(log.filename);
					else
						m_logFiles.etc.emplace_back(log.filename);

					file = CString{ ptrCha2wcha(log.filename.c_str()) };
					str.Format(L"%s ", file.GetBuffer());
					lists.emplace_back(str);
					m_cboObj.AddString(str);
				}
			}



		}
	}
	// 정렬 (내림차순)	
	std::sort(m_logFiles.sys_build.begin(), m_logFiles.sys_build.end(), [](const std::string& a, const std::string& b) {
		std::string lowerA = a, lowerB = b;
		std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
		std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
		return lowerA > lowerB; // 내림차순 정렬
		});

	std::sort(m_logFiles.sys_err.begin(), m_logFiles.sys_err.end(), [](const std::string& a, const std::string& b) {
		std::string lowerA = a, lowerB = b;
		std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
		std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
		return lowerA > lowerB; // 내림차순 정렬
		});

	std::sort(m_logFiles.auto_err.begin(), m_logFiles.auto_err.end(), [](const std::string& a, const std::string& b) {
		std::string lowerA = a, lowerB = b;
		std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
		std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
		return lowerA > lowerB; // 내림차순 정렬
		});

#if 1
	std::vector<CString> items;
	CString strItem;
	int itemCount = m_cboObj.GetCount();

	// 기존 아이템들을 가져와 벡터에 저장
	for (int i = 0; i < itemCount; i++)
	{
		m_cboObj.GetLBText(i, strItem);
		items.push_back(strItem);
	}

	// 기존 아이템 삭제
	m_cboObj.ResetContent();

	// 정렬 (오름차순)
	//std::sort(items.begin(), items.end(), [](const CString& a, const CString& b) 
	//	{
	//	return a.CompareNoCase(b) < 0;
	//	});

	// 정렬 (내림차순)
	std::sort(items.begin(), items.end(), [](const CString& a, const CString& b)
		{
			return a.CompareNoCase(b) > 0; // 내림차순으로 정렬
		});

	// 정렬된 리스트 다시 추가
	for (const auto& item : items)
	{
		m_cboObj.AddString(item);
	}

#endif
	//m_cboObj.ModifyStyle(0, CBS_SORT);
	m_cboObj.SetCurSel(0);
}

void CtapLog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CtapLog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		m_TimerID = SetTimer(UPDATE_TIMER_TAP_LOG_DLG, 1'000, NULL);
		if (is_dlg_view)
		{
			init_droplist();
			ClickLogFuncViewErrLog();
		}
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

void CtapLog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}

void CtapLog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}

void CtapLog::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

HBRUSH CtapLog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

BOOL CtapLog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CtapLog::OnCbnSelchangeCboLogObj()
{
	CString str{};
	m_cboObj.GetLBText(m_cboObj.GetCurSel(), str);
	log_datas.clear();
	CString log_file = L"Log/" + str;
	TinyC::mcc_log::view_log(ptrWcha2cha(log_file.GetBuffer()), log_datas);

	initGrid();
}

void CtapLog::ClickLogFuncViewErrLog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ini_group_logs();
	init_droplist();
	if (m_logFiles.auto_err.size())
	{
		log_datas.clear();
		std::string log_file{ "Log/" + *m_logFiles.auto_err.begin() };
		TinyC::mcc_log::view_log(log_file.c_str(), log_datas);

		initGrid();
	}
}
