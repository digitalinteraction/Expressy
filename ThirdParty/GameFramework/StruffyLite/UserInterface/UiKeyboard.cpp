////////////////////////////////////////////////////////////////////////////////
// UiKeyboard.cpp

#include "UiKeyboard.h"

UiKeyboard *UiKeyboard::m_pInstance = BtNull;

////////////////////////////////////////////////////////////////////////////////
// pInstance

UiKeyboard *UiKeyboard::pInstance()
{
	return m_pInstance;
}
