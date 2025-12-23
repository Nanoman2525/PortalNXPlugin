#include "Plugin.hpp"
#include "NXModule.hpp"
#include "Offsets.hpp"
#include "Command.hpp"
#include "Variable.hpp"

void *g_pCVar = nullptr;
void *g_pInputSystem = nullptr;
void *g_GameMovement = nullptr;
void *engineClient = nullptr;

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
	// Figure out which Portal game this is
	g_pCVar = interfaceFactory("VEngineCvar004", nullptr);
	if (!g_pCVar)
	{
		g_pCVar = interfaceFactory("VEngineCvar007", nullptr);
		if (!g_pCVar)
		{
			this->m_bNoUnload = true;
			return false; // We aren't familiar with this build (Possibly a new update for either game)
		}

		this->m_bIsGamePortal2 = true;
	}

	g_pInputSystem = interfaceFactory("InputSystemVersion001", nullptr);
	g_GameMovement = gameServerFactory("GameMovement001", nullptr);

	if (this->m_bIsGamePortal2)
	{
		engineClient = interfaceFactory("VEngineClient015", nullptr);
	}
	else
	{
		engineClient = interfaceFactory("VEngineClient014", nullptr);
	}

	// The rest of the logic depends on having correct offsets to work with, so run this first
	InitOffsets(this->m_bIsGamePortal2);

#ifdef __SWITCH__
	if (!InitNXModuleBases(this->m_bIsGamePortal2))
	{
		return false;
	}
#endif

	Command::RegisterAll();
	Variable::RegisterAll();

	// Remove Portal 2 patches and features if this is the Portal build
	if (!this->m_bIsGamePortal2)
	{
		extern Variable nx_enable_ladders;
		nx_enable_ladders.Unregister();

		extern Variable nx_max_players_override;
		nx_max_players_override.Unregister();

		extern Command nx_toggle_reportals;
		nx_toggle_reportals.Unregister();
	}

	void ToggleVTableDetours(bool bPatching);
	ToggleVTableDetours(true);

	void InitNXInput(bool bIsPortal2Build);
	InitNXInput(this->m_bIsGamePortal2);

	return true;
}

void CPlugin::Unload()
{
	if (this->m_bNoUnload)
	{
		return;
	}

	void ShutdownNXInput();
	ShutdownNXInput();

	void ToggleVTableDetours(bool bPatching);
	ToggleVTableDetours(false);

	Variable::UnregisterAll();
	Command::UnregisterAll();

	// Note that unloading plugins just crashes both games for some reason. Not sure why lol
}

const char* CPlugin::GetPluginDescription()
{
    return "PortalNXPlugin, Nanoman2525";
}

#pragma region NOT_USED
// We don't need to tamper with these native callbacks
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
bool CPlugin::IsGamePortal2() { return this->m_bIsGamePortal2; }
#pragma endregion
