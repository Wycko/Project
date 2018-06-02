#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Common.h"


class Camera
{
public:
	Camera();
	~Camera();

	bool Init( const float ScrWidth, const float ScrHeight );

	void Update();

	DirectX::XMFLOAT4X4 GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	DirectX::XMFLOAT4X4 GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	DirectX::XMFLOAT4X4 GetOrthograficMatrix() const
	{
		return m_OrthograficMatrix;
	}

	void setmove( const DirectX::XMFLOAT3 _move, const DirectX::XMFLOAT3 _rot )
	{
		m_CamRotation.x += _rot.x / 10;
		m_CamRotation.y += _rot.y / 10;
		m_CamRotation.z += _rot.z / 10;

		DirectX::XMVECTOR tmp;
		tmp = DirectX::XMLoadFloat3( &_move );
		tmp = DirectX::XMVectorScale( tmp, 0.005f );
		DirectX::XMStoreFloat3( &m_Velocity, tmp );
	}

private:
	DirectX::XMFLOAT3 m_CamPosition = DirectX::XMFLOAT3( 0.0f, 0.0f, -5.0f );
	// Pitch around x, Yaw around y, Roll around z
	DirectX::XMFLOAT3 m_CamRotation = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );

	DirectX::XMFLOAT4X4 m_ProjectionMatrix;
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_OrthograficMatrix;

	float m_FOV = 0.0f;
	float m_AspectRatio = 0.0f;
	float m_NearPlane = 0.1f;
	float m_FarPlane = 1000.0f;

	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_Postion;
};


#endif // !_CAMERA_H_
