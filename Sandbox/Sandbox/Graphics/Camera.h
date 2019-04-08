#pragma once

#include <DirectXMath.h>



class Camera
{
	public:
		Camera();
		~Camera();


		void SetPosition(DirectX::XMVECTOR position);
		void SetPosition(float x, float y, float z);

		void SetLookDirection(DirectX::XMVECTOR direction);

		void SetFrustum(float viewAngle, float aspectRatio, float nearZ, float farZ);

		const DirectX::XMMATRIX& GetViewMatrix();
		const DirectX::XMMATRIX& GetProjectionMatrix();

	private:
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;

		//view matrix needed variables
		DirectX::XMVECTOR eyePosition;
		DirectX::XMVECTOR target;

		//projection
		float viewAngle;
		float ratio;
		float nearZ;
		float farZ;

		bool dirtyView;
		bool dirtyProjection;
		void CalculateViewMatrix();
		void CalculateProjectionMatrix();

};