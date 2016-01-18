////////////////////////////////////////////////////////////////////////////////
// SbWidget.cpp

// Includes
#include "SbWidget.h"
#include "ShTouch.h"
#include "BtTime.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor

SbWidget::SbWidget()
{
	Reset();
}

////////////////////////////////////////////////////////////////////////////////
// Reset

void SbWidget::Reset()
{
	m_firstTouchIndex = -1;
	m_secondTouchIndex = -1;
	m_initialLength = 0;
	m_v2LocalMouse = MtVector2( 0, 0 );
	m_m4WorldTransform.SetIdentity();
	m_m4Rotation.SetIdentity();
}

////////////////////////////////////////////////////////////////////////////////
// SetPosition

void SbWidget::SetPosition( const MtVector2& v2Position )
{
	MtVector3 v3Position( v2Position.x, v2Position.y, 0 );
	m_m4WorldTransform.SetTranslation( v3Position );
}

////////////////////////////////////////////////////////////////////////////////
// SetDimension

void SbWidget::SetDimension( const MtVector2& v2Dimension )
{
	m_v2Dimension = v2Dimension;
}

////////////////////////////////////////////////////////////////////////////////
// GetFirstTouchIndex

BtU32 SbWidget::GetFirstTouchIndex()
{
	return m_firstTouchIndex;
}

////////////////////////////////////////////////////////////////////////////////
// IsSelected

BtBool SbWidget::IsSelected() const
{
	if( m_firstTouchIndex != -1 )
	{
		return BtTrue;
	}
	return BtFalse;
}

////////////////////////////////////////////////////////////////////////////////
// IsSelected

BtBool SbWidget::IsSelected( BtU32 touchIndex ) const
{
	if( m_firstTouchIndex == touchIndex )
	{
		return BtTrue;
	}
	return BtFalse;
}

////////////////////////////////////////////////////////////////////////////////
// Select

void SbWidget::Select( BtU32 touchIndex )
{
	MtVector2 v2MousePosition = ShTouch::GetPosition( touchIndex );

	if( IsSelected() == BtFalse )
	{
		m_tick = BtTime::GetElapsedTimeInSeconds();

		// This bit is not selected - select it
		m_firstTouchIndex = touchIndex;
		m_v2Offset = v2MousePosition - m_m4WorldTransform.GetTranslation();
	}
	else if( ( m_secondTouchIndex == -1 ) && ( touchIndex != m_firstTouchIndex ) )
	{
		// Cache each position
		MtVector2 v2FirstMousePosition = ShTouch::GetPosition( m_firstTouchIndex );

		// It is already selected so set another touch position against i
		m_secondTouchIndex = touchIndex;

		// Get the distance between touch positions
		MtVector2 v2Delta = v2MousePosition - v2FirstMousePosition;

		// Get the starting length
		m_initialLength = v2Delta.GetLength();

		// Get the distance between touch positions
		m_initialAngle = MtATan( v2Delta.y, v2Delta.x );
	}
}

////////////////////////////////////////////////////////////////////////////////
// DropSecondTouch

void SbWidget::DropSecondTouch()
{
	// Drop the second touch
	m_secondTouchIndex = -1;
	m_v2LocalMouse = MtVector2( 0, 0 );

	Recentre();

	// HACK
	// Make sure we have a proper offset if we've just dropped the second touch
    if( m_firstTouchIndex != -1 )
    {
        MtVector2 v2MousePosition = ShTouch::GetPosition(m_firstTouchIndex);
        m_v2Offset = v2MousePosition - m_m4WorldTransform.GetTranslation();
    }
}

////////////////////////////////////////////////////////////////////////////////
// DropFirstTouch

void SbWidget::Recentre()
{
	// Reset the world translation to the centre of the object
	MtMatrix4 m4Temp;
	m4Temp.SetTranslation( MtVector3(0, 0, 0) * m_m4WorldTransform );

	// Calculate re-centered rotation
	MtVector3 a( -1, 0, 0);
	MtVector3 b(  1, 0, 0);

	a *= m_m4WorldTransform;
	b *= m_m4WorldTransform;

	// Calculate the rotation
	BtFloat rotation = MtATan(b.y - a.y, b.x - a.x);
	MtMatrix4 m4Rotation;
	m4Rotation.SetRotationZ( rotation );

	m_m4Rotation = m4Rotation * m_m4Rotation;

	// Reset the position
	m_m4WorldTransform = m4Temp;
}

////////////////////////////////////////////////////////////////////////////////
// DropFirstTouch

void SbWidget::DropFirstTouch()
{
	m_firstTouchIndex = -1;
}

////////////////////////////////////////////////////////////////////////////////
// UnSelect

BtBool SbWidget::UnSelect( BtU32 touchIndex )
{
	if( m_secondTouchIndex == touchIndex )
	{
		DropSecondTouch();
        return BtTrue;
	}
	if( m_firstTouchIndex == touchIndex )
	{	
		DropFirstTouch();
        return BtTrue;
	}
	return BtFalse;
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbWidget::Update()
{
	if( IsSelected() == BtTrue )
	{
		if(m_secondTouchIndex == -1)			// Do only first touch
		{
			// We have two fingers on this object. We need to rotate and scale it
			MtVector2 v2FirstMousePosition = ShTouch::GetPosition(m_firstTouchIndex);

			// Calculate the translation
			MtVector2 v2Translate = v2FirstMousePosition - m_v2Offset;

			// Set the world translation
			MtMatrix4 m4Translate;
			m4Translate.SetTranslation(v2Translate.x, v2Translate.y, 0);
			
			// Set the world transform
			m_m4WorldTransform = m4Translate;
		}
		else
		{
			// We have two fingers on this object. We need to rotate and scale it
			MtVector2 v2FirstMousePosition = ShTouch::GetPosition(m_firstTouchIndex);

			// Calculate the translation
			MtVector2 v2Translate = v2FirstMousePosition - m_v2Offset;

			// Set the world translation
			MtMatrix4 m4Translate;
			m4Translate.SetTranslation(v2Translate.x, v2Translate.y, 0);

			// We have two fingers on this object. We need to rotate and scale it
			MtVector2 v2SecondMousePosition = ShTouch::GetPosition( m_secondTouchIndex );

			// Get the distance between touch positions
			MtVector2 v2Delta = v2SecondMousePosition - v2FirstMousePosition;

			// Calculate the rotation
			BtFloat rotation = MtATan( v2Delta.y, v2Delta.x ) - m_initialAngle;

			// Set the rotation
			MtMatrix4 m4Rotation;
			m4Rotation.SetRotationZ( rotation );

			// Transform to new center of scale and rotation
			MtMatrix4 m4TranslateToCenter;
			m4TranslateToCenter.SetTranslation( m_v2Offset.x, m_v2Offset.y, 0 );

			// Set the matrix
			MtMatrix4 m4Temp;
			m4Temp = m4TranslateToCenter.GetInverse() * m4Rotation * m4TranslateToCenter;
			
			// We've moved the centre of rotation - fix it
			m_m4WorldTransform = m4Temp * m4Translate;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Rotate

void SbWidget::Rotate( BtFloat rotationInRadians )
{
	MtMatrix4 m4Temp;
	m4Temp.SetRotationZ( rotationInRadians );
//    m_m4Rotation = m_m4Rotation * m4Temp;
    m_m4Rotation = m4Temp * m_m4Rotation;// * m4Temp;
}

////////////////////////////////////////////////////////////////////////////////
// SetWorldTransform

void SbWidget::SetWorldTransform( MtMatrix4 m4Transform )
{
	m_m4WorldTransform = m4Transform;
}

////////////////////////////////////////////////////////////////////////////////
// GetWorldTransform

MtMatrix4 SbWidget::GetWorldTransform()
{
	return m_m4WorldTransform;
}

////////////////////////////////////////////////////////////////////////////////
// GetLocalPosition

MtVector2 SbWidget::GetLocalPosition( const MtVector2& v2Position )
{
	return v2Position * GetWorldTransform().GetInverse();
}