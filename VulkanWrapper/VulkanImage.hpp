#ifndef NU_VULKAN_IMAGE_HPP
#define NU_VULKAN_IMAGE_HPP

#include "VulkanFunctions.hpp"

#include "VulkanMemoryBlock.hpp"
#include "VulkanImageView.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Image
{
	public:
		typedef std::unique_ptr<Image> Ptr;

		~Image();

		const VkMemoryRequirements& getMemoryRequirements() const;

		MemoryBlock* allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties);

		ImageView::Ptr createImageView(VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		Device& getDevice();
		const Device& getDevice() const;
		const VkImage& getHandle() const;
		const VkDevice& getDeviceHandle() const;

		bool ownMemoryBlock() const;
		bool isBoundToMemoryBlock() const;
		MemoryBlock* getMemoryBlock();

	private:
		friend class Device;
		friend class MemoryBlock;

		static Image::Ptr createImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap);

		Image(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap);

		bool init();
		bool release(); 
		
		void bindToMemoryBlock(MemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock);

		Device& mDevice;
		VkImage mImage;

		MemoryBlock::Ptr mOwnedMemoryBlock;
		MemoryBlock* mMemoryBlock;
		VkDeviceSize mOffsetInMemoryBlock;

		VkImageType mType;
		VkFormat mFormat;
		VkExtent3D mSize;
		uint32_t mNumMipmaps;
		uint32_t mNumLayers;
		VkSampleCountFlagBits mSamples;
		VkImageUsageFlags mUsageScenarios;
		bool mCubemap;

		mutable bool mMemoryRequirementsQueried;
		mutable VkMemoryRequirements mMemoryRequirements;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_IMAGE_HPP