///////////////////////////////////////////////////////////////////////////////
// SgBlendShapeDX11.cpp

// Includes
#include "BtTypes.h"
#include "SgBlendShapeImpl.h"

////////////////////////////////////////////////////////////////////////////////
// FixPointers

void SgBlendShapeImpl::FixPointers( BtU8 *pFileData )
{
	// Set the file data
	m_pFileData = (BaSgBlendShapeFileData*) pFileData;
}

////////////////////////////////////////////////////////////////////////////////
// Items

BtU32 SgBlendShapeImpl::Items()
{
	return m_pFileData->m_items;
}

////////////////////////////////////////////////////////////////////////////////
// Weight

void SgBlendShapeImpl::Weight( BtU32 iIndex, BtFloat weight )
{
	m_pFileData->m_weight[iIndex] = weight;
}

////////////////////////////////////////////////////////////////////////////////
// Weight

BtFloat SgBlendShapeImpl::Weight( BtU32 iIndex )
{
	return m_pFileData->m_weight[iIndex];
}

////////////////////////////////////////////////////////////////////////////////
// Weights

BtFloat* SgBlendShapeImpl::pWeights() const
{
	return m_pFileData->m_weight;
}