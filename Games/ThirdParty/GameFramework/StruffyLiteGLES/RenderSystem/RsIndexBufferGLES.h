////////////////////////////////////////////////////////////////////////////////
/// RsIndexBufferWin32.h

#ifndef __RsIndexBufferWin32_h__
#define __RsIndexBufferWin32_h__

#include "BaArchive.h"
#include "BaResource.h"
#include "BaFileData.h"
#include "RsGLES.h"

enum IndType
{
	IndType_Short = 1 << 0,
	IndType_Dynamic = 1 << 1,
};

// Class Declaration
class RsIndexBufferWinGL : public BaResource
{
public:

	DECLARE_RESOURCETYPE( BaRT_IndexBuffer );

	RsIndexBufferWinGL();

	void							FixPointers( BtU8 *pFileData, BaArchive *pArchive );
	void							CreateOnDevice();
	void							Copy( BtU8* pData, BtU32 nIndices );
	void							RemoveFromDevice();
	void							SetIndices();

private:

	friend class RsImplWinDX;

	GLuint							m_indexBuffer;
	BaIndexBufferFileData*			m_pFileData;
	BtU8*							m_pData;
	BtU32							m_indexSize;
	BtU32							m_lockType;
};

#endif // __RcIndexBuffer_h__
