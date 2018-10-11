#include "1 - Vertex Diffuse Lightning\VertexDiffuseLightning.hpp"
#include "2 - Fragment Specular Lightning\FragmentSpecularLightning.hpp"
#include "3 - Normal Mapped Geometry\NormalMappedGeometry.hpp"
#include "4 - Reflective and Refractive Geometry Using Cubemaps\ReflectiveAndRefractiveGeometryUsingCubemaps.hpp"

int main() 
{
	printf("1 - Vertex Diffuse Lightning\n");
	{
		nu::Window window("1 - Vertex Diffuse Lightning", 0, 0, 1280, 920);

		VertexDiffuseLightning sample;
		sample.initialize(window.getParameters());

		while (window.isOpen())
		{
			nu::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == nu::EventType::Close)
				{
					window.close();
				}
				if (event.type == nu::EventType::MouseMove)
				{
					if (sample.isReady())
					{
						sample.mouseMove(static_cast<int>(event.param1), static_cast<int>(event.param2));
						sample.onMouseEvent();
					}
				}
				if (event.type == nu::EventType::MouseClick)
				{
					if (sample.isReady())
					{
						sample.mouseClick(static_cast<size_t>(event.param1), event.param2 > 0);
						sample.onMouseEvent();
					}
				}
			}

			if (sample.isReady())
			{
				sample.updateTime();
				sample.draw();
				sample.mouseReset();
			}
		}

		sample.wait();
	}
	printf("\n\n");

	nu::Vulkan::ObjectTracker::checkForLeaks();

	printf("2 - Fragment Specular Lightning\n");
	{
		nu::Window window("2 - Fragment Specular Lightning", 0, 0, 1280, 920);

		FragmentSpecularLightning sample;
		sample.initialize(window.getParameters());

		while (window.isOpen())
		{
			nu::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == nu::EventType::Close)
				{
					window.close();
				}
				if (event.type == nu::EventType::MouseMove)
				{
					if (sample.isReady())
					{
						sample.mouseMove(static_cast<int>(event.param1), static_cast<int>(event.param2));
						sample.onMouseEvent();
					}
				}
				if (event.type == nu::EventType::MouseClick)
				{
					if (sample.isReady())
					{
						sample.mouseClick(static_cast<size_t>(event.param1), event.param2 > 0);
						sample.onMouseEvent();
					}
				}
			}

			if (sample.isReady())
			{
				sample.updateTime();
				sample.draw();
				sample.mouseReset();
			}
		}

		sample.wait();
	}
	printf("\n\n");

	nu::Vulkan::ObjectTracker::checkForLeaks();

	printf("3 - Normal Mapped Geometry\n");
	{
		nu::Window window("3 - Normal Mapped Geometry", 0, 0, 1280, 920);

		NormalMappedGeometry sample;
		sample.initialize(window.getParameters());

		while (window.isOpen())
		{
			nu::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == nu::EventType::Close)
				{
					window.close();
				}
				if (event.type == nu::EventType::MouseMove)
				{
					if (sample.isReady())
					{
						sample.mouseMove(static_cast<int>(event.param1), static_cast<int>(event.param2));
						sample.onMouseEvent();
					}
				}
				if (event.type == nu::EventType::MouseClick)
				{
					if (sample.isReady())
					{
						sample.mouseClick(static_cast<size_t>(event.param1), event.param2 > 0);
						sample.onMouseEvent();
					}
				}
			}

			if (sample.isReady())
			{
				sample.updateTime();
				sample.draw();
				sample.mouseReset();
			}
		}

		sample.wait();
	}
	printf("\n\n");

	nu::Vulkan::ObjectTracker::checkForLeaks();

	printf("4 - Reflective and Refractive Geometry Using Cubemaps\n");
	{
		nu::Window window("4 - Reflective and Refractive Geometry Using Cubemaps", 0, 0, 1280, 920);

		ReflectiveAndRefractiveGeometryUsingCubemaps sample;
		sample.initialize(window.getParameters());

		while (window.isOpen())
		{
			nu::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == nu::EventType::Close)
				{
					window.close();
				}
				if (event.type == nu::EventType::MouseMove)
				{
					if (sample.isReady())
					{
						sample.mouseMove(static_cast<int>(event.param1), static_cast<int>(event.param2));
						sample.onMouseEvent();
					}
				}
				if (event.type == nu::EventType::MouseClick)
				{
					if (sample.isReady())
					{
						sample.mouseClick(static_cast<size_t>(event.param1), event.param2 > 0);
						sample.onMouseEvent();
					}
				}
			}

			if (sample.isReady())
			{
				sample.updateTime();
				sample.draw();
				sample.mouseReset();
			}
		}

		sample.wait();
	}
	printf("\n\n");

	nu::Vulkan::ObjectTracker::checkForLeaks();

	system("pause");
	return 0;
}
