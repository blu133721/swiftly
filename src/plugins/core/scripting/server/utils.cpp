#include "../../scripting.h"
#include "../../../PluginManager.h"
#include "../../../../network/usermessages/usermessages.h"
#include "../../../../utils/utils.h"

bool scripting_IsWindows()
{
    return WIN_LINUX(true, false);
}

bool scripting_IsLinux()
{
    return WIN_LINUX(false, true);
}

std::string scripting_GetOS()
{
    return WIN_LINUX("Windows", "Linux");
}

SDKBaseClass scripting_GetCCSGameRules()
{
    SDKBaseClass val(gameRules, "CCSGameRules");
    return val;
}

std::string scripting_GetPluginPath(std::string plugin_name)
{
    Plugin* plugin = g_pluginManager->FetchPlugin(plugin_name);
    if (!plugin) return "";

    return string_format("%s/%s", g_pluginManager->GetPluginBasePath(plugin_name).c_str(), plugin->GetName().c_str());
}

PluginUserMessage scripting_GetUserMessage(std::string str)
{
    auto exploded = explode(str, "|");
    if (exploded.size() != 3) return PluginUserMessage("");

    INetworkMessageInternal* msg = (INetworkMessageInternal*)(strtol(exploded[0].c_str(), nullptr, 16));
    CNetMessage* netmsg = (CNetMessage*)(strtol(exploded[1].c_str(), nullptr, 16));
    uint64* clients = (uint64*)(strtol(exploded[2].c_str(), nullptr, 16));

    return PluginUserMessage(msg, netmsg, clients);
}

std::string scripting_CreateTextTable(std::vector<std::vector<std::string>> data)
{
    TextTable tbl('-', '|', '+');

    for (auto vec : data) {
        for (std::string str : vec) {
            tbl.add(" " + str + " ");
        }

        tbl.endOfRow();
    }

    std::stringstream outputTable;
    outputTable << tbl;
    return outputTable.str();
}

int scripting_GetPluginState(std::string plugin_name)
{
    if (!g_pluginManager->PluginExists(plugin_name)) return (int)PluginState_t::Stopped;
    Plugin* plugin = g_pluginManager->FetchPlugin(plugin_name);
    if (!plugin) return (int)PluginState_t::Stopped;

    return (int)plugin->GetPluginState();
}