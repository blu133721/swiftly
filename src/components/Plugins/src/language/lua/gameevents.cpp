#include "../../../inc/Scripting.h"

#ifdef _MSC_VER
#pragma warning(disable : 4146)
#pragma warning(disable : 4180)
#endif
#include <luacpp/luacpp.h>
#include <luacpp/func_utils.h>

void SetupLuaGameEvents(luacpp::LuaState *state, Plugin *plugin)
{
    state->CreateTable("players");

    state->DoString("function Internal_RegisterPlayer(slot, fake) players[slot] = Player(slot, fake); end");
    state->DoString("function Internal_UnregisterPlayer(slot) players[slot] = nil; end");
    state->DoString("function Internal_OnProgramLoad(plugin_name, bin) if OnProgramLoad then OnProgramLoad(plugin_name, bin) end end");
    state->DoString("function Internal_OnClientConnect(slot) if not OnClientConnect then return true end if not players[slot] then return false end return OnClientConnect(players[slot]) end");
    state->DoString("function Internal_ShouldHearVoice(slot) if not ShouldHearVoice then return true end if not players[slot] then return true end return ShouldHearVoice(players[slot]) end");
    state->DoString("function Internal_OnPlayerHurt(slot, attacker, dmgHealth, dmgArmor, hitgroup, weapon) "
                    "if not OnPlayerHurt then return end "
                    "if not players[slot] then return end "
                    "if not players[attacker] then return end "
                    "OnPlayerHurt(players[slot], players[attacker], dmgHealth, dmgArmor, hitgroup, weapon, ((players[slot]:GetHealth() - dmgHealth) <= 0)) end");
    state->DoString("function Internal_OnPluginStart() if OnPluginStart then OnPluginStart() end end");
    state->DoString("function Internal_OnPluginStop() if OnPluginStop then OnPluginStop() end end");
    state->DoString("function Internal_OnClientDisconnect(slot) if not OnClientDisconnect then return end if not players[slot] then return end OnClientDisconnect(players[slot]) end");
    state->DoString("function Internal_OnPlayerDeath(slot, attacker, assister, assistedflash, weapon, headshot, dominated, revenge, wipe, penetrated, noreplay, noscope, thrusmoke, attackerblind, distance, dmg_health, dmg_armor, hitgroup) "
                    "if not OnPlayerDeath then return end "
                    "if not players[slot] then return end "
                    "if attacker ~= -1 then if not players[attacker] then return end end "
                    "if assister ~= -1 then if not players[assister] then return end end "
                    "OnPlayerDeath(players[slot], players[attacker], players[assister], assistedflash, weapon, headshot, dominated, revenge, wipe, penetrated, noreplay, noscope, thrusmoke, attackerblind, distance, dmg_health, dmg_armor, hitgroup) "
                    "end");
    state->DoString("function Internal_OnMapLoad(map) if OnMapLoad then OnMapLoad(map) end end");
    state->DoString("function Internal_OnMapUnload(map) if OnMapUnload then OnMapUnload(map) end end");
    state->DoString("function Internal_BombPickup(slot) if not OnBombPickup then return end if not players[slot] then return end OnBombPickup(players[slot]) end");
    state->DoString("function Internal_BombDropped(slot) if not OnBombDropped then return end if not players[slot] then return end OnBombDropped(players[slot]) end");
    state->DoString("function Internal_BombExploded(slot, site) if not OnBombExploded then return end if not players[slot] then return end OnBombExploded(players[slot], site) end");
    state->DoString("function Internal_BombDefused(slot, site) if not OnBombDefused then return end if not players[slot] then return end OnBombDefused(players[slot], site) end");
    state->DoString("function Internal_BombAbortDefuse(slot, site) if not OnBombAbortDefuse then return end if not players[slot] then return end OnBombAbortDefuse(players[slot], site) end");
    state->DoString("function Internal_BombBeginDefuse(slot, site) if not OnBombBeginDefuse then return end if not players[slot] then return end OnBombBeginDefuse(players[slot], site) end");
    state->DoString("function Internal_BombPlanted(slot, site) if not OnBombPlanted then return end if not players[slot] then return end OnBombPlanted(players[slot], site) end");
    state->DoString("function Internal_BombAbortPlant(slot, site) if not OnBombAbortPlant then return end if not players[slot] then return end OnBombAbortPlant(players[slot], site) end");
    state->DoString("function Internal_BombBeginPlant(slot, site) if not OnBombBeginPlant then return end if not players[slot] then return end OnBombBeginPlant(players[slot], site) end");
    state->DoString("function Internal_OnRoundPrestart() if OnRoundPrestart then OnRoundPrestart() end end");
    state->DoString("function Internal_OnRoundStart(timelimit, fraglimit, objective) if OnRoundStart then OnRoundStart(timelimit, fraglimit, objective) end end");
    state->DoString("function Internal_OnRoundEnd(winner, reason, message, legacy, player_count, nomusic) if OnRoundEnd then OnRoundEnd(winner, reason, message, legacy, player_count, nomusic) end end");
    state->DoString("function Internal_OnGameTick(simulating, bFirstTick, bLastTick) if OnGameTick then OnGameTick(simulating, bFirstTick, bLastTick) end end");
    state->DoString("function Internal_OnClientGameMessage(slot, destination, message) if not OnClientGameMessage then return true end if not players[slot] then return true end return OnClientGameMessage(players[slot], destination, message) end");
    state->DoString("function Internal_OnPlayerChat(slot, text, teamonly) if not OnPlayerChat then return true end if not players[slot] then return false end return OnPlayerChat(players[slot], text, teamonly) end");
    state->DoString("function Internal_OnPlayerSpawn(slot) if not players[slot] then return end if OnPlayerSpawn then OnPlayerSpawn(players[slot]) end if not players[slot]:IsFirstSpawn() then players[slot]:SetFirstSpawn(true) end end");

    PRINT("Scripting - Lua", "Game Events loaded.\n");
}