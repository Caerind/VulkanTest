#include "1 - Vertex Diffuse Lightning\VertexDiffuseLightning.hpp"
#include "2 - Fragment Specular Lightning\FragmentSpecularLightning.hpp"
#include "3 - Normal Mapped Geometry\NormalMappedGeometry.hpp"
#include "4 - Reflective and Refractive Geometry Using Cubemaps\ReflectiveAndRefractiveGeometryUsingCubemaps.hpp"
#include "5 - Adding Shadows\AddingShadows.hpp"
#include "6 - Drawing Skybox\DrawingSkybox.hpp"
#include "7 - Drawing Billboards Using Geometry Shaders\DrawingBillboardsUsingGeometryShaders.hpp"
#include "8 - Drawing Particles Using Compute And Graphics Pipelines\DrawingParticlesUsingComputeAndGraphicsPipelines.hpp"
#include "9 - Rendering Tesselated Terrain\RenderingTesselatedTerrain.hpp"
#include "10 - Postprocessing\Postprocessing.hpp"

int main() 
{
	printf("1 - Vertex Diffuse Lightning\n");
	{
		nu::Window window("1 - Vertex Diffuse Lightning", 0, 0, 1280, 920);

		VertexDiffuseLightning sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("2 - Fragment Specular Lightning\n");
	{
		nu::Window window("2 - Fragment Specular Lightning", 0, 0, 1280, 920);

		FragmentSpecularLightning sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("3 - Normal Mapped Geometry\n");
	{
		nu::Window window("3 - Normal Mapped Geometry", 0, 0, 1280, 920);

		NormalMappedGeometry sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("4 - Reflective and Refractive Geometry Using Cubemaps\n");
	{
		nu::Window window("4 - Reflective and Refractive Geometry Using Cubemaps", 0, 0, 1280, 920);

		ReflectiveAndRefractiveGeometryUsingCubemaps sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	/*
	printf("5 - Adding Shadows\n");
	{
		nu::Window window("5 - Adding Shadows", 0, 0, 1280, 920);

		AddingShadows sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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
	*/

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("6 - Drawing Skybox\n");
	{
		nu::Window window("6 - Drawing Skybox", 0, 0, 1280, 920);

		DrawingSkybox sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("7 - Drawing Billboards Using Geometry Shaders\n");
	{
		nu::Window window("7 - Drawing Billboards Using Geometry Shaders", 0, 0, 1280, 920);

		DrawingBillboardsUsingGeometryShaders sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	/*
	printf("8 - Drawing Particles Using Compute And Graphics Pipelines\n");
	{
		nu::Window window("8 - Drawing Particles Using Compute And Graphics Pipelines", 0, 0, 1280, 920);

		DrawingParticlesUsingComputeAndGraphicsPipelines sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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
	*/

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("9 - Rendering Tesselated Terrain\n");
	{
		nu::Window window("9 - Rendering Tesselated Terrain", 0, 0, 1280, 920);

		RenderingTesselatedTerrain sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	printf("10 - Postprocessing\n");
	{
		nu::Window window("10 - Postprocessing", 0, 0, 1280, 920);

		Postprocessing sample;
		if (!sample.initialize(window.getParameters()))
		{
			printf("Could not initialize the sample\n");
			window.close();
		}

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

	#if defined VULKAN_OPTION_OBJECTTRACKER
		if (VulkanObjectTracker::hasLeaks())
		{
			printf("\n\n%d leaks found\n\n\n", VulkanObjectTracker::getLeaksCount());
		}
	#endif

	system("pause");
	return 0;
}
