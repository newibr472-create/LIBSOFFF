#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:19 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Parameters
//---------------------By Tg @Manish8x---------------------------

// Function PhotonDestructible.PhotonDestructibleSurfaceComponent.Server_ProcessHit
struct UPhotonDestructibleSurfaceComponent_Server_ProcessHit_Params
{
	struct FPhotonDestructibleSurfaceHitData           HitData;                                                  // (ConstParm, Parm, OutParm, ReferenceParm)
};

// Function PhotonDestructible.PhotonDestructibleSurfaceComponent.OnRep_SurfaceNetData
struct UPhotonDestructibleSurfaceComponent_OnRep_SurfaceNetData_Params
{
};

// Function PhotonDestructible.PhotonDestructibleSurfaceComponent.Client_OnProcessHit
struct UPhotonDestructibleSurfaceComponent_Client_OnProcessHit_Params
{
	struct FPhotonDestructibleSurfaceHitData           HitData;                                                  // (ConstParm, Parm, ReferenceParm)
};

// Function PhotonDestructible.PhotonDestructibleInstancedSurfaceComponent.Server_ProcessHit
struct UPhotonDestructibleInstancedSurfaceComponent_Server_ProcessHit_Params
{
	struct FPhotonDestructibleSurfaceHitData           HitData;                                                  // (ConstParm, Parm, OutParm, ReferenceParm)
};

// Function PhotonDestructible.PhotonDestructibleInstancedSurfaceComponent.OnRep_SurfaceNetData
struct UPhotonDestructibleInstancedSurfaceComponent_OnRep_SurfaceNetData_Params
{
};

// Function PhotonDestructible.PhotonDestructibleInstancedSurfaceComponent.Client_OnProcessHit
struct UPhotonDestructibleInstancedSurfaceComponent_Client_OnProcessHit_Params
{
	struct FPhotonDestructibleSurfaceHitData           HitData;                                                  // (ConstParm, Parm, ReferenceParm)
};

// Function PhotonDestructible.PhotonDestructiblePuppet.TriggerPuppetEvent
struct UPhotonDestructiblePuppet_TriggerPuppetEvent_Params
{
	int                                                EventID;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function PhotonDestructible.PhotonDestructiblePuppetContainer.TriggerPuppetEvent
struct UPhotonDestructiblePuppetContainer_TriggerPuppetEvent_Params
{
	struct FString                                     ObjectName;                                               // (Parm, ZeroConstructor)
	int                                                EventID;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function PhotonDestructible.PhotonDestructiblePuppetMgr.Client_TriggerPuppetEvent
struct UPhotonDestructiblePuppetMgr_Client_TriggerPuppetEvent_Params
{
	struct FGuid                                       PuppetGUID;                                               // (ConstParm, Parm, ReferenceParm, IsPlainOldData)
	int                                                EventID;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
};

}

