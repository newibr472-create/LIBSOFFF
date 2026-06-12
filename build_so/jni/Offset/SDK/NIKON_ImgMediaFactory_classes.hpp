#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:45 2026
 
namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Classes
//---------------------By Tg @Manish8x---------------------------

// Class ImgMediaFactory.ImgMediaSettings
// 0x0028 (0x0050 - 0x0028)
class UImgMediaSettings : public UObject
{
public:
	float                                              DefaultFps;                                               // 0x0028(0x0004) (Edit, ZeroConstructor, Config, IsPlainOldData)
	float                                              CacheBehindPercentage;                                    // 0x002C(0x0004) (Edit, ZeroConstructor, Config, IsPlainOldData)
	float                                              CacheSizeGB;                                              // 0x0030(0x0004) (Edit, ZeroConstructor, Config, IsPlainOldData)
	uint32_t                                           ExrDecoderThreads;                                        // 0x0034(0x0004) (Edit, ZeroConstructor, Config, IsPlainOldData)
	struct FString                                     DefaultProxy;                                             // 0x0038(0x0010) (Edit, ZeroConstructor, Config)
	bool                                               UseDefaultProxy;                                          // 0x0048(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0049(0x0007) MISSED OFFSET

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class ImgMediaFactory.ImgMediaSettings");
		return pStaticClass;
	}

};


}

