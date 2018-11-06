#include "Window.hpp"

#include "System/String.hpp"

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

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (VK_ESCAPE == wParam) {
				PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
			}
			break;
		case WM_CLOSE:
			PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

Window::Window()
{
	mParameters.hinstance = GetModuleHandle(nullptr);

	WNDCLASSEX windowClass =
	{
		sizeof(WNDCLASSEX),                // UINT         cbSize

		/* Win 3.x */
		CS_HREDRAW | CS_VREDRAW,           // UINT         style
		WindowProcedure,                   // WNDPROC      lpfnWndProc
		0,                                 // int          cbClsExtra
		0,                                 // int          cbWndExtra
		mParameters.hinstance,             // HINSTANCE    hInstance
		nullptr,                           // HICON        hIcon
		LoadCursor(nullptr, IDC_ARROW),    // HCURSOR      hCursor
		(HBRUSH)(COLOR_WINDOW + 1),        // HBRUSH       hbrBackground
		nullptr,                           // LPCSTR       lpszMenuName
		L"NumeaEngine",                    // LPCSTR       lpszClassName

		/* Win 4.0 */
		nullptr                            // HICON        hIconSm
	};

	if (!RegisterClassEx(&windowClass))
	{
		return;
	}
}

Window::Window(const std::string& windowTitle, int x, int y, int width, int height)
	: Window()
{
	create(windowTitle, x, y, width, height);
}

Window::~Window()
{
	close();
	if (mParameters.hinstance)
	{
		UnregisterClass(L"NumeaEngine", mParameters.hinstance);
	}
}

bool Window::create(const std::string& windowTitle, int x, int y, int width, int height)
{
	std::wstring wideWindowTitle;
	toWideString(windowTitle, wideWindowTitle);

	mParameters.hwnd = CreateWindow(L"NumeaEngine", wideWindowTitle.c_str(), WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, mParameters.hinstance, nullptr);
	if (!mParameters.hwnd)
	{
		return false;
	}

	ShowWindow(mParameters.hwnd, SW_SHOWNORMAL);

	UpdateWindow(mParameters.hwnd);

	mOpen = true;

	return true;
}

void Window::close()
{
	mOpen = false;
	if (mParameters.hwnd)
	{
		DestroyWindow(mParameters.hwnd);
		mParameters.hwnd = NULL;
	}
}

bool Window::isOpen() const
{
	return mOpen;
}

bool Window::pollEvent(Event& event)
{
	MSG message;
	if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		switch (message.message)
		{
			case USER_MESSAGE_MOUSE_CLICK:
				event.type = EventType::MouseClick;
				event.param1 = (int)message.wParam;
				event.param2 = (int)(message.lParam > 0);
				break;
			case USER_MESSAGE_MOUSE_MOVE:
				event.type = EventType::MouseMove;
				event.param1 = (int)message.wParam;
				event.param2 = (int)message.lParam;
				break;
			case USER_MESSAGE_MOUSE_WHEEL:
				event.type = EventType::MouseWheel;
				event.param1 = (int)message.wParam;
				event.param2 = 0;
				break;
			case USER_MESSAGE_RESIZE:
				event.type = EventType::Resize;
				event.param1 = 0;
				event.param2 = 0;
				break;
			case USER_MESSAGE_QUIT:
				event.type = EventType::Close;
				event.param1 = 0;
				event.param2 = 0;
				break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);

		return true;
	}

	return false;
}

const Vulkan::WindowParameters& Window::getParameters() const
{
	return mParameters;
}

#endif

} // namespace nu