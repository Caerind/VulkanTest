#ifndef NU_VULKAN_SHADER_MODULE_HPP
#define NU_VULKAN_SHADER_MODULE_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

// TODO : SpecializationInfo param
// TODO : Add GLSL->Spirv tool
// TODO : Get info about variables in shaders
// TODO : Auto find entrypoints

namespace nu
{
namespace Vulkan
{

class Device;
class ShaderModule
{
	public:
		// TODO : Add multi stage shaders
		enum ShaderStageFlags
		{
			None = 0,
			Vertex = 1,
			TessellationControl = 2,
			TessellationEvaluation = 4,
			Geometry = 8,
			Fragment = 16,
			Compute = 32
		};

		typedef std::unique_ptr<ShaderModule> Ptr;

		~ShaderModule();

		bool loadFromFile(const std::string& filename);

		void setVertexEntrypointName(const std::string& entrypoint);
		void setTessellationControlEntrypointName(const std::string& entrypoint);
		void setTessellationEvaluationEntrypointName(const std::string& entrypoint);
		void setGeometryEntrypointName(const std::string& entrypoint);
		void setFragmentEntrypointName(const std::string& entrypoint);
		void setComputeEntrypointName(const std::string& entrypoint);

		bool create();
		bool destroy();
		bool isCreated() const;
		const VkShaderModule& getHandle() const;

		const std::string& getVertexEntrypointName() const;
		const std::string& getTessellationControlEntrypointName() const;
		const std::string& getTessellationEvaluationEntrypointName() const;
		const std::string& getGeometryEntrypointName() const;
		const std::string& getFragmentEntrypointName() const;
		const std::string& getComputeEntrypointName() const;

		const std::vector<unsigned char>& getSpirvBlob() const;

		const ShaderStageFlags& getStages() const;

	private:
		friend class Device;
		ShaderModule(Device& device);

		friend class GraphicsPipeline;
		void addShaderStages(std::vector<VkPipelineShaderStageCreateInfo>& shaders);

		void generateShaderStages() const;

	private:
		Device& mDevice;
		VkShaderModule mShaderModule;

		ShaderStageFlags mStages;
		std::vector<unsigned char> mSpirvBlob;
		VkSpecializationInfo* mSpecializationInfo;

		std::string mVertexEntrypointName;
		std::string mTessellationControlEntrypointName;
		std::string mTessellationEvaluationEntrypointName;
		std::string mGeometryEntrypointName;
		std::string mFragmentEntrypointName;
		std::string mComputeEntrypointName;

		mutable std::vector<VkPipelineShaderStageCreateInfo> mShaderStageCreateInfos;
};

} // namespace Vulkan
} // namespace nu

inline nu::Vulkan::ShaderModule::ShaderStageFlags operator|(nu::Vulkan::ShaderModule::ShaderStageFlags a, nu::Vulkan::ShaderModule::ShaderStageFlags b)
{
	return static_cast<nu::Vulkan::ShaderModule::ShaderStageFlags>(static_cast<int>(a) | static_cast<int>(b));
}

#endif // NU_VULKAN_SHADER_MODULE_HPP