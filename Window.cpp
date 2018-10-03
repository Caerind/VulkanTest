#include "Window.hpp"

#include "VulkanFunctions.hpp"

namespace nu 
{

#ifdef VK_USE_PLATFORM_WIN32_KHR

namespace 
{
	enum UserMessage 
	{
		USER_MESSAGE_RESIZE = WM_USER + 1,
		USER_MESSAGE_QUIT,
		USER_MESSAGE_MOUSE_CLICK,
		USER_MESSAGE_MOUSE_MOVE,
		USER_MESSAGE_MOUSE_WHEEL
	};
}

LRESULT CALLBACK WindowProcedure( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	switch (message) 
	{
		case WM_LBUTTONDOWN:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 1);
			break;
		case WM_LBUTTONUP:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 0);
			break;
		case WM_RBUTTONDOWN:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 1);
			break;
		case WM_RBUTTONUP:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 0);
			break;
		case WM_MOUSEMOVE:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEWHEEL:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_WHEEL, HIWORD(wParam), 0);
			break;
		case WM_SIZE:
		case WM_EXITSIZEMOVE:
			PostMessage(hWnd, USER_MESSAGE_RESIZE, wParam, lParam);
			break;
		case WM_KEYDOWN:
			if (VK_ESCAPE == wParam) 
			{
				PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
			}
			break;
		case WM_CLOSE:
			PostMessage( hWnd, USER_MESSAGE_QUIT, wParam, lParam);
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam);
	}
	return 0;
}

Window::Window(const char* windowTitle, int x, int y, int width, int height, SampleBase& sample) 
	: mWindowParams()
	, mSample(sample)
	, mCreated(false) 
{
	mWindowParams.hinstance = GetModuleHandle(nullptr);

	WNDCLASSEX windowClass = {
		sizeof( WNDCLASSEX ),             // UINT         cbSize
										/* Win 3.x */
		CS_HREDRAW | CS_VREDRAW,          // UINT         style
		WindowProcedure,                  // WNDPROC      lpfnWndProc
		0,                                // int          cbClsExtra
		0,                                // int          cbWndExtra
		mWindowParams.hinstance,          // HINSTANCE    hInstance
		nullptr,                          // HICON        hIcon
		LoadCursor( nullptr, IDC_ARROW ), // HCURSOR      hCursor
		(HBRUSH)(COLOR_WINDOW + 1),       // HBRUSH       hbrBackground
		nullptr,                          // LPCSTR       lpszMenuName
		L"VulkanCookbook",                // LPCSTR       lpszClassName
										/* Win 4.0 */
		nullptr                           // HICON        hIconSm
	};

	if (!RegisterClassEx(&windowClass))
	{
		return;
	}

	// TODO : Use Window Title Parameter

	mWindowParams.hwnd = CreateWindow(L"VulkanCookbook", L"WindowTitle", WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, mWindowParams.hinstance, nullptr);
	if (!mWindowParams.hwnd) 
	{
		return;
	}

	mCreated = true;
}

Window::~Window() 
{
	if (mWindowParams.hwnd)
	{
		DestroyWindow(mWindowParams.hwnd);
	}

	if (mWindowParams.hinstance)
	{
		UnregisterClass(L"VulkanCookbook", mWindowParams.hinstance);
	}
}

void Window::render() 
{
	if (mCreated && mSample.initialize(mWindowParams)) 
	{
		ShowWindow(mWindowParams.hwnd, SW_SHOWNORMAL);
		UpdateWindow(mWindowParams.hwnd);

		MSG message;
		bool loop = true;
		while (loop) 
		{
			if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
			{
				switch (message.message) 
				{
					case USER_MESSAGE_MOUSE_CLICK:
						mSample.mouseClick(static_cast<size_t>(message.wParam), message.lParam > 0);
						break;
					case USER_MESSAGE_MOUSE_MOVE:
						mSample.mouseMove(static_cast<int>(message.wParam), static_cast<int>(message.lParam));
						break;
					case USER_MESSAGE_MOUSE_WHEEL:
						mSample.mouseWheel(static_cast<short>(message.wParam) * 0.002f);
						break;
					case USER_MESSAGE_RESIZE:
						// TODO : Resize
						//if( !Sample.Resize() ) {
						//  loop = false;
						//}
						break;
					case USER_MESSAGE_QUIT:
						loop = false;
						break;
				}
				TranslateMessage(&message);
				DispatchMessage(&message);
			} 
			else 
			{
				if (mSample.isReady()) 
				{
					mSample.updateTime();
					mSample.draw();
					mSample.mouseReset();
				}
			}
		}
	}

	mSample.deinitialize();
}

#endif // VK_USE_PLATFORM_WIN32_KHR

} // namespace nu
