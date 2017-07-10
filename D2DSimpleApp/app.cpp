#include "app.h"

class DPIScale
{
	static float scaleX;
	static float scaleY;

public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

App::App() 
	: m_hwnd(NULL)
	, m_pDirect2dFactory(NULL)
	, m_pRenderTarget(NULL)
	, m_pLightSlateGrayBrush(NULL)
	, m_pCornflowerBlueBrush(NULL)
	, m_pBlackBrush(NULL)
	, m_ptMouse(D2D1::Point2F())
	, m_ellipse(D2D1::Ellipse(D2D1::Point2F(50.f, 50.f), 30.f, 30.f))
{
}

App::~App()
{
	SafeRelease(&m_pDirect2dFactory);
	DiscardDeviceResources();
}

void App::RunMessageLoop()
{
	MSG msg;
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT App::Initialize()
{
	HRESULT hr;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		WNDCLASS wc = {};

		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DDemoApp";

		RegisterClassEx(&wcex);

		// Because the CreateWindow function takes its size in pixels,
		// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;

		// The factory returns the current system DPI. This is also the value it will use
		// to create its own windows.
		m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

		// Create the window.
		m_hwnd = CreateWindow(
			L"D2DDemoApp",
			L"Direct2D Demo App",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(600.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(600.f * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
		);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}

HRESULT App::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	return hr;
}

HRESULT App::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
		);

		if (SUCCEEDED(hr))
		{
			/// Create a gray, blue, and black brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray),
				&m_pLightSlateGrayBrush
			);

			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
				&m_pCornflowerBlueBrush
			);

			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pBlackBrush
			);
		}
	}

	return hr;
}

void App::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
	SafeRelease(&m_pBlackBrush);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT) lParam;
		App* pApp = (App*) pcs->lpCreateParams;
	
		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pApp)
		);
	
		DPIScale::Initialize(pApp->getDirect2DFactory());
	
		result = 1;
	}
	else
	{
		App* pApp = reinterpret_cast<App*>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));
	
		bool wasHandled = false;
	
		if (pApp)
		{
			switch (uMsg)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pApp->OnResize(width, height);
			}

			result = 0;
			wasHandled = true;
			break;
	
			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}

			result = 0;
			wasHandled = true;
			break;
	
			case WM_PAINT:
			{
				pApp->OnRender();
				ValidateRect(hwnd, NULL);
			}
			result = 0;
			wasHandled = true;
			break;
	
			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			
			case WM_LBUTTONDOWN:
			{
				pApp->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			}
			break;

			case WM_LBUTTONUP:
			{
				pApp->OnLButtonUp();
			}
			break;

			case WM_MOUSEMOVE:
			{
				pApp->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			}
			break;
			} /// end switch
		}
	
		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	
	return result;
}

void App::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	SetCapture(m_hwnd);

	// Remove comment to support ellipse squashing sample
	//m_ellipse.point = m_ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
	//m_ellipse.radiusX = m_ellipse.radiusY = 1.0f;

	InvalidateRect(m_hwnd, NULL, false);
}

void App::OnLButtonUp()
{
	ReleaseCapture();
}

void App::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	D2D1_POINT_2F cursor = DPIScale::PixelsToDips(pixelX, pixelY);

	// Remove comment to support ellipse squashing sample
	//const float width = (cursor.x - m_ptMouse.x) / 2;
	//const float height = (cursor.y - m_ptMouse.y) / 2;
	//const float x1 = width + m_ptMouse.x;
	//const float y1 =  height + m_ptMouse.y;

	//m_ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

	InvalidateRect(m_hwnd, NULL, false);
}

void App::DrawGrid(INT grid_width, INT grid_height)
{
	D2D1_SIZE_F screen_sz = m_pRenderTarget->GetSize();

	/// Draw a grid background
	INT width = static_cast<INT>(screen_sz.width);
	INT height = static_cast<INT>(screen_sz.height);
	constexpr FLOAT stroke_width{ 0.5 };

	for (INT x = 0; x < width; x += grid_width)
	{
		m_pRenderTarget->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(x), 0.f)
			, D2D1::Point2F(static_cast<FLOAT>(x), screen_sz.height)
			, m_pLightSlateGrayBrush
			, stroke_width
		);
	}

	for (INT y = 0; y < height; y += grid_height)
	{
		m_pRenderTarget->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(y))
			, D2D1::Point2F(screen_sz.width, static_cast<FLOAT>(y))
			, m_pLightSlateGrayBrush
			, stroke_width
		);
	}
}

void App::DrawClockHand(D2D1_ELLIPSE& ellipse, float length, float angle, float stroke_width)
{
	D2D_POINT_2F clock_hand = D2D1::Point2F(
		ellipse.point.x,
		ellipse.point.y - (ellipse.radiusY * length)
	);

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, ellipse.point));

	m_pRenderTarget->DrawLine(ellipse.point, clock_hand, m_pBlackBrush, stroke_width);

	m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
}

HRESULT App::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		DrawGrid(20, 20);

		D2D1_SIZE_F screen_sz = m_pRenderTarget->GetSize();
		const FLOAT half_w{ screen_sz.width / 2 };
		const FLOAT half_h{ screen_sz.height / 2 };

		const FLOAT box_sz{ 75 };

		D2D1_RECT_F rect_2 = D2D1::RectF(
			  half_w - box_sz
			, half_h - box_sz
			, half_w + box_sz
			, half_h + box_sz
		);

		D2D1_ELLIPSE ellipse = D2D1::Ellipse(
			  D2D1::Point2F(half_w, half_h)
			, 50.f
			, 50.f
		);

		/// Draw body
		m_pRenderTarget->FillRectangle(&rect_2, m_pLightSlateGrayBrush);
		m_pRenderTarget->DrawRectangle(&rect_2, m_pBlackBrush);

		/// Draw face
		m_pRenderTarget->FillEllipse(ellipse, m_pCornflowerBlueBrush);
		m_pRenderTarget->DrawEllipse(ellipse, m_pBlackBrush);

		/// Draw hands
		SYSTEMTIME time;
		GetLocalTime(&time);
		const FLOAT hour_angle = (360.f / 12) * time.wHour + (time.wMinute * 0.5f);
		const FLOAT min_angle = (360.f / 60) * time.wMinute;

		DrawClockHand(ellipse, 0.4f, hour_angle, 3.5f);
		DrawClockHand(ellipse, 0.7f, min_angle, 3.5f);

		/// Draw movable circle
		m_pRenderTarget->FillEllipse(m_ellipse, m_pBlackBrush);

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void App::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}