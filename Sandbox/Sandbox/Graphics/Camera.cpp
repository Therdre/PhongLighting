#include "Camera.h"

using namespace DirectX;

Camera::Camera(): viewMatrix(XMMatrixIdentity()), projectionMatrix(XMMatrixIdentity()),
	              eyePosition(), target(),
				  viewAngle(0.0f), ratio(0.0f),nearZ(0.0f),farZ(0.0f),
				  dirtyProjection(false),dirtyView(false)

{

}
Camera::~Camera()
{
	
}


void Camera::SetPosition(DirectX::XMVECTOR position)
{
	eyePosition = position;
	dirtyView = true;
}

void Camera::SetPosition(float x, float y, float z)
{
	eyePosition = XMVectorSet(x, y, z,1.0f);
	dirtyView = true;

}

void Camera::SetLookDirection(DirectX::XMVECTOR target)
{
	this->target = target;
	dirtyView = true;
}

void Camera::SetFrustum(float viewAngle, float aspectRatio, float nearZ, float farZ)
{
	this->viewAngle = viewAngle;
	this->ratio = aspectRatio;
	this->nearZ = nearZ;
	this->farZ = farZ;

	dirtyProjection = true;
	CalculateProjectionMatrix();
}

const DirectX::XMMATRIX& Camera::GetViewMatrix()
{
	if (dirtyView)
	{
		CalculateViewMatrix();
		dirtyView = false;
	}
	return viewMatrix;
}
const DirectX::XMMATRIX& Camera::GetProjectionMatrix()
{
	if (dirtyProjection)
	{
		CalculateProjectionMatrix();
		dirtyProjection = false;
	}
	return projectionMatrix;
}

void Camera::CalculateViewMatrix()
{
	XMVECTOR up = XMVector3Normalize(XMVector3Cross(eyePosition, target));
	viewMatrix = XMMatrixLookAtLH(eyePosition, target, XMVectorSet(0.0f,1.0f,0.0f,0.0f));
}

void Camera::CalculateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(viewAngle, ratio, nearZ, farZ);
}