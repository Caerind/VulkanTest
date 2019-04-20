#include "VulkanSampler.hpp"

#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanSampler::~VulkanSampler()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

VkFilter VulkanSampler::getMagFilter() const
{
	return mMagFilter;
}

VkFilter VulkanSampler::getMinFilter() const
{
	return mMinFilter;
}

VkSamplerMipmapMode VulkanSampler::getMipmapMode() const
{
	return mMipmapMode;
}

VkSamplerAddressMode VulkanSampler::getAddressModeU() const
{
	return mAddressModeU;
}

VkSamplerAddressMode VulkanSampler::getAddressModeV() const
{
	return mAddressModeV;
}

VkSamplerAddressMode VulkanSampler::getAddressModeW() const
{
	return mAddressModeW;
}

float VulkanSampler::getLodBias() const
{
	return mLodBias;
}

float VulkanSampler::getMinLod() const
{
	return mMinLod;
}

float VulkanSampler::getMaxLod() const
{
	return mMaxLod;
}

bool VulkanSampler::isAnisotropyEnabled() const
{
	return mAnisotropyEnabled;
}

float VulkanSampler::getMaxAnisotropy() const
{
	return mMaxAnisotropy;
}

bool VulkanSampler::isCompareEnabled() const
{
	return mCompareEnabled;
}

VkCompareOp VulkanSampler::getCompareOperator() const
{
	return mCompareOperator;
}

VkBorderColor VulkanSampler::getBorderColor() const
{
	return mBorderColor;
}

bool VulkanSampler::isUnnormalizedCoords() const
{
	return mUnnormalizedCoords;
}

const VkSampler& VulkanSampler::getHandle() const
{
	return mSampler;
}

VulkanSamplerPtr VulkanSampler::createSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
{
	VulkanSamplerPtr sampler(new VulkanSampler(magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, minLod, maxLod, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, borderColor, unnormalizedCoords));
	if (sampler != nullptr)
	{
		if (!sampler->init())
		{
			sampler.reset();
		}
	}
	return sampler;
}

VulkanSampler::VulkanSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
	: mSampler(VK_NULL_HANDLE)
	, mMagFilter(magFilter)
	, mMinFilter(minFilter)
	, mMipmapMode(mipmapMode)
	, mAddressModeU(uAddressMode)
	, mAddressModeV(vAddressMode)
	, mAddressModeW(wAddressMode)
	, mLodBias(lodBias)
	, mMinLod(minLod)
	, mMaxLod(maxLod)
	, mAnisotropyEnabled(anisotropyEnable)
	, mMaxAnisotropy(maxAnisotropy)
	, mCompareEnabled(compareEnable)
	, mCompareOperator(compareOperator)
	, mBorderColor(borderColor)
	, mUnnormalizedCoords(unnormalizedCoords)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanSampler::init()
{
	VkSamplerCreateInfo samplerCreateInfo = {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,    // VkStructureType          sType
		nullptr,                                  // const void             * pNext
		0,                                        // VkSamplerCreateFlags     flags
		mMagFilter,                               // VkFilter                 magFilter
		mMinFilter,                               // VkFilter                 minFilter
		mMipmapMode,                              // VkSamplerMipmapMode      mipmapMode
		mAddressModeU,                            // VkSamplerAddressMode     addressModeU
		mAddressModeV,                            // VkSamplerAddressMode     addressModeV
		mAddressModeW,                            // VkSamplerAddressMode     addressModeW
		mLodBias,                                 // float                    mipLodBias
		mAnisotropyEnabled,                       // VkBool32                 anisotropyEnable
		mMaxAnisotropy,                           // float                    maxAnisotropy
		mCompareEnabled,                          // VkBool32                 compareEnable
		mCompareOperator,                         // VkCompareOp              compareOp
		mMinLod,                                  // float                    minLod
		mMaxLod,                                  // float                    maxLod
		mBorderColor,                             // VkBorderColor            borderColor
		mUnnormalizedCoords                       // VkBool32                 unnormalizedCoordinates
	};

	VkResult result = vkCreateSampler(getDeviceHandle(), &samplerCreateInfo, nullptr, &mSampler);
	if (result != VK_SUCCESS || mSampler == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create sampler\n");
		return false;
	}
	return true;
}

void VulkanSampler::release()
{
	if (mSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(getDeviceHandle(), mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;
	}
}

VULKAN_NAMESPACE_END