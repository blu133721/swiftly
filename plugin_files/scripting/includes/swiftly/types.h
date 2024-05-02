#ifndef _swiftly_types_h
#define _swiftly_types_h

#include <stdint.h>

class Color
{
public:
    int r, g, b, a;
    Color(int m_r, int m_g, int m_b, int m_a) : r(m_r), g(m_g), b(m_b), a(m_a) {}
};

class Vector
{
public:
    float x, y, z;
    Vector(float m_x, float m_y, float m_z) : x(m_x), y(m_y), z(m_z) {}
};

enum ContentType
{
    APPLICATION_JSON = 1,
    APPLICATION_FORM_URLENCODE = 2,
    MULTIPART_FORMDATA = 3,
    TEXT_CSS = 4,
    TEXT_CSV = 5,
    TEXT_HTML = 6,
    TEXT_JAVASCRIPT = 7,
    TEXT_PLAIN = 8,
    TEXT_XML = 9,
};

enum PlayerStat : int
{
    KILLS = 0,
    DEATHS = 1,
    ASSISTS = 2,
    DAMAGE = 3
};

enum ELogType
{
    LOGLEVEL_DEBUG = 1,
    LOGLEVEL_WARNING = 2,
    LOGLEVEL_ERROR = 3,
    LOGLEVEL_COMMON = 4,
};

enum HudDestination
{
    HUD_PRINTNOTIFY = 1,
    HUD_PRINTCONSOLE = 2,
    HUD_PRINTTALK = 3,
    HUD_PRINTCENTER = 4
};

enum ENetworkDisconnectionReason
{
    NETWORK_DISCONNECT_INVALID = 0,
    NETWORK_DISCONNECT_SHUTDOWN = 1,
    NETWORK_DISCONNECT_DISCONNECT_BY_USER = 2,
    NETWORK_DISCONNECT_DISCONNECT_BY_SERVER = 3,
    NETWORK_DISCONNECT_LOST = 4,
    NETWORK_DISCONNECT_OVERFLOW = 5,
    NETWORK_DISCONNECT_STEAM_BANNED = 6,
    NETWORK_DISCONNECT_STEAM_INUSE = 7,
    NETWORK_DISCONNECT_STEAM_TICKET = 8,
    NETWORK_DISCONNECT_STEAM_LOGON = 9,
    NETWORK_DISCONNECT_STEAM_AUTHCANCELLED = 10,
    NETWORK_DISCONNECT_STEAM_AUTHALREADYUSED = 11,
    NETWORK_DISCONNECT_STEAM_AUTHINVALID = 12,
    NETWORK_DISCONNECT_STEAM_VACBANSTATE = 13,
    NETWORK_DISCONNECT_STEAM_LOGGED_IN_ELSEWHERE = 14,
    NETWORK_DISCONNECT_STEAM_VAC_CHECK_TIMEDOUT = 15,
    NETWORK_DISCONNECT_STEAM_DROPPED = 16,
    NETWORK_DISCONNECT_STEAM_OWNERSHIP = 17,
    NETWORK_DISCONNECT_SERVERINFO_OVERFLOW = 18,
    NETWORK_DISCONNECT_TICKMSG_OVERFLOW = 19,
    NETWORK_DISCONNECT_STRINGTABLEMSG_OVERFLOW = 20,
    NETWORK_DISCONNECT_DELTAENTMSG_OVERFLOW = 21,
    NETWORK_DISCONNECT_TEMPENTMSG_OVERFLOW = 22,
    NETWORK_DISCONNECT_SOUNDSMSG_OVERFLOW = 23,
    NETWORK_DISCONNECT_SNAPSHOTOVERFLOW = 24,
    NETWORK_DISCONNECT_SNAPSHOTERROR = 25,
    NETWORK_DISCONNECT_RELIABLEOVERFLOW = 26,
    NETWORK_DISCONNECT_BADDELTATICK = 27,
    NETWORK_DISCONNECT_NOMORESPLITS = 28,
    NETWORK_DISCONNECT_TIMEDOUT = 29,
    NETWORK_DISCONNECT_DISCONNECTED = 30,
    NETWORK_DISCONNECT_LEAVINGSPLIT = 31,
    NETWORK_DISCONNECT_DIFFERENTCLASSTABLES = 32,
    NETWORK_DISCONNECT_BADRELAYPASSWORD = 33,
    NETWORK_DISCONNECT_BADSPECTATORPASSWORD = 34,
    NETWORK_DISCONNECT_HLTVRESTRICTED = 35,
    NETWORK_DISCONNECT_NOSPECTATORS = 36,
    NETWORK_DISCONNECT_HLTVUNAVAILABLE = 37,
    NETWORK_DISCONNECT_HLTVSTOP = 38,
    NETWORK_DISCONNECT_KICKED = 39,
    NETWORK_DISCONNECT_BANADDED = 40,
    NETWORK_DISCONNECT_KICKBANADDED = 41,
    NETWORK_DISCONNECT_HLTVDIRECT = 42,
    NETWORK_DISCONNECT_PURESERVER_CLIENTEXTRA = 43,
    NETWORK_DISCONNECT_PURESERVER_MISMATCH = 44,
    NETWORK_DISCONNECT_USERCMD = 45,
    NETWORK_DISCONNECT_REJECTED_BY_GAME = 46,
    NETWORK_DISCONNECT_MESSAGE_PARSE_ERROR = 47,
    NETWORK_DISCONNECT_INVALID_MESSAGE_ERROR = 48,
    NETWORK_DISCONNECT_BAD_SERVER_PASSWORD = 49,
    NETWORK_DISCONNECT_DIRECT_CONNECT_RESERVATION = 50,
    NETWORK_DISCONNECT_CONNECTION_FAILURE = 51,
    NETWORK_DISCONNECT_NO_PEER_GROUP_HANDLERS = 52,
    NETWORK_DISCONNECT_RECONNECTION = 53,
    NETWORK_DISCONNECT_LOOPSHUTDOWN = 54,
    NETWORK_DISCONNECT_LOOPDEACTIVATE = 55,
    NETWORK_DISCONNECT_HOST_ENDGAME = 56,
    NETWORK_DISCONNECT_LOOP_LEVELLOAD_ACTIVATE = 57,
    NETWORK_DISCONNECT_CREATE_SERVER_FAILED = 58,
    NETWORK_DISCONNECT_EXITING = 59,
    NETWORK_DISCONNECT_REQUEST_HOSTSTATE_IDLE = 60,
    NETWORK_DISCONNECT_REQUEST_HOSTSTATE_HLTVRELAY = 61,
    NETWORK_DISCONNECT_CLIENT_CONSISTENCY_FAIL = 62,
    NETWORK_DISCONNECT_CLIENT_UNABLE_TO_CRC_MAP = 63,
    NETWORK_DISCONNECT_CLIENT_NO_MAP = 64,
    NETWORK_DISCONNECT_CLIENT_DIFFERENT_MAP = 65,
    NETWORK_DISCONNECT_SERVER_REQUIRES_STEAM = 66,
    NETWORK_DISCONNECT_STEAM_DENY_MISC = 67,
    NETWORK_DISCONNECT_STEAM_DENY_BAD_ANTI_CHEAT = 68,
    NETWORK_DISCONNECT_SERVER_SHUTDOWN = 69
};

enum WeaponSlot : uint32_t
{
    INVALID = 0xffffffff,
    RIFLE = 0x0,
    PISTOL = 0x1,
    KNIFE = 0x2,
    GRENADES = 0x3,
    C4 = 0x4,
    RSLOT6 = 0x5,
    RSLOT7 = 0x6,
    RSLOT8 = 0x7,
    RSLOT9 = 0x8,
    RSLOT10 = 0x9,
    RSLOT11 = 0xa,
    BOOSTS = 0xb,
    UTILITY = 0xc,
    COUNT = 0xd,
    CURRENT_WEAPON = 0xe,
    FIRST = 0x0,
    LAST = 0xC,
};

enum WeaponSilencerType : uint32_t
{
    NONE = 0x0,
    DETACHABLE = 0x1,
    INTEGRATED = 0x2,
};

enum PluginState : int
{
    STOPPED = 0,
    STARTED = 1
};

enum CollisionGroup
{
    ALWAYS = 0,
    NEVER,
    TRIGGER,
    CONDITIONALLY_SOLID,
    DEFAULT,
    DEBRIS,
    INTERACTIVE_DEBRIS,
    INTERACTIVE,
    PLAYER,
    BREAKABLE_GLASS,
    VEHICLE,
    PLAYER_MOVEMENT,
    NPC,
    IN_VEHICLE,
    WEAPON,
    VEHICLE_CLIP,
    PROJECTILE,
    DOOR_BLOCKER,
    PASSABLE_DOOR,
    DISSOLVING,
    PUSHAWAY,
    NPC_ACTOR,
    NPC_SCRIPTED,
    PZ_CLIP,
    PROPS,
    LAST_SHARED_COLLISION_GROUP
};

enum DamageTypes : uint32_t
{
    DMG_GENERIC = 0x0,
    DMG_CRUSH = 0x1,
    DMG_BULLET = 0x2,
    DMG_SLASH = 0x4,
    DMG_BURN = 0x8,
    DMG_VEHICLE = 0x10,
    DMG_FALL = 0x20,
    DMG_BLAST = 0x40,
    DMG_CLUB = 0x80,
    DMG_SHOCK = 0x100,
    DMG_SONIC = 0x200,
    DMG_ENERGYBEAM = 0x400,
    DMG_DROWN = 0x4000,
    DMG_POISON = 0x8000,
    DMG_RADIATION = 0x10000,
    DMG_DROWNRECOVER = 0x20000,
    DMG_ACID = 0x40000,
    DMG_PHYSGUN = 0x100000,
    DMG_DISSOLVE = 0x200000,
    DMG_BLAST_SURFACE = 0x400000,
    DMG_BUCKSHOT = 0x1000000,
    DMG_LASTGENERICFLAG = 0x1000000,
    DMG_HEADSHOT = 0x2000000,
    DMG_DANGERZONE = 0x4000000,
};

enum DamageFlags : uint32_t
{
    FLAG_NONE = 0x0,
    FLAG_SUPPRESS_HEALTH_CHANGES = 0x1,
    FLAG_SUPPRESS_PHYSICS_FORCE = 0x2,
    FLAG_SUPPRESS_EFFECTS = 0x4,
    FLAG_PREVENT_DEATH = 0x8,
    FLAG_FORCE_DEATH = 0x10,
    FLAG_ALWAYS_GIB = 0x20,
    FLAG_NEVER_GIB = 0x40,
    FLAG_REMOVE_NO_RAGDOLL = 0x80,
    FLAG_SUPPRESS_DAMAGE_MODIFICATION = 0x100,
    FLAG_ALWAYS_FIRE_DAMAGE_EVENTS = 0x200,
    FLAG_RADIUS_DMG = 0x400,
    FLAG_LASTDFLAG = 0x400,
    FLAG_IGNORE_ARMOR = 0x800,
};

enum CSPlayerState : uint32_t
{
    STATE_ACTIVE = 0x0,
    STATE_WELCOME = 0x1,
    STATE_PICKINGTEAM = 0x2,
    STATE_PICKINGCLASS = 0x3,
    STATE_DEATH_ANIM = 0x4,
    STATE_DEATH_WAIT_FOR_KEY = 0x5,
    STATE_OBSERVER_MODE = 0x6,
    STATE_GUNGAME_RESPAWN = 0x7,
    STATE_DORMANT = 0x8,
    NUM_PLAYER_STATES = 0x9,
};

enum CSPlayerBlockingUseAction_t : uint32_t
{
    k_CSPlayerBlockingUseAction_None = 0x0,
    k_CSPlayerBlockingUseAction_DefusingDefault = 0x1,
    k_CSPlayerBlockingUseAction_DefusingWithKit = 0x2,
    k_CSPlayerBlockingUseAction_HostageGrabbing = 0x3,
    k_CSPlayerBlockingUseAction_HostageDropping = 0x4,
    k_CSPlayerBlockingUseAction_OpeningSafe = 0x5,
    k_CSPlayerBlockingUseAction_EquippingParachute = 0x6,
    k_CSPlayerBlockingUseAction_EquippingHeavyArmor = 0x7,
    k_CSPlayerBlockingUseAction_EquippingContract = 0x8,
    k_CSPlayerBlockingUseAction_EquippingTabletUpgrade = 0x9,
    k_CSPlayerBlockingUseAction_TakingOffHeavyArmor = 0xa,
    k_CSPlayerBlockingUseAction_PayingToOpenDoor = 0xb,
    k_CSPlayerBlockingUseAction_CancelingSpawnRappelling = 0xc,
    k_CSPlayerBlockingUseAction_EquippingExoJump = 0xd,
    k_CSPlayerBlockingUseAction_PickingUpBumpMine = 0xe,
    k_CSPlayerBlockingUseAction_MapLongUseEntity_Pickup = 0xf,
    k_CSPlayerBlockingUseAction_MapLongUseEntity_Place = 0x10,
    k_CSPlayerBlockingUseAction_MaxCount = 0x11,
};

enum MoveType_t : uint8_t
{
    MOVETYPE_NONE = 0x0,
    MOVETYPE_OBSOLETE = 0x1,
    MOVETYPE_WALK = 0x2,
    MOVETYPE_FLY = 0x3,
    MOVETYPE_FLYGRAVITY = 0x4,
    MOVETYPE_VPHYSICS = 0x5,
    MOVETYPE_PUSH = 0x6,
    MOVETYPE_NOCLIP = 0x7,
    MOVETYPE_OBSERVER = 0x8,
    MOVETYPE_LADDER = 0x9,
    MOVETYPE_CUSTOM = 0xa,
    MOVETYPE_LAST = 0xb,
    MOVETYPE_INVALID = 0xb,
    MOVETYPE_MAX_BITS = 0x5,
};

enum PlayerFlags : uint32_t
{
    FL_ONGROUND = 0x0,
    FL_DUCKING = 0x1,
    FL_WATERJUMP = 0x2,
    FL_ONTRAIN = 0x3,
    FL_INRAIN = 0x4,
    FL_FROZEN = 0x5,
    FL_ATCONTROLS = 0x6,
    FL_CLIENT = 0x7,
    FL_FAKECLIENT = 0x8,
    FL_INWATER = 0x9,
};

#endif