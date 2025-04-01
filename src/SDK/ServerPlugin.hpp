#ifndef SERVERPLUGIN_HPP
#define SERVERPLUGIN_HPP

#include <cstdint> // uint64_t def

typedef void*(*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void*(*InstantiateInterfaceFn)();

class InterfaceReg
{
public:
	InterfaceReg(InstantiateInterfaceFn fn, const char *pName);

public:
	InstantiateInterfaceFn	m_CreateFn;
	const char				*m_pName;

	InterfaceReg			*m_pNext; // For the global list.
	static InterfaceReg		*s_pInterfaceRegs;
};

#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName)                         \
	static void *__Create##className##interfaceName##_interface() { return static_cast<interfaceName *>(&globalVarName); } \
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);

//-----------------------------------------------------------------------------
// edict_t definition
//-----------------------------------------------------------------------------
class IServerUnknown;
class IServerNetworkable;

class CBaseEdict
{
public:
	inline IServerUnknown* GetUnknown()
	{
		return 			m_pUnk;
	}

#ifdef _XBOX
	unsigned short 		m_fStateFlags;
#else
	int					m_fStateFlags;
#endif
	int 				m_NetworkSerialNumber;
	IServerNetworkable	*m_pNetworkable;

protected:
	IServerUnknown		*m_pUnk;
};
struct edict_t : public CBaseEdict
{
};

//-----------------------------------------------------------------------------
// Plugin callbacks interface class
//-----------------------------------------------------------------------------
class CCommand;

class IServerPluginCallbacks
{
public:
	virtual bool            Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) = 0;
	virtual void            Unload() = 0;
	virtual void            Pause() = 0;
	virtual void            UnPause() = 0;
	virtual const char*     GetPluginDescription() = 0;
	virtual void            LevelInit(char const* pMapName) = 0;
	virtual void            ServerActivate(edict_t* pEdictList, int edictCount, int clientMax) = 0;
	virtual void            GameFrame(bool simulating) = 0;
	virtual void            LevelShutdown() = 0;
	virtual void            ClientActive(edict_t* pEntity) = 0;
	virtual void            ClientFullyConnect(edict_t* pEdict) = 0;
	virtual void            ClientDisconnect(edict_t* pEntity) = 0;
	virtual void            ClientPutInServer(edict_t* pEntity, char const* playername) = 0;
	virtual void            SetCommandClient(int index) = 0;
	virtual void            ClientSettingsChanged(edict_t* pEdict) = 0;
	virtual int             ClientConnect(bool* bAllowConnect, edict_t* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) = 0;
	virtual int             ClientCommand(edict_t* pEntity, const CCommand& args) = 0;
	virtual int             NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) = 0;
	virtual void            OnQueryCvarValueFinished(int iCookie, edict_t* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) = 0;
	virtual void            OnEdictAllocated(edict_t* edict) = 0;
	virtual void            OnEdictFreed(const edict_t* edict) = 0;
};

//-----------------------------------------------------------------------------
// Game events interface class
//-----------------------------------------------------------------------------
class IGameEvent;

class IGameEventListener2
{
public:
	virtual	        ~IGameEventListener2() {};
	virtual void    FireGameEvent(IGameEvent* event) = 0;
	virtual int	    GetEventDebugID() = 0;
};

#endif // SERVERPLUGIN_HPP
