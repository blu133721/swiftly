#ifndef _nativehooks_h
#define _nativehooks_h

#include "FuncHook.h"
#include "../sdk/entity/CTakeDamageInfo.h"
#include "../sdk/entity/services.h"
#include "../sdk/entity/CGameRules.h"

class CBasePlayerController;
class CCommand;
class CGameRules;
class INetChannel;
class INetworkSerializable;
class CServerSideClient;
class IEngineServiceMgr;
class IGameEventManager2;

void Hook_LoggingSystem_LogDirect(int, int, const char *, ...);
void Hook_LoggingSystem_Log(int, int, const char *, ...);
void Hook_LoggingSystem_LogAssert(const char *, ...);
void Hook_Msg(const char *, ...);
void Hook_Warning(const char *, ...);
void Hook_ClientPrint(CBasePlayerController *, int, const char *, const char *, const char *, const char *, const char *);
void Hook_PrecacheResource(const char *, int64_t);
bool Hook_IsHearingClient(void *, int);
void Hook_CGameRules_Constructor(CGameRules *);
void Hook_SendNetMessage(INetChannel *pNetChan, INetworkSerializable *pNetMessage, void *pData, int a4);
void *Hook_HostStateRequest(void *a1, void **pRequest);
void Hook_CCSPlayerPawnBase_PostThink(CCSPlayerPawnBase *);
void Hook_CCSPlayer_MovementServices_CheckJumpPre(CCSPlayer_MovementServices *movServices, void *mt);
void Hook_CGameEventManager_Init(IGameEventManager2 *);
void FASTCALL Hook_CBaseEntity_TakeDamageOld(Z_CBaseEntity *, CTakeDamageInfo *);
bool InitializeHooks();

#endif