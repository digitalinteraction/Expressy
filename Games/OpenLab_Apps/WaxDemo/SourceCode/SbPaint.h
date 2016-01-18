////////////////////////////////////////////////////////////////////////////////
// TsPaint.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "MtVector3.h"
#include "MtMatrix4.h"
#include "BtArray.h"
#include "RsCamera.h"
#include "MtVector3.h"
#include "RsMaterial.h"
#include "RsVertex.h"

class RsMaterial;

// Class definition
class SbPaint
{
public:

	// Public function
	void				Reset();
	void				Setup( BaArchive *pArchive );
	void				Update();
	void				Render();
	void				PreRender();

	MtVector2			GetScreenPositionFromMouse(MtVector2 v2Position);

	// Accessors
    BtBool              IsPainting();
    BtFloat             GetRoll() { return m_roll; }
    
private:
	
	void				SetupRenderToTexture();
   
    static RsMaterial  *m_pRenderTarget;
    BtBool              m_isRender;
	BtBool				m_isBlob;
	BtBool				m_isClear;
	MtVector2			m_v2Last;
	MtVector2			m_v2LastL;
	MtVector2			m_v2LastR;
	BtBool				m_isFirst;
	RsMaterial		   *m_pBrush;
    BtBool              m_isPainting;
    RsVertex2           m_strokeVertex[4];
	RsVertex2           m_blobVertex[4];
	BtU32               m_width;
    BtU32               m_height;
    BtFloat             m_roll;
	BtFloat				m_hardness;
	BtFloat				m_thickness;
};

