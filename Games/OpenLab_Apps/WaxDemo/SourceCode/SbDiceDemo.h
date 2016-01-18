////////////////////////////////////////////////////////////////////////////////
// SbDiceDemo.h

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

// Class definition
class SbDiceDemo
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
	// Private members
	RsMaterial					   *m_pCircle;
};
