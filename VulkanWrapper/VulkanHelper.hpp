#ifndef NU_VULKAN_HELPER_HPP
#define NU_VULKAN_HELPER_HPP

#include "VulkanMemoryBlock.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanBufferView.hpp"

#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSampler.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class ImageHelper
{
	public:
		typedef std::unique_ptr<ImageHelper> Ptr;

		static ImageHelper::Ptr createImage2DAndView(Device& device, VkFormat format, VkExtent2D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect);
		static ImageHelper::Ptr createLayered2DImageWithCubemapView(Device& device, uint32_t size, uint32_t numMipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect);
		static ImageHelper::Ptr createSampledImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering);
		static ImageHelper::Ptr createCombinedImageSampler(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);
		static ImageHelper::Ptr createStorageImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations);
		static ImageHelper::Ptr createInputAttachment(Device& device, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect);

		Image* getImage();
		MemoryBlock* getMemoryBlock();
		ImageView* getImageView();
		Sampler* getSampler();

		bool hasSampler() const;

	protected:
		ImageHelper();

		Image::Ptr mImage;
		MemoryBlock::Ptr mMemoryBlock;
		ImageView::Ptr mImageView;
		Sampler::Ptr mSampler;
};

class BufferHelper
{
	public:
		typedef std::unique_ptr<BufferHelper> Ptr;

		static BufferHelper::Ptr createUniformTexelBuffer(Device& device, VkFormat format, VkDeviceSize size, VkImageUsageFlags usage);
		static BufferHelper::Ptr createStorageTexelBuffer(Device& device, VkFormat format, VkDeviceSize size, VkBufferUsageFlags usage, bool atomicOperations);
		static BufferHelper::Ptr createUniformBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);
		static BufferHelper::Ptr createStorageBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);

		Buffer* getBuffer();
		MemoryBlock* getMemoryBlock();
		BufferView* getBufferView();

		bool hasBufferView() const;

	protected:
		BufferHelper();

		Buffer::Ptr mBuffer;
		MemoryBlock::Ptr mMemoryBlock;
		BufferView::Ptr mBufferView;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_HELPER_HPP