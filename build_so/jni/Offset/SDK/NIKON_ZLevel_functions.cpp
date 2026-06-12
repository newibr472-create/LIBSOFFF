// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:30 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Functions
//---------------------By Tg @Manish8x---------------------------

// Function ZLevel.ZLevelData.ReBindLevelDataComponent
// (Final, Native, Public)

void AZLevelData::ReBindLevelDataComponent()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function ZLevel.ZLevelData.ReBindLevelDataComponent");

	AZLevelData_ReBindLevelDataComponent_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function ZLevel.ZLevelData.CheckMonsterSpotIsOnLand
// (Final, Native, Public)
// Parameters:
// class UZMonsterSpot*           MonsterSpot                    (Parm, ZeroConstructor, InstancedReference, IsPlainOldData)
// class UZMonsterSpotGroup*      SpotGroup                      (Parm, ZeroConstructor, InstancedReference, IsPlainOldData)

void AZLevelData::CheckMonsterSpotIsOnLand(class UZMonsterSpot* MonsterSpot, class UZMonsterSpotGroup* SpotGroup)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function ZLevel.ZLevelData.CheckMonsterSpotIsOnLand");

	AZLevelData_CheckMonsterSpotIsOnLand_Params params;
	params.MonsterSpot = MonsterSpot;
	params.SpotGroup = SpotGroup;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


}

