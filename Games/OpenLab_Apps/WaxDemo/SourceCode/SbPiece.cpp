////////////////////////////////////////////////////////////////////////////////
// SbPiece.cpp

// Includes
#include "SbPiece.h"
#include "ShTouch.h"
#include "BtTime.h"
#include "HlDraw.h"
#include "HlFont.h"
#include "UiKeyboard.h"
#include "ApConfig.h"
#include "ExWAX9.h"
#include <stdio.h>

static BtU32 PieceID = 0;

////////////////////////////////////////////////////////////////////////////////
// Constructor

SbPiece::SbPiece()
{
	Reset();
	m_height = 0;
	m_pieceID = PieceID;
	++PieceID;
}

////////////////////////////////////////////////////////////////////////////////
// Strip

void Strip( MtMatrix4 &m4Transform )
{
	MtMatrix4 m4NewTransform;
	m4NewTransform.SetTranslation( m4Transform.GetTranslation() );
	m4Transform = m4NewTransform;
}

////////////////////////////////////////////////////////////////////////////////
// ConstructExtents

void SbPiece::ConstructExtents()
{
	// Select the piece
	MtVector2 v2Half = m_v2Dimension * 0.5f;

	m_v3Pos[0] = MtVector3(-v2Half.x, -v2Half.y, 0);
	m_v3Pos[1] = MtVector3(-v2Half.x,  v2Half.y, 0);
	m_v3Pos[2] = MtVector3( v2Half.x, -v2Half.y, 0);
	m_v3Pos[3] = MtVector3( v2Half.x,  v2Half.y, 0);

	m_v3Pos[0] = m_v3Pos[0] * m_m4Rotation;
	m_v3Pos[1] = m_v3Pos[1] * m_m4Rotation;
	m_v3Pos[2] = m_v3Pos[2] * m_m4Rotation;
	m_v3Pos[3] = m_v3Pos[3] * m_m4Rotation;

	m_v3Pos[0] = m_v3Pos[0] * m_m4WorldTransform;
	m_v3Pos[1] = m_v3Pos[1] * m_m4WorldTransform;
	m_v3Pos[2] = m_v3Pos[2] * m_m4WorldTransform;
	m_v3Pos[3] = m_v3Pos[3] * m_m4WorldTransform;

	MtVector3 v3Normal[4];
	for(BtU32 i = 0; i < 4; i++)
	{
		v3Normal[i] = m_v3Pos[i];
		v3Normal[i].z = -1.0f;

		m_v2Pos[i].x = m_v3Pos[i].x;
		m_v2Pos[i].y = m_v3Pos[i].y;
	}

	plane[0] = MtPlane(m_v3Pos[0], m_v3Pos[2], v3Normal[2]);
	plane[1] = MtPlane(m_v3Pos[3], m_v3Pos[1], v3Normal[1]);
	plane[2] = MtPlane(m_v3Pos[1], m_v3Pos[0], v3Normal[1]);
	plane[3] = MtPlane(m_v3Pos[2], m_v3Pos[3], v3Normal[3]);
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbPiece::Update()
{
	if( IsSelected() )
	{
		if( UiKeyboard::pInstance()->IsHeld(UiKeyCode_T) )
		{
			Rotate( -0.01f );
		}
		if( UiKeyboard::pInstance()->IsHeld(UiKeyCode_Y) )
		{
			Rotate( 0.01f );
		}
	}

	SbWidget::Update();

	// Calculate the tab positions
	MtMatrix4 m4Translate;

	m4Translate.SetTranslation( MtVector3(0, m_v2Dimension.y / 2, 0) );
	m_tabs[North] = m4Translate;

	m4Translate.SetTranslation( MtVector3(m_v2Dimension.x / 2, 0, 0) );
	m_tabs[East]  = m4Translate;

	m4Translate.SetTranslation( MtVector3(0, -m_v2Dimension.y / 2, 0) );
	m_tabs[South] = m4Translate;

	m4Translate.SetTranslation( MtVector3(-m_v2Dimension.x / 2, 0, 0) );
	m_tabs[West]  = m4Translate;

	for(BtU32 i = 0; i < 4; i++)
	{
		m_translatedTabs[i] = m_tabs[i] * m_m4Rotation * m_m4WorldTransform;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbPiece::Render()
{
	MtVector2 v2Pos[4];
	for( BtU32 i=0; i<4; i++ )
	{
		v2Pos[i] = MtVector2( m_v3Pos[i].x, m_v3Pos[i].y );
	}

	HlDraw::RenderLine( v2Pos[0], v2Pos[1], RsColour::BlackColour(), MaxSortOrders-1 );
	HlDraw::RenderLine( v2Pos[2], v2Pos[3], RsColour::BlackColour(), MaxSortOrders-1  );
	HlDraw::RenderLine( v2Pos[1], v2Pos[3], RsColour::BlackColour(), MaxSortOrders - 1);
	HlDraw::RenderLine( v2Pos[2], v2Pos[0], RsColour::BlackColour(), MaxSortOrders - 1);

	if(ApConfig::IsDebug())
	{
		BtChar text[32];
		sprintf( text, "%d", m_pieceID );
		MtVector3 v3Position = m_m4WorldTransform.GetTranslation();
		MtVector2 v2Position( v3Position.x, v3Position.y );
		HlFont::Render( v2Position, text, RsColour::WhiteColour(), MaxSortOrders-1 );
	}
}
