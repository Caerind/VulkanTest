#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Device::Ptr Device::createDevice(PhysicalDevice& physicalDevice, const std::vector<uint32_t>& queueFamilyIndexes, VkPhysicalDeviceFeatures* desiredFeatures)
{
	Device::Ptr device(new Device(physicalDevice));
	if (device != nullptr)
	{
		if (!device->init(queueFamilyIndexes, desiredFeatures))
		{
			device.reset();
		}
	}
	return device;
}

Device::~Device()
{
	ObjectTracker::unregisterObject(ObjectType_Device);

	release();
}

Queue::Ptr Device::getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex)
{
	return Queue::Ptr(new Queue(*this, queueFamilyIndex, queueIndex));
}

Buffer::Ptr Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
	return Buffer::createBuffer(*this, size, usage);
}

CommandPool::Ptr Device::createCommandPool(VkCommandPoolCreateFlags parameters, uint32_t queueFamily)
{
	return CommandPool::createCommandPool(*this, parameters, queueFamily);
}

DescriptorPool::Ptr Device::createDescriptorPool(bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes)
{
	return DescriptorPool::createDescriptorPool(*this, freeIndividualSets, maxSetsCount, descriptorTypes);
}

DescriptorSetLayout::Ptr Device::createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	return DescriptorSetLayout::createDescriptorSetLayout(*this, bindings);
}

void Device::updateDescriptorSets(const std::vector<ImageDescriptorInfo>& imageDescriptorInfos, const std::vector<BufferDescriptorInfo>& bufferDescriptorInfos, const std::vector<TexelBufferDescriptorInfo>& texelBufferDescriptorInfos, const std::vector<CopyDescriptorInfo>& copyDescriptorInfos)
{
	std::vector<VkWriteDescriptorSet> writeDescriptors;
	std::vector<VkCopyDescriptorSet> copyDescriptors;

	// Image descriptors
	for (auto& imageDescriptor : imageDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			imageDescriptor.targetDescriptorSet,                                    // VkDescriptorSet                  dstSet
			imageDescriptor.targetDescriptorBinding,                                // uint32_t                         dstBinding
			imageDescriptor.targetArrayElement,                                     // uint32_t                         dstArrayElement
			static_cast<uint32_t>(imageDescriptor.imageInfos.size()),               // uint32_t                         descriptorCount
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
			static_cast<uint32_t>(bufferDescriptor.bufferInfos.size()),             // uint32_t                         descriptorCount
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
			static_cast<uint32_t>(texelBufferDescriptor.texelBufferViews.size()),   // uint32_t                         descriptorCount
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

	vkUpdateDescriptorSets(mDevice, static_cast<uint32_t>(writeDescriptors.size()), writeDescriptors.data(), static_cast<uint32_t>(copyDescriptors.size()), copyDescriptors.data());
}

Fence::Ptr Device::createFence(bool signaled)
{
	return Fence::createFence(*this, signaled);
}

bool Device::waitFences(const std::vector<Fence*>& fences, bool waitForAll, uint64_t timeout)
{
	if (fences.size() > 0)
	{
		std::vector<VkFence> fenceHandles;
		fenceHandles.reserve(fences.size());
		for (size_t i = 0; i < fences.size(); i++)
		{
			fenceHandles.push_back(fences[i]->getHandle());
		}

		VkResult result = vkWaitForFences(mDevice, static_cast<uint32_t>(fenceHandles.size()), fenceHandles.data(), waitForAll, timeout);
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Waiting on fence failed\n");
			return false;
		}
		return true;
	}
	return false;
}

bool Device::resetFences(const std::vector<Fence*>& fences)
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

		VkResult result = vkResetFences(mDevice, static_cast<uint32_t>(fenceHandles.size()), fenceHandles.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Error occurred when tried to reset fences\n");
			return false;
		}
		return true;
	}
	return false;
}

Image::Ptr Device::createImage(VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap)
{
	return Image::createImage(*this, type, format, size, numMipmaps, numLayers, samples, usageScenarios, cubemap);
}

MemoryBlock::Ptr Device::createMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties)
{
	return MemoryBlock::createMemoryBlock(*this, memoryRequirements, memoryProperties);
}

PipelineCache::Ptr Device::createPipelineCache(const std::vector<unsigned char>& cacheData)
{
	return PipelineCache::createPipelineCache(*this, cacheData);
}

PipelineLayout::Ptr Device::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges)
{
	return PipelineLayout::createPipelineLayout(*this, descriptorSetLayouts, pushConstantRanges);
}

ComputePipeline::Ptr Device::createComputePipeline(ShaderModule* computeShader, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions)
{
	return ComputePipeline::createComputePipeline(*this, computeShader, layout, cache, additionalOptions);
}

GraphicsPipeline::Ptr Device::initGraphicsPipeline(PipelineLayout& layout, RenderPass& renderPass, PipelineCache* cache)
{
	return GraphicsPipeline::Ptr(new GraphicsPipeline(*this, layout, renderPass, cache));
}

RenderPass::Ptr Device::initRenderPass()
{
	return RenderPass::Ptr(new RenderPass(*this));
}

Sampler::Ptr Device::createSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
{
	return Sampler::createSampler(*this, magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, minLod, maxLod, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, borderColor, unnormalizedCoords);
}

Semaphore::Ptr Device::createSemaphore()
{
	return Semaphore::createSemaphore(*this);
}

ShaderModule::Ptr Device::initShaderModule()
{
	return ShaderModule::Ptr(new ShaderModule(*this));
}

Swapchain::Ptr Device::createSwapchain(Surface* surface, std::vector<FrameResources>& framesResources)
{
	return Swapchain::createSwapchain(*this, surface, framesResources);
}

bool Device::waitForAllSubmittedCommands()
{
	VkResult result = vkDeviceWaitIdle(mDevice);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Waiting on a device failed\n");
		return false;
	}
	return true;
}

bool Device::isInitialized() const
{
	return mDevice != VK_NULL_HANDLE;
}

const VkDevice& Device::getHandle() const
{
	return mDevice;
}

const VkPhysicalDevice& Device::getPhysicalHandle() const
{
	return mPhysicalDevice.getHandle();
}

PhysicalDevice& Device::getPhysicalDevice()
{
	return mPhysicalDevice;
}

const PhysicalDevice& Device::getPhysicalDevice() const
{
	return mPhysicalDevice;
}

Device::Device(PhysicalDevice& physicalDevice)
	: mDevice(VK_NULL_HANDLE)
	, mPhysicalDevice(physicalDevice)
{
	ObjectTracker::registerObject(ObjectType_Device);
}

bool Device::init(const std::vector<uint32_t>& queueFamilyIndexes, VkPhysicalDeviceFeatures* desiredFeatures)
{
	std::vector<const char*> desiredExtensions;
	// TODO : Only if wanted
	if (true)
	{
		desiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	// TODO : Only on Debug
	if (true)
	{
		for (auto& desiredExtension : desiredExtensions)
		{
			if (!mPhysicalDevice.isExtensionSupported(desiredExtension))
			{
				// TODO : Numea Log System
				printf("Could not find a desired device extension : %s\n", desiredExtension);
				return false;
			}
		}
	}

	// TODO : Queue Priorities
	std::vector<float> priorities = { 1.0f };

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto& queueFamilyIndex : queueFamilyIndexes)
	{

		queueCreateInfos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType                  sType
			nullptr,                                          // const void                     * pNext
			0,                                                // VkDeviceQueueCreateFlags         flags
			queueFamilyIndex,                                 // uint32_t                         queueFamilyIndex
			static_cast<uint32_t>(priorities.size()),         // uint32_t                         queueCount
			priorities.data()                                 // const float                    * pQueuePriorities
		});
	};

	VkDeviceCreateInfo deviceCreateInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,               // VkStructureType                  sType
		nullptr,                                            // const void                     * pNext
		0,                                                  // VkDeviceCreateFlags              flags
		static_cast<uint32_t>(queueCreateInfos.size()),     // uint32_t                         queueCreateInfoCount
		queueCreateInfos.data(),                            // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
		0,                                                  // uint32_t                         enabledLayerCount
		nullptr,                                            // const char * const             * ppEnabledLayerNames
		static_cast<uint32_t>(desiredExtensions.size()),    // uint32_t                         enabledExtensionCount
		desiredExtensions.data(),                           // const char * const             * ppEnabledExtensionNames
		desiredFeatures                                     // const VkPhysicalDeviceFeatures * pEnabledFeatures
	};

	VkResult result = vkCreateDevice(mPhysicalDevice.getHandle(), &deviceCreateInfo, nullptr, &mDevice);
	if (result != VK_SUCCESS || mDevice == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create logical device\n");
		return false;
	}

	if (!Loader::ensureDeviceLevelFunctionsLoaded(mDevice, desiredExtensions))
	{
		return false;
	}

	return true;
}

bool Device::release()
{
	if (mDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(mDevice, nullptr);
		mDevice = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu