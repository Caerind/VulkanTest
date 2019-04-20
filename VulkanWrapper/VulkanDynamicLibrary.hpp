#pragma once 

#include "VulkanCore.hpp"

// TODO : Try to unload to know if we need to keep it loaded everytime ?

VULKAN_NAMESPACE_BEGIN

class VulkanDynamicLibrary
{
public:
	VulkanDynamicLibrary();
	~VulkanDynamicLibrary();
	
	bool loadLibrary();
	bool isLibraryLoaded() const;
	void unloadLibrary();
	
	void* loadFunction(const char* functionName) const;
	
private:	
	#if (defined VULKAN_PLATFORM_WINDOWS)
		HMODULE mLibraryHandle;
	#elif (defined VULKAN_PLATFORM_LINUX)
		void* mLibraryHandle;
	#endif
};

VULKAN_NAMESPACE_END