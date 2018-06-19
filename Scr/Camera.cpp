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
	const float Pitch = m_CamRotation.y;
	const float Yaw = m_CamRotation.x;
	const float Roll = m_CamRotation.z;

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


void Camera::Movement( const InputHandler& Input )
{
	if( Input.MouseIsPressed( Input.Mouse_Left_Button ) )
	{
		auto mouse = Input.GetMouse();
		m_CamRotation.x += DirectX::XMConvertToRadians( mouse.x / 10 );
		if( m_CamRotation.x > DirectX::XM_PI )
			m_CamRotation.x = -DirectX::XM_PI;

		else if( m_CamRotation.x < -DirectX::XM_PI )
			m_CamRotation.x = DirectX::XM_PI;

		m_CamRotation.y += DirectX::XMConvertToRadians( mouse.y / 10 );
		if( m_CamRotation.y > DirectX::XM_PI )
			m_CamRotation.y = -DirectX::XM_PI;

		else if( m_CamRotation.y < -DirectX::XM_PI )
			m_CamRotation.y = DirectX::XM_PI;

		m_CamRotation.z += DirectX::XMConvertToRadians( mouse.z / 10 );
		if( m_CamRotation.z > DirectX::XM_PI )
			m_CamRotation.z = -DirectX::XM_PI;

		else if( m_CamRotation.z < -DirectX::XM_PI )
			m_CamRotation.z = DirectX::XM_PI;
	}

	DirectX::XMFLOAT3 _move = { 0.0f, 0.0f, 0.0f };
	if( Input.KeyIsPressed( Input.Movement_Forward ) )
		_move.z += 1.0f;
	if( Input.KeyIsPressed( Input.Movement_Backward ) )
		_move.z -= 1.0f;
	if( Input.KeyIsPressed( Input.Movement_Right ) )
		_move.x += 1.0f;
	if( Input.KeyIsPressed( Input.Movement_Left ) )
		_move.x -= 1.0f;
	if( Input.KeyIsPressed( Input.Movement_Up ) )
		_move.y += 1.0f;
	if( Input.KeyIsPressed( Input.Movement_Down ) )
		_move.y -= 1.0f;


	DirectX::XMVECTOR tmp;
	tmp = DirectX::XMLoadFloat3( &_move );
	tmp = DirectX::XMVectorScale( tmp, 0.005f );
	DirectX::XMStoreFloat3( &m_Velocity, tmp );
}


void Camera::SetPositon( const DirectX::XMFLOAT3 Position )
{
	m_Postion = Position;
}


DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return m_Postion;
}


void Camera::SetRotation( const DirectX::XMFLOAT3 Rotation )
{	
	m_CamRotation.x = DirectX::XMConvertToRadians( Rotation.x );
	m_CamRotation.y = DirectX::XMConvertToRadians( -Rotation.y );
	m_CamRotation.z = DirectX::XMConvertToRadians( Rotation.z );
}


DirectX::XMFLOAT3 Camera::GetRotation() const
{
	DirectX::XMFLOAT3 temp;

	temp.x = DirectX::XMConvertToDegrees( m_CamRotation.x );
	temp.y = DirectX::XMConvertToDegrees( -m_CamRotation.y );
	temp.z = DirectX::XMConvertToDegrees( m_CamRotation.z );

	return temp;
}


void Camera::LookAtPoint( const DirectX::XMFLOAT3 Point )
{
	DirectX::XMVECTOR LookTo = DirectX::XMVectorSubtract( DirectX::XMLoadFloat3( &Point ), DirectX::XMLoadFloat3( &m_Postion ) );

	auto YawRadian = DirectX::XMVectorGetX( DirectX::XMVectorATan2( DirectX::XMVectorSplatX( LookTo ), DirectX::XMVectorSplatZ( LookTo ) ) );
	auto PitchRadian = DirectX::XMVectorGetX( DirectX::XMVectorATan2( DirectX::XMVectorSplatY( LookTo ), DirectX::XMVectorSplatX( LookTo ) ) );

	if( YawRadian < 0.0f )
	{
		if( PitchRadian > 0.0f )
			PitchRadian = DirectX::XM_PI - PitchRadian;

		else
		{
			PitchRadian += DirectX::XM_PI;
			PitchRadian *= -1.0f;
		}
	}

	m_CamRotation.z = 0.0f;
	m_CamRotation.x = YawRadian;
	m_CamRotation.y = -PitchRadian;
}


void Camera::CamReset()
{
	SetRotation( { 0.0f, 0.0f, 0.0f } );
	SetPositon( { 0.0f, 0.0f, 0.0f } );
}
