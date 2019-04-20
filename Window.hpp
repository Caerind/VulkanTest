#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include "VulkanWrapper/VulkanSurface.hpp"

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

		const VulkanWindowParameters& getParameters() const;

	private:
		VulkanWindowParameters mParameters;
		bool mOpen;
};

} // namespace nu