#ifndef SAMPLE_BASE_HPP
#define SAMPLE_BASE_HPP

#include <chrono>
#include <vector>

#include "Common.h" // TODO : Remove include

#include "../VulkanInstance.hpp"
#include "../VulkanDevice.hpp"
#include "../VulkanSurface.hpp"
#include "../VulkanSwapchain.hpp"
#include "../VulkanCommandPool.hpp"

class MouseStateParameters 
{
	public:
		MouseStateParameters();
		~MouseStateParameters();

		struct ButtonsState 
		{
			bool isPressed;
			bool wasClicked;
			bool wasRelease;
		} buttons[2];

		struct Position 
		{
			int x;
			int y;
			struct Delta 
			{
				int x;
				int y;
			} delta;
		} position;

		struct WheelState 
		{
			bool wasMoved;
			float distance;
		} wheel;
};

class TimerStateParameters 
{
	public:
		TimerStateParameters();
		~TimerStateParameters();

		float getTime() const;
		float getDeltaTime() const;

		void update();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mTime;
		std::chrono::duration<float> mDeltaTime;
};

class SampleBase
{
	public:
		SampleBase();
		virtual ~SampleBase();

		virtual bool initialize(nu::Vulkan::WindowParameters windowParameters) = 0;
		virtual bool draw() = 0;
		virtual bool resize() = 0;
		virtual void deinitialize() final;
		virtual void mouseClick(size_t buttonIndex, bool state) final;
		virtual void mouseMove(int x, int y) final;
		virtual void mouseWheel(float distance) final;
		virtual void mouseReset() final;
		virtual void updateTime() final;
		virtual bool isReady() final;
		virtual void wait() final;

	protected:
		virtual void onMouseEvent();

		virtual bool initializeVulkan(nu::Vulkan::WindowParameters windowParameters, VkPhysicalDeviceFeatures* desiredDeviceFeatures = nullptr) final;

	private:
		bool mReady;

	protected:
		MouseStateParameters mMouseState;
		TimerStateParameters mTimerState;

		nu::Vulkan::Instance::Ptr mInstance;
		nu::Vulkan::PhysicalDevice* mPhysicalDevice;
		nu::Vulkan::Device::Ptr mLogicalDevice;
		nu::Vulkan::Surface::Ptr mSurface;
		nu::Vulkan::Swapchain::Ptr mSwapchain;
		nu::Vulkan::CommandPool::Ptr mCommandPool;
		nu::Vulkan::Queue::Ptr mGraphicsQueue;
		nu::Vulkan::Queue::Ptr mComputeQueue;
		nu::Vulkan::Queue::Ptr mPresentQueue;
		std::vector<FrameResources> mFramesResources;
};

#endif // SAMPLE_BASE_HPP