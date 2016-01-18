////////////////////////////////////////////////////////////////////////////////
// ShCamera.cpp

#include "ApConfig.h"
#include "BtTime.h"
#include "MtMatrix3.h"
#include "SbCamera.h"
#include "ShTouch.h"
#include "UiKeyboard.h"
#include "RsUtil.h"
#include "ShHMD.h"
#include "FsFile.h"
#include "HlKeyboard.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Init

void SbCamera::Init()
{
	BtFloat width = (BtFloat)RsUtil::GetWidth();
	BtFloat height = (BtFloat)RsUtil::GetHeight();

	m_cameraData.m_camera = RsCamera( 0.1f, 10000.0f, width / height, RsViewport( 0, 0, (BtU32)width, (BtU32)height ), MtDegreesToRadians( 60.0f ) );
	m_cameraData.m_camera.SetDimension( RsUtil::GetDimension() );
	m_cameraData.m_camera.SetPerspective( BtTrue );

	m_cameraData.m_v3Position = MtVector3( 0, 100, 0 );
	m_cameraData.m_m3Rotation.SetIdentity();

	m_cameraData.m_speed = 100.0f;
	m_cameraData.m_pitch = 0;
	m_cameraData.m_yaw = 0;
}

////////////////////////////////////////////////////////////////////////////////
// SetDimension

void SbCamera::SetDimension( const MtVector2 &v2Dimension )
{
	BtFloat width = v2Dimension.x;
	BtFloat height = v2Dimension.y;

	BtFloat fieldOfView = m_cameraData.m_camera.FieldOfView();
	BtFloat nearPlane   = m_cameraData.m_camera.NearPlane();
	BtFloat farPlane    = m_cameraData.m_camera.FarPlane();
	m_cameraData.m_camera = RsCamera( nearPlane, farPlane, width / height, RsViewport( 0, 0, (BtU32)width, (BtU32)height ), fieldOfView );
	m_cameraData.m_camera.SetDimension( v2Dimension );
	m_cameraData.m_camera.SetPerspective( BtTrue );
}

////////////////////////////////////////////////////////////////////////////////
// SetPosition

void SbCamera::SetPosition( const MtVector3 &v3Position )
{
	m_cameraData.m_v3Position = v3Position;
}

////////////////////////////////////////////////////////////////////////////////
// GetPosition

MtVector3 SbCamera::GetPosition()
{
	return m_cameraData.m_v3Position;
}

////////////////////////////////////////////////////////////////////////////////
// SetSpeed

void SbCamera::SetSpeed( BtFloat speed )
{
	m_cameraData.m_speed = speed;
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbCamera::Update()
{
	// Set the camera's orientation
    MtMatrix4 m4FinalRotation;
    m4FinalRotation.SetIdentity();

	// Set the position
	m_cameraData.m_camera.SetPosition( m_cameraData.m_v3Position );

	// Set the rotation
	m_cameraData.m_camera.SetRotation( m4FinalRotation );

	// Update the camera
	m_cameraData.m_camera.Update();
}

////////////////////////////////////////////////////////////////////////////////
// MoveUpward

void SbCamera::MoveRight( BtFloat speed )
{
	speed = BtTime::GetTick() * speed;

	MtMatrix3 m3Rotation = m_cameraData.m_camera.GetRotation();
	m_cameraData.m_v3Position += m3Rotation.Col0() * speed;
}

////////////////////////////////////////////////////////////////////////////////
// MoveDownward

void SbCamera::MoveLeft( BtFloat speed )
{
	speed = BtTime::GetTick() * -speed;

	MtMatrix3 m3Rotation = m_cameraData.m_camera.GetRotation();
	m_cameraData.m_v3Position += m3Rotation.Col0() * speed;
}

////////////////////////////////////////////////////////////////////////////////
// MoveForward

void SbCamera::MoveForward( BtFloat speed )
{
	speed = BtTime::GetTick() * speed;

	MtMatrix3 m3Rotation = m_cameraData.m_camera.GetRotation();
	m_cameraData.m_v3Position += m3Rotation.Col2() * speed;
}

////////////////////////////////////////////////////////////////////////////////
// MoveBackward

void SbCamera::MoveBackward( BtFloat speed )
{
	speed = BtTime::GetTick() * speed;

	MtMatrix3 m3Rotation = m_cameraData.m_camera.GetRotation();
	 m_cameraData.m_v3Position -= m3Rotation.Col2() * speed;
}

////////////////////////////////////////////////////////////////////////////////
// StrafeRight

void SbCamera::StrafeRight( BtFloat speed )
{
	speed = BtTime::GetTick() * speed;
	MtMatrix3 m3Rotation =  m_cameraData.m_camera.GetRotation();
	 m_cameraData.m_v3Position += m3Rotation.Col0() * speed;
}

////////////////////////////////////////////////////////////////////////////////
// StrafeLeft

void SbCamera::StrafeLeft( BtFloat speed )
{
	speed = BtTime::GetTick() * speed;
	MtMatrix3 m3Rotation =  m_cameraData.m_camera.GetRotation();
	m_cameraData.m_v3Position -= m3Rotation.Col0() * speed;
}

////////////////////////////////////////////////////////////////////////////////
// GetCamera

RsCamera &SbCamera::GetCamera()
{
	return m_cameraData.m_camera;
}
