#ifndef NU_VERTEX_BUFFER_HPP
#define NU_VERTEX_BUFFER_HPP

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"

#include <memory>

namespace nu
{

class VertexBuffer
{
	public:
		typedef std::unique_ptr<VertexBuffer> Ptr;

		static VertexBuffer::Ptr createVertexBuffer(Vulkan::Device& device);
		~VertexBuffer();

	private:
		VertexBuffer();

		Vulkan::Buffer::Ptr mBuffer;
};

} // namespace nu

#endif // NU_VERTEX_BUFFER_HPP