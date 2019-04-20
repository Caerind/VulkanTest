#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanSampler : public VulkanDeviceObject<VulkanObjectType_Sampler>
{
	public:
		~VulkanSampler();

		VkFilter getMagFilter() const;
		VkFilter getMinFilter() const;
		VkSamplerMipmapMode getMipmapMode() const;
		VkSamplerAddressMode getAddressModeU() const;
		VkSamplerAddressMode getAddressModeV() const;
		VkSamplerAddressMode getAddressModeW() const;
		float getLodBias() const;
		float getMinLod() const;
		float getMaxLod() const;
		bool isAnisotropyEnabled() const;
		float getMaxAnisotropy() const;
		bool isCompareEnabled() const;
		VkCompareOp getCompareOperator() const;
		VkBorderColor getBorderColor() const;
		bool isUnnormalizedCoords() const;

		const VkSampler& getHandle() const; 

	private:
		friend class VulkanDevice;
		static VulkanSamplerPtr createSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		VulkanSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		bool init();
		void release();

		VkSampler mSampler;

		VkFilter mMagFilter;
		VkFilter mMinFilter;
		VkSamplerMipmapMode mMipmapMode;
		VkSamplerAddressMode mAddressModeU;
		VkSamplerAddressMode mAddressModeV;
		VkSamplerAddressMode mAddressModeW;
		float mLodBias;
		float mMinLod;
		float mMaxLod;
		bool mAnisotropyEnabled;
		float mMaxAnisotropy;
		bool mCompareEnabled;
		VkCompareOp mCompareOperator;
		VkBorderColor mBorderColor;
		bool mUnnormalizedCoords;
};

VULKAN_NAMESPACE_END