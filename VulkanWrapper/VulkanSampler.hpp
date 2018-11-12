#ifndef NU_VULKAN_SAMPLER_HPP
#define NU_VULKAN_SAMPLER_HPP

#include <memory>

#include "VulkanFunctions.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class Sampler
{
	public:
		typedef std::unique_ptr<Sampler> Ptr;

		~Sampler();

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

		Device& getDevice();
		const Device& getDevice() const;
		const VkSampler& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static Sampler::Ptr createSampler(Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		Sampler(Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		bool init();
		void release();

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