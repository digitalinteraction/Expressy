////////////////////////////////////////////////////////////////////////////////
// SbWaxModel.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "RsCamera.h"
#include "MtMatrix4.h"

class RsModel;
class BaArchive;

// Class definition
class SbWaxModel
{
public:

	// Public functions
	void							Init();
	void							Setup( BaArchive *pArchive );
	void							Update();
	void							Render();

	// Accessors

private:

	// Private functions

	// Private members
	SgNode						   *m_pWaxModel;
	RsShader					   *m_pShader;
};
