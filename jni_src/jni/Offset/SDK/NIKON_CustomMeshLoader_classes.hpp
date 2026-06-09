#pragma once

// SDK GENERATE BY TELEGRAM ;- (@Manish8x) (4.4.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @Manish8x
// GEN ONWed May 20 07:23:46 2026
 
namespace SDK
{
//---------------------By Tg @Manish8x---------------------------
//Classes
//---------------------By Tg @Manish8x---------------------------

// Class CustomMeshLoader.CustomMeshLoaderBPLibrary
// 0x0000 (0x0028 - 0x0028)
class UCustomMeshLoaderBPLibrary : public UBlueprintFunctionLibrary
{
public:

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class CustomMeshLoader.CustomMeshLoaderBPLibrary");
		return pStaticClass;
	}


	static class UStaticMesh* LoadSModelFromMemory(TArray<unsigned char> Data, const struct FString& MeshName);
	static void LoadSModelFileAsync(const struct FString& FilePath, const struct FString& MeshName, const struct FScriptDelegate& OnComplete);
	static class UStaticMesh* LoadSModelFile(const struct FString& FilePath, const struct FString& MeshName);
	static bool IsValidSModelFile(const struct FString& FilePath);
	static bool GetSModelFileInfo(const struct FString& FilePath, int* OutSectionCount, int* OutMaterialCount, int* OutVertexCount);
};


}

