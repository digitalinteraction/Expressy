////////////////////////////////////////////////////////////////////////////////
// RsUtil.h

#pragma once
#include "BtArray.h"
#include "BtTypes.h"
#include "MtVector2.h"
#include "RsColour.h"
#include "RsCaps.h"
#include "RsRenderTarget.h"

enum RsOrientation
{
	RsOrientation_Portrait,
	RsOrientation_PortraitUpsideDown,
	RsOrientation_Landscape,
	RsOrientation_LandscapeUpsideDown,
};

// Class declaration
class RsUtil
{
public:

	// Public functions			
	static void*							GetHandle();
	static void								EmptyRenderTargets();

	// Accessors
	static RsRenderTarget				   *GetNewRenderTarget();

	static RsCaps						   *GetCaps();

	static BtFloat							GetFPS();
	static MtVector2 						GetScreenCenter();
	
	static BtU32							GetNumScreenModes();
	static MtVector2 						GetScreenMode();

	static MtVector2 						GetDimension();
	static void								SetDimension( const MtVector2& v2Dimension );

	static MtVector2 						GetHalfDimension();

	static void								SetFullScreen( BtBool isFullScreen );
	static BtBool							GetFullScreen();

	static BtFloat	 						GetAspect();
	static MtVector2	 					GetScreenPosition( const MtVector2 &v2ScreenPosition );

	static RsOrientation					GetOrientation();
	static void								SetOrientation( RsOrientation orientation );

	static BtU32							GetWidth();
	static BtU32							GetHeight();
	
	static BtBool							Is16x9();

	static BtU32							GetRefreshRate();
};
