//////////////////////////////////////////////////////////
//                                                      //
//                 EXPORTED                             //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef NU_EXPORTED_VULKAN_FUNCTION
#define NU_EXPORTED_VULKAN_FUNCTION(function)
#endif

NU_EXPORTED_VULKAN_FUNCTION(vkGetInstanceProcAddr)

#undef NU_EXPORTED_VULKAN_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//                 GLOBAL LEVEL                         //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef NU_GLOBAL_LEVEL_VULKAN_FUNCTION
#define NU_GLOBAL_LEVEL_VULKAN_FUNCTION(function)
#endif

NU_GLOBAL_LEVEL_VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)
NU_GLOBAL_LEVEL_VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties)
NU_GLOBAL_LEVEL_VULKAN_FUNCTION(vkCreateInstance)

#undef NU_GLOBAL_LEVEL_VULKAN_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//                INSTANCE LEVEL                        //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef NU_INSTANCE_LEVEL_VULKAN_FUNCTION
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(function)
#endif

NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkEnumeratePhysicalDevices)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkEnumerateDeviceExtensionProperties)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceFeatures)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceProperties)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceMemoryProperties)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceFormatProperties)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkCreateDevice)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetDeviceProcAddr)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION(vkDestroyInstance)

#undef NU_INSTANCE_LEVEL_VULKAN_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//           INSTANCE LEVEL FROM EXTENSION              //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(function, extension)
#endif

NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceSupportKHR, VK_KHR_SURFACE_EXTENSION_NAME)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceFormatsKHR, VK_KHR_SURFACE_EXTENSION_NAME)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfacePresentModesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroySurfaceKHR, VK_KHR_SURFACE_EXTENSION_NAME)

NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateDebugReportCallbackEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroyDebugReportCallbackEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDebugReportMessageEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)


#if defined(VK_USE_PLATFORM_WIN32_KHR)
	NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateWin32SurfaceKHR, VK_KHR_WIN32_SURFACE_EXTENSION_NAME) 
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateXcbSurfaceKHR, VK_KHR_XLIB_SURFACE_EXTENSION_NAME) 
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateXlibSurfaceKHR, VK_KHR_XCB_SURFACE_EXTENSION_NAME) 
#endif

#undef NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION

//////////////////////////////////////////////////////////
//                                                      //
//                  DEVICE LEVEL                        //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef NU_DEVICE_LEVEL_VULKAN_FUNCTION
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION(function)
#endif

NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkGetDeviceQueue)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDeviceWaitIdle)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyDevice)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkGetBufferMemoryRequirements)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkAllocateMemory)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkBindBufferMemory)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdPipelineBarrier)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateImage)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkGetImageMemoryRequirements)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkBindImageMemory)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateImageView)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkMapMemory)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkFlushMappedMemoryRanges)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkUnmapMemory)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdCopyBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdCopyBufferToImage)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdCopyImageToBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkBeginCommandBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkEndCommandBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkQueueSubmit)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyImageView)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyImage)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkFreeMemory)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateCommandPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkAllocateCommandBuffers)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateSemaphore)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateFence)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkWaitForFences)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkResetFences)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyFence)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroySemaphore)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkResetCommandBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkFreeCommandBuffers)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkResetCommandPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyCommandPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateBufferView)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyBufferView)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkQueueWaitIdle)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateSampler)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateDescriptorSetLayout)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateDescriptorPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkAllocateDescriptorSets)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkUpdateDescriptorSets)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdBindDescriptorSets)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkFreeDescriptorSets)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkResetDescriptorPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyDescriptorPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyDescriptorSetLayout)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroySampler)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateRenderPass)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateFramebuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyFramebuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyRenderPass)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdBeginRenderPass)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdNextSubpass)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdEndRenderPass)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreatePipelineCache)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkGetPipelineCacheData)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkMergePipelineCaches)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyPipelineCache)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateGraphicsPipelines)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateComputePipelines)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyPipeline)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyEvent)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyQueryPool)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateShaderModule)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyShaderModule)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCreatePipelineLayout)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyPipelineLayout)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdBindPipeline)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdSetViewport)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdSetScissor)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdBindVertexBuffers)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdDraw)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdDrawIndexed)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdDispatch)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdCopyImage)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdPushConstants)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdClearColorImage)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdClearDepthStencilImage)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdBindIndexBuffer)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdSetLineWidth)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdSetDepthBias)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdSetBlendConstants)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdExecuteCommands)
NU_DEVICE_LEVEL_VULKAN_FUNCTION(vkCmdClearAttachments)

#undef NU_DEVICE_LEVEL_VULKAN_FUNCTION

//////////////////////////////////////////////////////////
//                                                      //
//             DEVICE LEVEL FROM EXTENSION              //
//                                                      //                   
//////////////////////////////////////////////////////////
#ifndef NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(function, extension)
#endif

NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateSwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetSwapchainImagesKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkAcquireNextImageKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkQueuePresentKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroySwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)

#undef NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION