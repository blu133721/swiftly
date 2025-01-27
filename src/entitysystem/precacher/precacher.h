#ifndef _precacher_h
#define _precacher_h

#include <cstdint>
#include <set>
#include <string>
#include <algorithm>

class IEntityResourceManifest;

class Precacher
{
private:
    int64_t precacheContext = 0;
    std::set<std::string> cacheItems;
    std::set<std::string> precachedItems;

public:
    Precacher(){};

    void AddItem(std::string model);

    bool HasModelInList(std::string model)
    {
        return (this->precachedItems.find(model) != this->precachedItems.end());
    }

    void CacheItems(IEntityResourceManifest* manifest);
};

extern Precacher *g_precacher;

#endif