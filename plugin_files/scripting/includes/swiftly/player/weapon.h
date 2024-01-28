#ifndef _swiftly_player_weapon_h
#define _swiftly_player_weapon_h

#include <stdint.h>
#include <map>
#include <string>

#include "../swiftly_memory.h"
#include "../types.h"
#include "../swiftly_utils.h"

typedef void (*Player_RemoveWeapon)(uint32_t, uint32_t);
typedef void (*Player_DropWeapon)(uint32_t, uint32_t);

typedef void (*Player_SetStatTrack)(uint32_t, uint32_t, bool);
typedef void (*Player_SetWear)(uint32_t, uint32_t, float);
typedef void (*Player_SetPaintKit)(uint32_t, uint32_t, int);
typedef void (*Player_SetSeed)(uint32_t, uint32_t, int);

typedef bool (*Player_GetStatTrack)(uint32_t, uint32_t);
typedef float (*Player_GetWear)(uint32_t, uint32_t);
typedef int (*Player_GetPaintKit)(uint32_t, uint32_t);
typedef int (*Player_GetSeed)(uint32_t, uint32_t);

typedef bool (*Player_WeaponExists)(uint32_t, uint32_t);

typedef WeaponSlot (*Player_GetType)(uint32_t, uint32_t);

typedef const char *(*Player_GetWeaponName)(uint32_t, uint32_t);

typedef void (*Player_SetNametag)(uint32_t, uint32_t, const char *);

class Weapon
{
private:
    uint32_t m_playerSlot;
    uint32_t m_weaponID;

public:
    Weapon(uint32_t playerSlot, uint32_t weaponID) : m_playerSlot(playerSlot), m_weaponID(weaponID) {}

    void Remove()
    {
        void *player_RemoveWeapon = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_Remove");
        if (player_RemoveWeapon)
            reinterpret_cast<Player_RemoveWeapon>(player_RemoveWeapon)(this->m_playerSlot, this->m_weaponID);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_Remove");
    }
    void Drop()
    {
        void *player_DropWeapon = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_Drop");
        if (player_DropWeapon)
            reinterpret_cast<Player_DropWeapon>(player_DropWeapon)(this->m_playerSlot, this->m_weaponID);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_Drop");
    }

    void SetStatTrack(bool stattrack)
    {
        void *player_SetStatTrack = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_SetStatTrack");
        if (player_SetStatTrack)
            reinterpret_cast<Player_SetStatTrack>(player_SetStatTrack)(this->m_playerSlot, this->m_weaponID, stattrack);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_SetStatTrack");
    }
    void SetWear(float wear)
    {
        void *player_SetWear = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_SetWear");
        if (player_SetWear)
            reinterpret_cast<Player_SetWear>(player_SetWear)(this->m_playerSlot, this->m_weaponID, wear);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_SetWear");
    }
    void SetPaintKit(int32_t paintkit)
    {
        void *player_SetPaintKit = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_SetPaintKit");
        if (player_SetPaintKit)
            reinterpret_cast<Player_SetPaintKit>(player_SetPaintKit)(this->m_playerSlot, this->m_weaponID, paintkit);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_SetPaintKit");
    }
    void SetSeed(int32_t seed)
    {
        void *player_SetSeed = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_SetSeed");
        if (player_SetSeed)
            reinterpret_cast<Player_SetSeed>(player_SetSeed)(this->m_playerSlot, this->m_weaponID, seed);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_SetSeed");
    }

    bool GetStatTrack()
    {
        void *player_GetStatTrack = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_GetStatTrack");
        if (player_GetStatTrack)
            return reinterpret_cast<Player_GetStatTrack>(player_GetStatTrack)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_GetStatTrack");
            return false;
        }
    }
    float GetWear()
    {
        void *player_GetWear = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_GetWear");
        if (player_GetWear)
            return reinterpret_cast<Player_GetWear>(player_GetWear)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_GetWear");
            return 0.0f;
        }
    }
    int32_t GetPaintKit()
    {
        void *player_GetPaintKit = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_GetPaintKit");
        if (player_GetPaintKit)
            return reinterpret_cast<Player_GetPaintKit>(player_GetPaintKit)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_GetPaintKit");
            return 0;
        }
    }
    int32_t GetSeed()
    {
        void *player_GetSeed = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_GetSeed");
        if (player_GetSeed)
            return reinterpret_cast<Player_GetSeed>(player_GetSeed)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_GetSeed");
            return 0;
        }
    }

    WeaponSlot GetType()
    {
        void *player_GetType = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_GetType");
        if (player_GetType)
            return reinterpret_cast<Player_GetType>(player_GetType)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_GetType");
            return WeaponSlot::INVALID;
        }
    }

    const char *GetName()
    {
        void *player_GetName = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_GetName");
        if (player_GetName)
            return reinterpret_cast<Player_GetWeaponName>(player_GetName)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_GetName");
            return "";
        }
    }

    bool Exists()
    {
        void *player_WeaponExists = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_Exists");
        if (player_WeaponExists)
            return reinterpret_cast<Player_WeaponExists>(player_WeaponExists)(this->m_playerSlot, this->m_weaponID);
        else
        {
            NOT_SUPPORTED("scripting_Player_Weapon_Exists");
            return false;
        }
    }

    void SetDefaultChangeSkinAttributes()
    {
        void *player_WeaponSetDefaultChangeSkinAttributes = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_SetDefaultChangeSkinAttributes");
        if (player_WeaponSetDefaultChangeSkinAttributes)
            reinterpret_cast<Player_WeaponExists>(player_WeaponSetDefaultChangeSkinAttributes)(this->m_playerSlot, this->m_weaponID);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_SetDefaultChangeSkinAttributes");
    }

    void SetNametag(const char *text)
    {
        void *player_WeaponSetNametag = FetchFunctionPtr(nullptr, "scripting_Player_Weapon_SetNametag");
        if (player_WeaponSetNametag)
            reinterpret_cast<Player_SetNametag>(player_WeaponSetNametag)(this->m_playerSlot, this->m_weaponID, text);
        else
            NOT_SUPPORTED("scripting_Player_Weapon_SetNametag");
    }

    uint32_t GetID()
    {
        return this->m_weaponID;
    }
};

#endif