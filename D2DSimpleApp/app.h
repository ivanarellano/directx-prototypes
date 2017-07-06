#include "headers.h"

#pragma once

/// Part 2:
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
/// Part 2

/// Part 3
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
private:
	// Initialize device-independent resources
	HRESULT CreateDeviceIndependentResources();

	// Initialize device-dependent resources
	HRESULT CreateDeviceResources();

	// Release device-dependent resource.
	void DiscardDeviceResources();

	// Draw content
	HRESULT OnRender();

	// Resize the render target
	void OnResize(UINT width, UINT height);

	// The windows procedure
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	);

	void DrawGrid(INT grid_width, INT grid_height);
	void DrawClockHand(D2D1_ELLIPSE& ellipse, float length, float angle, float stroke_width);

	HWND m_hwnd;

	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
	ID2D1SolidColorBrush* m_pBlackBrush;
};