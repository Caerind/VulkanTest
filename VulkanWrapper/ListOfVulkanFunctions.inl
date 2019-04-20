#if (defined VULKAN_LINKING_DYNAMIC)

//////////////////////////////////////////////////////////
//                                                      //
//                 EXPORTED                             //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef VULKAN_EXPORTED_FUNCTION
#define VULKAN_EXPORTED_FUNCTION(function)
#endif

VULKAN_EXPORTED_FUNCTION(vkGetInstanceProcAddr)

#undef VULKAN_EXPORTED_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//                 GLOBAL LEVEL                         //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef VULKAN_GLOBAL_LEVEL_FUNCTION
#define VULKAN_GLOBAL_LEVEL_FUNCTION(function)
#endif

VULKAN_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceExtensionProperties)
VULKAN_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceLayerProperties)
VULKAN_GLOBAL_LEVEL_FUNCTION(vkCreateInstance)

#undef VULKAN_GLOBAL_LEVEL_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//                INSTANCE LEVEL                        //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef VULKAN_INSTANCE_LEVEL_FUNCTION
#define VULKAN_INSTANCE_LEVEL_FUNCTION(function)
#endif

VULKAN_INSTANCE_LEVEL_FUNCTION(vkEnumeratePhysicalDevices)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkEnumerateDeviceExtensionProperties)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceFeatures)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceProperties)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceMemoryProperties)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceFormatProperties)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkCreateDevice)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkGetDeviceProcAddr)
VULKAN_INSTANCE_LEVEL_FUNCTION(vkDestroyInstance)

#undef VULKAN_INSTANCE_LEVEL_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//           INSTANCE LEVEL FROM EXTENSION              //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION
#define VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(function, extension)
#endif

VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkCreateDebugReportCallbackEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkDestroyDebugReportCallbackEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkDebugReportMessageEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)

VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkCreateDebugUtilsMessengerEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkDestroyDebugUtilsMessengerEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)

// TODO : Only if OPTION_KHR ?
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceSupportKHR, VK_KHR_SURFACE_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceFormatsKHR, VK_KHR_SURFACE_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfacePresentModesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkDestroySurfaceKHR, VK_KHR_SURFACE_EXTENSION_NAME)
#if defined(VK_USE_PLATFORM_WIN32_KHR)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkCreateWin32SurfaceKHR, VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
#elif defined(VK_USE_PLATFORM_XCB_KHR)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkCreateXcbSurfaceKHR, VK_KHR_XLIB_SURFACE_EXTENSION_NAME)
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(vkCreateXlibSurfaceKHR, VK_KHR_XCB_SURFACE_EXTENSION_NAME)
#endif

#undef VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION

//////////////////////////////////////////////////////////
//                                                      //
//                  DEVICE LEVEL                        //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef VULKAN_DEVICE_LEVEL_FUNCTION
#define VULKAN_DEVICE_LEVEL_FUNCTION(function)
#endif

VULKAN_DEVICE_LEVEL_FUNCTION(vkGetDeviceQueue)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDeviceWaitIdle)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyDevice)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkGetBufferMemoryRequirements)
VULKAN_DEVICE_LEVEL_FUNCTION(vkAllocateMemory)
VULKAN_DEVICE_LEVEL_FUNCTION(vkBindBufferMemory)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdPipelineBarrier)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateImage)
VULKAN_DEVICE_LEVEL_FUNCTION(vkGetImageMemoryRequirements)
VULKAN_DEVICE_LEVEL_FUNCTION(vkBindImageMemory)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateImageView)
VULKAN_DEVICE_LEVEL_FUNCTION(vkMapMemory)
VULKAN_DEVICE_LEVEL_FUNCTION(vkFlushMappedMemoryRanges)
VULKAN_DEVICE_LEVEL_FUNCTION(vkUnmapMemory)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdCopyBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdCopyBufferToImage)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdCopyImageToBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkBeginCommandBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkEndCommandBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkQueueSubmit)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyImageView)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyImage)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkFreeMemory)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateCommandPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkAllocateCommandBuffers)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateSemaphore)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateFence)
VULKAN_DEVICE_LEVEL_FUNCTION(vkWaitForFences)
VULKAN_DEVICE_LEVEL_FUNCTION(vkResetFences)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyFence)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroySemaphore)
VULKAN_DEVICE_LEVEL_FUNCTION(vkResetCommandBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkFreeCommandBuffers)
VULKAN_DEVICE_LEVEL_FUNCTION(vkResetCommandPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyCommandPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateBufferView)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyBufferView)
VULKAN_DEVICE_LEVEL_FUNCTION(vkQueueWaitIdle)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateSampler)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateDescriptorSetLayout)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateDescriptorPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkAllocateDescriptorSets)
VULKAN_DEVICE_LEVEL_FUNCTION(vkUpdateDescriptorSets)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdBindDescriptorSets)
VULKAN_DEVICE_LEVEL_FUNCTION(vkFreeDescriptorSets)
VULKAN_DEVICE_LEVEL_FUNCTION(vkResetDescriptorPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyDescriptorPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyDescriptorSetLayout)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroySampler)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateRenderPass)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateFramebuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyFramebuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyRenderPass)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdBeginRenderPass)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdNextSubpass)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdEndRenderPass)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreatePipelineCache)
VULKAN_DEVICE_LEVEL_FUNCTION(vkGetPipelineCacheData)
VULKAN_DEVICE_LEVEL_FUNCTION(vkMergePipelineCaches)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyPipelineCache)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateGraphicsPipelines)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateComputePipelines)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyPipeline)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyEvent)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyQueryPool)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreateShaderModule)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyShaderModule)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCreatePipelineLayout)
VULKAN_DEVICE_LEVEL_FUNCTION(vkDestroyPipelineLayout)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdBindPipeline)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdSetViewport)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdSetScissor)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdBindVertexBuffers)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdDraw)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdDrawIndexed)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdDispatch)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdCopyImage)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdPushConstants)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdClearColorImage)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdClearDepthStencilImage)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdBindIndexBuffer)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdSetLineWidth)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdSetDepthBias)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdSetBlendConstants)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdExecuteCommands)
VULKAN_DEVICE_LEVEL_FUNCTION(vkCmdClearAttachments)

#undef VULKAN_DEVICE_LEVEL_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//             DEVICE LEVEL FROM EXTENSION              //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION
#define VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(function, extension)
#endif

VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(vkCreateSwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(vkGetSwapchainImagesKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(vkAcquireNextImageKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(vkQueuePresentKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(vkDestroySwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)

#undef VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION

#endif // VULKAN_LINKING_DYNAMIC