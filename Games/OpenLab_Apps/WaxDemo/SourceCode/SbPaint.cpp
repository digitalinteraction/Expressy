////////////////////////////////////////////////////////////////////////////////
// SbPaint.cpp

// Includes
#include "SbPaint.h"
#include "RsRenderTarget.h"
#include "HlFont.h"
#include "ShTouch.h"
#include "HlDraw.h"
#include "RsUtil.h"
#include "RsTexture.h"
#include "RsMaterial.h"
#include "UiKeyboard.h"
#include "ExWAX9.h"
#include "BtTime.h"
#include <stdio.h>

RsMaterial *SbPaint::m_pRenderTarget;
const BtFloat MaxThickness = 5.0f;

////////////////////////////////////////////////////////////////////////////////
// Setup

void SbPaint::Setup( BaArchive *pArchive )
{
	m_pRenderTarget = pArchive->GetMaterial( "paint" );
	m_pBrush = pArchive->GetMaterial( "circle" );
	m_isClear = BtTrue;
    m_isRender = BtFalse;
    m_width = m_pRenderTarget->Width();
    m_height = m_pRenderTarget->Height();
}

//////////////////////////////////////////////////////////////////
// SetupRenderToTexture

void SbPaint::SetupRenderToTexture()
{
	// Make a new render target
	RsRenderTarget *pRenderTarget = RsUtil::GetNewRenderTarget();

	// Set the render target name
	pRenderTarget->SetName( "painting" );

	// Make a new render target
	RsCamera camera( (BtFloat)m_width, (BtFloat)m_height, 0.01f, 10000.0f, RsViewport(0, 0, m_width, m_height) );

	// Update the camera
	camera.Update();

	// Set the texture
    RsTexture *pTexture = m_pRenderTarget->GetTexture(0);
    pRenderTarget->SetTexture( pTexture );

	// Set the camera
	pRenderTarget->SetCamera(camera);

	// Clear the render target
	if( m_isClear )
	{
		m_isClear = BtFalse;
	
		pRenderTarget->SetCleared( BtTrue );
	}
	else
	{
		pRenderTarget->SetCleared( BtFalse );
	}

	// Clear the z buffer
	pRenderTarget->SetZCleared( BtFalse );

	// Clear the screen to black
	pRenderTarget->SetClearColour( RsColour::WhiteColour() );
	
	// Apply this render target
	pRenderTarget->Apply();
}

////////////////////////////////////////////////////////////////////////////////
// Reset

void SbPaint::Reset()
{
	m_isClear = BtTrue;
	m_v2Last    = MtVector2( 0, 0 );
	m_isFirst = BtFalse;
    m_isPainting = BtFalse;
	m_hardness = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////
// GetScreenPositionFromMouse

MtVector2 SbPaint::GetScreenPositionFromMouse( MtVector2 v2Position )
{
	MtVector2 v2ScreenDimension = RsUtil::GetDimension();

	MtVector2 v2ScreenPosition = v2Position;

	v2ScreenPosition.x /= v2ScreenDimension.x;
	v2ScreenPosition.y /= v2ScreenDimension.y;

	v2ScreenPosition.x *= m_width;
	v2ScreenPosition.y *= m_height;
	v2ScreenPosition.y = m_height - v2ScreenPosition.y;
	return v2ScreenPosition;
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbPaint::Update()
{
    if( ShTouch::IsShaken() )
    {
        m_isClear = BtTrue;
    }
    if( UiKeyboard::pInstance()->IsPressed( UiKeyCode_R ) )
    {
        Reset();
    }
	if (UiKeyboard::pInstance()->IsPressed(UiKeyCode_S))
	{
		m_hardness = 0.25f;
	}

    // Get the rotational angle
    MtMatrix4 m4Transform = ExWAX9::GetTransform();
    MtVector3 v3XAxis = m4Transform.XAxis();
  
    m_roll = v3XAxis.y;  
    m_isRender = BtFalse;
	m_isBlob = BtFalse;

	// Calculate the thickness
	BtFloat edge = MtAbs(m_roll);
	BtFloat minWidth = RsUtil::GetHeight() * 0.001f;
	BtFloat maxWidth = RsUtil::GetHeight() * 0.010f;
	BtFloat width = MtLerp(edge, minWidth, maxWidth);
	m_thickness = width * MaxThickness * m_hardness;

	BtU32 colour = RsColour::BlueColour().asWord();

    for(BtU32 i=0; i<MaxTouches; i++ )
    {
        MtVector2 v2Position = ShTouch::GetPosition(i);

        if( ShTouch::IsPressed(i) )
        {
            m_v2Last = v2Position;
			m_isFirst = BtTrue;
        }
           
        if( ShTouch::IsHeld(i) )
        {
            m_isPainting = BtTrue;
            
            MtVector2 v2Delta = v2Position - m_v2Last;

//			m_hardness += BtTime::GetTick();

            if (v2Delta.GetLength() > 0.01f) // 1% = 11 pixels of 1080p
            {
				// Increase the thickness
				m_hardness += 0.1f;
				if (m_hardness > 1.0f)
				{
					m_hardness = 1.0f;
				}
				m_thickness = width * MaxThickness * m_hardness;
                
				MtVector2 v2ScreenPosition = GetScreenPositionFromMouse( v2Position );

                MtVector3 v3Delta( v2Delta.x, v2Delta.y, 0 );
				v3Delta.Normalise();
				
				MtVector3 v3Cross(v3Delta.y, v3Delta.x, 0);
                v3Cross *= m_thickness;
                
                if( m_isFirst == BtTrue )
                {
					MtVector2 v2LastScreenPosition = GetScreenPositionFromMouse( m_v2Last );
                    m_v2LastL = v2LastScreenPosition + MtVector2(-v3Cross.x, -v3Cross.y);
					m_v2LastR = v2LastScreenPosition + MtVector2( v3Cross.x,  v3Cross.y);
					m_isFirst = BtFalse;
                }
                else
                {
                    m_strokeVertex[0].m_v2Position = m_v2LastR;
					m_strokeVertex[0].m_colour = colour;
					m_strokeVertex[0].m_v2UV = MtVector2(0.5, 0.5);
                    
                    m_strokeVertex[1].m_v2Position = m_v2LastL;
					m_strokeVertex[1].m_colour = colour;
					m_strokeVertex[1].m_v2UV = MtVector2(0.5, 0.5);
                    
                    m_strokeVertex[2].m_v2Position = v2ScreenPosition + MtVector2(v3Cross.x, v3Cross.y);
					m_strokeVertex[2].m_colour = colour;
					m_strokeVertex[2].m_v2UV = MtVector2(0.5, 0.5);
                    
                    m_strokeVertex[3].m_v2Position = v2ScreenPosition + MtVector2(-v3Cross.x, -v3Cross.y);
					m_strokeVertex[3].m_colour = colour;
					m_strokeVertex[3].m_v2UV = MtVector2(0.5, 0.5);

					m_v2LastR = m_strokeVertex[2].m_v2Position;
					m_v2LastL = m_strokeVertex[3].m_v2Position;
                    
                    m_isRender = BtTrue;
                }

				m_v2Last = v2Position;
            }

			if (1)
			{
				m_isBlob = BtTrue;

				MtVector2 v2ScreenPosition = GetScreenPositionFromMouse(v2Position);

				MtVector3 v3Cross(m_thickness, m_thickness, 0);

				m_blobVertex[0].m_v2Position = v2ScreenPosition + MtVector2(-v3Cross.x, -v3Cross.y);
				m_blobVertex[0].m_colour = colour;
				m_blobVertex[0].m_v2UV = MtVector2(0, 0);

				m_blobVertex[1].m_v2Position = v2ScreenPosition + MtVector2(-v3Cross.x,  v3Cross.y);
				m_blobVertex[1].m_colour = colour;
				m_blobVertex[1].m_v2UV = MtVector2(0, 1);

				m_blobVertex[2].m_v2Position = v2ScreenPosition + MtVector2( v3Cross.x, -v3Cross.y);
				m_blobVertex[2].m_colour = colour;
				m_blobVertex[2].m_v2UV = MtVector2(1, 0);

				m_blobVertex[3].m_v2Position = v2ScreenPosition + MtVector2( v3Cross.x,  v3Cross.y);
				m_blobVertex[3].m_colour = colour;
				m_blobVertex[3].m_v2UV = MtVector2(1, 1);
			}
		}
        else if(ShTouch::IsReleased(i))
        {
            m_isFirst = BtTrue;
            m_v2Last  = v2Position;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// PreRender

void SbPaint::PreRender()
{
    SetupRenderToTexture();
    
	if (m_isBlob)
	{
		m_pBrush->Render(RsPT_TriangleStrip, m_blobVertex, 4, MaxSortOrders - 1);
	}

    if( m_isRender )
    {
	   m_pBrush->Render(RsPT_TriangleStrip, m_strokeVertex, 4, MaxSortOrders - 1);
	}
}

////////////////////////////////////////////////////////////////////////////////
// IsPainting

BtBool SbPaint::IsPainting()
{
    return m_isPainting;
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbPaint::Render()
{
    MtVector2 v2Dimension = RsUtil::GetDimension();
	m_pRenderTarget->Render( MtVector2(0, 0), v2Dimension, RsColour::WhiteColour(), MaxSortOrders - 1);
}
