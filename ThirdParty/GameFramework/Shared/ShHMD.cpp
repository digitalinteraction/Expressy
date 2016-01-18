////////////////////////////////////////////////////////////////////////////////
// ShHMD.cpp

// Includes
#include "ShHMD.h"
#include "RsTexture.h"
#include "RsMaterial.h"

BtBool		 ShHMD::m_isHMD = BtFalse;
RsMaterial  *ShHMD::m_materials[2];
MtMatrix4 	 ShHMD::m_projection;
MtVector3	 ShHMD::m_v3Position;
MtVector2	 ShHMD::OculusDimension( 0, 0 );
MtQuaternion ShHMD::m_quaternion( 0, 0, 0, 1 );
MtMatrix3	 ShHMD::m_m3Rotation;
//BtBool		 ShHMD::m_isMirror = BtFalse;
BtBool		 ShHMD::m_isMirror = BtTrue;

////////////////////////////////////////////////////////////////////////////////
// SetHMD

void ShHMD::SetHMD( BtBool isEnabled )
{
	m_isHMD = isEnabled;
}

////////////////////////////////////////////////////////////////////////////////
// IsHMD

BtBool ShHMD::IsHMD()
{
	return m_isHMD;
}

////////////////////////////////////////////////////////////////////////////////
// IsMirror

BtBool ShHMD::IsMirror()
{
	return m_isMirror;
}

////////////////////////////////////////////////////////////////////////////////
// SetMirror

void ShHMD::SetMirror( BtBool isMirror )
{
	m_isMirror = isMirror;
}

////////////////////////////////////////////////////////////////////////////////
// SetQuaternion

void ShHMD::SetQuaternion( MtQuaternion &quaternion )
{
	m_quaternion = quaternion;
}

////////////////////////////////////////////////////////////////////////////////
// GetQuaternion

MtQuaternion ShHMD::GetQuaternion()
{
	return m_quaternion;
}

////////////////////////////////////////////////////////////////////////////////
// SetPosition

void ShHMD::SetPosition( MtVector3 &v3Position )
{
	m_v3Position = v3Position;
}

////////////////////////////////////////////////////////////////////////////////
// GetPosition

MtVector3 ShHMD::GetPosition()
{
	return m_v3Position;
}

////////////////////////////////////////////////////////////////////////////////
// GetRotation

MtMatrix3 ShHMD::GetRotation()
{
	return MtMatrix3( m_quaternion );
}

////////////////////////////////////////////////////////////////////////////////
// SetDimension

void ShHMD::SetDimension( MtVector2 v2Dimension )
{
	OculusDimension = v2Dimension;
}

////////////////////////////////////////////////////////////////////////////////
// GetDimension

MtVector2 ShHMD::GetDimension()
{
	//return m_pLeftEye->GetDimension();
	return OculusDimension;
}

////////////////////////////////////////////////////////////////////////////////
// GetLeftEye

RsMaterial *ShHMD::GetMaterial( BtU32 index )
{
	BtAssert( m_materials[index] != BtNull );
	return m_materials[index];
}

////////////////////////////////////////////////////////////////////////////////
// SetLeftEye

void ShHMD::SetMaterial( BtU32 index, RsMaterial *pMaterial )
{
	m_materials[index] = pMaterial;
}

////////////////////////////////////////////////////////////////////////////////
// SetProjectionMatrix

void ShHMD::SetProjectionMatrix( const MtMatrix4 &m4Projection)
{
	m_projection = m4Projection;
}

////////////////////////////////////////////////////////////////////////////////
// GetLeftEye

MtMatrix4 ShHMD::GetProjectionMatrix()
{
	return m_projection;
}
