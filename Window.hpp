#ifndef NU_WINDOW_HPP
#define NU_WINDOW_HPP

#ifdef _WIN32
#include <Windows.h>
#endif

#include "VulkanSurface.hpp"

namespace nu
{

enum EventType
{
	MouseClick,
	MouseMove,
	MouseWheel,
	Resize,
	Close
};

struct Event
{
	EventType type;
	int param1;
	int param2;
};

class Window
{
	public:
		Window();
		Window(const std::string& windowTitle, int x, int y, int width, int height);
		~Window();

		bool create(const std::string& windowTitle, int x, int y, int width, int height);
		void close();
		bool isOpen() const;

		bool pollEvent(Event& event);

		const Vulkan::WindowParameters& getParameters() const;

	private:
		Vulkan::WindowParameters mParameters;
		bool mOpen;
};

} // namespace nu

#endif // NU_WINDOW_HPP