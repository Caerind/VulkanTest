#pragma once

#include "Camera.hpp"

class OrbitingCamera : public Camera
{
	public:
		virtual const nu::Vector3f& getTarget() const final;
		virtual float getDistance() const final;
		virtual float getHorizontalAngle() const final;
		virtual float getVerticalAngle() const final;

		void setDistance(float distance);
		void rotateHorizontally(float angle);
		void rotateVertically(float angle);

		OrbitingCamera();
		OrbitingCamera(const nu::Vector3f& target,
					   float distance,
					   float horizontalAngle = 0.0f,
					   float verticalAngle = 0.0f);
		OrbitingCamera(const OrbitingCamera& other);
		virtual ~OrbitingCamera();

		OrbitingCamera& operator=(const OrbitingCamera& other);

	private:
		nu::Vector3f mTarget;
		float mDistance;
		float mHorizontalAngle;
		float mVerticalAngle;
};
