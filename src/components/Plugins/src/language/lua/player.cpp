#include "../../../inc/Scripting.h"

#ifdef _MSC_VER
#pragma warning(disable : 4146)
#pragma warning(disable : 4180)
#endif
#include <luacpp/luacpp.h>
#include <luacpp/func_utils.h>

#include <rapidjson/document.h>

class LuaPlayerClass
{
public:
    int playerSlot;
    bool fakeClient;
    bool firstSpawn = false;

    LuaPlayerClass(int m_playerSlot, bool m_fakeClient) : playerSlot(m_playerSlot), fakeClient(m_fakeClient) {}
};

class LuaPlayerArgsClass
{
public:
    int playerSlot;

    LuaPlayerArgsClass(int m_playerSlot) : playerSlot(m_playerSlot) {}
};

void SetupLuaPlayer(luacpp::LuaState *state, Plugin *plugin)
{
    auto playerClass = state->CreateClass<LuaPlayerClass>("Player").DefConstructor<int, bool>();

    auto healthClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto armorClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto clantagClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto teamClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto varsClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto statsClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto moneyClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto coordsClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();
    auto velocityClass = state->CreateClass<LuaPlayerArgsClass>().DefConstructor<int>();

    playerClass.DefMember("GetSteamID", [](LuaPlayerClass *base) -> uint64_t
                          { return scripting_Player_GetSteamID(base->playerSlot); })
        .DefMember("GetName", [](LuaPlayerClass *base) -> const char *
                   { return scripting_Player_GetName(base->playerSlot); })
        .DefMember("GetHealth", [](LuaPlayerClass *base) -> int
                   { return scripting_Player_GetHealth(base->playerSlot); })
        .DefMember("IsFirstSpawn", [](LuaPlayerClass *base) -> bool
                   { return (base->firstSpawn == false); })
        .DefMember("SetFirstSpawn", [](LuaPlayerClass *base, bool firstSpawn) -> void
                   { base->firstSpawn = firstSpawn; })
        .DefMember("GetConnectedTime", [](LuaPlayerClass *base) -> uint32_t
                   { return scripting_Player_GetConnectedTime(base->playerSlot); })
        .DefMember("SendMsg", [](LuaPlayerClass *base, int dest, const char *message) -> void
                   { return scripting_Player_SendMessage(base->playerSlot, dest, message); })
        .DefMember("GetConvarValue", [](LuaPlayerClass *base, const char *name) -> const char *
                   { return scripting_Player_GetConvar(base->playerSlot, name); })
        .DefMember("IsAuthenticated", [](LuaPlayerClass *base) -> bool
                   { return scripting_Player_IsAuthenticated(base->playerSlot); })
        .DefMember("Kill", [](LuaPlayerClass *base) -> void
                   { scripting_Player_Kill(base->playerSlot); })
        .DefMember("Drop", [](LuaPlayerClass *base, int reason) -> void
                   { scripting_Player_Drop(base->playerSlot, (ENetworkDisconnectionReason)reason); })
        .DefMember("IsFakeClient", [](LuaPlayerClass *base) -> bool
                   { return base->fakeClient; })
        .DefMember("GetSlot", [](LuaPlayerClass *base) -> int
                   { return base->playerSlot; })
        .DefMember("health", [healthClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return healthClass.CreateInstance(base->playerSlot); })
        .DefMember("armor", [armorClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return armorClass.CreateInstance(base->playerSlot); })
        .DefMember("clantag", [clantagClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return clantagClass.CreateInstance(base->playerSlot); })
        .DefMember("team", [teamClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return teamClass.CreateInstance(base->playerSlot); })
        .DefMember("vars", [varsClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return varsClass.CreateInstance(base->playerSlot); })
        .DefMember("stats", [statsClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return statsClass.CreateInstance(base->playerSlot); })
        .DefMember("money", [moneyClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return moneyClass.CreateInstance(base->playerSlot); })
        .DefMember("coords", [coordsClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return coordsClass.CreateInstance(base->playerSlot); })
        .DefMember("velocity", [velocityClass](LuaPlayerClass *base) -> luacpp::LuaObject
                   { return velocityClass.CreateInstance(base->playerSlot); });

    healthClass.DefMember("Get", [](LuaPlayerArgsClass *base) -> int
                          { return scripting_Player_GetHealth(base->playerSlot); })
        .DefMember("Set", [](LuaPlayerArgsClass *base, int health) -> void
                   { scripting_Player_SetHealth(base->playerSlot, health); })
        .DefMember("Take", [](LuaPlayerArgsClass *base, int health) -> void
                   { scripting_Player_TakeHealth(base->playerSlot, health); });

    armorClass.DefMember("Get", [](LuaPlayerArgsClass *base) -> int
                         { return scripting_Player_GetArmor(base->playerSlot); })
        .DefMember("Set", [](LuaPlayerArgsClass *base, int armor) -> void
                   { scripting_Player_SetArmor(base->playerSlot, armor); })
        .DefMember("Take", [](LuaPlayerArgsClass *base, int armor) -> void
                   { scripting_Player_TakeArmor(base->playerSlot, armor); });

    clantagClass.DefMember("Get", [](LuaPlayerArgsClass *base) -> const char *
                           { return scripting_Player_GetClanTag(base->playerSlot); })
        .DefMember("Set", [](LuaPlayerArgsClass *base, const char *tag) -> void
                   { scripting_Player_SetClanTag(base->playerSlot, tag); });

    teamClass.DefMember("Get", [](LuaPlayerArgsClass *base) -> uint8
                        { return scripting_Player_GetTeam(base->playerSlot); })
        .DefMember("Set", [](LuaPlayerArgsClass *base, int team) -> void
                   { scripting_Player_SetTeam(base->playerSlot, team); });

    varsClass.DefMember("Get", [state](LuaPlayerArgsClass *base, const char *key) -> luacpp::LuaObject
                        {
                            rapidjson::Document doc;
                            doc.Parse(scripting_Player_GetVar(base->playerSlot, key));
                            if(doc.HasParseError()) return state->CreateNil();

                            if(doc["value"].IsString()) return state->CreateString(doc["value"].GetString());
                            else if(doc["value"].IsInt()) return state->CreateInteger(doc["value"].GetInt());
                            else if(doc["value"].IsInt64()) return state->CreateInteger(doc["value"].GetInt64());
                            else if(doc["value"].IsUint()) return state->CreateInteger(doc["value"].GetUint());
                            else if(doc["value"].IsUint64()) return state->CreateInteger(doc["value"].GetUint64());
                            else if(doc["value"].IsBool()) return state->CreateInteger(doc["value"].GetBool());
                            else if(doc["value"].IsFloat()) return state->CreateNumber(doc["value"].GetFloat());
                            else if(doc["value"].IsDouble()) return state->CreateNumber(doc["value"].IsDouble());
                            else return state->CreateNil(); })
        .DefMember("Set", [plugin](LuaPlayerArgsClass *base, const char *key, luacpp::LuaObject value) -> void
                   {
                        if(value.GetType() == LUA_TBOOLEAN) scripting_Player_SetVar(base->playerSlot, key, 7, value.ToBool());
                        else if(value.GetType() == LUA_TSTRING) scripting_Player_SetVar(base->playerSlot, key, 1, value.ToString());
                        else if(value.GetType() == LUA_TNUMBER) {
                            lua_rawgeti(plugin->GetLuaRawState(), LUA_REGISTRYINDEX, value.GetRefIndex());
                            if(lua_isinteger(plugin->GetLuaRawState(), -1) == 1) scripting_Player_SetVar(base->playerSlot, key, 8, value.ToInteger());
                            else {
                                double val = value.ToNumber();
                                scripting_Player_SetVar(base->playerSlot, key, 4, val);
                            }
                            lua_pop(plugin->GetLuaRawState(), 1);
                        } });

    statsClass.DefMember("Get", [](LuaPlayerArgsClass *base, int stat) -> int
                         { return scripting_Player_FetchMatchStat(base->playerSlot, (PlayerStat)stat); })
        .DefMember("Set", [](LuaPlayerArgsClass *base, int stat, int value) -> void
                   { scripting_Player_SetMatchStat(base->playerSlot, (PlayerStat)stat, value); });

    moneyClass.DefMember("Get", [](LuaPlayerArgsClass *base) -> int
                         { return scripting_Player_GetMoney(base->playerSlot); })
        .DefMember("Set", [](LuaPlayerArgsClass *base, int money) -> void
                   { scripting_Player_SetMoney(base->playerSlot, money); })
        .DefMember("Take", [](LuaPlayerArgsClass *base, int money) -> void
                   { scripting_Player_TakeMoney(base->playerSlot, money); });

    coordsClass.DefMember("Get", [state](LuaPlayerArgsClass *base) -> luacpp::LuaObject
                          {
                            rapidjson::Document doc;
                            doc.Parse(scripting_Player_GetCoords(base->playerSlot));
                            if(doc.HasParseError()) return state->CreateNil();
                            if(!doc["value"].IsObject()) return state->CreateNil();

                            float x = doc["value"]["x"].GetFloat();
                            float y = doc["value"]["y"].GetFloat();
                            float z = doc["value"]["z"].GetFloat();

                            LuaFuncWrapper wrapper(state->Get("vector3"));
                            wrapper.PrepForExec();
                            luacpp::PushValues(wrapper.GetML(), x, y, z);
                            return wrapper.ExecuteWithReturn<luacpp::LuaObject>("vector3", 3); })
        .DefMember("Set", [state](LuaPlayerArgsClass *base, luacpp::LuaObject coordsObj) -> void
                   {
                        if(coordsObj.GetType() != LUA_TTABLE) {
                            PRINT("Runtime", "Coords field needs to be a vector3.\n");
                            return;
                        }

                        luacpp::LuaTable coords = luacpp::LuaTable(coordsObj);

                        if(coords.Get("x").GetType() == LUA_TNIL || coords.Get("y").GetType() == LUA_TNIL || coords.Get("z").GetType() == LUA_TNIL) {
                            PRINT("Runtime", "Coords field needs to be a vector3.\n");
                            return;
                        }

                        scripting_Player_SetCoords(base->playerSlot, (float)coords.GetNumber("x"), (float)coords.GetNumber("y"), (float)coords.GetNumber("z")); });

    velocityClass.DefMember("Get", [state](LuaPlayerArgsClass *base) -> luacpp::LuaObject
                            {
                            rapidjson::Document doc;
                            doc.Parse(scripting_Player_GetVelocity(base->playerSlot));
                            if(doc.HasParseError()) return state->CreateNil();
                            if(!doc["value"].IsObject()) return state->CreateNil();

                            float x = doc["value"]["x"].GetFloat();
                            float y = doc["value"]["y"].GetFloat();
                            float z = doc["value"]["z"].GetFloat();

                            LuaFuncWrapper wrapper(state->Get("vector3"));
                            wrapper.PrepForExec();
                            luacpp::PushValues(wrapper.GetML(), x, y, z);
                            return wrapper.ExecuteWithReturn<luacpp::LuaObject>("vector3", 3); })
        .DefMember("Set", [state](LuaPlayerArgsClass *base, luacpp::LuaObject coordsObj) -> void
                   {
                        if(coordsObj.GetType() != LUA_TTABLE) {
                            PRINT("Runtime", "Coords field needs to be a vector3.\n");
                            return;
                        }

                        luacpp::LuaTable coords = luacpp::LuaTable(coordsObj);

                        if(coords.Get("x").GetType() == LUA_TNIL || coords.Get("y").GetType() == LUA_TNIL || coords.Get("z").GetType() == LUA_TNIL) {
                            PRINT("Runtime", "Coords field needs to be a vector3.\n");
                            return;
                        }

                        scripting_Player_SetVelocity(base->playerSlot, (float)coords.GetNumber("x"),(float)coords.GetNumber("y"), (float)coords.GetNumber("z")); });

    PRINT("Scripting - Lua", "Player loaded.\n");
}