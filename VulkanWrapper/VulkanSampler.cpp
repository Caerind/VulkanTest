#include "VulkanSampler.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Sampler::Ptr Sampler::createSampler(Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
{
	Sampler::Ptr sampler(new Sampler(device, magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, minLod, maxLod, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, borderColor, unnormalizedCoords));
	if (sampler != nullptr)
	{
		if (!sampler->init())
		{
			sampler.reset();
		}
	}
	return sampler;
}

Sampler::~Sampler()
{
	ObjectTracker::unregisterObject(ObjectType_Sampler);

	release();
}

bool Sampler::isInitialized() const
{
	return mSampler != VK_NULL_HANDLE;
}

const VkSampler& Sampler::getHandle() const
{
	return mSampler;
}

const VkDevice& Sampler::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Sampler::Sampler(Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
	: mDevice(device)
	, mSampler(VK_NULL_HANDLE)
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
	ObjectTracker::registerObject(ObjectType_Sampler);
}

bool Sampler::init()
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

	VkResult result = vkCreateSampler(mDevice.getHandle(), &samplerCreateInfo, nullptr, &mSampler);
	if (result != VK_SUCCESS || mSampler == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create sampler\n");
		return false;
	}
	return true;
}

bool Sampler::release()
{
	if (mSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(mDevice.getHandle(), mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu