#include "../../scripting.h"
#include "../../../PluginManager.h"
#include "../../../../player/playermanager/PlayerManager.h"
#include "../../../../memory/hooks/FuncHook.h"

PluginEvent* noReturnEvent = nullptr;

void OnClientKeyStateChange(int playerid, std::string key, bool pressed)
{
    if (pressed)
    {
        Player* player = g_playerManager->GetPlayer(playerid);
        if (!player)
            return;

        player->PerformMenuAction(key);
    }

    if(noReturnEvent == nullptr) noReturnEvent = new PluginEvent("core", nullptr, nullptr);

    g_pluginManager->ExecuteEvent("core", "OnClientKeyStateChange", encoders::msgpack::SerializeToString({ playerid, key, pressed }), noReturnEvent);
}

bool OnClientCommand(int playerid, std::string command)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnClientCommand", encoders::msgpack::SerializeToString({ playerid, command }), event);

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
    return response;
}

bool OnClientChat(int playerid, std::string text, bool teamonly)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnClientChat", encoders::msgpack::SerializeToString({ playerid, text, teamonly }), event);

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
    return response;
}

void OnClientConvarQuery(int playerid, std::string convar_name, std::string convar_value)
{
    if(noReturnEvent == nullptr) noReturnEvent = new PluginEvent("core", nullptr, nullptr);

    g_pluginManager->ExecuteEvent("core", "OnClientConvarQuery", encoders::msgpack::SerializeToString({ playerid, convar_name, convar_value }), noReturnEvent);
}

void Hook_CBaseEntity_TakeDamage(CBaseEntity* _this, CTakeDamageInfo* damageInfo);
FuncHook<decltype(Hook_CBaseEntity_TakeDamage)> CBaseEntity_TakeDamage(Hook_CBaseEntity_TakeDamage, "CBaseEntity_TakeDamage");

void Hook_CBaseEntity_TakeDamage(CBaseEntity* _this, CTakeDamageInfo* damageInfo)
{
    CCSPlayerPawn* playerPawn = (CCSPlayerPawn*)_this;
    if(!playerPawn) return CBaseEntity_TakeDamage(_this, damageInfo);
    
    CCSPlayerController* playerController = (CCSPlayerController*)playerPawn->m_hController().Get();
    if(!playerController) return CBaseEntity_TakeDamage(_this, damageInfo);

    int playerid = playerController->GetPlayerSlot();
    int attackerid = -1;

    CCSPlayerPawn* attackerPawn = (CCSPlayerPawn*)damageInfo->m_hAttacker.Get();
    if(attackerPawn) {
        CCSPlayerController* attackerController = (CCSPlayerController*)attackerPawn->m_hController().Get();
        if(attackerController) attackerid = attackerController->GetPlayerSlot();
    }

    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    g_pluginManager->ExecuteEvent("core", "OnPlayerDamage", encoders::msgpack::SerializeToString({ playerid, attackerid, string_format("%p", damageInfo), string_format("%p", damageInfo->m_hInflictor.Get()), string_format("%p", damageInfo->m_hAbility.Get()) }), event);

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

    if(!response) return;

    CBaseEntity_TakeDamage(_this, damageInfo);
}

void Hook_CEntityIdentity_AcceptInput(CEntityIdentity* _this, CUtlSymbolLarge* inputName, CEntityInstance* activator, CEntityInstance* caller, variant_t* value, int outputid);
FuncHook<decltype(Hook_CEntityIdentity_AcceptInput)> TCEntityIdentity_AcceptInput(Hook_CEntityIdentity_AcceptInput, "CEntityIdentity_AcceptInput");

void Hook_CEntityIdentity_AcceptInput(CEntityIdentity* _this, CUtlSymbolLarge* inputName, CEntityInstance* activator, CEntityInstance* caller, variant_t* value, int outputid)
{
    PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    std::vector<std::any> msgpackData = {
        string_format("%p", _this),
        inputName->String(),
        string_format("%p", activator),
        string_format("%p", caller),
        value->ToString() == nullptr ? "(null)" : value->ToString(),
        outputid,
    };

    g_pluginManager->ExecuteEvent("core", "OnEntityAcceptInput", encoders::msgpack::SerializeToString(msgpackData), event);

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

    if(!response) return;

    TCEntityIdentity_AcceptInput(_this, inputName, activator, caller, value, outputid);
}