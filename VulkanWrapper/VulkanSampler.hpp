#ifndef NU_VULKAN_SAMPLER_HPP
#define NU_VULKAN_SAMPLER_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Sampler
{
	public:
		typedef std::unique_ptr<Sampler> Ptr;

		static Sampler::Ptr createSampler(Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		~Sampler();

		bool isInitialized() const;
		const VkSampler& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		Sampler(Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		bool init();
		bool release();

		Device& mDevice;
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

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SAMPLER_HPP