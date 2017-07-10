#include "headers.h"
#include "MyEllipse.h"
#include <iostream>
#include <list>
#include <memory>

#pragma once

/// - Declare function for releasing interfaces
/// - Declare macro for error handling
/// - Declare macro for retrieving the module's base address
template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		*ppInterfaceToRelease = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!b) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif // DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Declare methods for initializing the class, creating and discarding resources,
// handling the message loop, and the windows procedure.
class App
{
public:
	App();
	~App();

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();

	// Process and dispatch messages
	void RunMessageLoop();

	// Draw content
	HRESULT OnRender();

	// Resize the render target
	void OnResize(UINT width, UINT height);

	ID2D1Factory* getDirect2DFactory() const { return m_pDirect2dFactory; }

	void OnLButtonDown(INT pixelX, INT pixelY, DWORD flags);
	void OnLButtonUp();
	void OnMiddleButtonDown(DWORD flags);
	void OnMouseMove(INT pixelX, INT pixelY, DWORD flags);
private:
	enum class CursorMode { Draw, Selection, Drag, None };

	// Initialize device-independent resources
	HRESULT CreateDeviceIndependentResources();

	// Initialize device-dependent resources
	HRESULT CreateDeviceResources();

	// Release device-dependent resource.
	void DiscardDeviceResources();

	std::shared_ptr<MyEllipse> Selection();
	void ClearSelection() { m_selection = m_ellipses.end(); }
	void InsertEllipse(FLOAT dipX, FLOAT dipY);

	void DrawGrid(INT grid_width, INT grid_height);
	void DrawClock();
	void DrawClockHand(D2D1_ELLIPSE& ellipse, FLOAT length, FLOAT angle, FLOAT stroke_width);

	void SetMode(CursorMode m);

	HWND m_hwnd;

	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
	ID2D1SolidColorBrush* m_pBlackBrush;

	std::list<std::shared_ptr<MyEllipse>> m_ellipses;
	std::list<std::shared_ptr<MyEllipse>>::iterator m_selection;
	CursorMode m_mode;
	D2D1_POINT_2F m_ptMouse;
	HCURSOR h_cursor;
};