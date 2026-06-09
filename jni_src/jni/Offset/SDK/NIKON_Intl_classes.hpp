#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:31 2026
 
namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Classes
//---------------------By Tg @Manish8x---------------------------

// Class Intl.StatManager
// 0x01E0 (0x0208 - 0x0028)
class UStatManager : public UObject
{
public:
	unsigned char                                      UnknownData00[0x1E0];                                     // 0x0028(0x01E0) MISSED OFFSET

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class Intl.StatManager");
		return pStaticClass;
	}


	void ReportUAStatsEvent(TArray<struct FString> InReportChannels, const struct FString& InEventName, TMap<struct FString, struct FString> InParams, bool InIsRealTime);
	void ReportRevenue(int InPurchaseEventType, const struct FString& InCurrencyCode, const struct FString& InExpense, TMap<struct FString, struct FString> InParams, const struct FString& InExtraJson);
	void ReportPurchase(int InPurchaseEventType, const struct FString& InCurrencyCode, const struct FString& InExpense, bool isRealTime);
	void ReportEventWithString(int EventType, const struct FString& _eventBody, bool isRealTime);
	void ReportEventWithParam(int EventType, TMap<struct FString, struct FString> _params, bool isRealTime);
	void ReportEventWithNoParam(int EventType, bool isRealTime);
	static class UStatManager* GetInstance();
};


}

