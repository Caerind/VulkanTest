#ifndef NU_VULKAN_FUNCTIONS_HPP
#define NU_VULKAN_FUNCTIONS_HPP

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR // TODO : Cross platform
#include "ThirdParty/vulkan.h"

namespace nu
{

#define NU_EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;

#include "ListOfVulkanFunctions.inl"

} // namespace nu

#endif // NU_VULKAN_FUNCTIONS_HPP
