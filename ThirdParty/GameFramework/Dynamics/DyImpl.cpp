////////////////////////////////////////////////////////////////////////////////
/// DyImpl.cpp

#include "DyImpl.h"
#include "DyWorld.h"
#include "btRigidBody.h"

DyWorld DyImpl::m_world;
BtBool DyImpl::m_isPaused = BtFalse;

////////////////////////////////////////////////////////////////////////////////
// Create

//static
void DyImpl::Create()
{
	m_world.Create();
}

////////////////////////////////////////////////////////////////////////////////
// Update

//static
void DyImpl::Update()
{
	m_world.Update();
}

////////////////////////////////////////////////////////////////////////////////
// Destroy

//static
void DyImpl::Destroy()
{
	m_world.Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// AddRigidBody

void DyImpl::AddRigidBody( btRigidBody* pRigidBody )
{
	m_world.AddRigidBody( pRigidBody );
}

////////////////////////////////////////////////////////////////////////////////
// RemoveSoftBody

void DyImpl::RemoveSoftBody(btSoftBody* pSoftBody)
{
	m_world.RemoveSoftBody(pSoftBody);
}

////////////////////////////////////////////////////////////////////////////////
// RemoveRigidBody

void DyImpl::RemoveRigidBody( btRigidBody* pRigidBody )
{
	m_world.RemoveRigidBody( pRigidBody );
}

////////////////////////////////////////////////////////////////////////////////
// pWorld

DyWorld* DyImpl::pWorld()
{
	return &m_world;
}

////////////////////////////////////////////////////////////////////////////////
// IsPaused

BtBool DyImpl::IsPaused()
{
	return m_isPaused;
}

////////////////////////////////////////////////////////////////////////////////
// SetPaused

void DyImpl::SetPaused(BtBool isPaused)
{
	m_isPaused = isPaused;
}
