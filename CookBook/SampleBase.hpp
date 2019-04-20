#pragma once

#include <chrono>
#include <vector>

#include "Common.hpp" // TODO : Remove include

#include "../VulkanWrapper/VulkanWrapper.hpp"

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

		virtual bool initialize(VulkanWindowParameters windowParameters) = 0;
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

		virtual bool initializeVulkan(VulkanWindowParameters windowParameters, VkPhysicalDeviceFeatures* desiredDeviceFeatures = nullptr) final;

	private:
		bool mReady;

	protected:
		MouseStateParameters mMouseState;
		TimerStateParameters mTimerState;

		VulkanSurfacePtr mSurface;
		VulkanSwapchainPtr mSwapchain;
		VulkanCommandPoolPtr mCommandPool;
		VulkanQueue* mGraphicsQueue;
		VulkanQueue* mComputeQueue;
		VulkanQueue* mPresentQueue;
		std::vector<FrameResources> mFramesResources;
};