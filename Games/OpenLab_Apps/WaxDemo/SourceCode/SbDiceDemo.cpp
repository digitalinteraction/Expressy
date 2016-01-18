////////////////////////////////////////////////////////////////////////////////
// SbDiceDemo.cpp

// Includes
#include <stdio.h>
#include "BaArchive.h"
#include "SbDiceDemo.h"
#include "MtVector2.h"
#include "RsColour.h"
#include "RsFont.h"
#include "RsUtil.h"
#include "BtTime.h"
#include "UiKeyboard.h"
#include "HlDraw.h"
#include "ExWAX9.h"
#include "HlFont.h"
#include "ShTouch.h"
#include "RsMaterial.h"
#include "ApConfig.h"
#include "SbPiece.h"
////////////////////////////////////////////////////////////////////////////////
// Setup

void SbDiceDemo::Setup( BaArchive *pGameArchive )
{
	m_pCircle = pGameArchive->GetMaterial( "circle" );
}

////////////////////////////////////////////////////////////////////////////////
// Reset

void SbDiceDemo::Reset()
{
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbDiceDemo::Update()
{
	if( UiKeyboard::pInstance()->IsPressed( UiKeyCode_R ) )
	{
		Reset();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbDiceDemo::Render()
{
}
