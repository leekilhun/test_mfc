// win_Imgui.cpp: MFC 다이얼로그에서 ImGuizmo 연동 구현
//

#include "pch.h"
#include "use_imguizmo.h"
#include "afxdialogex.h"

#include "win_imguizmo.h"


#define UPDATE_TIMER_POP_UNIT_IMGUI_DLG_1 3

// DirectX 전역 변수
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

// Cwin_imguizmo 대화 상자
IMPLEMENT_DYNAMIC(Cwin_imguizmo, CDialogEx)

Cwin_imguizmo::Cwin_imguizmo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMGUI_IMGUIZMO, pParent)
{
}

Cwin_imguizmo::~Cwin_imguizmo()
{
}

void Cwin_imguizmo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cwin_imguizmo, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void Cwin_imguizmo::OnDestroy()
{
	CDialogEx::OnDestroy();
	if (m_timerID != 0) KillTimer(m_timerID);
	CleanupImGui();
}

void Cwin_imguizmo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
}

void Cwin_imguizmo::OnTimer(UINT_PTR nIDEvent)
{
	RenderImGui();
	CDialogEx::OnTimer(nIDEvent);
}

BOOL Cwin_imguizmo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!CreateDeviceD3D(m_hWnd)) {
		MessageBox(L"DirectX 11 초기화 실패!", L"오류", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	InitImGui();
	m_timerID = SetTimer(UPDATE_TIMER_POP_UNIT_IMGUI_DLG_1, 16, nullptr);

	return TRUE;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL Cwin_imguizmo::PreTranslateMessage(MSG* pMsg)
{
	if (ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void Cwin_imguizmo::InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetSafeHwnd());
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

void Cwin_imguizmo::RenderImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	float identityMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	float viewMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -5, 1 };
	float projectionMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, -1, 0, 0, -0.02f, 0 };

	float gridSize = 10.0f;

	ImGuizmo::DrawGrid(viewMatrix, projectionMatrix, identityMatrix, gridSize);

	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
	ImVec4 clearColor{ 0.45f, 0.55f, 0.60f, 1.00f };
	g_pd3dDeviceContext->ClearRenderTargetView(g_pRenderTargetView, (float*)&clearColor);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(1, 0);
}

void Cwin_imguizmo::CleanupImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
}

void Cwin_imguizmo::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (g_pd3dDevice != nullptr && nType != SIZE_MINIMIZED)
	{
		if (g_pRenderTargetView) {
			g_pRenderTargetView->Release();
			g_pRenderTargetView = nullptr;
		}

		HRESULT hr = g_pSwapChain->ResizeBuffers(0, cx, cy, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr)) {
			OutputDebugString(L"[Error] DXGI SwapChain ResizeBuffers 실패\n");
			return;
		}

		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		if (FAILED(hr) || !pBackBuffer) {
			OutputDebugString(L"[Error] SwapChain GetBuffer 실패\n");
			return;
		}

		hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr)) {
			OutputDebugString(L"[Error] DirectX CreateRenderTargetView 실패\n");
			return;
		}

		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)cx, (float)cy);
	}
}

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 1280;
	sd.BufferDesc.Height = 720;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	D3D_FEATURE_LEVEL featureLevel;
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
		D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice,
		&featureLevel, &g_pd3dDeviceContext))) {
		return false;
	}

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_pRenderTargetView) {
		g_pRenderTargetView->Release();
		g_pRenderTargetView = nullptr;
	}
}

#if 0

#include "pch.h"
#include "use_imguizmo.h"
#include "afxdialogex.h"
#include "win_imguizmo.h"


//-----------------------------------------------------------------------------
// DirectX 및 ImGui 관련 헤더
//-----------------------------------------------------------------------------
#include <DirectXMath.h>
#include <d3d11.h>

// DirectX 전역 변수

// DirectX 전역 변수
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

// DirectX 초기화 함수
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 1280;
	sd.BufferDesc.Height = 720;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	D3D_FEATURE_LEVEL featureLevel;
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
		D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice,
		&featureLevel, &g_pd3dDeviceContext)))
	{
		return false;
	}

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

void CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget() {
	if (g_pRenderTargetView) { g_pRenderTargetView->Release(); g_pRenderTargetView = nullptr; }
}



//-----------------------------------------------------------------------------
// Cwin_imguizmo 대화 상자
//-----------------------------------------------------------------------------
// 
#define UPDATE_TIMER_POP_UNIT_IMGUI_DLG_1 3

IMPLEMENT_DYNAMIC(Cwin_imguizmo, CDialogEx)

Cwin_imguizmo::Cwin_imguizmo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMGUI_IMGUIZMO, pParent)
{

}

Cwin_imguizmo::~Cwin_imguizmo()
{
}

void Cwin_imguizmo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cwin_imguizmo, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Cwin_Imgui 메시지 처리기


void Cwin_imguizmo::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_timerID != 0) KillTimer(m_timerID);

	CleanupImGui();
}


void Cwin_imguizmo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void Cwin_imguizmo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	RenderImGui();

	CDialogEx::OnTimer(nIDEvent);
}


BOOL Cwin_imguizmo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (!CreateDeviceD3D(m_hWnd)) {
		MessageBox(L"DirectX 11 초기화 실패!", L"오류", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	// ImGui 초기화
	InitImGui();

	m_timerID = SetTimer(UPDATE_TIMER_POP_UNIT_IMGUI_DLG_1, 16, nullptr);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL Cwin_imguizmo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void Cwin_imguizmo::InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetSafeHwnd());
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

void Cwin_imguizmo::RenderImGui()
{
	using namespace DirectX;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGuizmo 테스트용 예제 코드
	// ImGuizmo 테스트용 예제 코드
	ImGuizmo::BeginFrame();

	// 단위 행렬 (기본 변환 행렬)
	float identityMatrix[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};

	// 카메라 뷰 행렬 (예제용, 필요에 맞게 수정)
	float viewMatrix[16] =
	{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, -5, 1  // 카메라를 원점에서 5만큼 뒤로 이동
	};

	// 투영 행렬 (예제용, 필요에 맞게 수정)
	float projectionMatrix[16] =
	{
			1, 0,  0, 0,
			0, 1,  0, 0,
			0, 0, -1, -1,
			0, 0, -0.02f, 0
	};

	// 그리드 크기 설정
	float gridSize = 10.0f;

	// 올바른 DrawGrid 함수 호출
	ImGuizmo::DrawGrid(viewMatrix, projectionMatrix, identityMatrix, gridSize);

	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
	ImVec4 v4_data{ 0.45f, 0.55f, 0.60f, 1.00f };
	g_pd3dDeviceContext->ClearRenderTargetView(g_pRenderTargetView, (float*)&v4_data);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(1, 0);
}

void Cwin_imguizmo::CleanupImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
}

void Cwin_imguizmo::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// 창 크기 변경 시 처리 (최소화 상태가 아닐 때만)
	if (g_pd3dDevice != nullptr && nType != SIZE_MINIMIZED)
	{
		// 기존 렌더 타겟 해제
		if (g_pRenderTargetView)
		{
			g_pRenderTargetView->Release();
			g_pRenderTargetView = nullptr;
		}

		// 스왑 체인 버퍼 크기 조정
		HRESULT hr = g_pSwapChain->ResizeBuffers(0, cx, cy, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr))
		{
			OutputDebugString(L"[Error] DXGI SwapChain ResizeBuffers 실패\n");
			return;
		}

		// 새 렌더 타겟 생성
		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		if (FAILED(hr) || !pBackBuffer)
		{
			OutputDebugString(L"[Error] SwapChain GetBuffer 실패\n");
			return;
		}

		hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
		pBackBuffer->Release(); // 참조 해제
		if (FAILED(hr))
		{
			OutputDebugString(L"[Error] DirectX CreateRenderTargetView 실패\n");
			return;
		}

		// 렌더 타겟을 다시 설정
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

		// ImGui 창 크기 동기화
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)cx, (float)cy);

		// 디버깅 로그 출력
		wchar_t debugMsg[256];
		swprintf(debugMsg, 256, L"[Info] 창 크기 변경됨: %d x %d\n", cx, cy);
		OutputDebugString(debugMsg);
	}
}

#endif


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
// Helper to wire demo markers located in code to an interactive browser
// 
//typedef void (*ImGuiMarkerCallback)(const char* file, int line, const char* section, void* user_data);
//ImGuiMarkerCallback GImGuiMarkerCallback = NULL;
//void* GImGuiMarkerCallbackUserData = NULL;
//#define IMGUI_MARKER(section)  do { if (GImGuiMarkerCallback != NULL) GImGuiMarkerCallback(__FILE__, __LINE__, section, GImGuiMarkerCallbackUserData); } while (0)


