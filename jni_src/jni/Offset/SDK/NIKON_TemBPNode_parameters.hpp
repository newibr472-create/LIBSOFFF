#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:34 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Parameters
//---------------------By Tg @Manish8x---------------------------

// Function TemBPNode.TemBPLibrary.TempUIMsgInvokeRef
struct UTemBPLibrary_TempUIMsgInvokeRef_Params
{
	class UObject*                                     ObjContext;                                               // (Parm, ZeroConstructor, IsPlainOldData)
	struct FString                                     MsgName;                                                  // (Parm, ZeroConstructor)
	struct FString                                     ModuleName;                                               // (Parm, ZeroConstructor)
	TArray<struct FTemBPDataAddr>                      ParamArray;                                               // (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
};

// Function TemBPNode.TemBPLibrary.TempBridgeEventInvokeRef
struct UTemBPLibrary_TempBridgeEventInvokeRef_Params
{
	class UObject*                                     ObjContext;                                               // (Parm, ZeroConstructor, IsPlainOldData)
	struct FString                                     EventType;                                                // (Parm, ZeroConstructor)
	struct FString                                     EventID;                                                  // (Parm, ZeroConstructor)
	TArray<struct FTemBPDataAddr>                      ParamArray;                                               // (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
};

// Function TemBPNode.TemBPLibrary.AddrFromWild
struct UTemBPLibrary_AddrFromWild_Params
{
	struct FString                                     DataType;                                                 // (Parm, ZeroConstructor)
	struct FTemBPDataAddr                              InAny;                                                    // (ConstParm, Parm, OutParm, ReferenceParm)
	struct FTemBPDataAddr                              ReturnValue;                                              // (Parm, OutParm, ReturnParm)
};

// Function TemBPNode.TemBPLibrary.AddrFromSet
struct UTemBPLibrary_AddrFromSet_Params
{
	struct FString                                     DataType;                                                 // (Parm, ZeroConstructor)
	struct FTemBPDataAddr                              ReturnValue;                                              // (Parm, OutParm, ReturnParm)
};

// Function TemBPNode.TemBPLibrary.AddrFromMap
struct UTemBPLibrary_AddrFromMap_Params
{
	struct FString                                     DataType;                                                 // (Parm, ZeroConstructor)
	TMap<int, int>                                     InAny;                                                    // (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
	struct FTemBPDataAddr                              ReturnValue;                                              // (Parm, OutParm, ReturnParm)
};

// Function TemBPNode.TemBPLibrary.AddrFromArray
struct UTemBPLibrary_AddrFromArray_Params
{
	struct FString                                     DataType;                                                 // (Parm, ZeroConstructor)
	TArray<int>                                        InAny;                                                    // (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
	struct FTemBPDataAddr                              ReturnValue;                                              // (Parm, OutParm, ReturnParm)
};

}

