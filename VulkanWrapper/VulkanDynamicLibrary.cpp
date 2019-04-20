#include "VulkanDynamicLibrary.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanDynamicLibrary::VulkanDynamicLibrary()
	: mLibraryHandle(nullptr)
{
}

VulkanDynamicLibrary::~VulkanDynamicLibrary()
{
	if (isLibraryLoaded())
	{
		unloadLibrary();
	}
}

bool VulkanDynamicLibrary::loadLibrary()
{
	#if (defined VULKAN_PLATFORM_WINDOWS)
		mLibraryHandle = LoadLibrary("vulkan-1.dll");
	#elif (defined VULKAN_PLATFORM_LINUX)
		mLibraryHandle = dlopen("libvulkan.so.1", RTLD_NOW);
	#endif
	
	return mLibraryHandle != nullptr;
}

bool VulkanDynamicLibrary::isLibraryLoaded() const
{
	return mLibraryHandle != nullptr;
}

void VulkanDynamicLibrary::unloadLibrary()
{
	if (mLibraryHandle != nullptr)
	{
		#if (defined VULKAN_PLATFORM_WINDOWS)
			FreeLibrary(mLibraryHandle);
		#elif (defined VULKAN_PLATFORM_LINUX)
			dlclose(mLibraryHandle);
		#endif
		
		mLibraryHandle = nullptr;
	}
}

void* VulkanDynamicLibrary::loadFunction(const char* functionName) const
{
	if (!isLibraryLoaded())
	{
		return nullptr;
	}
	
	#if (defined VULKAN_PLATFORM_WINDOWS)
		return GetProcAddress(mLibraryHandle, functionName);
	#elif (defined VULLAN_PLATFORM_LINUX)
		return dlsym(mLibraryHandle, functionName);
	#endif
}

VULKAN_NAMESPACE_END