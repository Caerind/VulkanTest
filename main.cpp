#include "VulkanLoader.hpp"
#include "VulkanSmartObjects.hpp"
#include "Window.hpp"
#include "Vulkan.hpp"

#include "Camera.hpp"

#include <cstdio>
#include <cstdlib>
#include <vector>

int main()
{
	{ // To visualize that the destruction of every Vulkan object is fine 

		nu::Window window;
		if (!window.create("Numea", 0, 0, 800, 600))
		{
			printf("Could not create window\n");
			system("pause");
			return EXIT_FAILURE;
		}

		nu::Vulkan vulkan;
		if (!vulkan.initialize(window.getParameters()))
		{
			printf("Could not initialize Vulkan\n");
			system("pause");
			return EXIT_FAILURE;
		}

		nu::Camera camera;
		camera.perspective(50.0f, 800.0f / 600.0f, 0.01f, 1000.0f);
		camera.lookAt({ -100.0f, 100.0f, -100.0f }, nu::Vector3f::zero);

		nu::Matrix4f modelMatrix(nu::Matrix4f::identity);

		while (window.isOpen())
		{
			nu::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == nu::EventType::Close)
				{
					window.close();
				}
				if (event.type == nu::EventType::Resize)
				{
					if (!vulkan.resize())
					{
						printf("Cant resize vulkan\n");
						window.close();
					}
				}
			}

			if (vulkan.isReady())
			{
				vulkan.updateTime();

				vulkan.updateMatrices(modelMatrix, camera.getViewMatrix(), camera.getProjectionMatrix());

				if (!vulkan.draw())
				{
					return false;
				}
			}
		}

		vulkan.deinitialize();
	}

	system("pause");
	return EXIT_SUCCESS;
}