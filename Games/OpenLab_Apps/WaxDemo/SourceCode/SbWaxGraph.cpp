////////////////////////////////////////////////////////////////////////////////
// SbWaxGraph.cpp

// Includes
#include "BaArchive.h"
#include "SbWaxGraph.h"
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

const BtU32 items = 100;
static MtVector3 v3Values[items];

////////////////////////////////////////////////////////////////////////////////
// Setup

void SbWaxGraph::Setup( BaArchive *pGameArchive )
{
	// Cache our model
	m_pWaxModel = pGameArchive->GetNode( "cube" );

	// Cache the main shader
	m_pShader = pGameArchive->GetShader( "shader" );
	m_pShader->SetDirectionalLight( MtVector3( 0, 0, 0 ) );
	m_pShader->SetAmbient( RsColour( 1.0f, 1.0f, 1.0f, 1.0f ) );

	m4Rotate.SetIdentity();
    
    for( BtU32 i=0; i<items; i++ )
    {
        v3Values[i] = MtVector3( 0, 0, 0 );
    }
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbWaxGraph::Update()
{
	MtMatrix4 m4Transform = MtMatrix4( ExWAX9::GetTransform() );
    
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
// AddIMUData

void SbWaxGraph::AddIMUData()
{
    MtVector3 v3Accel = ExWAX9::GetAccelerometer();
    
    static int a = 0;
    
    if( a == 0 )
    {
        v3Values[items-1]= v3Accel;
        
        for( BtU32 i=0; i<items-1; i++ )
        {
            v3Values[i] = v3Values[i+1];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// RenderGraph

void SbWaxGraph::RenderGraph()
{
    //AddIMUData();
    
    BtFloat width = RsUtil::GetWidth();
    
    BtFloat height = RsUtil::GetHeight();
    BtFloat halfHeight = height / 2.0f;
    
    BtFloat notches = (BtFloat)items;
    BtFloat notchWidth  = width / notches;
    BtFloat notchHeight = height / 100.0f;
    
    MtVector2 v2Start( notchWidth, height / 2.0f );
    
    HlDraw::RenderLine( MtVector2( 0, halfHeight ), MtVector2( width, halfHeight ), RsColour::WhiteColour() );
    
    for( BtU32 i=0; i<items; i++ )
    {
        MtVector2 v2A = v2Start - MtVector2( 0, notchHeight );
        MtVector2 v2B = v2Start + MtVector2( 0, notchHeight );
        HlDraw::RenderLine( v2A, v2B, RsColour::WhiteColour() );
    }
    
    for( BtU32 i=0; i<items-1; i++ )
    {
        MtVector3 v3Acc1 = v3Values[i];
        v3Acc1 = v3Acc1 * height / 3.0f;
        
        MtVector3 v3Acc2 = v3Values[i+1];
        v3Acc2 = v3Acc2 * height / 3.0f;
        
        MtVector2 v2A = v2Start + MtVector2( 0, v3Acc1.x );
        MtVector2 v2B = v2Start + MtVector2( notchWidth, v3Acc2.x );
        HlDraw::RenderLine( v2A, v2B, RsColour::RedColour() );

        v2A = v2Start + MtVector2( 0, v3Acc1.y );
        v2B = v2Start + MtVector2( notchWidth, v3Acc2.y );
        HlDraw::RenderLine( v2A, v2B, RsColour::GreenColour() );
        
        v2A = v2Start + MtVector2( 0, v3Acc1.z );
        v2B = v2Start + MtVector2( notchWidth, v3Acc2.z );
        HlDraw::RenderLine( v2A, v2B, RsColour::BlueColour() );

        v2Start.x += (int)notchWidth;
    }
    
    MtVector3 v3Accel = ExWAX9::GetAccelerometer();
    BtChar text[100];
    sprintf( text, "%.2f\n%.2f\n%.2f", v3Accel.x, v3Accel.y, v3Accel.z );
    HlFont::RenderHeavy( MtVector2( 0, 100 ), text, MaxSortOrders-1 );
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbWaxGraph::Render()
{
	// Apply the shader
	m_pShader->Apply();

	// Render the cube
	m_pWaxModel->Render();
    
    // Render the graph
    RenderGraph();
    
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
