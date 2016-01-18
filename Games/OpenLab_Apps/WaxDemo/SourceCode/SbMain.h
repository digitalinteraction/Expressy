////////////////////////////////////////////////////////////////////////////////
// SbMain.h

// Include guard
#pragma once
#include "BtTypes.h"
#include "BaArchive.h"
#include "MtVector3.h"
#include "SbCamera.h"
#include "SdSound.h"
#include "GaProject.h"
#include "SdSound.h"
#include "SbJigsaw.h"
#include "SbPiano.h"
#include "SbWaxModel.h"
#include "SbPhysics.h"
#include "SbWaxGraph.h"
#include "SbPaint.h"

class SbMain;
class RsTexture;
class RsShader;
class RsMaterial;
class SgNode;

const BtFloat WinWidth  = 800.0f;
const BtFloat WinHeight = 600.0f;
const BtFloat NormalWidth  = 960.0f;
const BtFloat NormalHeight = 640.0f;

// Class definition
class SbMain : public GaProject
{
public:

	// Public functions
	void							Init();
	void							Reset();
	void							Create();
	void							Update();
	void							Render();
	void							PreRender();
	void							PostRender();
	void							Destroy();

	// Accessors
    static BtU32                    GetDemo() { return demo; }
    
private:

	// Private functions
	void							SetupRender();

	void							UpdateTest();
	void							RenderScene();

	void							RenderTests();
	void							RenderFont();

	// Private members
	BaArchive						m_gameArchive;
	BaArchive						m_utilityArchive;
	BaArchive						m_pianoArchive;

	// Resources
	RsMaterial					   *m_pTouch;
	RsShader					   *m_pShader;

	MtVector3						m_v3Position;
	SbCamera						m_camera;
	RsMaterial					   *m_pWhite2;
	RsMaterial					   *m_pWhite3;

	// Test classes
	SbPhysics						m_physics;
    SbWaxGraph                      m_graph;
	SbWaxModel						m_model;
	SbPiano							m_piano;
	SbJigsaw						m_jigsaw;
	SbPaint							m_paint;
    
    BtBool                          m_isDemoName;
    static BtU32                    demo;
};
