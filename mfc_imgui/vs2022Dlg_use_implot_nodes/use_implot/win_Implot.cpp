// win_Imgui.cpp: 구현 파일
//

#include "pch.h"
#include "use_implot.h"
#include "afxdialogex.h"
#include "win_Implot.h"


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
ImGuiMarkerCallback GImGuiMarkerCallback = NULL;
void* GImGuiMarkerCallbackUserData = NULL;
#define IMGUI_MARKER(section)  do { if (GImGuiMarkerCallback != NULL) GImGuiMarkerCallback(__FILE__, __LINE__, section, GImGuiMarkerCallbackUserData); } while (0)


//-----------------------------------------------------------------------------
// imgui example
//-----------------------------------------------------------------------------
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

// Cwin_Imgui 대화 상자

#define UPDATE_TIMER_POP_UNIT_IMGUI_DLG 2

IMPLEMENT_DYNAMIC(Cwin_Implot, CDialogEx)

Cwin_Implot::Cwin_Implot(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMGUI_WIND, pParent)
{

}

Cwin_Implot::~Cwin_Implot()
{
}

void Cwin_Implot::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cwin_Implot, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Cwin_Imgui 메시지 처리기


void Cwin_Implot::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_timerID != 0) KillTimer(m_timerID);

	CleanupImGui();
}


void Cwin_Implot::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void Cwin_Implot::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	RenderImGui();

	CDialogEx::OnTimer(nIDEvent);
}


BOOL Cwin_Implot::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (!CreateDeviceD3D(m_hWnd)) {
		MessageBox(L"DirectX 11 초기화 실패!", L"오류", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	// ImGui 초기화
	InitImGui();


	m_timerID = SetTimer(UPDATE_TIMER_POP_UNIT_IMGUI_DLG, 16, nullptr);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL Cwin_Implot::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void Cwin_Implot::InitImGui()
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

void Cwin_Implot::RenderImGui()
{
	using namespace DirectX;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	static bool show_popup = false;
	

#define ShoW_ImGui_exam
#define ShoW_ImPlot_exam

#ifdef ShoW_ImGui_exam
	//ImGui::ShowDemoWindow();


// UI 생성
	if (ImGui::Begin("Main Window"))
	{
		if (ImGui::Button("Open Popup"))
			ImGui::OpenPopup("MyPopup", ImGuiWindowFlags_AlwaysAutoResize);  // 반드시 호출

		// 팝업 처리
		// OpenPopup이 실행된 후 다음 프레임에서 BeginPopup 실행
		if (ImGui::BeginPopup("MyPopup"))
		{
			ImGui::Text("Hello! This is a popup.");
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

	}
	ImGui::End();



#else

	// ✅ 예제 로그 창 추가
	static bool show_log = true;
	ShowExampleAppLog(&show_log);
	// UI 요소 추가 가능
	ImGui::Begin("Main Window");

	ImGui::End();



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


	//stack을 share 후 사용하는 듯
	//if (ImGui::BeginPopup("Log Monitor", ImGuiWindowFlags_AlwaysAutoResize))
	//{
	//	ImGui::Text("Logging...");
	//	if (ImGui::Button("Close")) {
	//		ImGui::CloseCurrentPopup();
	//	}
	//	ImGui::EndPopup();
	//}

#endif

#ifdef ShoW_ImPlot_exam
	ImPlot::ShowDemoWindow();
#else
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
#endif
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


	// 렌더링
	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	ImVec4 v4_data{ 0.45f, 0.55f, 0.60f, 1.00f };
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&v4_data);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(1, 0);
}

void Cwin_Implot::CleanupImGui()
{
	ImNodes::DestroyContext(); // ✅ ImNodes 정리
	ImPlot::DestroyContext();  // ✅ ImPlot 정리


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
}





//-----------------------------------------------------------------------------
// implot 예제
//-----------------------------------------------------------------------------
/*

#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "implot.h"

#pragma comment(lib, "d3d11.lib")

// 전역 변수
HWND hWnd = NULL;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateRenderTarget()
{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
}

void CleanupRenderTarget()
{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

bool CreateDeviceD3D(HWND hWnd)
{
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
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

		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevelArray, 2,
																			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
				return false;

		CreateRenderTarget();
		return true;
}

void CleanupDeviceD3D()
{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "ImPlotDX11", NULL };
		RegisterClassEx(&wc);
		hWnd = CreateWindow(wc.lpszClassName, "ImPlot DX11 Demo", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, NULL, NULL, wc.hInstance, NULL);

		if (!CreateDeviceD3D(hWnd)) {
				CleanupDeviceD3D();
				UnregisterClass(wc.lpszClassName, wc.hInstance);
				return 1;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

		bool done = false;
		while (!done)
		{
				MSG msg;
				while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
						if (msg.message == WM_QUIT) done = true;
				}
				if (done) break;

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				ImPlot::ShowDemoWindow();

				ImGui::Render();
				const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
				g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
				g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				g_pSwapChain->Present(1, 0);
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		DestroyWindow(hWnd);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
				return true;
		switch (msg)
		{
		case WM_SIZE:
				if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
				{
						CleanupRenderTarget();
						g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
						CreateRenderTarget();
				}
				return 0;
		case WM_SYSCOMMAND:
				if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
				break;
		case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
}



*/





