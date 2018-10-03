#ifndef NU_VULKAN_SHADER_MODULE_HPP
#define NU_VULKAN_SHADER_MODULE_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

namespace nu
{
namespace Vulkan
{

class Device;
class ShaderModule
{
	public:

		// TODO : Add multi stage shaders
		enum ShaderStage
		{
			Vertex,
			TessellationControl,
			TessellationEvaluation,
			Geometry,
			Fragment,
			Compute
		};

		typedef std::unique_ptr<ShaderModule> Ptr;

		static ShaderModule::Ptr createShaderModule(Device& device, ShaderStage stage, const std::vector<unsigned char>& sourceCode, const std::string& entryPoint = "main", VkSpecializationInfo* specializationInfo = nullptr);

		~ShaderModule();

		const VkPipelineShaderStageCreateInfo& getShaderStageCreateInfo() const;

		bool isInitialized() const;
		const VkShaderModule& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		ShaderModule(Device& device, ShaderStage stage, const std::vector<unsigned char>& sourceCode, const std::string& entryPoint = "main", VkSpecializationInfo* specializationInfo = nullptr);

		bool init();
		bool release();

		Device& mDevice;
		VkShaderModule mShaderModule;

		ShaderStage mStage;
		std::vector<unsigned char> mSourceCode;
		std::string mEntryPoint;
		VkSpecializationInfo* mSpecializationInfo;

		mutable bool mShaderStageCreateInfoFilled;
		mutable VkPipelineShaderStageCreateInfo mShaderStageCreateInfo;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SHADER_MODULE_HPP