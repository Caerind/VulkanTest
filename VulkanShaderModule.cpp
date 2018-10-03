#include "VulkanShaderModule.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

ShaderModule::Ptr ShaderModule::createShaderModule(Device& device, ShaderStage stage, const std::vector<unsigned char>& sourceCode, const std::string& entryPoint, VkSpecializationInfo* specializationInfo)
{
	ShaderModule::Ptr shaderModule(new ShaderModule(device, stage, sourceCode, entryPoint, specializationInfo));
	if (shaderModule != nullptr)
	{
		if (!shaderModule->init())
		{
			shaderModule.reset();
		}
	}
	return shaderModule;
}

ShaderModule::~ShaderModule()
{
	ObjectTracker::unregisterObject(ObjectType_ShaderModule);

	release();
}

const VkPipelineShaderStageCreateInfo& ShaderModule::getShaderStageCreateInfo() const
{
	if (!mShaderStageCreateInfoFilled)
	{
		mShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; // VkStructureType sType
		mShaderStageCreateInfo.pNext = nullptr; // const void* pNext
		mShaderStageCreateInfo.flags = 0; // VkPipelineShaderStageCreateFlags flags
		switch (mStage) // VkShaderStageFlagBits stage
		{
			case ShaderStage::Vertex: mShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
			case ShaderStage::TessellationControl: mShaderStageCreateInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
			case ShaderStage::TessellationEvaluation: mShaderStageCreateInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
			case ShaderStage::Geometry: mShaderStageCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
			case ShaderStage::Fragment: mShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
			case ShaderStage::Compute: mShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
			default: 
				// TODO : Use NUmea Log System
				printf("Unhandled shader stage\n"); 
				break;
		}
		mShaderStageCreateInfo.module = mShaderModule; // VkShaderModule module
		mShaderStageCreateInfo.pName = mEntryPoint.c_str(); // const char* pName
		mShaderStageCreateInfo.pSpecializationInfo = mSpecializationInfo; // const VkSpecializationInfo* pSpecializationInfo

		mShaderStageCreateInfoFilled = true;
	}

	return mShaderStageCreateInfo;
}

bool ShaderModule::isInitialized() const
{
	return mShaderModule != VK_NULL_HANDLE;
}

const VkShaderModule& ShaderModule::getHandle() const
{
	return mShaderModule;
}

const Device& ShaderModule::getDeviceHandle() const
{
	return mDevice;
}

ShaderModule::ShaderModule(Device& device, ShaderStage stage, const std::vector<unsigned char>& sourceCode, const std::string& entryPoint, VkSpecializationInfo* specializationInfo)
	: mDevice(device)
	, mShaderModule(VK_NULL_HANDLE)
	, mStage(stage)
	, mSourceCode(sourceCode)
	, mEntryPoint(entryPoint)
	, mSpecializationInfo(specializationInfo)
	, mShaderStageCreateInfoFilled(false)
	, mShaderStageCreateInfo()
{
	ObjectTracker::registerObject(ObjectType_ShaderModule);
}

bool ShaderModule::init()
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,              // VkStructureType              sType
		nullptr,                                                  // const void                 * pNext
		0,                                                        // VkShaderModuleCreateFlags    flags
		mSourceCode.size(),                                       // size_t                       codeSize
		reinterpret_cast<uint32_t const *>(mSourceCode.data())    // const uint32_t             * pCode
	};

	VkResult result = vkCreateShaderModule(mDevice.getHandle(), &shaderModuleCreateInfo, nullptr, &mShaderModule);
	if (result != VK_SUCCESS || mShaderModule == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a shader module\n");
		return false;
	}

	return true;
}

bool ShaderModule::release()
{
	if (mShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice.getHandle(), mShaderModule, nullptr);
		mShaderModule = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu