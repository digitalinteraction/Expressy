////////////////////////////////////////////////////////////////////////////////
// SbJigsaw.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "RsCamera.h"
#include "MtMatrix4.h"
#include "SdSound.h"
#include "ShTouch.h"
#include "SbPiece.h"

class RsModel;
class BaArchive;
class RsMaterial;
class SbPiece;

// Class definition
class SbJigsaw
{
public:

	// Public functions
	void							Init();
	void							Reset();
	void							Setup( BaArchive *pArchive );
	void							Update();
	void							Render();

	// Accessors

private:

	// Private functions
    void                            ExpressiveTouch();
    void                            Sort();
	void							Render( SbPiece &piece );
	void							MoveBoard( SbPiece *piece );
	void							Clear();
	void						    IsLinked( SbPiece *piece );

	// Private members
	BtFloat							m_pieceWidth;
	BtFloat							m_pieceHeight;
	BtFloat							m_halfPieceWidth;
	BtFloat							m_halfPieceHeight;
	RsMaterial					   *m_pCircle;
	RsMaterial					   *m_pJigsaw;
	SbPiece						   *m_pLinked;
};
