#ifndef NU_WINDOW_HPP
#define NU_WINDOW_HPP

#ifdef _WIN32
#include <Windows.h>
#endif

#include "Cookbook/SampleBase.hpp"

namespace nu
{

class Window
{
	public:
		Window(const char* windowTitle, int x, int y, int width, int height, SampleBase& sample);
		~Window();

		void render();

	private:
		Vulkan::WindowParameters mWindowParams;
		SampleBase& mSample;
		bool mCreated;
};

} // namespace nu

#endif // NU_WINDOW_HPP