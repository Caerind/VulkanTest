#include "VulkanDescriptorSetLayout.hpp"

#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanDescriptorSetLayoutPtr VulkanDescriptorSetLayout::createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	VulkanDescriptorSetLayoutPtr descriptorSetLayout(new VulkanDescriptorSetLayout(bindings));
	if (descriptorSetLayout != nullptr)
	{
		if (!descriptorSetLayout->init())
		{
			descriptorSetLayout.reset();
		}
	}
	return descriptorSetLayout;
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	release();
	
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanDescriptorSetLayout::isInitialized() const
{
	return mDescriptorSetLayout != VK_NULL_HANDLE;
}

const VkDescriptorSetLayout& VulkanDescriptorSetLayout::getHandle() const
{
	return mDescriptorSetLayout;
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	: mDescriptorSetLayout(VK_NULL_HANDLE)
	, mBindings(bindings)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanDescriptorSetLayout::init()
{
	// TODO : Flags ?
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
		nullptr,                                              // const void                         * pNext
		0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
		static_cast<VulkanU32>(mBindings.size()),              // uint32_t                             bindingCount
		mBindings.data()                                      // const VkDescriptorSetLayoutBinding * pBindings
	};

	VkResult result = vkCreateDescriptorSetLayout(getDeviceHandle(), &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout);
	if (result != VK_SUCCESS || mDescriptorSetLayout == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create a layout for descriptor sets");
		return false;
	}

	return true;
}

bool VulkanDescriptorSetLayout::release()
{
	if (mDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(getDeviceHandle(), mDescriptorSetLayout, nullptr);
		mDescriptorSetLayout = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END