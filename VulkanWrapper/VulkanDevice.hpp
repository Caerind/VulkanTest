#ifndef NU_VULKAN_DEVICE_HPP
#define NU_VULKAN_DEVICE_HPP

#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"

#include <memory>
#include <vector>

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

namespace nu
{
namespace Vulkan
{

struct QueueInfo
{
	uint32_t familyIndex;
	std::vector<float> priorities;
};

struct ImageDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet;
	uint32_t targetDescriptorBinding;
	uint32_t targetArrayElement;
	VkDescriptorType targetDescriptorType;
	std::vector<VkDescriptorImageInfo> imageInfos;
};

struct BufferDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet;
	uint32_t targetDescriptorBinding;
	uint32_t targetArrayElement;
	VkDescriptorType targetDescriptorType;
	std::vector<VkDescriptorBufferInfo> bufferInfos;
};

struct TexelBufferDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet;
	uint32_t targetDescriptorBinding;
	uint32_t targetArrayElement;
	VkDescriptorType targetDescriptorType;
	std::vector<VkBufferView> texelBufferViews;
};

struct CopyDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet;
	uint32_t targetDescriptorBinding;
	uint32_t targetArrayElement;
	VkDescriptorSet sourceDescriptorSet;
	uint32_t sourceDescriptorBinding;
	uint32_t sourceArrayElement;
	uint32_t descriptorCount;
};

class Device
{
	public:
		typedef std::unique_ptr<Device> Ptr;

		static Device::Ptr createDevice(PhysicalDevice& physicalDevice, const std::vector<uint32_t>& queueFamilyIndexes, VkPhysicalDeviceFeatures* desiredFeatures);

		~Device();

		Queue::Ptr getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex);

		Buffer::Ptr createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

		CommandPool::Ptr createCommandPool(VkCommandPoolCreateFlags parameters, uint32_t queueFamily);

		DescriptorPool::Ptr createDescriptorPool(bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes); 
		DescriptorSetLayout::Ptr createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		void updateDescriptorSets(const std::vector<ImageDescriptorInfo>& imageDescriptorInfos, const std::vector<BufferDescriptorInfo>& bufferDescriptorInfos, const std::vector<TexelBufferDescriptorInfo>& texelBufferDescriptorInfos, const std::vector<CopyDescriptorInfo>& copyDescriptorInfos);

		Fence::Ptr createFence(bool signaled = false);
		bool waitFences(const std::vector<Fence*>& fences, bool waitForAll, uint64_t timeout);
		bool resetFences(const std::vector<Fence*>& fences);

		Image::Ptr createImage(VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap);
		
		MemoryBlock::Ptr createMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties);

		PipelineCache::Ptr createPipelineCache(const std::vector<unsigned char>& cacheData = {});

		PipelineLayout::Ptr createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges);

		ComputePipeline::Ptr createComputePipeline(ShaderModule* computeShader, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		GraphicsPipeline::Ptr initGraphicsPipeline(PipelineLayout& layout, RenderPass& renderPass, PipelineCache* cache = nullptr);

		RenderPass::Ptr initRenderPass();

		Sampler::Ptr createSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		Semaphore::Ptr createSemaphore();
		
		ShaderModule::Ptr initShaderModule();

		Swapchain::Ptr createSwapchain(Surface* surface, std::vector<FrameResources>& framesResources);

		bool waitForAllSubmittedCommands();

		bool isInitialized() const;
		const VkDevice& getHandle() const;
		const VkPhysicalDevice& getPhysicalHandle() const;
		PhysicalDevice& getPhysicalDevice();
		const PhysicalDevice& getPhysicalDevice() const;

	private:
		Device(PhysicalDevice& physicalDevice);

		bool init(const std::vector<uint32_t>& queueFamilyIndexes, VkPhysicalDeviceFeatures* desiredFeatures);
		bool release();

		VkDevice mDevice;
		PhysicalDevice& mPhysicalDevice;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_DEVICE_HPP