////////////////////////////////////////////////////////////////////////////////
// SbPiano.cpp

// Includes
#include <stdio.h>
#include "BaArchive.h"
#include "SbPiano.h"
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
#include "BtCollection.h"

// http://theremin.music.uiowa.edu/MISpiano.html

SdSound	   *m_majorSounds[32];
SdSound	   *m_minorSounds[32];

BtBool		m_isMajor[MaxTouches];
BtS32		m_currKey[MaxTouches];
BtS32		m_channels[MaxTouches];

////////////////////////////////////////////////////////////////////////////////
// Setup

void SbPiano::Setup( BaArchive *pSoundArchive, BaArchive *pGameArchive )
{
	m_pPianoMaterial = pGameArchive->GetMaterial("piano");

	m_majorSounds[0]  = pSoundArchive->GetSound( "C3" );
	m_majorSounds[1]  = pSoundArchive->GetSound( "D3" );
	m_majorSounds[2]  = pSoundArchive->GetSound( "E3" );
	m_majorSounds[3]  = pSoundArchive->GetSound( "F3" );
	m_majorSounds[4]  = pSoundArchive->GetSound( "G3" );
	m_majorSounds[5]  = pSoundArchive->GetSound( "A3" );
	m_majorSounds[6]  = pSoundArchive->GetSound( "B3" );

	m_majorSounds[7]  = pSoundArchive->GetSound( "C4" );
	m_majorSounds[8]  = pSoundArchive->GetSound( "D4" );
	m_majorSounds[9]  = pSoundArchive->GetSound( "E4" );
	m_majorSounds[10] = pSoundArchive->GetSound( "F4" );
	m_majorSounds[11] = pSoundArchive->GetSound( "G4" );
	m_majorSounds[12] = pSoundArchive->GetSound( "A4" );
	m_majorSounds[13] = pSoundArchive->GetSound( "B4" );	// 14

	// Sharps
	m_minorSounds[0]  = pSoundArchive->GetSound("cs3");
	m_minorSounds[1]  = pSoundArchive->GetSound("eb3");
	m_minorSounds[2]  = 0;
	m_minorSounds[3]  = pSoundArchive->GetSound("gb3");
	m_minorSounds[4]  = pSoundArchive->GetSound("ab3");
	m_minorSounds[5]  = pSoundArchive->GetSound("bb3");
	m_minorSounds[6]  = 0;
	m_minorSounds[7]  = pSoundArchive->GetSound("db4");
	m_minorSounds[8]  = pSoundArchive->GetSound("eb4");
	m_minorSounds[9]  = 0;
	m_minorSounds[10] = pSoundArchive->GetSound("gb4");
	m_minorSounds[11] = pSoundArchive->GetSound("ab4");
	m_minorSounds[12] = pSoundArchive->GetSound("bb4");		// 10
	m_minorSounds[13] = 0;									// 14 + 10 = 24

	for( BtU32 i=0; i<MaxTouches; i++ )
	{
		m_currKey[i] = -1;
		m_channels[i] = -1;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Update
struct LastTouch : public BtLinkedList<LastTouch>
{
    BtFloat force;
};

BtCollection< LastTouch, 256> gLastTouches;
BtFloat avForce = 0;

void SbPiano::Update()
{
	BtFloat screenWidth  = (BtFloat)RsUtil::GetWidth();
	BtFloat screenHeight = (BtFloat)RsUtil::GetHeight();
	BtFloat halfScreenHeight = screenHeight / 2;

	BtFloat widthRatio = screenWidth / m_pPianoMaterial->GetDimension().x;
	BtFloat pianoHeight = m_pPianoMaterial->GetDimension().y * widthRatio;

	MtVector2 v2Dimension((BtFloat)RsUtil::GetWidth(), pianoHeight);

	BtFloat halfPianoHeight = pianoHeight / 2;

	BtFloat startHeight       = halfScreenHeight - halfPianoHeight;
	BtFloat endHeight         = halfScreenHeight + halfPianoHeight;
	BtFloat minorKeyEndHeight = startHeight + halfPianoHeight + ( pianoHeight / 16.0f );

	BtFloat majorKeyWidth = screenWidth / 14.0f;
	BtFloat minorKeyWidth = majorKeyWidth / 2;

	// Did we touch a minor key?
	BtFloat startX = minorKeyWidth + (minorKeyWidth / 2);

    for( BtU32 touchIndex = 0; touchIndex < MaxTouches; touchIndex ++ )
    {
        m_currKey[touchIndex] = -1;
    }
    
    // Calculate the force of the touch
    MtVector3 v3Acceloremeter = ExWAX9::GetAccelerometer();
    
    BtFloat currentForceLength = v3Acceloremeter.GetLength();
    
    LastTouch lastTouch;
    lastTouch.force = currentForceLength;
    gLastTouches.AddTail( lastTouch );
    BtCollectionIterator<LastTouch, 256> myIterator( gLastTouches );
    
    BtU32 numElements = gLastTouches.GetNumElements();
    
    if( numElements > 9 )
    {
        myIterator.GetFirstElement();
        myIterator.DeleteElement();
    }
    
    // Sum the average force
    avForce = 0;
    for(LastTouch* pIterator = myIterator.GetFirstElement(); pIterator; pIterator = myIterator.GetNextElement())
    {
        avForce += pIterator->force;
    }
    avForce /= gLastTouches.GetNumElements();
    
    if( avForce > 1.0f )
    {
        avForce = 1.0f;
    }

	if( avForce < 0.1f )
	{
		avForce = 0.1f;
	}
    
    // Hit th keys
	for( BtU32 touchIndex = 0; touchIndex < MaxTouches; touchIndex ++ )
	{
		if( ShTouch::IsHeld(touchIndex) )
		{
			if(touchIndex == 2)
			{
				int a = 0;
				a++;
			}
	
			MtVector2 v2Position = ShTouch::GetPosition(touchIndex);

			BtU32 minorKeyIndex = 0;

			for(BtFloat x = startX; x < RsUtil::GetWidth() - minorKeyWidth; x = x + majorKeyWidth)
			{
				if((v2Position.x > x) && (v2Position.x < x + minorKeyWidth))
				{
					if((v2Position.y > startHeight) && (v2Position.y < minorKeyEndHeight ))
					{
						if( m_minorSounds[ minorKeyIndex ] )
						{
							m_currKey[touchIndex] = minorKeyIndex;
							m_isMajor[touchIndex] = BtFalse;
						}
					}
				}
				++minorKeyIndex;
			}

			if( m_currKey[touchIndex] == -1 )
			{
				// Did we touch a major key?
				BtU32 majorKeyIndex = 0;

				for(BtFloat x = 0; x < RsUtil::GetWidth(); x = x + majorKeyWidth )
				{
					if( ( v2Position.x > x  ) && ( v2Position.x < x + majorKeyWidth ) )
					{
						if( ( v2Position.y > startHeight ) && ( v2Position.y < endHeight ) )
						{
							m_currKey[touchIndex] = majorKeyIndex;
							m_isMajor[touchIndex] = BtTrue;
						}
					}
					++majorKeyIndex;
				}
			}
		}
	}

    // Whenever we play a sample we return the channel that its playing on
    // We store that in m_channels against the touch index
    
	for(BtU32 touchIndex = 0; touchIndex < MaxTouches; touchIndex++)
	{
        // At the start of each update we assume no keys have been pressed
        // We find the channel based on the key
        // If it is valid i.e. != -1 we stop that sample
        // and clear the channel
        
		if( m_currKey[touchIndex] == -1 )
		{
			BtS32 channel = m_channels[touchIndex];
			if( channel != -1 )
			{
				SdSound::Stop( channel );
				m_channels[touchIndex] = -1;
			}
		}
		else
		{
            BtFloat pitch  = 1.0f;
            BtFloat volume = avForce;
            
			if( SdSound::IsPlaying( m_channels[touchIndex] ) == BtFalse )
			{
				BtU32 key = m_currKey[touchIndex];

				if( m_isMajor[touchIndex] )
				{
					m_channels[touchIndex] = m_majorSounds[ key ]->Play( pitch, volume );
				}
				else
				{
					m_channels[touchIndex] = m_minorSounds[ key ]->Play( pitch, volume );
				}
                
                gLastTouches.Empty();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbPiano::Render()
{
	BtFloat widthRatio  = RsUtil::GetWidth() / m_pPianoMaterial->GetDimension().x;
	BtFloat pianoHeight = m_pPianoMaterial->GetDimension().y * widthRatio;

	MtVector2 v2Dimension( (BtFloat)RsUtil::GetWidth(), pianoHeight );

	BtFloat halfPianoHeight = pianoHeight / 2;

	BtFloat startHeight = RsUtil::GetHalfDimension().y - halfPianoHeight;

	m_pPianoMaterial->Render( MtVector2( 0, startHeight ), v2Dimension, RsColour::WhiteColour(), MaxSortOrders-1 );

	BtFloat endHeight = RsUtil::GetHalfDimension().y + halfPianoHeight;

	if( ApConfig::IsDebug() )
	{
		BtFloat width = RsUtil::GetWidth() / 14.0f;

		// Render the major keys
		for( BtFloat x=0; x<RsUtil::GetWidth(); x=x + width )
		{
            HlDraw::RenderLine( MtVector2( x, startHeight ),
								 MtVector2( x, endHeight ),
								 RsColour::GreenColour(),
								 MaxSortOrders-1 );
		}

		// Render the minor keys
		float halfwidth   = width / 2;
		endHeight  = startHeight + halfPianoHeight;
		endHeight += pianoHeight / 16;

		BtFloat startX = halfwidth + ( halfwidth / 2 );

		for(BtFloat x = startX; x < RsUtil::GetWidth() - halfwidth; x = x + halfwidth)
		{
            HlDraw::RenderLine( MtVector2(x, startHeight),
								 MtVector2(x, endHeight),
								 RsColour::BlueColour(),
								 MaxSortOrders - 1);
		}
	}
    
    static BtFloat currentForce = 0;
    currentForce = MtMax( currentForce, avForce );
    if( currentForce > 0 )
    {
        currentForce -= 0.01f;
    }
    else{
        currentForce = 0;
    }
    BtChar text[32];
    sprintf( text, "%.2f", currentForce );
    HlFont::Render( MtVector2( 0, 25 ), text, RsColour::BlackColour(), MaxSortOrders-1);
}
