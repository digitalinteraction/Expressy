////////////////////////////////////////////////////////////////////////////////
// SbWaxModel.cpp

// Includes
#include "BaArchive.h"
#include "SbWaxModel.h"
#include "MtVector2.h"
#include "MtVector3.h"
#include "RsCamera.h"
#include "RsColour.h"
#include "RsFont.h"
#include "RsShader.h"
#include "RsUtil.h"
#include "RsRenderTarget.h"
#include "SgNode.h"
#include "MtAABB.h"
#include "SgMesh.h"
#include "SgAnimator.h"
#include "BtTime.h"
#include <stdio.h>
#include "UiKeyboard.h"
#include "HlDraw.h"
#include "ApConfig.h"
#include "SgBone.h"
#include "HlModel.h"
#include "ExWAX9.h"
#include "HlFont.h"
#include "BtCollection.h"
#include "ShTouch.h"

MtMatrix4 m4Rotate;

////////////////////////////////////////////////////////////////////////////////
// Setup

void SbWaxModel::Setup( BaArchive *pGameArchive )
{
	// Cache our model
	m_pWaxModel = pGameArchive->GetNode( "cube" );

	// Cache the main shader
	m_pShader = pGameArchive->GetShader( "shader" );
	m_pShader->SetDirectionalLight( MtVector3( 0, 0, 0 ) );
	m_pShader->SetAmbient( RsColour( 1.0f, 1.0f, 1.0f, 1.0f ) );

	m4Rotate.SetIdentity();
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbWaxModel::Update()
{
    MtMatrix3 m3Transform = ExWAX9::GetTransform();
	MtMatrix4 m4Transform = MtMatrix4( m3Transform );
     
	if( UiKeyboard::pInstance()->IsHeld( UiKeyCode_Z ) )
	{
		MtMatrix4 m4Temp;
		m4Temp.SetRotationY( MtDegreesToRadians( 1.0f ) );
		m4Rotate = m4Temp * m4Rotate;
	}

	if( UiKeyboard::pInstance()->IsHeld( UiKeyCode_X ) )
	{
		MtMatrix4 m4Temp;
		m4Temp.SetRotationY(MtDegreesToRadians(-1.0f));
		m4Rotate = m4Temp * m4Rotate;
	}
	m4Transform = m4Rotate * m4Transform;

 	m_pWaxModel->SetLocalTransform( m4Transform );
	m_pWaxModel->Update();
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbWaxModel::Render()
{
	// Apply the shader
	m_pShader->Apply();

	// Render the cube
	m_pWaxModel->Render();
    
    MtMatrix4 m4Transform = m_pWaxModel->GetWorldTransform();
    MtVector3 v3Translate = m4Transform.GetTranslation();
   
    MtVector3 v3Pos( 0, 0, 1 );
    v3Pos = v3Pos * m4Transform;
    HlDraw::RenderLine( v3Translate, v3Pos, RsColour::GreenColour() );

    v3Pos = MtVector3( 1, 0, 0 );
    v3Pos = v3Pos * m4Transform;
    HlDraw::RenderLine( v3Translate, v3Pos, RsColour::RedColour() );
    
    v3Pos = MtVector3( 0, 1, 0 );
    v3Pos = v3Pos * m4Transform;
    HlDraw::RenderLine( v3Translate, v3Pos, RsColour::BlueColour() );
}
