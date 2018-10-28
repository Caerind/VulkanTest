#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../Math/Matrix4.hpp"

class Camera 
{
	public:
		virtual const nu::Matrix4f& getMatrix() const final;
		virtual const nu::Vector3f& getPosition() const final;
		virtual const nu::Vector3f& getRightVector() const final;
		virtual const nu::Vector3f& getUpVector() const final;
		virtual const nu::Vector3f& getForwardVector() const final;

	protected:
		Camera();
		Camera(const nu::Vector3f& position,
			   const nu::Vector3f& rightVector,
			   const nu::Vector3f& upVector,
			   const nu::Vector3f& forwardVector);
		Camera(const Camera& camera);
		virtual ~Camera() = 0;

		Camera& operator=(const Camera& camera);

		mutable nu::Matrix4f mViewMatrix;
		nu::Vector3f mPosition;
		nu::Vector3f mRightVector;
		nu::Vector3f mUpVector;
		nu::Vector3f mForwardVector;
		mutable bool mDirty;
};

#endif // CAMERA_HPP