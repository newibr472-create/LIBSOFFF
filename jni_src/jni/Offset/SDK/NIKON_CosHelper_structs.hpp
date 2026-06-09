#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:38 2026
 
namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Enums
//---------------------By Tg @Manish8x---------------------------

// Enum CosHelper.ECosHelperFileInfoType
enum class ECosHelperFileInfoType : uint8_t
{
	ECosHelperFileInfoType__None   = 0,
	ECosHelperFileInfoType__ContentLength = 1,
	ECosHelperFileInfoType__ETag   = 2,
	ECosHelperFileInfoType__LastModifiedUtcTimestamp = 3,
	ECosHelperFileInfoType__ECosHelperFileInfoType_MAX = 4
};



//---------------------By Tg @Manish8x---------------------------
//Script Structs
//---------------------By Tg @Manish8x---------------------------

// ScriptStruct CosHelper.CosHelperInitializeInfo
// 0x0058
struct FCosHelperInitializeInfo
{
	bool                                               bUseAuthorization;                                        // 0x0000(0x0001) (BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
	int                                                SignExpirationTime;                                       // 0x0004(0x0004) (BlueprintVisible, ZeroConstructor, IsPlainOldData)
	struct FString                                     AppID;                                                    // 0x0008(0x0010) (BlueprintVisible, ZeroConstructor)
	struct FString                                     BucketName;                                               // 0x0018(0x0010) (BlueprintVisible, ZeroConstructor)
	struct FString                                     Region;                                                   // 0x0028(0x0010) (BlueprintVisible, ZeroConstructor)
	struct FString                                     SecretId;                                                 // 0x0038(0x0010) (BlueprintVisible, ZeroConstructor)
	struct FString                                     SecretKey;                                                // 0x0048(0x0010) (BlueprintVisible, ZeroConstructor)
};

}

