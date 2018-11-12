#ifndef NU_VULKAN_IMAGE_HPP
#define NU_VULKAN_IMAGE_HPP

#include <memory>
#include <vector>

#include "VulkanFunctions.hpp"

#include "VulkanMemoryBlock.hpp"
#include "VulkanImageView.hpp"

// TODO : Binding on MemoryBlock side or Image/Buffer side ?

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
		bool ownMemoryBlock() const;
		bool isBoundToMemoryBlock() const;
		MemoryBlock* getMemoryBlock();
		VkDeviceSize getOffsetInMemoryBlock() const;

		ImageView* createImageView(VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);
		ImageView* getImageView(uint32_t index);
		const ImageView* getImageView(uint32_t index) const;
		uint32_t getImageViewCount() const;
		void clearImageViews();

		VkImageType getType() const;
		VkFormat getFormat() const;
		VkExtent3D getSize() const;
		uint32_t getNumMipmaps() const;
		uint32_t getNumLayers() const;
		VkSampleCountFlagBits getSamples() const;
		VkImageUsageFlags getUsage() const;
		bool isCubemap() const;

		bool isSwapchainImage() const;

		Device& getDevice();
		const Device& getDevice() const;
		const VkImage& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static Image::Ptr createImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap);

		friend class Swapchain;
		static Image::Ptr createImageFromSwapchain(Device& device, VkImage swapchainImageHandle, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap);

		Image(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap);

		bool init();
		void release(); 

		friend class MemoryBlock;
		void bindToMemoryBlock(MemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock);

		Device& mDevice;
		VkImage mImage;

		MemoryBlock::Ptr mOwnedMemoryBlock;
		MemoryBlock* mMemoryBlock;
		VkDeviceSize mOffsetInMemoryBlock;

		std::vector<ImageView::Ptr> mImageViews;

		VkImageType mType;
		VkFormat mFormat;
		VkExtent3D mSize;
		uint32_t mNumMipmaps;
		uint32_t mNumLayers;
		VkSampleCountFlagBits mSamples;
		VkImageUsageFlags mUsage;
		bool mCubemap;

		bool mIsSwapchainImage;

		mutable bool mMemoryRequirementsQueried;
		mutable VkMemoryRequirements mMemoryRequirements;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_IMAGE_HPP