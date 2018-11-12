#include "VulkanSampler.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Sampler::~Sampler()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_Sampler);
}

VkFilter Sampler::getMagFilter() const
{
	return mMagFilter;
}

VkFilter Sampler::getMinFilter() const
{
	return mMinFilter;
}

VkSamplerMipmapMode Sampler::getMipmapMode() const
{
	return mMipmapMode;
}

VkSamplerAddressMode Sampler::getAddressModeU() const
{
	return mAddressModeU;
}

VkSamplerAddressMode Sampler::getAddressModeV() const
{
	return mAddressModeV;
}

VkSamplerAddressMode Sampler::getAddressModeW() const
{
	return mAddressModeW;
}

float Sampler::getLodBias() const
{
	return mLodBias;
}

float Sampler::getMinLod() const
{
	return mMinLod;
}

float Sampler::getMaxLod() const
{
	return mMaxLod;
}

bool Sampler::isAnisotropyEnabled() const
{
	return mAnisotropyEnabled;
}

float Sampler::getMaxAnisotropy() const
{
	return mMaxAnisotropy;
}

bool Sampler::isCompareEnabled() const
{
	return mCompareEnabled;
}

VkCompareOp Sampler::getCompareOperator() const
{
	return mCompareOperator;
}

VkBorderColor Sampler::getBorderColor() const
{
	return mBorderColor;
}

bool Sampler::isUnnormalizedCoords() const
{
	return mUnnormalizedCoords;
}

Device& Sampler::getDevice()
{
	return mDevice;
}

const Device& Sampler::getDevice() const
{
	return mDevice;
}

const VkSampler& Sampler::getHandle() const
{
	return mSampler;
}

const VkDevice& Sampler::getDeviceHandle() const
{
	return mDevice.getHandle();
}

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

void Sampler::release()
{
	if (mSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(mDevice.getHandle(), mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;
	}
}

} // namespace Vulkan
} // namespace nu