#include "ConsoleFilter.h"
#include "../common.h"
#include "../configuration/Configuration.h"
#include "../resourcemonitor/ResourceMonitor.h"

#include "../files/Files.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

void ConFilterError(std::string text)
{
    if (!g_SMAPI)
        return;

    PRINTF("Console Filter", "%s\n", text.c_str());
}

void ConsoleFilter::Toggle()
{
    this->m_status = !this->m_status;
}

void ConsoleFilter::LoadFilters()
{
    this->filter.clear();
    this->counter.clear();

    rapidjson::Document confilterFile;
    confilterFile.Parse(Files::Read("addons/swiftly/configs/console_filter.json").c_str());
    if (confilterFile.HasParseError())
        return ConFilterError(string_format("A parsing error has been detected.\nError (offset %u): %s\n", (unsigned)confilterFile.GetErrorOffset(), GetParseError_En(confilterFile.GetParseError())));

    if (confilterFile.IsArray())
        return ConFilterError("Console filters file cannot be an array.");

    for (auto it = confilterFile.MemberBegin(); it != confilterFile.MemberEnd(); ++it)
    {
        std::string key = it->name.GetString();

        if (!it->value.IsString())
        {
            ConFilterError(string_format("The field \"%s\" is not a string.", key));
            continue;
        }

        this->filter.insert(std::make_pair(key, it->value.GetString()));
        this->counter.insert(std::make_pair(key, 0));
    }
}

bool ConsoleFilter::NeedFiltering(std::string message)
{
    if (!this->Status())
        return false;

    auto start = std::chrono::high_resolution_clock::now();

    for (std::map<std::string, std::string>::iterator it = this->filter.begin(); it != this->filter.end(); ++it)
    {
        std::string key = it->first;
        std::string val = it->second;

        if (message.find(val) != std::string::npos)
        {
            if (this->counter.find(key) != this->counter.end())
                this->counter[key]++;

            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            if (g_ResourceMonitor->IsEnabled())
                g_ResourceMonitor->RecordTime("swiftly-core", "ConsoleFilter::NeedFiltering", std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000);

            return true;
        }
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    if (g_ResourceMonitor->IsEnabled())
        g_ResourceMonitor->RecordTime("swiftly-core", "ConsoleFilter::NeedFiltering", std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000);

    return false;
}