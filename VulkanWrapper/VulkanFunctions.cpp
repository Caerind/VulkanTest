#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

#if (defined VULKAN_LINKING_DYNAMIC)

#define VULKAN_EXPORTED_FUNCTION(name) PFN_##name name;
#define VULKAN_GLOBAL_LEVEL_FUNCTION(name) PFN_##name name;
#define VULKAN_INSTANCE_LEVEL_FUNCTION(name) PFN_##name name;
#define VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;
#define VULKAN_DEVICE_LEVEL_FUNCTION(name) PFN_##name name;
#define VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;

#include "ListOfVulkanFunctions.inl"

#endif

VULKAN_NAMESPACE_END