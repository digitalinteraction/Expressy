////////////////////////////////////////////////////////////////////////////////
// SbPiano.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "RsCamera.h"
#include "MtMatrix4.h"
#include "SdSound.h"
#include "ShTouch.h"

class RsModel;
class BaArchive;
class RsMaterial;

// Class definition
class SbPiano
{
public:

	// Public functions
	void							Init();
	void							Setup( BaArchive *pSoundArchive, BaArchive *pGameArchive);
	void							Update();
	void							Render();

	// Accessors

private:

	// Private functions

	// Private members
	RsMaterial					   *m_pPianoMaterial;
};
