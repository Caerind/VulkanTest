#include "VulkanFunctions.hpp"

namespace nu 
{
namespace Vulkan
{

#define NU_EXPORTED_VULKAN_FUNCTION(name) PFN_##name name;
#define NU_GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;

#include "ListOfVulkanFunctions.inl"

} // namespace Vulkan
} // namespace nu
