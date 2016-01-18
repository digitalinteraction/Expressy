////////////////////////////////////////////////////////////////////////////////
// ShWAX9

// Include guard
#pragma once

// Includes
#include "BtBase.h"
#include "BtTypes.h"
#include "MtVector2.h"
#include "MtQuaternion.h"
#include "MtMatrix4.h"
#include "MtMatrix3.h"
#include "BtQueue.h"

enum ShWaxActionType
{
    ShCameraActionType_Reset,
};

struct ShWaxAction
{
    ShWaxActionType             m_action;
};

// Class definition
class ExWAX9
{
public:

	static void                         Update();
    
	// Accessors
    static void                         SetQuaternion( MtQuaternion &quaternion );
    static void                         SetIMUAccelerometer( MtVector3 &v3Accelerometer );
    static void                         SetIMURotation( MtVector3 &v3Rotation );
    static void                         SetAttitude( MtVector3 &v3Attitude );
	static MtMatrix4                    GetRotationTransform();
    static MtVector3                    GetRotation();
    static MtVector3                    GetAccelerometer();
    static MtQuaternion                 GetQuaternion();
    static MtMatrix3                    GetTransform();
    static BtBool                       IsReady();
    
    static void                         PushAction( ShWaxAction action );
    static ShWaxAction                  PopAction();
    static BtU32                        GetNumItems();
    
    // State functions
    
    // Public functions
private:
    
        // Private members
    static BtQueue<ShWaxAction, 128>    m_actions;
    static MtVector3                    m_v3Accelerometer;
	static MtVector3                    m_v3Rotation;
    static MtVector3                    m_v3Attitude;
    static MtQuaternion                 m_quaternion;
    static MtMatrix3                    m_m3Transform;
    static BtBool                       m_isReady;
    static BtBool                       m_isReset;
};
