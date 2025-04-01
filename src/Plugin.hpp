#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "SDK/ServerPlugin.hpp"

class CPlugin : public IServerPluginCallbacks, public IGameEventListener2
{
public:
    CPlugin();
    ~CPlugin();

    // IServerPluginCallbacks
	virtual bool            Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void            Unload();
	virtual void            Pause();
	virtual void            UnPause();
	virtual const char*     GetPluginDescription();
	virtual void            LevelInit(char const* pMapName);
	virtual void            ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
	virtual void            GameFrame(bool simulating);
	virtual void            LevelShutdown();
	virtual void            ClientActive(edict_t* pEntity);
	virtual void            ClientFullyConnect(edict_t* pEdict);
	virtual void            ClientDisconnect(edict_t* pEntity);
	virtual void            ClientPutInServer(edict_t* pEntity, char const* playername);
	virtual void            SetCommandClient(int index);
	virtual void            ClientSettingsChanged(edict_t* pEdict);
	virtual int             ClientConnect(bool* bAllowConnect, edict_t* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen);
	virtual int             ClientCommand(edict_t* pEntity, const CCommand& args);
	virtual int             NetworkIDValidated(const char* pszUserName, const char* pszNetworkID);
	virtual void            OnQueryCvarValueFinished(int iCookie, edict_t* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue);
	virtual void            OnEdictAllocated(edict_t* edict);
	virtual void            OnEdictFreed(const edict_t* edict);

    // IGameEventListener2
	virtual void 			FireGameEvent(IGameEvent* event);
	virtual int	 			GetEventDebugID();

private:
    // IGameEventListener2
    int 					m_nDebugID;
};

extern CPlugin plugin;

#endif // PLUGIN_HPP
