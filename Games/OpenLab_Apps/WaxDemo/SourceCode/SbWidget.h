////////////////////////////////////////////////////////////////////////////////
// SbWidget.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "MtVector2.h"
#include "MtMatrix4.h"

////////////////////////////////////////////////////////////////////////////////

class RsTexture;
class UiInput;
class BaArchive;
class RsSprite;
class RsMaterial;

// Class definition
class SbWidget
{
public:

	// Constructor
	SbWidget();

	// Public functions
	void									Select( BtU32 touchIndex );
	BtBool									UnSelect( BtU32 touchIndex );
	void									Update();
	void									Reset();
	void									Rotate( BtFloat rotate );

	// Accessors
	void									SetPosition( const MtVector2& v2Position );
	void									SetDimension( const MtVector2& v2Dimension );	
	BtU32									GetFirstTouchIndex();
	BtBool									IsSelected() const;
	BtBool									IsSelected( BtU32 touchIndex ) const;
	MtVector2								GetScale();
	MtVector2								GetLocalPosition( const MtVector2& v2Position );
	MtMatrix4								GetWorldTransform();
	void									SetWorldTransform( MtMatrix4 m4Transform );
	void									SetWorldTranslate( MtMatrix4 m4Transform );
	
//protected:

	// Private functions
	void									Recentre();
	void									DropFirstTouch();
	void									DropSecondTouch();

	// Members
	MtVector2								m_v2Dimension;
	MtVector2								m_v2Offset;
	MtMatrix4								m_m4WorldTransform;
	MtVector2								m_v2LocalMouse;
	MtMatrix4								m_m4Rotation;
	BtS32									m_firstTouchIndex;
	BtS32									m_secondTouchIndex;
	BtFloat									m_initialAngle;
	BtFloat									m_initialLength;
	BtFloat									m_tick;
};
