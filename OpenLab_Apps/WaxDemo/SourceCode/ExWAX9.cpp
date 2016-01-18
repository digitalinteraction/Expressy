////////////////////////////////////////////////////////////////////////////////
// ShWAX9

// Includes
#include "ExWAX9.h"
#include "RsUtil.h"
#include "BtMemory.h"
#include "UiKeyboard.h"
#include "BtQueue.h"

// Statics
MtVector3 ExWAX9::m_v3Accelerometer( 0, 0, 0 );
MtVector3 ExWAX9::m_v3Rotation( 0, 0, 0 );
MtQuaternion ExWAX9::m_quaternion( 0, 0, 0, 1 );
MtMatrix3 ExWAX9::m_m3Transform = MtMatrix3::m_identity;
BtBool ExWAX9::m_isReady = BtFalse;
BtBool ExWAX9::m_isReset = BtTrue;
BtQueue<ShWaxAction, 128> ExWAX9::m_actions;

////////////////////////////////////////////////////////////////////////////////
// Update

//static
void ExWAX9::Update()
{
	if( UiKeyboard::pInstance()->IsPressed( UiKeyCode_9 ) )
	{
		m_v3Accelerometer = MtVector3( 0, 9.8f, 0 );

		BtFloat length = m_v3Accelerometer.GetLength();
		(void)length;

		int a=0;
		a++;
	}
	else
	{
		m_v3Accelerometer = MtVector3( 0, 0, 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
// SetQuaternion

//static
void ExWAX9::SetQuaternion( MtQuaternion &quaternion )
{
    m_quaternion = quaternion;
    m_quaternion.Normalise();
    
    m_m3Transform = MtMatrix3( m_quaternion );
    
    m_isReady = BtTrue;
}

////////////////////////////////////////////////////////////////////////////////
// SetIMUAccelerometer

//static
void ExWAX9::SetIMUAccelerometer( MtVector3 &v3Accelerometer )
{
    m_v3Accelerometer = v3Accelerometer;
}

////////////////////////////////////////////////////////////////////////////////
// SetIMURotation

//static
void ExWAX9::SetIMURotation( MtVector3 &v3Rotation )
{
    m_v3Rotation = v3Rotation;
}

////////////////////////////////////////////////////////////////////////////////
// GetAccelerometer

//static
MtVector3 ExWAX9::GetAccelerometer()
{
    return m_v3Accelerometer;
}

////////////////////////////////////////////////////////////////////////////////
// GetTransform

//static
MtMatrix3 ExWAX9::GetTransform()
{
    if( m_isReset == BtTrue )
    {
        m_m3Transform.SetIdentity();
        m_isReset = BtFalse;
    }
    
    return m_m3Transform;
}

////////////////////////////////////////////////////////////////////////////////
// IsReady

//static
BtBool ExWAX9::IsReady()
{
    return m_isReady;
}

////////////////////////////////////////////////////////////////////////////////
// GetQuaternion

//static
MtQuaternion ExWAX9::GetQuaternion()
{
    return m_quaternion;
}

////////////////////////////////////////////////////////////////////////////////
// GetRotation

//static
MtVector3 ExWAX9::GetRotation()
{
    return m_v3Rotation;
}

////////////////////////////////////////////////////////////////////////////////
// GetRotationTransform

//static
MtMatrix4 ExWAX9::GetRotationTransform()
{
	return MtMatrix4( m_quaternion );
}
