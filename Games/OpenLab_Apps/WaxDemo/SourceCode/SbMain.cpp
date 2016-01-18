////////////////////////////////////////////////////////////////////////////////
// SbMain.cpp

// Includes
#include <stdio.h>
#include "SbMain.h"
#include "ApConfig.h"
#include "RsUtil.h"
#include "RsShader.h"
#include "SbCamera.h"
#include "HlMaterial.h"
#include "HlFont.h"
#include "HlDraw.h"
#include "HlMouse.h"
#include "UiKeyboard.h"
#include "RsMaterial.h"
#include "ShHMD.h"
#include "BtTime.h"
#include "ExWAX9.h"

BtU32 SbMain::demo = 0;
BtU32 nextDemo = 1;
static BtFloat delayTime = 0;
const BtU32 MaxDemos = 5;

////////////////////////////////////////////////////////////////////////////////
// Init

void SbMain::Init()
{
	m_isClosing = BtFalse;
	m_isClosed  = BtFalse;

	MtVector2 v2Dimension;

	ApConfig::SetTitle( "WaxRenderer" );

	if( ApConfig::IsWin() && !ShHMD::IsHMD())
	{
	//	v2Dimension = MtVector2( 1020.0f, ;
		v2Dimension = MtVector2(1600.0f, 1200.0f);
	//	800.0f, 600.0f
        ApConfig::SetDebug( BtTrue );
        RsUtil::SetDimension(v2Dimension);
    }
	else
	{
		v2Dimension = RsUtil::GetDimension();
        ApConfig::SetDebug( BtFalse );
	}
   
	m_camera.Init();
	m_camera.SetDimension( MtVector2( v2Dimension.x, v2Dimension.y ) );
	m_camera.SetPosition( MtVector3( 0, 0, -5.0f ) );
	m_camera.SetSpeed( 10.0f );

	RdRandom::SetRandomSeed();

	// Load the game archive
	if( ApConfig::GetPlatform() == ApPlatform_WinGL )
	{
		ApConfig::SetResourcePath( "..\\waxdemo\\release\\" );
		ApConfig::SetExtension( ".winglz" );
	}
    
	int test = 1;
	int one  = 1;
	if( one && test )
	{
		int a=0;
		a++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Create

void SbMain::Create()
{
	m_gameArchive.Load( "game" );
	m_utilityArchive.Load("utility");
	m_pianoArchive.Load("piano");

	m_pWhite2 = m_gameArchive.GetMaterial( "white2" );
	m_pWhite3 = m_gameArchive.GetMaterial( "white3" );
	m_pShader = m_gameArchive.GetShader( "shader" );
	 
	m_model.Setup( &m_gameArchive );
	m_piano.Setup( &m_pianoArchive, &m_gameArchive );
	m_jigsaw.Setup( &m_gameArchive );
	m_physics.Setup( &m_gameArchive );
    m_graph.Setup( &m_gameArchive );
	m_paint.Setup(&m_gameArchive);
    
	RsMaterial *pMaterial2 = m_gameArchive.GetMaterial("white2");
	RsMaterial *pMaterial3 = m_gameArchive.GetMaterial("white3");
	RsMaterial *pMaterial3notest = m_gameArchive.GetMaterial("white3noztest");

	HlFont::Setup( &m_utilityArchive, "vera" );
	HlMouse::Setup( &m_utilityArchive );
	HlDraw::Setup( pMaterial2, pMaterial3, pMaterial3notest );
}

////////////////////////////////////////////////////////////////////////////////
// Reset

void SbMain::Reset()
{
	m_paint.Reset();
	m_physics.Reset();
}

////////////////////////////////////////////////////////////////////////////////
// UpdateTest

void SbMain::UpdateTest()
{
	m_camera.Update();

	if( nextDemo != demo )
	{
		m_physics.Reset();
        m_paint.Reset();
		demo = nextDemo;
	}
   
    if(demo == 0)
    {
        m_camera.SetPosition(MtVector3(0, 0, -5.0f));
        m_camera.Update();
        m_graph.Update();
    }
	if(demo == 1)
	{
		m_camera.SetPosition( MtVector3(0, 0, -5.0f) );
		m_camera.Update();
		m_model.Update();
	}
	if(demo == 2)
	{
		m_piano.Update();
	}
	if(demo == 3)
	{
		m_jigsaw.Update();
	}
	if(demo == 4)
	{
		m_camera.SetPosition(MtVector3(0, 0, -60.0f));
		m_camera.Update();
		m_physics.Update();
	}
	if (demo == 5)
	{
		m_camera.SetPosition(MtVector3(0, 0, -60.0f));
		m_camera.Update();
		m_paint.Update();
	}

	if(UiKeyboard::pInstance()->IsPressed(UiKeyCode_F1))
	{
		ApConfig::SetDebug( !ApConfig::IsDebug() );
	}
	if(UiKeyboard::pInstance()->IsPressed(UiKeyCode_1))
	{
		nextDemo = 1;
	}
	if(UiKeyboard::pInstance()->IsPressed(UiKeyCode_2))
	{
		nextDemo = 2;
	}
	if(UiKeyboard::pInstance()->IsPressed(UiKeyCode_3))
	{
		nextDemo = 3;
	}
	if(UiKeyboard::pInstance()->IsPressed(UiKeyCode_4))
	{
		nextDemo = 4;
	}
	if (UiKeyboard::pInstance()->IsPressed(UiKeyCode_5))
	{
		nextDemo = 5;
	}

    m_isDemoName = BtFalse;
    
    BtBool outside = BtTrue;
    
    for( BtU32 touchIndex = 0; touchIndex < MaxTouches; touchIndex ++ )
    {
        if( ShTouch::IsHeld(touchIndex) )
        {
            MtVector2 v2Position = ShTouch::GetPosition(touchIndex);
            if( v2Position.x > RsUtil::GetWidth() * 0.9f )
            {
                if( v2Position.y < RsUtil::GetHeight() * 0.1f )
                {
                    outside = BtFalse;
                    
                    m_isDemoName = BtTrue;
                }
            }
        }
    }
    
    if( outside )
    {
        delayTime = 0;
    }
    else
    {
        delayTime += BtTime::GetTick();
    }
    
    if( delayTime > 3.0f )
    {
        delayTime = 0;
        
        nextDemo = demo + 1;
        if( nextDemo > MaxDemos )
        {
            nextDemo = 0;
        }
        ShTouch::SetNumberOfTaps(0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbMain::Update()
{
	// Are we closing
	if( m_isClosing == BtTrue )
	{
		// Unload the archive
		m_gameArchive.Unload();

		// Unload the archive
		m_utilityArchive.Unload();

		// Unload the archive
		m_pianoArchive.Unload();

		// Read to close
		m_isClosed = BtTrue;
	}
	else
	{
		UpdateTest();
	}
}

//////////////////////////////////////////////////////////////////
// SetupRender

void SbMain::SetupRender()
{
	// Apply the shader
	m_pShader->Apply();

	// Make a new render target
	RsRenderTarget *pRenderTarget = RsUtil::GetNewRenderTarget();

	// Set the camera
	pRenderTarget->SetCamera( m_camera.GetCamera() );

	// Clear the render target
	pRenderTarget->SetCleared( BtTrue );

	// Clear the z buffer
	pRenderTarget->SetZCleared( BtTrue );

	// Set a good clear colour
	//pRenderTarget->SetClearColour( RsColour( 0.5f, 0.6f, 0.7f, 1.0f ) );

	pRenderTarget->SetClearColour( RsColour::BlackColour() );

    if( demo == 0 )
        pRenderTarget->SetClearColour( RsColour( 0.1f, 0.1f, 0.1f, 1.0f ) );
    
    if( demo == 1 )
        pRenderTarget->SetClearColour( RsColour::BlueColour() );
    
    if( demo == 2 )
        pRenderTarget->SetClearColour( RsColour::WhiteColour() );
    
    if( demo == 3 )
        pRenderTarget->SetClearColour( RsColour::WhiteColour() );

	if( demo == 4 )
		pRenderTarget->SetClearColour( RsColour( 0.5f, 0.4f, 0.3f ) );

	if (demo == 5)
		pRenderTarget->SetClearColour( RsColour::WhiteColour() );

	// Apply this render target
	pRenderTarget->Apply();
}

////////////////////////////////////////////////////////////////////////////////
// RenderScene

void SbMain::RenderScene()
{
    if( demo == 0 )
    {
        m_graph.Render();
    }
	if( demo == 1 )
	{
		m_model.Render();
	}
	else if(demo == 2)
	{
		m_piano.Render();
	}
	else if(demo == 3)
	{
		m_jigsaw.Render();
	}
	else if(demo == 4)
	{
		m_physics.Render();
	}
	else if (demo == 5)
	{
		m_paint.Render();
	}
}

////////////////////////////////////////////////////////////////////////////////
// RenderTests

void SbMain::RenderTests()
{
	// Apply the shader
	m_pShader->Apply();

	// Empty the render targets at the start
	RsUtil::EmptyRenderTargets();

	if (demo == 5)
	{
		m_paint.PreRender();
	}

	SetupRender();
	RenderScene();

	if (m_isDemoName)
	{
		BtChar text[64];

		if (demo == 0)
		{
			sprintf(text, "0 Graph and model");
		}
		else if (demo == 1)
		{
			sprintf(text, "1 Model");
		}
		else if (demo == 2)
		{
			sprintf(text, "2 Piano");
		}
		else if (demo == 3)
		{
			sprintf(text, "3 Jigsaw");
		}
		else if (demo == 4)
		{
			sprintf(text, "4 Craps table");
		}
		else if (demo == 5)
		{
			sprintf(text, "5 Paint");
		}
		//HlFont::RenderHeavy(MtVector2(0, 0), text, MaxSortOrders - 1);

     	sprintf(text + strlen( text ), " - Touch-%.0fs", 3.0f - delayTime);
		MtVector2 v2Dimension = HlFont::GetDimension(text);
		MtVector2 v2Pos = RsUtil::GetDimension();
		v2Pos.x -= v2Dimension.x;
		v2Pos.y = 0;
		HlFont::RenderHeavy(v2Pos, text, MaxSortOrders - 1);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbMain::Render()
{
	if( m_isClosing == BtTrue )
	{
		return;
	}

	// Render the tests
	RenderTests();

	// Render the mouse
	HlMouse::Render();
}

////////////////////////////////////////////////////////////////////////////////
// PreRender

void SbMain::PreRender()
{
}

////////////////////////////////////////////////////////////////////////////////
// PostRender

void SbMain::PostRender()
{
	// Render the mouse
	//m_renderMouse.Render();
}

////////////////////////////////////////////////////////////////////////////////
// Destroy

void SbMain::Destroy()
{
}
