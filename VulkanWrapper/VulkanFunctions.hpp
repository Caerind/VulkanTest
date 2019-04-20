#pragma once

#include "VulkanCore.hpp"

VULKAN_NAMESPACE_BEGIN

#if (defined VULKAN_LINKING_DYNAMIC)

#define VULKAN_EXPORTED_FUNCTION(name) extern PFN_##name name;
#define VULKAN_GLOBAL_LEVEL_FUNCTION(name) extern PFN_##name name;
#define VULKAN_INSTANCE_LEVEL_FUNCTION(name) extern PFN_##name name;
#define VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;
#define VULKAN_DEVICE_LEVEL_FUNCTION(name) extern PFN_##name name;
#define VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;

#include "ListOfVulkanFunctions.inl"

#endif

VULKAN_NAMESPACE_END