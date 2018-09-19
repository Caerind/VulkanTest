#ifndef NU_WINDOW_HPP
#define NU_WINDOW_HPP

#include "System/String.hpp"
#include "WindowParameters.hpp"
#include "VulkanFunctions.hpp"
#include "Event.hpp"

namespace nu
{

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

		const WindowParameters& getParameters() const;

	private:
		WindowParameters mParameters;
		bool mOpen;
};

} // namespace nu

#endif // NU_WINDOW_HPP

