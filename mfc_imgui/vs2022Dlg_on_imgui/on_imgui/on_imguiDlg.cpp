
// on_imguiDlg.cpp : implementation file
//
// on_imguiDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "on_imgui.h"
#include "on_imguiDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define UPDATE_TIMER_POP_UNIT_INIT_DLG 1

//-----------------------------------------------------------------------------
// DirectX 및 ImGui 관련 헤더
//-----------------------------------------------------------------------------
#include <DirectXMath.h>
#include <d3d11.h>

// DirectX 전역 변수
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
static ID3D11DepthStencilView* g_pDepthStencilView = nullptr;

// DirectX 초기화 함수
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

bool CreateDeviceD3D(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&g_pSwapChain,
		&g_pd3dDevice,
		&featureLevel,
		&g_pd3dDeviceContext
	);

	if (FAILED(hr)) return false;

	// 깊이 버퍼 생성
	//D3D11_TEXTURE2D_DESC descDepth = {};
	//descDepth.Width = sd.BufferDesc.Width;
	//descDepth.Height = sd.BufferDesc.Height;
	//descDepth.MipLevels = 1;
	//descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//descDepth.SampleDesc.Count = 1;
	//descDepth.SampleDesc.Quality = 0;
	//descDepth.Usage = D3D11_USAGE_DEFAULT;
	//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	//ID3D11Texture2D* pDepthStencil = nullptr;
	//g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
	//if (pDepthStencil)
	//{

	//	g_pd3dDevice->CreateDepthStencilView(pDepthStencil, nullptr, &g_pDepthStencilView);
	//	pDepthStencil->Release();
	//}
	//else
	//{
	//	AfxMessageBox(L"empty ");
	//	return false;
	//}



	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if (g_pDepthStencilView) { g_pDepthStencilView->Release(); g_pDepthStencilView = nullptr; }
}

void CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
}

void CleanupRenderTarget() {
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}



//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
// Helper to wire demo markers located in code to an interactive browser
typedef void (*ImGuiMarkerCallback)(const char* file, int line, const char* section, void* user_data);
// extern 선언만 유지 (정의 X)
//extern ImGuiMarkerCallback GImGuiMarkerCallback;
//extern void* GImGuiMarkerCallbackUserData;
ImGuiMarkerCallback GImGuiMarkerCallback = NULL;
void* GImGuiMarkerCallbackUserData = NULL;
#define IMGUI_MARKER(section)  do { if (GImGuiMarkerCallback != NULL) GImGuiMarkerCallback(__FILE__, __LINE__, section, GImGuiMarkerCallbackUserData); } while (0)

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.

	ExampleAppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have random access to the result of our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of
				// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
				// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
				// within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
				// on your side is recommended. Using ImGuiListClipper requires
				// - A) random access into your data
				// - B) items all being the  same height,
				// both of which we can handle since we have an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display
				// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
				// it possible (and would be recommended if you want to search through tens of thousands of entries).
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
			// Using a scrollbar or mouse-wheel will take away from the bottom edge.
			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}
};


#include <vector>
#include <string>
#include <memory>
#include <stdarg.h>

static void ShowExampleAppLog(bool* p_open);

class ClogView
{
public:
	ClogView() : AutoScroll(true) { Clear(); }

	void Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void AddLog(const char* fmt, ...)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void Draw(const char* title, bool* p_open = nullptr)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (clear) Clear();
			if (copy) ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}

private:
	ImGuiTextBuffer Buf;
	ImGuiTextFilter Filter;
	ImVector<int> LineOffsets;
	bool AutoScroll;
};

std::unique_ptr<ClogView> logView;

//-----------------------------------------------------------------------------
// ConimguiDlg 클래스 구현
//-----------------------------------------------------------------------------
ConimguiDlg::ConimguiDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ON_IMGUI_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ConimguiDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ConimguiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL ConimguiDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
 //GetSafeHwnd()
	if (!CreateDeviceD3D(m_hWnd)) {
		MessageBox(L"DirectX 11 초기화 실패!", L"오류", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// ImGui 초기화
	InitImGui();

	logView = std::make_unique<ClogView>();

	m_TimerID = SetTimer(UPDATE_TIMER_POP_UNIT_INIT_DLG, 16, nullptr);
	return TRUE;
}

void ConimguiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

		//// ImGui 렌더링
		//RenderImGui();

		//// 화면 갱신
		//Invalidate(FALSE);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// ✅ ImGui 초기화
void ConimguiDlg::InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetSafeHwnd());
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	ImPlot::CreateContext(); // ✅ ImPlot 초기화
	ImNodes::CreateContext(); // ✅ ImNodes 초기화
}


// ✅ ImGui 렌더링
void ConimguiDlg::RenderImGui()
{
	using namespace DirectX;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

// ✅ ImGui 메인 윈도우 크기 조정 (MFC 컨트롤 영역 피함)
	CRect rect;
	GetClientRect(&rect);
	ImGui::SetNextWindowPos(ImVec2((float)rect.right - 420.0f, 10.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

	// ✅ ImPlot 예제 그래프
	if (ImGui::Begin("ImPlot Example"))
	{
		static float xs[100], ys[100];
		for (int i = 0; i < 100; ++i)
		{
			xs[i] = i * 0.01f;
			ys[i] = (float)sin(xs[i] * 10.0f); // sin 함수 그래프
		}

		if (ImPlot::BeginPlot("Sine Wave"))
		{
			ImPlot::PlotLine("sin(x)", xs, ys, 100);
			ImPlot::EndPlot();
		}
		ImGui::End();
	}

	// ✅ ImNodes 창 크기 설정
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Node Editor"))
	{
		ImNodes::BeginNodeEditor();

		ImNodes::BeginNode(1);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text("Node A");
		ImNodes::EndNodeTitleBar();
		ImNodes::BeginInputAttribute(2);
		ImGui::Text("Input");
		ImNodes::EndInputAttribute();
		ImNodes::BeginOutputAttribute(3);
		ImGui::Text("Output");
		ImNodes::EndOutputAttribute();
		ImNodes::EndNode();

		ImNodes::EndNodeEditor();
		ImGui::End();
	}

	// ✅ 예제 로그 창 추가
	//static bool show_log = true;
	//ShowExampleAppLog(&show_log);
	 // UI 요소 추가 가능
	//ImGui::Begin("Main Window");
	static bool show_log = true;
	if (logView)
	{
		logView->Draw("Example: Log", &show_log);
	}
	//ImGui::End();



	// ✅ Overlay 윈도우 추가 (FPS, 디버그 정보)
	static bool show_overlay = true;
	if (show_overlay)
	{
		const float DISTANCE = 10.0f;
		ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - DISTANCE, DISTANCE);
		ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowBgAlpha(0.35f); // 투명도 설정

		if (ImGui::Begin("Overlay", &show_overlay, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text("Overlay Window");
			ImGui::Separator();
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			if (ImGui::Button("Close")) show_overlay = false;
			ImGui::End();
		}
	}

#if 0
	// ImGuizmo 설정
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(0, 0, (float)rect.Width(), (float)rect.Height());

	// 카메라 설정
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0, 3, -5, 1),
		DirectX::XMVectorSet(0, 0, 0, 1),
		DirectX::XMVectorSet(0, 1, 0, 0)
	);

	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(45.0f),
		(float)rect.Width() / rect.Height(),
		0.1f,
		100.0f
	);

	DirectX::XMFLOAT4X4 view, proj;
	DirectX::XMStoreFloat4x4(&view, viewMatrix);
	DirectX::XMStoreFloat4x4(&proj, projMatrix);

	static DirectX::XMFLOAT4X4 model = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
	};

	// ImGuizmo 조작
	ImGui::Begin("3D Editor");
	ImGuizmo::Manipulate(
		&view.m[0][0],
		&proj.m[0][0],
		ImGuizmo::TRANSLATE,
		ImGuizmo::LOCAL,
		&model.m[0][0]
	);
	ImGui::End();
#endif
	// ImGuizmo 활성화
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(0, 0, (float)rect.Width(), (float)rect.Height());
	ImGuizmo::Enable(true);

	// 변환 행렬 설정
	static DirectX::XMMATRIX objectMatrix = DirectX::XMMatrixIdentity();
	float matrix[16];
	DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(matrix), objectMatrix);

	// 뷰 행렬 및 프로젝션 행렬 설정
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0, 3, -5, 1),
		DirectX::XMVectorSet(0, 0, 0, 1),
		DirectX::XMVectorSet(0, 1, 0, 0)
	);
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(45.0f), 1.6f, 0.1f, 100.0f
	);

	float view[16];
	float projection[16];
	DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(view), viewMatrix);
	DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(projection), projectionMatrix);

	// ImGuizmo 조작 추가
	ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, matrix);

	static DirectX::XMFLOAT4X4 model = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
	};
	// ImGuizmo 조작
	ImGui::Begin("3D Editor");
	ImGuizmo::Manipulate(
		&view[0],
		&projection[0],
		ImGuizmo::TRANSLATE,
		ImGuizmo::LOCAL,
		&model.m[0][0]
	);
	ImGui::End();


	// 렌더링
	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	ImVec4 v4_data{ 0.45f, 0.55f, 0.60f, 1.00f };
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&v4_data);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(1, 0);
}

// ✅ ImGui 정리
void ConimguiDlg::CleanupImGui()
{
	ImNodes::DestroyContext(); // ✅ ImNodes 정리
	ImPlot::DestroyContext();  // ✅ ImPlot 정리

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
}



// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ConimguiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void ConimguiDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	RenderImGui();
	CDialogEx::OnTimer(nIDEvent);
}


void ConimguiDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (g_pSwapChain && nType != SIZE_MINIMIZED) {
		CleanupRenderTarget();
		g_pSwapChain->ResizeBuffers(0, cx, cy, DXGI_FORMAT_UNKNOWN, 0);
		CreateRenderTarget();
	}
}

void ConimguiDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	CleanupImGui();

	if (m_TimerID != 0) KillTimer(m_TimerID);
}



// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL ConimguiDlg::PreTranslateMessage(MSG* pMsg) {
	if (ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}



// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
	static ExampleAppLog log;

	// For the demo: add a debug button _BEFORE_ the normal log window contents
	// We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
	// Most of the contents of the window will be added by the log.Draw() call.
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Example: Log", p_open);
	IMGUI_MARKER("Examples/Log");
	if (ImGui::SmallButton("[Debug] Add 5 entries"))
	{
		static int counter = 0;
		const char* categories[3] = { "info", "warn", "error" };
		const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
		for (int n = 0; n < 5; n++)
		{
			const char* category = categories[counter % IM_ARRAYSIZE(categories)];
			const char* word = words[counter % IM_ARRAYSIZE(words)];
			log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
				ImGui::GetFrameCount(), category, (float)ImGui::GetTime(), word);
			counter++;
		}
	}
	ImGui::End();

	// Actually call in the regular Log helper (which will Begin() into the same window as we just did)
	log.Draw("Example: Log", p_open);
}
