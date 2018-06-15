#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Common.h"
#include "InputHandler.h"


class Camera
{
public:
	Camera();
	~Camera();

	bool				Init( const float ScrWidth, const float ScrHeight );
	void				Update();
	void				Movement( const InputHandler& Input );

	void				SetPositon( const DirectX::XMFLOAT3 Position );
	DirectX::XMFLOAT3	GetPosition() const;

	void				SetRotation( const DirectX::XMFLOAT3 Rotation );
	DirectX::XMFLOAT3	GetRotation() const;

	void				LookAtPoint( const DirectX::XMFLOAT3 Point );

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

private:
	DirectX::XMFLOAT3 m_CamPosition = DirectX::XMFLOAT3( 0.0f, 0.0f, -5.0f );
	// Pitch around x, Yaw around y, Roll around z
	DirectX::XMFLOAT3 m_CamRotation = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );

	DirectX::XMFLOAT4X4 m_ProjectionMatrix;
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_OrthograficMatrix;

	float m_FOV			= 0.0f;
	float m_AspectRatio	= 0.0f;
	float m_NearPlane	= 0.1f;
	float m_FarPlane	= 1000.0f;

	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_Postion;
};


#endif // !_CAMERA_H_
