#include "Camera.h"


Camera::Camera()
{
}


Camera::~Camera()
{
}


bool Camera::Init( const float ScrWidth, const float ScrHeight )
{
	m_AspectRatio = ScrWidth / ScrHeight;
	if( m_AspectRatio <= 0 )
		return false;

	m_FOV = DirectX::XM_PI / 4.0f;

	DirectX::XMStoreFloat4x4( &m_ProjectionMatrix, DirectX::XMMatrixPerspectiveFovLH( m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane ) );
	DirectX::XMStoreFloat4x4( &m_OrthograficMatrix, DirectX::XMMatrixOrthographicLH( ScrWidth, ScrHeight, m_NearPlane, m_FarPlane ) );

	m_Postion = m_CamPosition;

	return true;
}


void Camera::Update()
{
	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians
	const float Pitch = DirectX::XMConvertToRadians( m_CamRotation.x );
	const float Yaw = DirectX::XMConvertToRadians( m_CamRotation.y );
	const float Roll = DirectX::XMConvertToRadians( m_CamRotation.z );

	DirectX::XMMATRIX RotationMatrix;
	// Create the rotation matrix from the yaw, pitch, and roll values
	RotationMatrix = DirectX::XMMatrixRotationRollPitchYaw( Pitch, Yaw, Roll );

	// Transform vectors based on camera's rotation matrix
	DirectX::XMVECTOR WorldUp;
	DirectX::XMVECTOR WorldAhead;
	DirectX::XMVECTOR LocalUp = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );
	DirectX::XMVECTOR LocalAhead = DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );

	WorldUp = DirectX::XMVector3Transform( LocalUp, RotationMatrix );
	WorldAhead = DirectX::XMVector3Transform( LocalAhead, RotationMatrix );

	DirectX::XMVECTOR PosDelta, Pos, Vel;

	Pos = DirectX::XMLoadFloat3( &m_Postion );
	Vel = DirectX::XMLoadFloat3( &m_Velocity );

	// Move the eye position
	PosDelta = DirectX::XMVector3Transform( Vel, RotationMatrix );

	Pos = DirectX::XMVectorAdd( Pos, PosDelta );

	// Update the LookAt position based on the eye position
	DirectX::XMVECTOR LookAt;
	LookAt = DirectX::XMVectorAdd( Pos, WorldAhead );

	// Create and Store the ViewMatrix
	DirectX::XMStoreFloat4x4( &m_ViewMatrix, DirectX::XMMatrixLookAtLH( Pos, LookAt, WorldUp ) );
	DirectX::XMStoreFloat3( &m_Postion, Pos );
}