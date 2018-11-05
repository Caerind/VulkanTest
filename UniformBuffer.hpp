#ifndef NU_UNIFORM_BUFFER_HPP
#define NU_UNIFORM_BUFFER_HPP

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"

#include <memory>

namespace nu
{

class UniformBuffer
{
	public:
		typedef std::unique_ptr<UniformBuffer> Ptr;

		static UniformBuffer::Ptr createUniformBuffer(Vulkan::Device& device);
		~UniformBuffer();

	private:
		UniformBuffer();

		Vulkan::Buffer::Ptr mBuffer;
};

} // namespace nu

#endif // NU_UNIFORM_BUFFER_HPP