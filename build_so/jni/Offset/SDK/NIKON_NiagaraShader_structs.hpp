#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:47 2026
 
namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Enums
//---------------------By Tg @Manish8x---------------------------

// Enum NiagaraShader.FNiagaraCompileEventSeverity
enum class EFNiagaraCompileEventSeverity : uint8_t
{
	FNiagaraCompileEventSeverity__Log = 0,
	FNiagaraCompileEventSeverity__Warning = 1,
	FNiagaraCompileEventSeverity__Error = 2,
	FNiagaraCompileEventSeverity__FNiagaraCompileEventSeverity_MAX = 3
};



//---------------------By Tg @Manish8x---------------------------
//Script Structs
//---------------------By Tg @Manish8x---------------------------

// ScriptStruct NiagaraShader.NiagaraDataInterfaceGPUParamInfo
// 0x0020
struct FNiagaraDataInterfaceGPUParamInfo
{
	struct FString                                     DataInterfaceHLSLSymbol;                                  // 0x0000(0x0010) (ZeroConstructor)
	struct FString                                     DIClassName;                                              // 0x0010(0x0010) (ZeroConstructor)
};

// ScriptStruct NiagaraShader.NiagaraCompileEvent
// 0x0048
struct FNiagaraCompileEvent
{
	EFNiagaraCompileEventSeverity                      Severity;                                                 // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0001(0x0007) MISSED OFFSET
	struct FString                                     Message;                                                  // 0x0008(0x0010) (ZeroConstructor)
	struct FGuid                                       NodeGuid;                                                 // 0x0018(0x0010) (IsPlainOldData)
	struct FGuid                                       PinGuid;                                                  // 0x0028(0x0010) (IsPlainOldData)
	TArray<struct FGuid>                               StackGuids;                                               // 0x0038(0x0010) (ZeroConstructor)
};

}

