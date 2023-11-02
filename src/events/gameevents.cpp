#include "gameevents.h"
#include "../sdk/CBasePlayerController.h"
#include "../sdk/CBasePlayerPawn.h"
#include "../hooks/GameEvents.h"

CUtlVector<CGameEventListener *> g_GameEventListener;

void RegisterEventListeners()
{
    if (!g_gameEventManager)
        return;

    FOR_EACH_VEC(g_GameEventListener, i)
    {
        g_gameEventManager->AddListener(g_GameEventListener[i], g_GameEventListener[i]->GetName(), true);
    }
}

void UnregisterEventListeners()
{
    if (!g_gameEventManager)
        return;

    FOR_EACH_VEC(g_GameEventListener, i)
    {
        g_gameEventManager->RemoveListener(g_GameEventListener[i]);
    }

    g_GameEventListener.Purge();
}

GAME_EVENT(player_spawn)
{
    gameevents::emit<OnPlayerSpawn>(pEvent);
}

GAME_EVENT(player_chat)
{
    gameevents::emit<OnPlayerChat>(pEvent);
}