////////////////////////////////////////////////////////////////////////////////
// SbPhysics.cpp

// Includes
#include "DyImpl.h"
#include "RsVertex.h"
#include "RsTexture.h"
#include "BaArchive.h"
#include "SbPhysics.h"
#include "RsMaterial.h"
#include "SgNode.h"
#include "UiKeyboard.h"
#include "RsShader.h"
#include "ShTouch.h"
#include "ExWAX9.h"
#include "HlFont.h"
#include "BtCollection.h"
#include "RsUtil.h"
#include "SgMesh.h"
#include "ApConfig.h"
#include "HlKeyboard.h"

struct LastDiceTouch : public BtLinkedList<LastDiceTouch>
{
    MtVector3 v3Force;
};

BtCollection< LastDiceTouch, 256> gLastDiceTouches;
MtVector3 v3AvForce( 0, 0, 0);

////////////////////////////////////////////////////////////////////////////////
// Setup

void SbPhysics::Setup( BaArchive *pArchive )
{
	DyImpl::Create();
	DyImpl::pWorld()->Gravity( MtVector3( 0, -9.8f, 0 ) );
	DyImpl::pWorld()->SetSubsteps( 0);

	m_pFloor = pArchive->GetNode( "floor" )->GetDuplicate();
	m_floor.CreateBox( 0.0f, MtVector3( 100.0f, 1.0f, 100.0f ) );
	m_pCube = pArchive->GetNode("cube");
	m_dice.CreateBox( 10.0f, MtVector3( 2.0f, 2.0f, 2.0f ) );

	MtAABB aabv = m_pCube->GetMesh()->GetAABB();
	(void)aabv;

	m_dice.SetFriction(0.8f);
	m_floor.SetFriction(0.8f);

	Reset();
	m_isThrown = BtFalse;
}

////////////////////////////////////////////////////////////////////////////////
// Reset

void SbPhysics::Reset()
{
	MtMatrix4 m4Transform;
	m4Transform.SetTranslation( 0.0f, -10.0f, 0.0f );
	
	//MtMatrix4 m4Rotate;
	//m4Rotate.SetRotationY( MtDegreesToRadians( 90.0f ) );
	
	m_floor.SetWorldTransform( m4Transform );
	m4Transform.SetTranslation(0.0f, 0, 0.0f);

	// Set the dice
	m4Transform.SetTranslation(0.0f, 0.0f, -50.0f);
	m_dice.SetWorldTransform(m4Transform);
	m_dice.SetWorldVelocity(MtVector3(0, 0, 0));
	m_dice.SetWorldAngularVelocity(MtVector3(0, 0, 0));
	
	m_isThrown = BtFalse;
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbPhysics::Update()
{
	if( UiKeyboard::pInstance()->IsPressed(UiKeyCode_R) )
	{
		Reset();
	}
	if( !m_isThrown )
	{
		Reset();
	}
    
    // Calculate the force of the touch
    MtVector3 v3Acceloremeter = ExWAX9::GetAccelerometer();// * ShWAX9::GetTransform();

    LastDiceTouch lastTouch;
    lastTouch.v3Force = v3Acceloremeter;
    BtCollectionIterator<LastDiceTouch, 256> myIterator( gLastDiceTouches );
    
    BtU32 numElements = gLastDiceTouches.GetNumElements();
    
    if( numElements > 9 )
    {
        myIterator.GetFirstElement();
        myIterator.DeleteElement();
    }
    gLastDiceTouches.AddTail( lastTouch );
    
	if (UiKeyboard::pInstance()->IsPressed(PauseKey))
	{
		ApConfig::SetPaused(!ApConfig::IsPaused());
	}

    static int frames = 0;
    
    // Throw the dice
    for(BtU32 touchIndex = 0; touchIndex < MaxTouches; touchIndex++)
    {
        if( ShTouch::IsPressed(touchIndex) )
        {
			if (!ApConfig::IsWin())
			{
				Reset();
			}

            // Sum the average force
            v3AvForce = MtVector3( 0, 0, 0 );
            for( LastDiceTouch* pIterator = myIterator.GetFirstElement(); pIterator; pIterator = myIterator.GetNextElement())
            {
                v3AvForce += pIterator->v3Force;
            }
            v3AvForce /= gLastDiceTouches.GetNumElements();
     
            BtFloat length = v3AvForce.GetLength() * 30.0f;
            
            m_isThrown = BtTrue;
            m_dice.SetWorldVelocity( MtVector3( v3AvForce.x * 20.0f, 0, length ) );
			
			if (ApConfig::IsWin())
			{
				m_dice.SetWorldVelocity(MtVector3(2.0f, 0, 10.0f ));
				m_dice.SetWorldAngularVelocity(MtVector3(1.0f, 1.0f, 1.0f));
			}

            gLastDiceTouches.Empty();
       
            frames = 0;
        }
    }
    
	if (!ApConfig::IsWin())
	{
		if ((frames > 0) && (frames < 10))
		{
			// Calculate the force of the touch
			MtVector3 v3Acceloremeter = ExWAX9::GetAccelerometer();
			m_dice.SetWorldAngularVelocity(v3Acceloremeter * 10.0f);
		}
	}
    
    ++frames;
    
	DyImpl::pWorld()->Update();
	m_floor.Update();
	m_dice.Update();
	m_pFloor->SetLocalTransform(m_floor.GetWorldTransform());
	m_pFloor->Update();
	m_pCube->SetLocalTransform(m_dice.GetWorldTransform());
	m_pCube->Update();
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbPhysics::Render()
{
    //BtChar text[32];
    //sprintf( text, "%.2f %.2f %.2f", v3AvForce.x, v3AvForce.y, v3AvForce.z );
    //HlFont::Render( MtVector2( 0, 25 ), text, RsColour::WhiteColour(), MaxSortOrders-1);
    
	m_pFloor->Render();
	m_pCube->Render();
}

////////////////////////////////////////////////////////////////////////////////
// Destroy

void SbPhysics::Destroy()
{
	DyImpl::Destroy();
}