#pragma once

#include "VulkanFunctions.hpp"

// TODO : Binding on MemoryBlock side or Image/Buffer side ?

VULKAN_NAMESPACE_BEGIN

class VulkanImage : public VulkanDeviceObject<VulkanObjectType_Image>
{
	public:
		~VulkanImage();

		const VkMemoryRequirements& getMemoryRequirements() const;

		VulkanMemoryBlock* allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties);
		bool ownMemoryBlock() const;
		bool isBoundToMemoryBlock() const;
		VulkanMemoryBlock* getMemoryBlock();
		VkDeviceSize getOffsetInMemoryBlock() const;

		VulkanImageView* createImageView(VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);
		VulkanImageView* getImageView(VulkanU32 index);
		const VulkanImageView* getImageView(VulkanU32 index) const;
		VulkanU32 getImageViewCount() const;
		void clearImageViews();

		VkImageType getType() const;
		VkFormat getFormat() const;
		VkExtent3D getSize() const;
		VulkanU32 getNumMipmaps() const;
		VulkanU32 getNumLayers() const;
		VkSampleCountFlagBits getSamples() const;
		VkImageUsageFlags getUsage() const;
		bool isCubemap() const;

		bool isSwapchainImage() const;

		const VkImage& getHandle() const;

	private:
		friend class VulkanDevice;
		static VulkanImagePtr createImage(VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap);

		friend class VulkanSwapchain;
		static VulkanImagePtr createImageFromSwapchain(VulkanDevice& device, VkImage swapchainImageHandle, VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap);

		VulkanImage(VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap);

		bool init();
		void release(); 

		friend class VulkanMemoryBlock;
		void bindToMemoryBlock(VulkanMemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock);

	private:
		VkImage mImage;

		VulkanMemoryBlockPtr mOwnedMemoryBlock;
		VulkanMemoryBlock* mMemoryBlock;
		VkDeviceSize mOffsetInMemoryBlock;

		std::vector<VulkanImageViewPtr> mImageViews;

		VkImageType mType;
		VkFormat mFormat;
		VkExtent3D mSize;
		VulkanU32 mNumMipmaps;
		VulkanU32 mNumLayers;
		VkSampleCountFlagBits mSamples;
		VkImageUsageFlags mUsage;
		bool mCubemap;

		bool mIsSwapchainImage;

		mutable bool mMemoryRequirementsQueried;
		mutable VkMemoryRequirements mMemoryRequirements;
};

VULKAN_NAMESPACE_END