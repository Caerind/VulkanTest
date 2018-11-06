#include "VulkanDescriptorSetLayout.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

DescriptorSetLayout::Ptr DescriptorSetLayout::createDescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	DescriptorSetLayout::Ptr descriptorSetLayout(new DescriptorSetLayout(device, bindings));
	if (descriptorSetLayout != nullptr)
	{
		if (!descriptorSetLayout->init())
		{
			descriptorSetLayout.reset();
		}
	}
	return descriptorSetLayout;
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	ObjectTracker::unregisterObject(ObjectType_DescriptorSetLayout);

	release();
}

bool DescriptorSetLayout::isInitialized() const
{
	return mDescriptorSetLayout != VK_NULL_HANDLE;
}

const VkDescriptorSetLayout& DescriptorSetLayout::getHandle() const
{
	return mDescriptorSetLayout;
}

const VkDevice& DescriptorSetLayout::getDeviceHandle() const
{
	return mDevice.getHandle();
}

DescriptorSetLayout::DescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	: mDevice(device)
	, mDescriptorSetLayout(VK_NULL_HANDLE)
	, mBindings(bindings)
{
	ObjectTracker::registerObject(ObjectType_DescriptorSetLayout);
}

bool DescriptorSetLayout::init()
{
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
		nullptr,                                              // const void                         * pNext
		0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
		static_cast<uint32_t>(mBindings.size()),              // uint32_t                             bindingCount
		mBindings.data()                                      // const VkDescriptorSetLayoutBinding * pBindings
	};

	VkResult result = vkCreateDescriptorSetLayout(mDevice.getHandle(), &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout);
	if (result != VK_SUCCESS || mDescriptorSetLayout == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a layout for descriptor sets\n");
		return false;
	}

	return true;
}

bool DescriptorSetLayout::release()
{
	if (mDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(mDevice.getHandle(), mDescriptorSetLayout, nullptr);
		mDescriptorSetLayout = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu