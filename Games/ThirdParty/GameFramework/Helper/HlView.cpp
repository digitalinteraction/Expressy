////////////////////////////////////////////////////////////////////////////////
// HlView.cp

// Includes
#include "HlView.h"
#include "BtTime.h"
#include "BtString.h"
#include "RsUtil.h"
#include "ApConfig.h"
#include "RsTexture.h"
#include "RsMaterial.h"
#include "HlTimer.h"
#include "BaArchive.h"
#include "ErrorLog.h"
#include "FsFile.h"
#include "BaUserData.h"
#include "TinyXML2.h"
#include "RsSprite.h"
#include "MtVector2.h"
#include "HlScreenSize.h"
#include "ShJoystick.h"
#include "ShTouch.h"
#include "UiKeyboard.h"
#include "RsRenderTarget.h"
#include "HlKeyboard.h"
#include "HlUserData.h"
#include "HlDraw.h"

// statics
BaArchive HlView::m_archive;
BtBool    HlView::m_isLoaded;
BtChar	  HlView::m_lastArchiveName[32];
BtArray<HlMenuItems, 64> HlView::m_items;
HlView *HlView::m_pLast = BtNull;

////////////////////////////////////////////////////////////////////////////////
// Setup

void HlView::Setup(const BtChar *archiveName, const BtChar *screenName, BtBool isBackground)
{
	BtStrCopy(m_archiveName, 32, archiveName);
	BtStrCopy(m_screenName, 32, screenName);
	BtStrCopy( m_lastArchiveName, 32, "" );
    m_isLoaded = BtFalse;

	// Set whether we want to render a background
	m_isBackground = isBackground;

	// Used to decide what size we are rendering to
	m_v2RenderDimension = RsUtil::GetDimension();

	m_backgroundSortOrder = MaxSortOrders-1;

	m_pCurrentMenuItem = BtNull;
	m_pCurrentMenuItemSelected = BtNull;
}

////////////////////////////////////////////////////////////////////////////////
// GetNumItems

BtU32 HlView::GetNumItems()
{
	return m_items.GetNumItems();
}

////////////////////////////////////////////////////////////////////////////////
// SetBackground

void HlView::SetBackground(BtBool isBackground)
{
	m_isBackground = isBackground;
}

////////////////////////////////////////////////////////////////////////////////
// IsEqual

BtBool HlView::IsEqual( const BtChar *string )
{
	if( strcmp( m_pCurrentMenuItemSelected->m_id, string ) == 0 )
	{
		return BtTrue;
	}
	return BtFalse;
}

////////////////////////////////////////////////////////////////////////////////
// LoadMenu

void HlView::LoadMenu()
{
	// Empty the items
	m_items.Empty();

	m_pCurrentMenuItemSelected = BtNull;

	tinyxml2::XMLDocument doc;
	BtBool isParsed = HlUserData::GetXML(doc, &m_archive, m_screenName );
	if(isParsed)
	{
       tinyxml2::XMLNode *pNode = doc.FirstChildElement( "MENU" );

		// Is there a child?
		while( pNode )
		{
			tinyxml2::XMLNode *pItem = pNode->FirstChildElement( "ITEM" );
	
			while( pItem )
			{
				HlMenuItems item;

				// Find the fade
				tinyxml2::XMLElement *pFade = pItem->FirstChildElement( "FADE" );
				if( pFade )
				{
					const BtChar *pFadeString = pFade->GetText();
					item.m_fade = atof( pFadeString );
				}

				// Get the joystick
				tinyxml2::XMLElement *pJoystickButton = pItem->FirstChildElement( "JOYSTICKBUTTON" );
				if( pJoystickButton )
				{
					const BtChar *pJoystickButtonString = pJoystickButton->GetText();
					if( BtStrCompareNoCase( pJoystickButtonString, "PAUSE" ) )
					{
						item.m_joystickButtonID = JoystickButton_Pause;
					}
					if( BtStrCompareNoCase( pJoystickButtonString, "A" ) )
					{
						item.m_joystickButtonID = JoystickButton_A;
					}
					else if( BtStrCompareNoCase( pJoystickButtonString, "B" ) )
					{
						item.m_joystickButtonID = JoystickButton_B;
					}
					else if( BtStrCompareNoCase( pJoystickButtonString, "Y" ) )
					{
						item.m_joystickButtonID = JoystickButton_Y;
					}
					else if( BtStrCompareNoCase( pJoystickButtonString, "X" ) )
					{
						item.m_joystickButtonID = JoystickButton_X;
					}
                    else if( BtStrCompareNoCase( pJoystickButtonString, "LEFTSHOULDER" ) )
					{
						item.m_joystickButtonID = JoystickButton_LeftShoulder;
					}
                    else if( BtStrCompareNoCase( pJoystickButtonString, "RIGHTSHOULDER" ) )
					{
						item.m_joystickButtonID = JoystickButton_RightShoulder;
					}
                    else if( BtStrCompareNoCase( pJoystickButtonString, "DPADLEFT" ) )
					{
						item.m_joystickButtonID = JoystickButton_Left;
					}
                    else if( BtStrCompareNoCase( pJoystickButtonString, "DPADRIGHT" ) )
					{
						item.m_joystickButtonID = JoystickButton_Right;
					}
				}

				// Get the screen width
				tinyxml2::XMLElement *pScreenWidth = pItem->FirstChildElement( "SCREENWIDTH" );
				if( pScreenWidth )
				{
					const BtChar *pScreenWidthString = pScreenWidth->GetText();
					item.m_v2OriginalScreenSize.x = atoi( pScreenWidthString );
				}
		
				// Get the screen height
				tinyxml2::XMLElement *pScreenHeight = pItem->FirstChildElement( "SCREENHEIGHT" );
				if( pScreenHeight )
				{
					const BtChar *pScreenWidthString = pScreenHeight->GetText();
					item.m_v2OriginalScreenSize.y = atoi( pScreenWidthString );
				}

				// Get the down
				GetFloatFromXML(pItem, "ANGLE", item.m_angle);

				// Navigation

				// Get the left
				GetIntFromXML(pItem, "LEFT", item.m_left );

				// Get the right
				GetIntFromXML(pItem, "RIGHT", item.m_right );

				// Get the up
				GetIntFromXML(pItem, "UP", item.m_up );

				// Get the down
				GetIntFromXML(pItem, "DOWN", item.m_down );

				// Get the width
				GetFloatFromXML(pItem, "WIDTH", item.m_v2OriginalDimension.x);

				// Get the height
				GetFloatFromXML(pItem, "HEIGHT", item.m_v2OriginalDimension.y);

				// Get the index 
				GetIntFromXML(pItem, "INDEX", item.m_spriteIndex);

				// Get the sprite name
				tinyxml2::XMLElement *pSpriteItem = pItem->FirstChildElement("SPRITE");
				if( pSpriteItem )
				{
					const BtChar *pSpriteName = pSpriteItem->GetText();
					item.m_pSprite = m_archive.GetSprite( pSpriteName );

					// Get the index
					tinyxml2::XMLElement *pElement = pItem->FirstChildElement( "SORTORDER" );
					if (pElement)
					{
						const BtChar *sortOrder = pElement->GetText();
						if (sortOrder)
						{
							item.m_sortOrder = atoi(sortOrder);
						}
					}

					// Find the sprite dimension
					item.m_v2OriginalDimension = item.m_pSprite->GetDimension(item.m_spriteIndex);
				}

				// Get the selected index
				GetIntFromXML( pItem, "SELECTEDINDEX", item.m_selectedSpriteIndex );

				// Get whether there is a background
				GetBoolFromXML(pItem, "BACKGROUND", m_isBackground );

				// Get the background sort order
 				GetUIntFromXML( pItem, "BACKGROUNDSORTORDER", m_backgroundSortOrder );
				
				// Get the x 
				const BtChar *xstring = pItem->FirstChildElement( "X" )->GetText();

				// Get the y 
				const BtChar *ystring = pItem->FirstChildElement( "Y" )->GetText();

				BtFloat x, y;

				if( atof( xstring ) == 0 )
				{
					if( BtStrCompareNoCase( xstring, "c" ) )
					{
						item.m_alignment |= HlMenuItemAlignment_HorizontalCentre;
						x = (item.m_v2OriginalScreenSize.x * 0.5f) - (item.m_v2OriginalDimension.x * 0.5f);
					}
					else if( BtStrCompareNoCase( xstring, "l" ) )
					{
						x = 0;
						item.m_alignment |= HlMenuItemAlignment_Left;
					}
					else if( BtStrCompareNoCase( xstring, "r" ) )
					{
						item.m_alignment |= HlMenuItemAlignment_Right;
						x = item.m_v2OriginalScreenSize.x - item.m_v2OriginalDimension.x;
					}
                    else
                    {
                        x = 0;
                    }
				}
				else
				{
					x = atof(xstring);
				}
		
				if (atof(ystring) == 0)
				{
                	if( BtStrCompareNoCase( ystring, "c" ) )
					{
						item.m_alignment |= HlMenuItemAlignment_VerticalCentre;
						y = (item.m_v2OriginalScreenSize.y * 0.5f) - (item.m_v2OriginalDimension.y * 0.5f);
					}
					else if ( BtStrCompareNoCase( ystring, "t") )
					{
						y = 0;
						item.m_alignment |= HlMenuItemAlignment_Top;
					}
					else if ( BtStrCompareNoCase( ystring, "b") )
					{
						y = item.m_v2OriginalScreenSize.y - item.m_v2OriginalDimension.y;
						item.m_alignment |= HlMenuItemAlignment_Bottom;
					}
                    else
                    {
                        y = 0;
                    }
				}
				else
				{
					y = atof(ystring);
				}

				// Set the final position
				item.m_v2OriginalPosition = MtVector2( x, y );

				// Refactor the position
				item.m_v2Position  = HlScreenSize::Refactor( item.m_v2OriginalScreenSize, item.m_v2OriginalPosition );

				// Refactor the size
				item.m_v2Dimension = HlScreenSize::Refactor( item.m_v2OriginalScreenSize, item.m_v2OriginalDimension );

				// Get the ID
				const BtChar *pID = pItem->FirstChildElement( "ID" )->GetText();
				BtStrCopy( item.m_id, 64, pID );

				item.m_isVisible = BtTrue;
				tinyxml2::XMLElement *pIsVisible = pItem->FirstChildElement( "VISIBLE" );
				if( pIsVisible )
				{
					const BtChar *pIsVisibleText = pIsVisible->GetText();

					if( BtStrCompareNoCase( pIsVisibleText, "TRUE" ) )
					{
						item.m_isVisible = BtTrue;
					}
					else if( BtStrCompareNoCase( pIsVisibleText, "FALSE" ) )
					{
						item.m_isVisible = BtFalse;
					}
				}

				// Get the fade when selected
				tinyxml2::XMLElement *pFadeWhenSelected = pItem->FirstChildElement( "FADEWHENSELECTED" );
				if( pFadeWhenSelected )
				{
					const BtChar *pFadeWhenSelectedString = pFadeWhenSelected->GetText();
					item.m_fadeWhenSelected = atof( pFadeWhenSelectedString );
				}
				else
				{
					item.m_fadeWhenSelected = 0.5f;
				}

				// Is this menu item selectable
				item.m_isSelectable = BtFalse;
				tinyxml2::XMLElement *pIsSlectable = pItem->FirstChildElement( "SELECTABLE" );
				if( pIsSlectable )
				{
					const BtChar *pIsSlectableText = pIsSlectable->GetText();

					if( BtStrCompareNoCase( pIsSlectableText, "TRUE" ) )
					{
						item.m_isSelectable = BtTrue;
					}
				}

				// Get whether this is apple TV only
				tinyxml2::XMLElement *pJoystickOnly = pItem->FirstChildElement("JOYSTICK");
				if (pJoystickOnly)
				{
					const BtChar *pJoystickOnlyText = pJoystickOnly->GetText();

					if (BtStrCompareNoCase(pJoystickOnlyText, "TRUE"))
					{
						item.m_joystick = HlMenuJoystick_Yes;
					}
					else
					{
						item.m_joystick = HlMenuJoystick_No;
					}
				}

				// Is this shown on Apple TV?
				GetBoolFromXML(pItem, "APPLETV", item.m_isAppleTV);

				// Add each item
				m_items.Add( item );

				// Move to the next child
				pItem = pItem->NextSiblingElement();
			}

			// Move to the next child
			pNode = pNode->NextSiblingElement();
		}
	}
	
	int a=0;
	a++;
}

////////////////////////////////////////////////////////////////////////////////
// UpdateMenu

void HlView::UpdateMenu()
{
	// Re-load the menu
	if (ApConfig::IsWin() && UiKeyboard::pInstance()->IsPressed(UiKeyCode_F2))
	{
		int a = 0;
		a++;
	}

	BtU32 numItems = m_items.GetNumItems();

	for (BtU32 i = 0; i < numItems; i++)
	{
		// Cache each item
		HlMenuItems &item = m_items[i];

		item.m_isPressed = BtFalse;
		item.m_isReleased = BtFalse;
		item.m_isHeld = BtFalse;
	}

	m_pCurrentMenuItemSelected = BtNull;

	// Refactor these for the current viewport
	for (BtU32 i = 0; i < numItems; i++)
	{
		// Cache each item
		HlMenuItems &item = m_items[i];

		// Refactor the position
		item.m_v2Position = HlScreenSize::Refactor(item.m_v2OriginalScreenSize, item.m_v2OriginalPosition, m_v2RenderDimension);

		// Refactor the size
		item.m_v2Dimension = HlScreenSize::Refactor(item.m_v2OriginalScreenSize, item.m_v2OriginalDimension, m_v2RenderDimension);
	}

    {
        // Move the cursor left
        if (ShJoystick::IsPressed(JoystickButton_Left ))
        {
            if (m_pCurrentMenuItem && m_pCurrentMenuItem->m_left != -1)
            {
                m_pCurrentMenuItem = FindItem(m_pCurrentMenuItem->m_left );
            }
        }

        // Move the cursor right
        if (ShJoystick::IsPressed(JoystickButton_Right ))
        {
            if (m_pCurrentMenuItem && m_pCurrentMenuItem->m_right != -1)
            {
                m_pCurrentMenuItem = FindItem(m_pCurrentMenuItem->m_right);
            }
        }

        // Move the cursor up
        if (ShJoystick::IsPressed(JoystickButton_Up))
        {
            if ( m_pCurrentMenuItem && m_pCurrentMenuItem->m_up != -1)
            {
                m_pCurrentMenuItem = FindItem(m_pCurrentMenuItem->m_up);
            }
        }

        // Move the cursor around
        if (ShJoystick::IsPressed(JoystickButton_Down))
        {
            if (m_pCurrentMenuItem && m_pCurrentMenuItem->m_down != -1)
            {
                m_pCurrentMenuItem = FindItem(m_pCurrentMenuItem->m_down );
            }
        }

        // Respond to enter
		if (UiKeyboard::pInstance()->IsReleased(UiKeyCode_RETURN))
		{
			m_pCurrentMenuItemSelected = m_pCurrentMenuItem;
			m_pCurrentMenuItemSelected->m_isPressed = BtFalse;
			m_pCurrentMenuItemSelected->m_isHeld = BtFalse;
			m_pCurrentMenuItemSelected->m_isReleased = BtTrue;
		}
		else if (UiKeyboard::pInstance()->IsPressed(UiKeyCode_RETURN))
		{
			m_pCurrentMenuItemSelected = m_pCurrentMenuItem;
			m_pCurrentMenuItemSelected->m_isPressed = BtFalse;
			m_pCurrentMenuItemSelected->m_isHeld = BtTrue;
			m_pCurrentMenuItemSelected->m_isReleased = BtFalse;
		}
		else if (UiKeyboard::pInstance()->IsHeld(UiKeyCode_RETURN))
		{
			m_pCurrentMenuItemSelected = m_pCurrentMenuItem;
			m_pCurrentMenuItemSelected->m_isPressed = BtFalse;
			m_pCurrentMenuItemSelected->m_isReleased = BtFalse;
			m_pCurrentMenuItemSelected->m_isHeld = BtTrue;
		}
    }

	if (!ApConfig::IsAppleTV())
	{
		// Have any buttons been touched with the mouse?
		for (BtU32 touch = 0; touch < MaxTouches; touch++)
		{
			BtU32 numItems = m_items.GetNumItems();

			for (BtU32 i = 0; i < numItems; i++)
			{
				// Cache each item
				HlMenuItems &item = m_items[i];

				if (item.m_joystickButtonID != JoystickButton_MAX)
				{
					if (ShJoystick::IsReleased(item.m_joystickButtonID))
					{
						item.m_isReleased = BtTrue;
						m_pCurrentMenuItem = &item;
					}
					if (ShJoystick::IsHeld(item.m_joystickButtonID))
					{
						item.m_isHeld = BtTrue;
					}
					if (ShJoystick::IsPressed(item.m_joystickButtonID))
					{
						item.m_isPressed = BtTrue;
					}
				}
			}
		}
        
        // Have any positions been touched?
        for (BtU32 touch = 0; touch < MaxTouches; touch++)
        {
            MtVector2 v2MousePosition = ShTouch::GetPosition(touch);
            
            for (BtU32 i = 0; i < numItems; i++)
            {
                // Cache each item
                HlMenuItems &item = m_items[i];
                
                if( item.m_isSelectable )
                {
                    if( v2MousePosition.x > item.m_v2Position.x )
                    {
                        if( v2MousePosition.x < item.m_v2Position.x + item.m_v2Dimension.x )
                        {
                            if( v2MousePosition.y > item.m_v2Position.y )
                            {
                                if( v2MousePosition.y < item.m_v2Position.y + item.m_v2Dimension.y )
                                {
                                    if( ShTouch::IsPressed( touch ) )
                                    {
                                        m_pCurrentMenuItemSelected = &item;
                                        item.m_isPressed = BtTrue;
                                    }
                                    if( ShTouch::IsHeld( touch ) )
                                    {
                                        if( &item == m_pCurrentMenuItemSelected )
                                        {
                                            m_pCurrentMenuItemSelected = &item;
                                            item.m_isHeld = BtTrue;
                                        }
                                    }
                                    if( ShTouch::IsReleased( touch ) )
                                    {
                                        if( &item == m_pCurrentMenuItemSelected )
                                        {
                                            m_pCurrentMenuItem = m_pCurrentMenuItemSelected;
                                            item.m_isReleased = BtTrue;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
	}
    else
    {
        for (BtU32 touch = 0; touch < MaxTouches; touch++)
        {
			if (ShTouch::IsReleased(touch))
            {
                m_pCurrentMenuItemSelected = m_pCurrentMenuItem;
                m_pCurrentMenuItemSelected->m_isPressed = BtFalse;
                m_pCurrentMenuItemSelected->m_isHeld = BtFalse;
                m_pCurrentMenuItemSelected->m_isReleased = BtTrue;
            }
			else if (ShTouch::IsPressed(touch))
			{
				m_pCurrentMenuItemSelected = m_pCurrentMenuItem;
				m_pCurrentMenuItemSelected->m_isPressed = BtTrue;
				m_pCurrentMenuItemSelected->m_isHeld = BtFalse;
				m_pCurrentMenuItemSelected->m_isReleased = BtFalse;
			}
			else if (ShTouch::IsHeld(touch))
			{
				m_pCurrentMenuItemSelected = m_pCurrentMenuItem;
				m_pCurrentMenuItemSelected->m_isPressed = BtFalse;
				m_pCurrentMenuItemSelected->m_isHeld = BtTrue;
				m_pCurrentMenuItemSelected->m_isReleased = BtFalse;
			}
		}
    }
}

////////////////////////////////////////////////////////////////////////////////
// Load

void HlView::Load()
{
	Unload();

	// Load the archive and flag the archive as loaded
	m_archive.Load( m_archiveName );
    
	// Flag the archive as loaded
	m_isLoaded = BtTrue;

	// Make a note of the archive name
    BtStrCopy( m_lastArchiveName, 32, m_archiveName );

	// Load the menu now we have the archive
	LoadMenu();

	if( m_isBackground )
	{
		// The pointer to the material needs to be cached at runtime rather than at setup
		// Get the material which has the same name as the archive for simplicity
		m_pBackgroundMaterial = m_archive.GetMaterial( m_screenName );
	}

	// Set the selection to NULL
	m_pCurrentMenuItemSelected = BtNull;

	if (m_items.GetNumItems() && !m_pCurrentMenuItem)
	{
		m_pCurrentMenuItem = &m_items[0];
	}
}

////////////////////////////////////////////////////////////////////////////////
// Update

void HlView::Update()
{
	if( BtStrCompare( m_lastArchiveName, m_archiveName ) == BtFalse )
	{
		if( m_pLast )
		{
			m_pLast->OnExit();
		}

		Unload();
		Load();
		OnEntry();
	}
 
	// Update the menu
	UpdateMenu();

	m_pLast = this;
}

////////////////////////////////////////////////////////////////////////////////
// OnEntry

void HlView::OnEntry()
{
	int a=0;
	a++;
}

////////////////////////////////////////////////////////////////////////////////
// OnExit

void HlView::OnExit()
{
	int a = 0;
	a++;
}

////////////////////////////////////////////////////////////////////////////////
// Unload

void HlView::Unload()
{
	if( m_isLoaded )
	{	
		BtStrCopy( m_lastArchiveName, "");

		// Unload the menu items
		m_items.Empty();

		// Unload the archive
		m_archive.Unload();

		// Unload the archive
		m_isLoaded = BtFalse;
	}	
}

////////////////////////////////////////////////////////////////////////////////
// FindItem

HlMenuItems *HlView::FindItem( const BtChar *name )
{
	BtU32 numItems = m_items.GetNumItems();

	for( BtU32 i=0; i<numItems; i++ )
	{
		// Cache each item
		HlMenuItems &item = m_items[i];

		if( BtStrCompareNoCase( item.m_id, name ) )
		{
			return &item;
		}
	}
	return BtNull;
}

////////////////////////////////////////////////////////////////////////////////
// FindItem

HlMenuItems *HlView::FindItem( BtU32 id )
{
	// Cache each item
	HlMenuItems &item = m_items[id];

	return &item;
}

////////////////////////////////////////////////////////////////////////////////
// RenderMenu

void HlView::RenderMenu()
{
	BtU32 numItems = m_items.GetNumItems();

	for( BtU32 i=0; i<numItems; i++ )
	{
		// Cache each item
		HlMenuItems &item = m_items[i];

		if( item.m_isVisible )
		{
			// Setup the colour
			RsColour colour( RsColour::WhiteColour() );
			colour.Alpha( item.m_fade );

			if (item.m_pSprite)
			{
				// Get the index of the sprite
				BtU32 index = item.m_spriteIndex;

				if (item.m_isHeld == BtTrue)
				{
					if (item.m_selectedSpriteIndex != -1)
					{
						index = item.m_selectedSpriteIndex;
					}
					else
					{
						colour = RsColour(item.m_fadeWhenSelected, item.m_fadeWhenSelected, item.m_fadeWhenSelected, 1.0f);
					}
				}

				if (ShJoystick::IsConnected() == BtTrue)
				{
					if (item.m_joystick != HlMenuJoystick_No)
					{
						// Render each item
						item.m_pSprite->Render(BtFalse, item.m_v2Position, item.m_v2Dimension, index, 0, colour, MaxSortOrders - 1);
					}
				}
				else
				{
					if (item.m_joystick != HlMenuJoystick_Yes)
					{
						// Render each item
						item.m_pSprite->Render(BtFalse, item.m_v2Position, item.m_v2Dimension, index, 0, colour, MaxSortOrders - 1);
					}
				}
			}
			
			//if( ApConfig::IsDebug() )
			{
				RsColour light(1.0f, 1.0f, 1.0f, 0.25f);

				if( &item == m_pCurrentMenuItem)
				{
					light = RsColour(1.0f, 0.7f, 0.5f, 0.8f);
				}
				HlDraw::RenderRotatedQuad( item.m_v2Position, item.m_v2Dimension, item.m_angle, light, MaxSortOrders - 1);
			}
		}
        else
        {
            HlDraw::RenderQuad( item.m_v2Position, item.m_v2Dimension, RsColour( 1.0f, 0, 0, 0.5f), MaxSortOrders-1 );
        }
	}
}

////////////////////////////////////////////////////////////////////////////////
// Render

void HlView::Render()
{
	if( !m_isLoaded )
	{
		return;
	}

//	if( BtStrCompare( m_lastArchiveName, m_archiveName ) == BtFalse )
//	{
//		Unload();
//		Load();
//	}

	if( UiKeyboard::pInstance()->IsPressed( ReloadMenuKey ) )
	{
		Unload();
        Load();
    }

	if( m_isBackground )
    {
		BtAssert( m_pBackgroundMaterial );

		// Render from the top left corner 0, 0 downward
        RsColour colour( RsColour::WhiteColour() );
		MtVector2 v2Position = MtVector2(0, 0);
        m_pBackgroundMaterial->Render( v2Position, m_v2RenderDimension, colour, m_backgroundSortOrder );
    }

	// Render the menu
	RenderMenu();
}

////////////////////////////////////////////////////////////////////////////////
// SetVisible

void HlView::SetVisible( BtU32 id, BtBool isVisible )
{
	HlMenuItems &item = m_items[id];
	item.m_isVisible = isVisible;
}

////////////////////////////////////////////////////////////////////////////////
// SetRenderDimension

void HlView::SetRenderDimension(MtVector2 v2Dimension)
{
	m_v2RenderDimension = v2Dimension;
}

////////////////////////////////////////////////////////////////////////////////
// SetVisible

void HlView::SetVisible( const BtChar *name, BtBool isVisible )
{
	BtU32 numItems = m_items.GetNumItems();

	for( BtU32 i=0; i<numItems; i++ )
	{
		// Cache each item
		HlMenuItems &item = m_items[i];

		if( BtStrCompareNoCase( item.m_id, name ) )
		{
			// Set the visible flag
			item.m_isVisible = isVisible;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// GetVisible

BtBool HlView::GetVisible( BtU32 id )
{
	HlMenuItems &item = m_items[id];
	return item.m_isVisible;
}