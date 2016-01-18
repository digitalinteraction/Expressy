////////////////////////////////////////////////////////////////////////////////
// SbWaxGraph.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "RsCamera.h"
#include "MtMatrix4.h"

class RsModel;
class BaArchive;

// Class definition
class SbWaxGraph
{
public:

	// Public functions
	void							Init();
	void							Setup( BaArchive *pArchive );
	void							Update();
	void							Render();
    
    static void                     AddIMUData();
    
	// Accessors

private:

	// Private functions
    void                            RenderGraph();
    
	// Private members
	SgNode						   *m_pWaxModel;
	RsShader					   *m_pShader;
    MtMatrix4                       m4Rotate;
};
