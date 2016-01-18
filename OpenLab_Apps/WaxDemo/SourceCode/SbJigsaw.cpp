////////////////////////////////////////////////////////////////////////////////
// SbJigsaw.cpp

// Includes
#include <stdio.h>
#include "BaArchive.h"
#include "SbJigsaw.h"
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
#include "ErrorLog.h"

SbPiece g_unsortedPieces[32];
SbPiece *g_sortedPieces[32];

BtU32 NumPieces = 0;

const BtU32 columns = 5;
const BtU32 rows = 4;
BtU32 heightTick = 0;

const BtFloat MaxMatchAngle = 10.0f;
const BtFloat MaxBreakAngle = 30.0f;
const BtFloat SmallDistance = 200.0f;
const BtFloat BigDistance   = 20.0f;

////////////////////////////////////////////////////////////////////////////////
// Setup

void SbJigsaw::Setup( BaArchive *pGameArchive )
{
	Reset();

	m_pCircle = pGameArchive->GetMaterial( "circle" );
	m_pJigsaw = pGameArchive->GetMaterial( "jigsaw" );
}

////////////////////////////////////////////////////////////////////////////////
// Reset

void SbJigsaw::Reset()
{
	BtFloat screenWidth  = (BtFloat)RsUtil::GetWidth();
	BtFloat screenHeight = (BtFloat)RsUtil::GetHeight();

	m_pieceWidth  = screenWidth / ( columns + 1 );
	m_pieceHeight = screenHeight / ( rows + 1 );

	m_halfPieceWidth  = m_pieceWidth / 2;
	m_halfPieceHeight = m_pieceHeight / 2;

	BtFloat spacePieceWidth  = screenWidth / columns;
	BtFloat spacePieceHeight = screenHeight / rows;

	BtFloat startx = spacePieceWidth / 2;
	BtFloat y = spacePieceHeight / 2;

	NumPieces = 0;

	BtFloat U = 1.0f / columns;
	BtFloat V = 1.0f / rows;

	for(BtU32 j = 0; j < rows; j++)
	{
		BtFloat x = startx;
		BtU32 rowStart = NumPieces;

		for(BtU32 i = 0; i < columns; i++)
		{
			SbPiece &piece = g_unsortedPieces[NumPieces];
			g_sortedPieces[NumPieces] = &piece;

			piece.m_pieceID = NumPieces;
			piece.m_height = 0;
            piece.m_m4Rotation.SetIdentity();
			piece.SetPosition(MtVector2(x, y));

			// Set the UVS
			piece.x  = U * i;
			piece.x2 = piece.x + U;
			piece.y  = V * j;
			piece.y2 = piece.y + V;

			piece.m_pPieces[0] = piece.m_pPieces[1] = piece.m_pPieces[2] = piece.m_pPieces[3] = BtNull;
			piece.SetDimension( MtVector2( m_pieceWidth, m_pieceHeight ) );
            
			// Randomise
			MtMatrix4 m4Rotation;
			m4Rotation.SetRotationZ(MtDegreesToRadians(RdRandom::GetFloat(0, 360)));
            
            BtBool randomise = BtTrue;
            if( randomise )
            {
                piece.m_m4Rotation = m4Rotation;
                x = RdRandom::GetNumber( spacePieceWidth / 2,  screenWidth  - spacePieceWidth  );
                y = RdRandom::GetNumber( spacePieceHeight / 2, screenHeight - spacePieceHeight );
            }
            
            piece.SetPosition( MtVector2( x, y ) );

			if(j < rows - 1)
			{
				piece.m_pPieces[North] = &g_unsortedPieces[rowStart + columns + i];
			}
			if(j > 0)
			{
				piece.m_pPieces[South] = &g_unsortedPieces[rowStart - columns + i ];
			}
			if( i < columns - 1 )
			{
				piece.m_pPieces[East] = &g_unsortedPieces[NumPieces+1];
			}
			if( i > 0 )
			{
				piece.m_pPieces[West] = &g_unsortedPieces[NumPieces-1];
			}

			x += spacePieceWidth;

			NumPieces++;
		}

		y += spacePieceHeight;
	}
    
	Clear();
    Sort();
}

////////////////////////////////////////////////////////////////////////////////
// Clear

void SbJigsaw::Clear()
{
	for(BtU32 i = 0; i < NumPieces; i++)
	{
		SbPiece &piece = g_unsortedPieces[i];

		for(BtU32 j = 0; j < 4; j++)
		{
			piece.m_isLinked[j] = BtFalse;
            g_unsortedPieces[i].m_isLinked[j] = BtFalse;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Sort

void SbJigsaw::Sort()
{
    for(BtU32 i = 0; i < NumPieces; i++)
    {
        g_sortedPieces[i] = &g_unsortedPieces[i];

    }
    
    for(BtU32 i = 0; i < NumPieces; i++)
    {
        for(BtU32 j = 0; j < NumPieces; j++)
        {
            if( i != j )
            {
                if( g_sortedPieces[i]->m_height < g_sortedPieces[j]->m_height)
                {
                    BtSwap(g_sortedPieces[i], g_sortedPieces[j]);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// ExpressiveTouch

void SbJigsaw::ExpressiveTouch()
{
    static MtVector3 v3Last( 0, 0, 0 );
    
    MtVector3 v3XAxis( 1, 0, 0 );
    v3XAxis = v3XAxis * ExWAX9::GetTransform();

    MtVector3 v3ZAxis( 0, 0, 1 );
    v3ZAxis = v3ZAxis * ExWAX9::GetTransform();

    MtVector3 v3Delta = v3Last - v3XAxis;

    BtFloat ang = 0;

    if( v3Delta.GetLength() > 0.01f )
    {
        BtFloat dp = v3XAxis.DotProduct( v3Last );
        
        MtVector3 v3Normal = v3XAxis.CrossProduct( v3Last );
        
        ang = MtACos( dp );
        
        // Decide whether the gesture was clockwise or anticlockwise
        dp = v3Normal.DotProduct( v3ZAxis );
        if( dp < 0 )
        {
            ang = -ang;
        }
        v3Last = v3XAxis;
    }

    // If we have only one touch on screen - then allow rotation of a jigsaw piece
    if( ShTouch::GetNumberOfTouches() == 1 )
    {
        // Rotate any selected pieces
        for( BtS32 i=NumPieces-1; i>=0; i-- )
        {
            SbPiece *pPiece = g_sortedPieces[i];
            
            if( pPiece->IsSelected() )
            {
                pPiece->Rotate( ang );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Update

void SbJigsaw::Update()
{
	// Construct the extents
	for(BtU32 i = 0; i < NumPieces; i++)
	{
		SbPiece &piece = g_unsortedPieces[i];
		piece.ConstructExtents();
	}

	for(BtU32 touchIndex = 0; touchIndex < MaxTouches; touchIndex++)
	{
		MtVector2 v2Position = ShTouch::GetPosition(touchIndex);

		for( BtS32 i=NumPieces-1; i>=0; i-- )
		{
			SbPiece *pPiece = g_sortedPieces[i];
	
			BtBool inside = BtTrue;
	
			if( ShTouch::IsPressed(touchIndex) || ShTouch::IsReleased(touchIndex) )
			{
				for(BtU32 i = 0; i < 4; i++)
				{
					if( pPiece->plane[i].SideOfPlane(MtVector3(v2Position.x, v2Position.y, 0)) < 0)
					{
						inside = BtFalse;
					}
				}
	
				if(inside)
				{
					// Clear whether the piece has been processed
					for(BtU32 i = 0; i < NumPieces; i++)
					{
						g_unsortedPieces[i].m_isChecked = BtFalse;
					}

					if( ShTouch::IsPressed(touchIndex) )
					{
						m_pLinked = BtNull;

						IsLinked( pPiece );
                        Sort();

						for(BtU32 i = 0; i < NumPieces; i++)
						{
							g_unsortedPieces[i].m_isChecked = BtFalse;
						}
						
						// Select the existing piece
						if( m_pLinked )
						{
							m_pLinked->Select(touchIndex);
						}
						else
						{
							pPiece->Select(touchIndex);
						}
                        break;
					}
				}
	
				// Drops a piece by matching against the touch index
				if( ShTouch::IsReleased( touchIndex ) )
				{
					if( pPiece->UnSelect( touchIndex ) )
                    {
                        IsLinked( pPiece );
                        Sort();
                        break;
                    }
				}
				int a = 0;
				a++;
			}
		}
	}
    
    ExpressiveTouch();
    
    if( UiKeyboard::pInstance()->IsPressed( UiKeyCode_R ) )
	{
		Reset();
	}

	// Update the pieces
	for( BtU32 i=0; i<NumPieces; i++ )
	{
		g_unsortedPieces[i].Update();
		g_unsortedPieces[i].m_isChecked = BtFalse;
	}

    const BtFloat smallDistance  = RsUtil::GetWidth() / SmallDistance;
    const BtFloat biggerDistance = RsUtil::GetWidth() / BigDistance;

	// Check if the pieces are near each other
	for(BtU32 i = 0; i < NumPieces; i++)
	{
		SbPiece *pieceA = g_sortedPieces[i];

		for(BtU32 j = 0; j < 4; j++)
		{
			SbPiece *pieceB = pieceA->m_pPieces[j];

			if( pieceB )
			{
				if( pieceA->m_pPieces[j] == pieceB )
				{
					BtU32 opposite = j ^ 1;

					MtVector2 v2PositionA = pieceA->m_translatedTabs[j].GetTranslation();
					MtVector2 v2PositionB = pieceB->m_translatedTabs[opposite].GetTranslation();

					MtVector2 v2Delta = v2PositionA - v2PositionB;

					BtFloat length = v2Delta.GetLength();
                    
                    MtVector3 v3Pos1( 1, 0, 0 );
                    MtVector3 v3Pos2( 1, 0, 0 );
                    
                    BtFloat dp = v3Pos1.DotProduct( v3Pos2 );
                    v3Pos1 *= pieceA->m_m4Rotation;
                    v3Pos2 *= pieceB->m_m4Rotation;
                    v3Pos1  = v3Pos1.GetNormalise();
                    v3Pos2  = v3Pos2.GetNormalise();
                    
                    dp = MtAbs( v3Pos1.DotProduct( v3Pos2 ) );
                    
                    BtFloat ang = MtRadiansToDegrees( MtACos( dp ) );
                    
					// Check the distance
					if( length < smallDistance )
					{
                        if( ( dp >= 1.0f - 0.0001f ) || ( ang < MaxMatchAngle ) )
						{
							pieceA->m_isLinked[j] = BtTrue;
							pieceB->m_isLinked[opposite] = BtTrue;
						}
					}
                    else if( length > biggerDistance )
                    {
                        if( ( dp >= 1.0f - 0.0001f ) || ( ang > MaxBreakAngle ) )
                        {
                            pieceA->m_isLinked[j] = BtFalse;
                            pieceB->m_isLinked[opposite] = BtFalse;
                        }
                    }
				}
			}

			int a=0;
			a++;
		}
	}

	// Move the joined pieces
	for(BtU32 i = 0; i < NumPieces; i++)
	{
		SbPiece &piece = g_unsortedPieces[i];
		piece.m_isChecked = BtFalse;
	}

	for(BtU32 i = 0; i < NumPieces; i++)
	{
		SbPiece &piece = g_unsortedPieces[i];

		if( piece.IsSelected() )
		{
			MoveBoard( &piece );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// IsLinked

void SbJigsaw::IsLinked( SbPiece *piece )
{
	piece->m_isChecked = BtTrue;
    piece->m_height = ++heightTick;

	//ErrorLog::Printf( "%d\n", piece->m_pieceID );

	if( piece->m_firstTouchIndex != -1 )
	{
		m_pLinked = piece;
	}

	// Move the connected the pieces
	for(BtU32 j = 0; j < 4; j++)
	{
		if( piece->m_isLinked[j] )
		{
			SbPiece *pPieceB = piece->m_pPieces[j];

			if( !pPieceB->m_isChecked )
			{
				IsLinked(pPieceB);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// MoveBoard

void SbJigsaw::MoveBoard( SbPiece *piece )
{
	piece->m_isChecked = BtTrue;

	MtMatrix4 m4Rotation  = piece->m_m4Rotation;

	// Move the connected the pieces
	for(BtU32 j = 0; j < 4; j++)
	{
		if( piece->m_isLinked[j] )
		{
			SbPiece *pPieceB = piece->m_pPieces[j];

			MtMatrix4 m4Translate;

			if(j == East)
			{
				m4Translate.SetTranslation(MtVector3(m_pieceWidth, 0, 0));
			}
			else if(j == West)
			{
				m4Translate.SetTranslation(MtVector3(-m_pieceWidth, 0, 0));
			}
			else if(j == North )
			{
				m4Translate.SetTranslation(MtVector3( 0, m_pieceHeight, 0));
			}
			else if(j == South )
			{
				m4Translate.SetTranslation(MtVector3(0, -m_pieceHeight, 0));
			}

			if( !pPieceB->m_isChecked )
			{
				MtMatrix4 m4Temp = m4Translate * m4Rotation;
				pPieceB->m_m4WorldTransform = m4Temp * piece->m_m4WorldTransform;
				pPieceB->m_m4Rotation.SetIdentity();
				pPieceB->Recentre();
				MoveBoard( pPieceB );	
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Render

void SbJigsaw::Render( SbPiece &piece )
{
	// Allocate vertex
	RsVertex3 vertex[4];
	RsVertex3 *pVertex = &vertex[0];

	MtVector2 *v2Pos = piece.m_v2Pos;

	RsVertex3 *pQuad = pVertex;

	// Copy these into vertex
	pVertex->m_v3Position = MtVector3( v2Pos[0].x, v2Pos[0].y, 0.1f);
	pVertex->m_v2UV = MtVector2( piece.x, 1.0f - piece.y ); 
	pVertex->m_colour = 0xFFFFFFFF;	   
	++pVertex;						   
									   
	pVertex->m_v3Position = MtVector3( v2Pos[1].x, v2Pos[1].y, 0.1f);
	pVertex->m_v2UV = MtVector2( piece.x, 1.0f - piece.y2 ); 
	pVertex->m_colour = 0xFFFFFFFF;	   
	++pVertex;						   
									   
	pVertex->m_v3Position = MtVector3( v2Pos[2].x, v2Pos[2].y, 0.1f);
	pVertex->m_v2UV = MtVector2( piece.x2, 1.0f - piece.y ); 
	pVertex->m_colour = 0xFFFFFFFF;	   
	++pVertex;						   
									   
	pVertex->m_v3Position = MtVector3( v2Pos[3].x, v2Pos[3].y, 0.1f);
	pVertex->m_v2UV = MtVector2( piece.x2, 1.0f - piece.y2 );
	pVertex->m_colour = 0xFFFFFFFF;
	++pVertex;

	// Flip the V's
	pQuad[0].m_v2UV.y = 1.0f - pQuad[0].m_v2UV.y;
	pQuad[1].m_v2UV.y = 1.0f - pQuad[1].m_v2UV.y;
	pQuad[2].m_v2UV.y = 1.0f - pQuad[2].m_v2UV.y;
	pQuad[3].m_v2UV.y = 1.0f - pQuad[3].m_v2UV.y;

	// Cache the display width and height
	MtVector2 v2TargetDimension = RsRenderTarget::GetCurrent()->GetCameraDimension();
	BtFloat fScaleWidth = 1.0f / v2TargetDimension.x;
	BtFloat fScaleHeight = 1.0f / v2TargetDimension.y;

	// Scale the position to local screen space -1 to 1
	for(BtU32 i = 0; i < 4; i++)
	{
		// Flip the y
		pQuad[i].m_v3Position.y = v2TargetDimension.y - pQuad[i].m_v3Position.y;

		// Scale from 0..width to 0..1
		pQuad[i].m_v3Position.x *= fScaleWidth;
		pQuad[i].m_v3Position.y *= fScaleHeight;

		// Scale from 0..1 to 0..2
		pQuad[i].m_v3Position.x *= 2.0f;
		pQuad[i].m_v3Position.y *= 2.0f;

		// Translate from 0..2 to -1..1
		pQuad[i].m_v3Position.x -= 1.0f;
		pQuad[i].m_v3Position.y -= 1.0f;
	}

	m_pJigsaw->Render(RsPT_TriangleStrip, pQuad, 4, MaxSortOrders - 1);
}

////////////////////////////////////////////////////////////////////////////////
// Render

void SbJigsaw::Render()
{
 //   BtChar text[32];
 //   sprintf( text, "%.2f", changeAngle );
 //   HlFont::Render( MtVector2( 0, 20 ), text, RsColour::WhiteColour(), MaxSortOrders-1 );
    
	for(BtU32 i = 0; i < NumPieces; i++)
	{
		// Use the sorted order
		BtU32 sortID = g_sortedPieces[i]->m_pieceID;
		
		SbPiece &piece = g_unsortedPieces[sortID];

		// Render the piece
		Render( piece );

		// Render the borders
		g_unsortedPieces[sortID].Render();
	}

	if( ApConfig::IsDebug() )
	{
		MtVector3 v2Dimension(32, 32, 0);
		MtVector3 v2HalfDimension = v2Dimension / 2;

		for(BtU32 i = 0; i < NumPieces; i++)
		{
			SbPiece &piece = g_unsortedPieces[i];

			for( BtU32 j=0; j<4; j++ )
			{
				if( piece.m_pPieces[j] )
				{
					RsColour colour(RsColour::WhiteColour());

					if( piece.m_isLinked[j] )
					{
						colour = RsColour::GreenColour();
					}
	
					MtMatrix4 m4Transform = piece.m_translatedTabs[j];
					m_pCircle->Render( m4Transform.GetTranslation() - v2HalfDimension, v2Dimension, colour, MaxSortOrders - 1);
				}
			}
		}
	}
}
