#include "VulkanLoader.hpp"
#include "VulkanSmartObjects.hpp"

#include <cstdio>
#include <cstdlib>
#include <vector>

int main()
{
	if (!nu::VulkanLoader::load())
	{
		printf("Vulkan not loaded\n");
		system("pause");
		nu::VulkanLoader::unload();
		return EXIT_FAILURE;
	}
	printf("Vulkan loaded !\n");
	system("pause");
	nu::VulkanLoader::unload();
	return EXIT_SUCCESS;
}