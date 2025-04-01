#include "ServerPlugin.hpp"
#include <string.h>

InterfaceReg::InterfaceReg(InstantiateInterfaceFn fn, const char *pName)
	: m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

InterfaceReg *InterfaceReg::s_pInterfaceRegs = nullptr;

static void *CreateInterfaceInternal(const char *pName, int *pReturnCode)
{
	InterfaceReg *pCur;

	for (pCur = InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
	{
		if (!strcmp(pCur->m_pName, pName))
		{
			if (pReturnCode)
			{
				*pReturnCode = 0; // IFACE_OK
			}
			return pCur->m_CreateFn();
		}
	}

	if (pReturnCode)
	{
		*pReturnCode = 1; // IFACE_FAILED
	}
	return nullptr;
}

extern "C" void *CreateInterface(const char *pName, int *pReturnCode)
{
	return CreateInterfaceInternal(pName, pReturnCode);
}
