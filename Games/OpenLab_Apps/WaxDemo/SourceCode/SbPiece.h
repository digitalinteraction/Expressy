////////////////////////////////////////////////////////////////////////////////
// SbPiece.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "MtVector2.h"
#include "MtMatrix4.h"
#include "SbWidget.h"
#include "BtArray.h"

////////////////////////////////////////////////////////////////////////////////

class RsTexture;
class UiInput;
class BaArchive;
class RsSprite;
class RsMaterial;

const BtU32 East  = 0;
const BtU32 West  = 1;
const BtU32 North = 2;
const BtU32 South = 3;

// Class definition
class SbPiece : public SbWidget
{
public:

	// Constructor
	SbPiece();

	// Public functions
	void							Update();
	void							ConstructExtents();
	void							Render();

//private:

	// Private functions
	
	// Members
	BtU32							m_pieceID;
	BtU32							m_height;

	MtVector3						m_v3Pos[4];
	MtVector2						m_v2Pos[4];
	SbPiece						   *m_pPieces[4];
	BtBool							m_isLinked[4];
	MtMatrix4						m_tabs[4];
	MtPlane							plane[4];
	MtMatrix4						m_translatedTabs[4];
	BtBool							m_isChecked;

	BtFloat							x;
	BtFloat							y;
	BtFloat							x2;
	BtFloat							y2;
};
