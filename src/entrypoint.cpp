#include <stdio.h>
#include <thread>
#include "entrypoint.h"

#include <tier1/convar.h>
#include <interfaces/interfaces.h>
#include <metamod_oslink.h>

#include <steam/steam_gameserver.h>

#include "addons/addons.h"
#include "encoders/msgpack.h"
#include "addons/clients.h"
#include "configuration/Configuration.h"
#include "commands/CommandsManager.h"
#include "crashreporter/CallStack.h"
#include "crashreporter/CrashReport.h"
#include "database/DatabaseManager.h"
#include "gameevents/gameevents.h"
#include "logs/Logger.h"
#include "precacher/precacher.h"
#include "translations/Translations.h"
#include "filters/ConsoleFilter.h"
#include "menus/MenuManager.h"
#include "resourcemonitor/ResourceMonitor.h"
#include "hooks/NativeHooks.h"
#include "player/PlayerManager.h"
#include "plugins/PluginManager.h"
#include "plugins/core/scripting.h"
#include "signatures/Signatures.h"
#include "signatures/Patches.h"
#include "signatures/Offsets.h"
#include "voicemanager/VoiceManager.h"
#include "usermessages/usermessages.h"

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);
SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);
SH_DECL_HOOK5_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0, CPlayerSlot, ENetworkDisconnectionReason, const char*, uint64, const char*);
SH_DECL_HOOK1_void(IServerGameClients, ClientSettingsChanged, SH_NOATTRIB, 0, CPlayerSlot);
SH_DECL_HOOK6_void(IServerGameClients, OnClientConnected, SH_NOATTRIB, 0, CPlayerSlot, const char*, uint64, const char*, const char*, bool);
SH_DECL_HOOK6(IServerGameClients, ClientConnect, SH_NOATTRIB, 0, bool, CPlayerSlot, const char*, uint64, const char*, bool, CBufferString*);
SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandHandle, const CCommandContext&, const CCommand&);
SH_DECL_HOOK0_void(IServerGameDLL, GameServerSteamAPIActivated, SH_NOATTRIB, 0);
SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, CPlayerSlot, const CCommand&);

#ifdef _WIN32
FILE _ioccc[] = { *stdin, *stdout, *stderr };
extern "C" FILE* __cdecl __iob_func(void)
{
    return _ioccc;
}
#endif

//////////////////////////////////////////////////////////////
/////////////////  Core Variables & Functions  //////////////
////////////////////////////////////////////////////////////

Swiftly g_Plugin;
ISource2Server* server = nullptr;
IServerGameClients* gameclients = nullptr;
IVEngineServer2* engine = nullptr;
IServerGameClients* g_clientsManager = nullptr;
ICvar* icvar = nullptr;
ICvar* g_pcVar = nullptr;
IGameResourceService* g_pGameResourceService = nullptr;
CEntitySystem* g_pEntitySystem = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
IGameEventManager2* g_gameEventManager = nullptr;
IGameEventSystem* g_pGameEventSystem = nullptr;
CEntityListener g_entityListener;
CSteamGameServerAPIContext g_SteamAPI;
CSchemaSystem* g_pSchemaSystem2 = nullptr;
CCSGameRules* gameRules = nullptr;

class CGameResourceService
{
public:
    CGameEntitySystem* GetGameEntitySystem()
    {
        return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(this) + g_Offsets->GetOffset("GameEntitySystem"));
    }
};

CGameEntitySystem* GameEntitySystem()
{
    return g_pGameEntitySystem;
}

//////////////////////////////////////////////////////////////
/////////////////      Internal Variables      //////////////
////////////////////////////////////////////////////////////

CUtlVector<FuncHookBase*> g_vecHooks;

Addons g_addons;
CommandsManager* g_commandsManager = nullptr;
Configuration* g_Config = nullptr;
ConsoleFilter* g_conFilter = nullptr;
Translations* g_translations = nullptr;
Logger* g_Logger = nullptr;
PlayerManager* g_playerManager = nullptr;
PluginManager* g_pluginManager = nullptr;
Offsets* g_Offsets = nullptr;
Signatures* g_Signatures = nullptr;
Precacher* g_precacher = nullptr;
DatabaseManager* g_dbManager = nullptr;
MenuManager* g_MenuManager = nullptr;
ResourceMonitor* g_ResourceMonitor = nullptr;
Patches* g_Patches = nullptr;
CallStack* g_callStack = nullptr;
EventManager* eventManager = nullptr;
VoiceManager g_voiceManager;

//////////////////////////////////////////////////////////////
/////////////////          Core Class          //////////////
////////////////////////////////////////////////////////////

PLUGIN_EXPOSE(Swiftly, g_Plugin);
bool Swiftly::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    g_SMAPI->AddListener(this, this);

    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);
    GET_V_IFACE_ANY(GetServerFactory, gameclients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
    GET_V_IFACE_ANY(GetServerFactory, g_clientsManager, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkMessages, INetworkMessages, NETWORKMESSAGES_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceService, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameEventSystem, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pSchemaSystem2, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkSystem, INetworkSystem, NETWORKSYSTEM_INTERFACE_VERSION);

    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, this, &Swiftly::Hook_GameFrame, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, gameclients, this, &Swiftly::Hook_ClientDisconnect, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, OnClientConnected, gameclients, this, &Swiftly::Hook_OnClientConnected, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientConnect, gameclients, this, &Swiftly::Hook_ClientConnect, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &Swiftly::Hook_OnClientCommand, false);
    SH_ADD_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &Swiftly::Hook_StartupServer, true);
    SH_ADD_HOOK_MEMFUNC(ICvar, DispatchConCommand, icvar, this, &Swiftly::Hook_DispatchConCommand, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameServerSteamAPIActivated, server, this, &Swiftly::Hook_GameServerSteamAPIActivated, false);

    g_pCVar = icvar;

    ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

    if (!BeginCrashListener())
        PRINTRET("Crash Reporter failed to initialize.\n", false);

    g_pluginManager = new PluginManager();
    g_Config = new Configuration();
    g_conFilter = new ConsoleFilter();
    g_Signatures = new Signatures();
    g_Offsets = new Offsets();
    g_Patches = new Patches();
    g_playerManager = new PlayerManager();
    g_Logger = new Logger();
    g_translations = new Translations();
    g_precacher = new Precacher();
    g_commandsManager = new CommandsManager();
    g_dbManager = new DatabaseManager();
    g_MenuManager = new MenuManager();
    g_ResourceMonitor = new ResourceMonitor();
    g_callStack = new CallStack();
    eventManager = new EventManager();

    if (g_Config->LoadConfiguration())
        PRINT("The configurations has been succesfully loaded.\n");
    else
        PRINTRET("Failed to load configurations. The plugin will not work.\n", false);

    g_Logger->AddLogger("core", false);

    g_Config->LoadPluginConfigurations();
    g_Signatures->LoadSignatures();
    g_Offsets->LoadOffsets();
    g_Patches->LoadPatches();
    g_Patches->PerformPatches();

    g_dbManager->LoadDatabases();

    g_addons.LoadAddons();
    g_addons.Initialize();
    eventManager->Initialize();

    if (!InitializeHooks())
        PRINTRET("Hooks failed to initialize.\n", false)
    else
        PRINT("Hooks initialized succesfully.\n");

    g_playerManager->LoadPlayers();
    g_conFilter->LoadFilters();
    g_translations->LoadTranslations();

    if (g_Config->FetchValue<bool>("core.console_filtering"))
        g_conFilter->Toggle();

    g_pluginManager->LoadPlugins("");
    g_pluginManager->StartPlugins();

    if (late)
    {
        g_SteamAPI.Init();
        m_CallbackDownloadItemResult.Register(this, &Swiftly::OnAddonDownloaded);
    }

    g_addons.SetupThread();

    g_voiceManager.OnAllInitialized();

    PRINT("Succesfully started.\n");

    return true;
}

void Swiftly::Hook_GameServerSteamAPIActivated()
{
    if (!CommandLine()->HasParm("-dedicated") || g_SteamAPI.SteamUGC())
        return;

    g_SteamAPI.Init();
    m_CallbackDownloadItemResult.Register(this, &Swiftly::OnAddonDownloaded);

    std::thread([&]() -> void
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));

            if (g_addons.GetStatus())
                g_addons.RefreshAddons(true); })
        .detach();

    RETURN_META(MRES_IGNORED);
}

void Swiftly::OnAddonDownloaded(DownloadItemResult_t* pResult)
{
    g_addons.OnAddonDownloaded(pResult);
}

void Swiftly::AllPluginsLoaded()
{
}

bool Swiftly::Unload(char* error, size_t maxlen)
{
    g_pluginManager->StopPlugins();
    g_pluginManager->UnloadPlugins();

    UnloadHooks();
    g_voiceManager.OnShutdown();
    g_addons.Destroy();
    eventManager->Shutdown();

    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, this, &Swiftly::Hook_GameFrame, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, gameclients, this, &Swiftly::Hook_ClientDisconnect, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, OnClientConnected, gameclients, this, &Swiftly::Hook_OnClientConnected, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientConnect, gameclients, this, &Swiftly::Hook_ClientConnect, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &Swiftly::Hook_OnClientCommand, false);
    SH_REMOVE_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &Swiftly::Hook_StartupServer, true);
    SH_REMOVE_HOOK_MEMFUNC(ICvar, DispatchConCommand, icvar, this, &Swiftly::Hook_DispatchConCommand, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameServerSteamAPIActivated, server, this, &Swiftly::Hook_GameServerSteamAPIActivated, false);

    g_pGameEntitySystem->RemoveListenerEntity(&g_entityListener);

    ConVar_Unregister();
    return true;
}

std::string currentMap = "None";

void Swiftly::OnLevelInit(char const* pMapName, char const* pMapEntities, char const* pOldLevel, char const* pLandmarkName, bool loadGame, bool background)
{
    if (!g_precacher->GetSoundsPrecached())
    {
        g_precacher->CacheSounds();
        g_precacher->SetSoundsPrecached(true);
    }

    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnMapLoad", encoders::msgpack::SerializeToString({ pMapName }), event);
    delete event;

    currentMap = pMapName;
}

void Swiftly::OnLevelShutdown()
{
    g_precacher->Reset();
    g_translations->LoadTranslations();
    g_Config->LoadPluginConfigurations();

    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnMapUnload", encoders::msgpack::SerializeToString({ currentMap }), event);
    delete event;
}

bool bDone = false;
void Swiftly::Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
    if (!bDone)
    {
        g_pGameEntitySystem = ((CGameResourceService*)g_pGameResourceService)->GetGameEntitySystem();
        g_pEntitySystem = g_pGameEntitySystem;

        g_pGameEntitySystem->AddListenerEntity(&g_entityListener);

        bDone = true;
    }

    g_ClientsPendingAddon.RemoveAll();

    if (g_addons.GetStatus())
        g_addons.RefreshAddons();
}

void Swiftly::UpdatePlayers()
{
    // Credits to: https://github.com/Source2ZE/ServerListPlayersFix (Source2ZE Team)
    if (!engine->GetServerGlobals())
        return;

    for (int i = 0; i < engine->GetServerGlobals()->maxClients; i++)
    {
        auto steamId = engine->GetClientSteamID(CPlayerSlot(i));
        if (steamId)
        {
            auto controller = (CBasePlayerController*)g_pEntitySystem->GetEntityInstance(CEntityIndex(i + 1));
            if (controller)
                g_SteamAPI.SteamGameServer()->BUpdateUserData(*steamId, controller->m_iszPlayerName(), gameclients->GetPlayerScore(CPlayerSlot(i)));
        }
    }
}

struct GameFrameMsgPackCache
{
    bool simulating;
    bool bFirstTick;
    bool bLastTick;
};

PluginEvent* gameFrameEvent = nullptr;
std::string gameFramePack;
GameFrameMsgPackCache gameFrameCache = {
    false,
    false,
    false,
};

void Swiftly::Hook_GameFrame(bool simulating, bool bFirstTick, bool bLastTick)
{
    PERF_RECORD("GameFrame", "core")
        static double g_flNextUpdate = 0.0;

    //////////////////////////////////////////////////////////////
    /////////////////         Server List          //////////////
    ////////////////////////////////////////////////////////////

    double curtime = Plat_FloatTime();
    if (curtime > g_flNextUpdate)
    {
        // Credits to: https://github.com/Source2ZE/ServerListPlayersFix (Source2ZE Team)
        UpdatePlayers();

        g_flNextUpdate = curtime + 5.0;
    }

    //////////////////////////////////////////////////////////////
    /////////////////         Game Event           //////////////
    ////////////////////////////////////////////////////////////
    if (gameFrameCache.bFirstTick != bFirstTick || gameFrameCache.bLastTick != bLastTick || gameFrameCache.simulating != simulating || gameFrameEvent == nullptr)
    {
        gameFramePack = encoders::msgpack::SerializeToString({ simulating, bFirstTick, bLastTick });

        gameFrameCache.bFirstTick = bFirstTick;
        gameFrameCache.bLastTick = bLastTick;
        gameFrameCache.simulating = simulating;
        if (gameFrameEvent == nullptr)
            gameFrameEvent = new PluginEvent("core", nullptr, nullptr);
    }
    g_pluginManager->ExecuteEvent("core", "OnGameTick", gameFramePack, gameFrameEvent);

    //////////////////////////////////////////////////////////////
    /////////////////            Player            //////////////
    ////////////////////////////////////////////////////////////
    for (uint16_t i = 0; i < g_playerManager->GetPlayerCap(); i++)
    {
        Player* player = g_playerManager->GetPlayer(i);
        if (!player)
            continue;
        if (player->IsFakeClient())
            continue;

        CBasePlayerPawn* pawn = player->GetPawn();
        if (!pawn)
            continue;

        CPlayer_MovementServices* movementServices = pawn->m_pMovementServices();
        if (!movementServices)
            continue;

        player->SetButtons(movementServices->m_nButtons().m_pButtonStates()[0]);

        if (player->HasCenterText())
            player->RenderCenterText();
        if (player->HasMenuShown())
            player->RenderMenu();
    }

    //////////////////////////////////////////////////////////////
    /////////////////         Next Frames          //////////////
    ////////////////////////////////////////////////////////////
    while (!m_nextFrame.empty())
    {
        m_nextFrame.front()();
        m_nextFrame.pop_front();
    }
}

void Swiftly::Hook_ClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnClientDisconnect", encoders::msgpack::SerializeToString({ slot.Get() }), event);

    Player* player = g_playerManager->GetPlayer(slot);
    if (player)
        g_playerManager->UnregisterPlayer(slot);
}

void Swiftly::Hook_OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer)
{
    if (bFakePlayer)
    {
        Player* player = new Player(true, slot.Get(), pszName, 0, "127.0.0.1");
        g_playerManager->RegisterPlayer(player);
    }
}

bool Swiftly::Hook_ClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason)
{
    if (!g_addons.OnClientConnect(xuid))
        RETURN_META_VALUE(MRES_IGNORED, true);

    std::string ip_address = explode(pszNetworkID, ":")[0];
    Player* player = new Player(false, slot.Get(), pszName, xuid, ip_address);
    g_playerManager->RegisterPlayer(player);

    player->SetConnected(true);

    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnClientConnect", encoders::msgpack::SerializeToString({ slot.Get() }), event);

    bool response = true;
    try
    {
        response = std::any_cast<bool>(event->GetReturnValue());
    }
    catch (std::bad_any_cast& e)
    {
        response = true;
    }
    delete event;

    if (!response)
        RETURN_META_VALUE(MRES_SUPERCEDE, false);

    RETURN_META_VALUE(MRES_IGNORED, true);
}

bool OnClientCommand(int playerid, std::string command);
bool OnClientChat(int playerid, std::string text, bool teamonly);

const char* wws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& rrtrim(std::string& s, const char* t = wws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& lltrim(std::string& s, const char* t = wws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& strim(std::string& s, const char* t = wws)
{
    return lltrim(rrtrim(s, t), t);
}

void Swiftly::Hook_OnClientCommand(CPlayerSlot slot, const CCommand& cmd)
{
    g_voiceManager.OnClientCommand(slot, cmd);

    if (!OnClientCommand(slot.Get(), cmd.GetCommandString()))
        RETURN_META(MRES_SUPERCEDE);

    RETURN_META(MRES_IGNORED);
}

void Swiftly::Hook_DispatchConCommand(ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args)
{
    CPlayerSlot slot = ctx.GetPlayerSlot();

    std::string command = args.Arg(0);

    if (slot.Get() != -1)
    {
        if (command == "say" || command == "say_team")
        {
            Player* player = g_playerManager->GetPlayer(slot);
            if (!player)
                return;

            CCSPlayerController* controller = player->GetPlayerController();
            bool teamonly = (command == "say_team");

            std::vector<std::string> textSplitted = explode(args.GetCommandString(), " ");
            textSplitted.erase(textSplitted.begin());
            std::string text = implode(textSplitted, " ");
            if (text.front() == '\'' || text.front() == '"')
                text.erase(text.begin());
            if (text.back() == '\'' || text.back() == '"')
                text.pop_back();

            if (strim(text).length() == 0)
                RETURN_META(MRES_SUPERCEDE);

            if (controller)
            {
                IGameEvent* pEvent = g_gameEventManager->CreateEvent("player_chat");

                if (pEvent)
                {
                    pEvent->SetBool("teamonly", teamonly);
                    pEvent->SetInt("userid", slot.Get());
                    pEvent->SetString("text", text.c_str());

                    g_gameEventManager->FireEvent(pEvent, true);
                }
            }

            int handleCommandReturn = g_commandsManager->HandleCommand(player, text);
            if (handleCommandReturn == 2)
                RETURN_META(MRES_SUPERCEDE);
            else if (!OnClientChat(slot.Get(), text, teamonly))
                RETURN_META(MRES_SUPERCEDE);
            else
            {
                std::vector<std::string> msg;
                if (teamonly)
                    msg.push_back(ProcessColor(string_format("{teamcolor}[%s]{default}", controller->m_iTeamNum() == CS_TEAM_CT ? "CT" : (controller->m_iTeamNum() == CS_TEAM_T ? "T" : "SPEC")), controller->m_iTeamNum()));
                if (player->tag.length() > 0)
                    msg.push_back(ProcessColor(string_format("%s%s{default}", player->tagcolor.empty() ? "{default}" : player->tagcolor.c_str(), player->tag.c_str()), controller->m_iTeamNum()));
                msg.push_back(string_format("%s%s%s:", ProcessColor(player->namecolor, controller->m_iTeamNum()).c_str(), player->GetName(), ProcessColor("{default}", CS_TEAM_CT).c_str()));
                msg.push_back(string_format("%s%s", ProcessColor(player->chatcolor, controller->m_iTeamNum()).c_str(), replace(text, "\n", "\u2029").c_str()));

                std::string formatted_msg = (" " + implode(msg, " "));

                for (uint16_t i = 0; i < g_playerManager->GetPlayerCap(); i++)
                {
                    Player* p = g_playerManager->GetPlayer(i);
                    if (p == nullptr)
                        continue;

                    CCSPlayerController* pcontroller = p->GetPlayerController();
                    if (!pcontroller)
                        continue;

                    if (teamonly)
                    {
                        if (pcontroller->m_iTeamNum() == controller->m_iTeamNum())
                            pcontroller->SendMsg(HUD_PRINTTALK, formatted_msg.c_str());
                    }
                    else
                        pcontroller->SendMsg(HUD_PRINTTALK, formatted_msg.c_str());
                }
            }
            RETURN_META(MRES_SUPERCEDE);
        }
    }
}

void Swiftly::NextFrame(std::function<void()> fn)
{
    m_nextFrame.push_back(fn);
}

void CEntityListener::OnEntitySpawned(CEntityInstance* pEntity)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnEntitySpawned", encoders::msgpack::SerializeToString({ string_format("%p", (void*)pEntity) }), event);
    delete event;
}

void CEntityListener::OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent)
{
}

void CEntityListener::OnEntityCreated(CEntityInstance* pEntity)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnEntityCreated", encoders::msgpack::SerializeToString({ string_format("%p", (void*)pEntity) }), event);
    delete event;

    if (std::string(pEntity->GetClassname()) == "cs_gamerules")
        gameRules = ((CCSGameRulesProxy*)pEntity)->m_pGameRules;
}

void CEntityListener::OnEntityDeleted(CEntityInstance* pEntity)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnEntityDeleted", encoders::msgpack::SerializeToString({ string_format("%p", (void*)pEntity) }), event);
    delete event;
}

bool Swiftly::Pause(char* error, size_t maxlen)
{
    return true;
}

bool Swiftly::Unpause(char* error, size_t maxlen)
{
    return true;
}

const char* Swiftly::GetLicense()
{
    return "MIT License";
}

const char* Swiftly::GetVersion()
{
#ifndef SWIFTLY_VERSION
    return "Local";
#else
    return (std::string("v") + std::string(SWIFTLY_VERSION)).c_str();
#endif
}

const char* Swiftly::GetDate()
{
    return __DATE__;
}

const char* Swiftly::GetLogTag()
{
    return "SWIFTLY";
}

const char* Swiftly::GetAuthor()
{
    return "Swiftly Development Team";
}

const char* Swiftly::GetDescription()
{
    return "Swiftly - Framework";
}

const char* Swiftly::GetName()
{
    return "Swiftly";
}

const char* Swiftly::GetURL()
{
    return "https://github.com/swiftly-solution/swiftly";
}