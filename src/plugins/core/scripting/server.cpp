#include "../scripting.h"
#include "../../../sdk/entity/CGameRules.h"

PluginServer::PluginServer(std::string m_plugin_name)
{
    REGISTER_CALLSTACK(this->plugin_name, string_format("PluginServer::PluginServer(m_plugin_name=\"%s\")", m_plugin_name.c_str()));

    this->plugin_name = m_plugin_name;
}

std::string PluginServer::GetMap()
{
    REGISTER_CALLSTACK(this->plugin_name, "PluginServer::GetMap()");
    if (!engine->GetServerGlobals())
        return "unknown_map";

    return engine->GetServerGlobals()->mapname.ToCStr();
}

bool PluginServer::IsMapValid(std::string map)
{
    REGISTER_CALLSTACK(this->plugin_name, string_format("PluginServer::IsMapValid(map=\"%s\")", map.c_str()));
    return (engine->IsMapValid(map.c_str()) == 1);
}

void PluginServer::ChangeMap(std::string map, bool workshop)
{
    REGISTER_CALLSTACK(this->plugin_name, string_format("PluginServer::ChangeMap(map=\"%s\", workshop=%d)", map.c_str(), workshop));
    if (!IsMapValid(map))
        return;

    Execute(string_format("%s %s", workshop ? "host_workshop_map" : "map", map.c_str()));
}

uint16_t PluginServer::GetMaxPlayers()
{
    REGISTER_CALLSTACK(this->plugin_name, "PluginServer::GetMaxPlayers()");
    return engine->GetServerGlobals()->maxClients;
}

void PluginServer::Execute(std::string cmd)
{
    REGISTER_CALLSTACK(this->plugin_name, string_format("PluginServer::Execute(cmd=\"%s\")", cmd.c_str()));
    engine->ServerCommand(cmd.c_str());
}

float PluginServer::GetCurrentTime()
{
    REGISTER_CALLSTACK(this->plugin_name, "PluginServer::GetCurrentTime()");
    if (!engine->GetServerGlobals())
        return 0.0f;

    return engine->GetServerGlobals()->curtime;
}

int PluginServer::GetTickCount()
{
    REGISTER_CALLSTACK(this->plugin_name, "PluginServer::GetTickCount()");
    if (!engine->GetServerGlobals())
        return 0;

    return engine->GetServerGlobals()->tickcount;
}

