#include "Plugin.hpp"
#include "NXModule.hpp"

// Create a global for this plugin
CPlugin g_Plugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPlugin, IServerPluginCallbacks, "ISERVERPLUGINCALLBACKS003", g_Plugin);

CPlugin::CPlugin()
{
	this->m_nDebugID = 42; // EVENT_DEBUG_ID_INIT
}
CPlugin::~CPlugin()
{
	this->m_nDebugID = 13; // EVENT_DEBUG_ID_SHUTDOWN
}

bool CPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
#ifdef __SWITCH__
	if (!InitNXModuleBases())
	{
		return false;
	}
#endif

	return true;
}

const char* CPlugin::GetPluginDescription()
{
#if PORTAL
    return "Portal NX Plugin";
#elifdef PORTAL2
    return "Portal 2 NX Plugin";
#endif
}

#pragma region NOT_USED
// We don't need to tamper with these native callbacks
void CPlugin::Unload() {}
void CPlugin::Pause() {}
void CPlugin::UnPause() {}
void CPlugin::LevelInit(char const* pMapName) {}
void CPlugin::ServerActivate(edict_t* pEdictList, int edictCount, int clientMax) {}
void CPlugin::GameFrame(bool simulating) {}
void CPlugin::LevelShutdown() {}
void CPlugin::ClientActive(edict_t* pEntity) {}
void CPlugin::ClientFullyConnect(edict_t* pEdict) {}
void CPlugin::ClientDisconnect(edict_t* pEntity) {}
void CPlugin::ClientPutInServer(edict_t* pEntity, char const* playername) {}
void CPlugin::SetCommandClient(int index) {}
void CPlugin::ClientSettingsChanged(edict_t* pEdict) {}
int CPlugin::ClientConnect(bool* bAllowConnect, edict_t* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) { return 0; }
int CPlugin::ClientCommand(edict_t* pEntity, const CCommand& args) { return 0; }
int CPlugin::NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) { return 0; }
void CPlugin::OnQueryCvarValueFinished(int iCookie, edict_t* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) {}
void CPlugin::OnEdictAllocated(edict_t* edict) {}
void CPlugin::OnEdictFreed(const edict_t* edict) {}
void CPlugin::FireGameEvent(IGameEvent* event) {}
int CPlugin::GetEventDebugID() { return this->m_nDebugID; }
#pragma endregion
