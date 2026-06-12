#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:35 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Parameters
//---------------------By Tg @Manish8x---------------------------

// Function CreativeLua.CreativeBridgeLuaVM.UGCLuaError
struct UCreativeBridgeLuaVM_UGCLuaError_Params
{
	int                                                ErrCode;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function CreativeLua.CreativeBridgeLuaVM.SetUGCSuspendStateFix
struct UCreativeBridgeLuaVM_SetUGCSuspendStateFix_Params
{
	bool                                               bEnable;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function CreativeLua.CreativeBridgeLuaVM.RegisterSluaCallUgcluaEventHandler
struct UCreativeBridgeLuaVM_RegisterSluaCallUgcluaEventHandler_Params
{
};

// Function CreativeLua.CreativeBridgeLuaVM.PostInit
struct UCreativeBridgeLuaVM_PostInit_Params
{
};

// Function CreativeLua.CreativeBridgeLuaVM.GetUGCSuspendStateFix
struct UCreativeBridgeLuaVM_GetUGCSuspendStateFix_Params
{
	bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

// Function CreativeLua.CreativeEnvLuaVMFactory.CreateCreativeEnvLuaVM
struct UCreativeEnvLuaVMFactory_CreateCreativeEnvLuaVM_Params
{
	struct FString                                     InLuaFilePath;                                            // (Parm, ZeroConstructor)
	class UObject*                                     WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
	class UCreativeEnvLuaVM*                           ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

}

