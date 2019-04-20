#include "VulkanDevice.hpp"

#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanLoader.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanComputePipeline.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanFence.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanImage.hpp"
#include "VulkanMemoryBlock.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanQueue.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSampler.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanShaderModule.hpp"
#include "VulkanSwapchain.hpp"

VULKAN_NAMESPACE_BEGIN

#if (defined VULKAN_MONO_DEVICE)

VulkanDevice& VulkanDevice::get()
{
	VULKAN_ASSERT(sDevice != nullptr);
	return *sDevice;
}

bool VulkanDevice::initialize(VulkanPhysicalDevice& physicalDevice, VkPhysicalDeviceFeatures* desiredFeatures, const VulkanQueueParameters& queueParameters, VulkanSurface* surface)
{
	if (sDevice != nullptr)
	{
		VULKAN_LOG_WARNING("Device already initialized. If you want to use many device, do not define VULKAN_MONO_DEVICE in VulkanConfig.hpp");
		return true;
	}
	sDevice = new VulkanDevice(physicalDevice);
	if (sDevice != nullptr)
	{
		if (!sDevice->initializeInternal(desiredFeatures, queueParameters, surface))
		{
			uninitialize();
		}
	}
	return sDevice != nullptr;
}

bool VulkanDevice::uninitialize()
{
	if (sDevice == nullptr)
	{
		VULKAN_LOG_WARNING("Device not initialized");
		return true;
	}
	
	sDevice->uninitializeInternal();
	
	delete sDevice;
	sDevice = nullptr;
	
	return true;
}

bool VulkanDevice::initialized()
{
	return sDevice != nullptr;
}

#else // (defined VULKAN_MONO_DEVICE)

VulkanDevice& VulkanDevice::get(VulkanDeviceId id)
{
	VULKAN_ASSERT(sDevices.find(id) != sDevices.end());
	VULKAN_ASSERT(sDevices[id] != nullptr);
	return *sDevices[id];
}

bool VulkanDevice::initialize(VulkanDeviceId id, VulkanPhysicalDevice& physicalDevice, VkPhysicalDeviceFeatures* desiredFeatures, const VulkanQueueParameters& queueParameters, VulkanSurface* surface)
{
	if (sDevices.find(id) != sDevices.end())
	{
		VULKAN_LOG_WARNING("Device[%d] already initialized", id);
		return true;
	}
	VulkanDevice* device = new VulkanDevice(physicalDevice);
	if (device != nullptr)
	{
		if (device->initializeInternal(desiredFeatures, queueParameters, surface))
		{
			sDevices[id] = device;
		}
		else
		{
			device->uninitializeInternal();
			delete device;
			device = nullptr;
		}
	}
	return device != nullptr;
}

bool VulkanDevice::uninitialize(VulkanDeviceId id)
{
	auto itr = sDevices.find(id);
	if (itr == sDevices.end() || itr->second == nullptr)
	{
		if (itr != sDevices.end() && itr->second == nullptr)
		{
			sDevices.erase(itr);
		}
		VULKAN_LOG_WARNING("Device[%d] not initialized", id);
		return true;
	}
	
	VulkanDevice* device = itr->second;
	
	device->uninitializeInternal();
	delete device;
	device = nullptr;
	
	sDevices.erase(itr);
	
	return true;
}

bool VulkanDevice::initialized(VulkanDeviceId id)
{
	auto itr = sDevices.find(id);
	return itr != sDevices.end() && itr->second != nullptr;
}

#endif // (defined VULKAN_MONO_DEVICE)

VulkanBufferPtr VulkanDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
	return VulkanBuffer::createBuffer(size, usage);
}

VulkanCommandPoolPtr VulkanDevice::createCommandPool(VkCommandPoolCreateFlags parameters, VulkanU32 queueFamily)
{
	return VulkanCommandPool::createCommandPool(parameters, queueFamily);
}

VulkanDescriptorPoolPtr VulkanDevice::createDescriptorPool(bool freeIndividualSets, VulkanU32 maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes)
{
	return VulkanDescriptorPool::createDescriptorPool(freeIndividualSets, maxSetsCount, descriptorTypes);
}

VulkanDescriptorSetLayoutPtr VulkanDevice::createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	return VulkanDescriptorSetLayout::createDescriptorSetLayout(bindings);
}

void VulkanDevice::updateDescriptorSets(const std::vector<VulkanImageDescriptorInfo>& imageDescriptorInfos, const std::vector<VulkanBufferDescriptorInfo>& bufferDescriptorInfos, const std::vector<VulkanTexelBufferDescriptorInfo>& texelBufferDescriptorInfos, const std::vector<VulkanCopyDescriptorInfo>& copyDescriptorInfos)
{
	std::vector<VkWriteDescriptorSet> writeDescriptors;
	writeDescriptors.reserve(imageDescriptorInfos.size() + bufferDescriptorInfos.size() + texelBufferDescriptorInfos.size());
	std::vector<VkCopyDescriptorSet> copyDescriptors;
	copyDescriptors.reserve(copyDescriptorInfos.size());

	// Image descriptors
	for (auto& imageDescriptor : imageDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			imageDescriptor.targetDescriptorSet,                                    // VkDescriptorSet                  dstSet
			imageDescriptor.targetDescriptorBinding,                                // uint32_t                         dstBinding
			imageDescriptor.targetArrayElement,                                     // uint32_t                         dstArrayElement
			static_cast<VulkanU32>(imageDescriptor.imageInfos.size()),               // uint32_t                         descriptorCount
			imageDescriptor.targetDescriptorType,                                   // VkDescriptorType                 descriptorType
			imageDescriptor.imageInfos.data(),                                      // const VkDescriptorImageInfo    * pImageInfo
			nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
			nullptr                                                                 // const VkBufferView             * pTexelBufferView
		});
	}

	// Buffer descriptors
	for (auto& bufferDescriptor : bufferDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			bufferDescriptor.targetDescriptorSet,                                   // VkDescriptorSet                  dstSet
			bufferDescriptor.targetDescriptorBinding,                               // uint32_t                         dstBinding
			bufferDescriptor.targetArrayElement,                                    // uint32_t                         dstArrayElement
			static_cast<VulkanU32>(bufferDescriptor.bufferInfos.size()),             // uint32_t                         descriptorCount
			bufferDescriptor.targetDescriptorType,                                  // VkDescriptorType                 descriptorType
			nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
			bufferDescriptor.bufferInfos.data(),                                    // const VkDescriptorBufferInfo   * pBufferInfo
			nullptr                                                                 // const VkBufferView             * pTexelBufferView
		});
	}

	// Texel buffer descriptors
	for (auto& texelBufferDescriptor : texelBufferDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			texelBufferDescriptor.targetDescriptorSet,                              // VkDescriptorSet                  dstSet
			texelBufferDescriptor.targetDescriptorBinding,                          // uint32_t                         dstBinding
			texelBufferDescriptor.targetArrayElement,                               // uint32_t                         dstArrayElement
			static_cast<VulkanU32>(texelBufferDescriptor.texelBufferViews.size()),   // uint32_t                         descriptorCount
			texelBufferDescriptor.targetDescriptorType,                             // VkDescriptorType                 descriptorType
			nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
			nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
			texelBufferDescriptor.texelBufferViews.data()                           // const VkBufferView             * pTexelBufferView
		});
	}

	// Copy descriptors
	for (auto& copyDescriptor : copyDescriptorInfos) 
	{
		copyDescriptors.push_back({
			VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,                                  // VkStructureType    sType
			nullptr,                                                                // const void       * pNext
			copyDescriptor.sourceDescriptorSet,                                    // VkDescriptorSet    srcSet
			copyDescriptor.sourceDescriptorBinding,                                // uint32_t           srcBinding
			copyDescriptor.sourceArrayElement,                                     // uint32_t           srcArrayElement
			copyDescriptor.targetDescriptorSet,                                    // VkDescriptorSet    dstSet
			copyDescriptor.targetDescriptorBinding,                                // uint32_t           dstBinding
			copyDescriptor.targetArrayElement,                                     // uint32_t           dstArrayElement
			copyDescriptor.descriptorCount                                         // uint32_t           descriptorCount
		});
	}

	vkUpdateDescriptorSets(mDevice, static_cast<VulkanU32>(writeDescriptors.size()), writeDescriptors.data(), static_cast<VulkanU32>(copyDescriptors.size()), copyDescriptors.data());
}

VulkanFencePtr VulkanDevice::createFence(bool signaled)
{
	return VulkanFence::createFence(signaled);
}

// TODO : Move implementation on static method in fence, but still allow call from here too
bool VulkanDevice::waitFences(const std::vector<VulkanFence*>& fences, bool waitForAll, VulkanU64 timeout)
{
	if (fences.size() > 0)
	{
		std::vector<VkFence> fenceHandles;
		fenceHandles.reserve(fences.size());
		for (size_t i = 0; i < fences.size(); i++)
		{
			fenceHandles.push_back(fences[i]->getHandle());
		}

		VkResult result = vkWaitForFences(mDevice, static_cast<VulkanU32>(fenceHandles.size()), fenceHandles.data(), waitForAll, timeout);
		if (result != VK_SUCCESS)
		{
			// TODO : Log more
			VULKAN_LOG_ERROR("Waiting on fence failed");
			return false;
		}
		return true;
	}
	// TODO : Warning ?
	return false;
}

// TODO : Move implementation on static method in fence, but still keep it here too
bool VulkanDevice::resetFences(const std::vector<VulkanFence*>& fences)
{
	if (fences.size() > 0)
	{
		std::vector<VkFence> fenceHandles;
		fenceHandles.reserve(fences.size());
		for (size_t i = 0; i < fences.size(); i++)
		{
			if (fences[i] != nullptr)
			{
				fenceHandles.push_back(fences[i]->getHandle());
			}
		}

		VkResult result = vkResetFences(mDevice, static_cast<VulkanU32>(fenceHandles.size()), fenceHandles.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Log more
			VULKAN_LOG_ERROR("Error occurred when tried to reset fences");
			return false;
		}
		return true;
	}
	// TODO : Warning ?
	return false;
}

VulkanImagePtr VulkanDevice::createImage(VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap)
{
	return VulkanImage::createImage(type, format, size, numMipmaps, numLayers, samples, usageScenarios, cubemap);
}

VulkanMemoryBlockPtr VulkanDevice::createMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties)
{
	return VulkanMemoryBlock::createMemoryBlock(memoryRequirements, memoryProperties);
}

VulkanPipelineCachePtr VulkanDevice::createPipelineCache(const std::vector<VulkanU8>& cacheData)
{
	return VulkanPipelineCache::createPipelineCache(cacheData);
}

VulkanPipelineLayoutPtr VulkanDevice::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges)
{
	return VulkanPipelineLayout::createPipelineLayout(descriptorSetLayouts, pushConstantRanges);
}

VulkanComputePipelinePtr VulkanDevice::createComputePipeline(VulkanShaderModule* computeShader, VulkanPipelineLayout* layout, VulkanPipelineCache* cache, VkPipelineCreateFlags additionalOptions)
{
	return VulkanComputePipeline::createComputePipeline(computeShader, layout, cache, additionalOptions);
}

VulkanGraphicsPipelinePtr VulkanDevice::initGraphicsPipeline(VulkanPipelineLayout& layout, VulkanRenderPass& renderPass, VulkanPipelineCache* cache)
{
	return VulkanGraphicsPipelinePtr(new VulkanGraphicsPipeline(*this, layout, renderPass, cache));
}

VulkanRenderPassPtr VulkanDevice::initRenderPass()
{
	return VulkanRenderPassPtr(new VulkanRenderPass());
}

VulkanSamplerPtr VulkanDevice::createSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, VulkanF32 lodBias, VulkanF32 minLod, VulkanF32 maxLod, bool anisotropyEnable, VulkanF32 maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
{
	return VulkanSampler::createSampler(magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, minLod, maxLod, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, borderColor, unnormalizedCoords);
}

VulkanSemaphorePtr VulkanDevice::createSemaphore()
{
	return VulkanSemaphore::createSemaphore();
}

VulkanShaderModulePtr VulkanDevice::initShaderModule()
{
	return VulkanShaderModulePtr(new VulkanShaderModule());
}

VulkanSwapchainPtr VulkanDevice::createSwapchain(VulkanSurface& surface, std::vector<FrameResources>& framesResources)
{
	return VulkanSwapchain::createSwapchain(*this, surface, framesResources);
}

bool VulkanDevice::waitForAllSubmittedCommands()
{
	VkResult result = vkDeviceWaitIdle(mDevice);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Waiting on a device failed");
		return false;
	}
	return true;
}

bool VulkanDevice::isInitialized() const
{
	return mDevice != VK_NULL_HANDLE;
}

const VkDevice& VulkanDevice::getHandle() const
{
	return mDevice;
}

const VulkanPhysicalDevice& VulkanDevice::getPhysicalDevice() const
{
	return mPhysicalDevice;
}

const VkPhysicalDevice& VulkanDevice::getPhysicalDeviceHandle() const
{
	return mPhysicalDevice.getHandle();
}

const std::vector<VkExtensionProperties>& VulkanDevice::getAvailableExtensions() const
{
	return mPhysicalDevice.getAvailableExtensions();	
}

const VkPhysicalDeviceFeatures& VulkanDevice::getSupportedFeatures() const
{
	return mPhysicalDevice.getSupportedFeatures();
}

const VkPhysicalDeviceProperties& VulkanDevice::getProperties() const
{
	return mPhysicalDevice.getProperties();	
}

const std::vector<VkQueueFamilyProperties>& VulkanDevice::getQueueFamiliesProperties() const
{
	return mPhysicalDevice.getQueueFamiliesProperties();
}

const VkPhysicalDeviceMemoryProperties& VulkanDevice::getMemoryProperties() const
{
	return mPhysicalDevice.getMemoryProperties();	
}

const VkFormatProperties& VulkanDevice::getFormatProperties(VkFormat format) const
{
	return mPhysicalDevice.getFormatProperties(format);	
}

bool VulkanDevice::isExtensionSupported(const char* extensionName) const
{
	return mPhysicalDevice.isExtensionSupported(extensionName);	
}

VulkanQueue* VulkanDevice::getQueue(VulkanU32 index)
{
	return mQueueManager.getQueue(index);
}

const VulkanQueue* VulkanDevice::getQueue(VulkanU32 index) const
{
	return mQueueManager.getQueue(index);
}

VulkanU32 VulkanDevice::getQueueCount() const
{
	return mQueueManager.getQueueCount();
}

VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice)
	: mDevice(VK_NULL_HANDLE)
	, mPhysicalDevice(physicalDevice)
	, mQueueManager(*this)
{
	if (!physicalDevice.areAllPropertiesQueried())
	{
		if (!physicalDevice.queryAllProperties())
		{
			VULKAN_LOG_WARNING("Could not query all the properties of a physical device (PhysicalDevice: %d)", physicalDevice.getHandle());
		}
	}
}

VulkanDevice::~VulkanDevice()
{
	uninitializeInternal();
}

bool VulkanDevice::initializeInternal(VkPhysicalDeviceFeatures* desiredFeatures, const VulkanQueueParameters& queueParameters, VulkanSurface* surface)
{
	// Extensions
	std::vector<const char*> desiredExtensions;
	#if (defined VULKAN_KHR)
	{
		desiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
	#endif
	{
		bool missingExtension = false;
		for (auto& desiredExtension : desiredExtensions)
		{
			if (!mPhysicalDevice.isExtensionSupported(desiredExtension))
			{
				VULKAN_LOG_ERROR("Could not find a desired device extension : %s", desiredExtension);
			}
		}
		if (missingExtension)
		{
			return false;
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	if (!mQueueManager.initialize(queueParameters, surface, queueCreateInfos))
	{
		return false;
	}

	// TODO : Layers ?
	VkDeviceCreateInfo deviceCreateInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,               // VkStructureType                  sType
		nullptr,                                            // const void                     * pNext
		0,                                                  // VkDeviceCreateFlags              flags
		static_cast<VulkanU32>(queueCreateInfos.size()),    // uint32_t                         queueCreateInfoCount
		queueCreateInfos.data(),                            // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
		0,                                                  // uint32_t                         enabledLayerCount
		nullptr,                                            // const char * const             * ppEnabledLayerNames
		static_cast<VulkanU32>(desiredExtensions.size()),   // uint32_t                         enabledExtensionCount
		desiredExtensions.data(),                           // const char * const             * ppEnabledExtensionNames
		desiredFeatures                                     // const VkPhysicalDeviceFeatures * pEnabledFeatures
	};

	VkResult result = vkCreateDevice(mPhysicalDevice.getHandle(), &deviceCreateInfo, nullptr, &mDevice);
	if (result != VK_SUCCESS || mDevice == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create device (PhysicalDevice: %d)", mPhysicalDevice.getHandle());
		return false;
	}

	if (!VulkanLoader::ensureDeviceLevelFunctionsLoaded(mDevice, desiredExtensions))
	{
		return false;
	}

	if (!mQueueManager.initializeQueues())
	{
		return false;
	}

	return true;
}

bool VulkanDevice::uninitializeInternal()
{
	if (mDevice != VK_NULL_HANDLE)
	{
		mQueueManager.release();
		vkDestroyDevice(mDevice, nullptr);
		mDevice = VK_NULL_HANDLE;
	}
	return true;
}

#if (defined VULKAN_MONO_DEVICE)
VulkanDevice* VulkanDevice::sDevice = nullptr;
#else
std::unordered_map<VulkanDeviceId, VulkanDevice*> VulkanDevice::sDevices;
#endif

VULKAN_NAMESPACE_END