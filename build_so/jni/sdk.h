// sdk.h — Minimal UE4 SDK for BGMI 4.4.0 ESP
// Extracted from NIKON SDK dump. Only what's needed for ESP/Aimbot.
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

// VTable helper
template<typename Fn>
inline Fn GetVFunction(void *thiz, int idx) {
    auto VTable = *reinterpret_cast<void***>(const_cast<void*>(thiz));
    return (Fn)(VTable[idx]);
}

// Basic types
struct FVector { 
    float X=0, Y=0, Z=0; 
    FVector()=default;
    FVector(float x,float y,float z):X(x),Y(y),Z(z){}
    FVector operator-(const FVector&o)const{return{X-o.X,Y-o.Y,Z-o.Z};}
    FVector operator+(const FVector&o)const{return{X+o.X,Y+o.Y,Z+o.Z};}
    float Size()const{return sqrtf(X*X+Y*Y+Z*Z);}
};
struct FVector2D { float X=0, Y=0; };
struct FRotator { float Pitch=0, Yaw=0, Roll=0; };
struct FLinearColor { float R=0, G=0, B=0, A=1; };

struct FName {
    int32_t ComparisonIndex;
    int32_t Number;
};

// TArray
template<class T>
struct TArray {
    T* Data = nullptr;
    int32_t Count = 0;
    int32_t Max = 0;
    int Num() const { return Count; }
    T& operator[](int i) { return Data[i]; }
    const T& operator[](int i) const { return Data[i]; }
    bool IsValidIndex(int i) const { return i >= 0 && i < Count; }
};

// FString
struct FString : public TArray<wchar_t> {};

// FNameEntry
class FNameEntry {
public:
    static const auto NAME_WIDE_MASK = 0x1;
    static const auto NAME_INDEX_SHIFT = 1;
    int32_t Index;
    char pad[0x8]; // 64-bit
    union {
        char AnsiName[1024];
        wchar_t WideName[1024];
    };
    const int32_t GetIndex() const { return Index >> NAME_INDEX_SHIFT; }
    bool IsWide() const { return Index & NAME_WIDE_MASK; }
    const char* GetAnsiName() const { return AnsiName; }
};

// TNameEntryArray
template<int32_t ElementsPerChunk>
class TStaticIndirectArray {
public:
    enum { ChunkTableSize = 512 };
    FNameEntry** Chunks[ChunkTableSize];
    int32_t NumElements;
    int32_t NumChunks;
    
    bool IsValidIndex(int32_t index) const { return index >= 0 && index < NumElements; }
    FNameEntry* GetById(int32_t index) const {
        int chunk = index / ElementsPerChunk;
        int within = index % ElementsPerChunk;
        if (chunk >= NumChunks || !Chunks[chunk]) return nullptr;
        return Chunks[chunk][within];
    }
};
typedef TStaticIndirectArray<16384> TNameEntryArray;

// FUObjectItem
class UObject;
class FUObjectItem {
public:
    UObject* Object;
    int32_t Flags;
    int32_t ClusterIndex;
    int32_t SerialNumber;
};

// TUObjectArray
class TUObjectArray {
public:
    int32_t Num() const { return NumElements; }
    UObject* GetByIndex(int32_t index) const {
        if (index < 0 || index >= NumElements) return nullptr;
        return Objects[index].Object;
    }
    FUObjectItem* GetItemByIndex(int32_t index) const {
        if (index < 0 || index >= NumElements) return nullptr;
        return &Objects[index];
    }
private:
    FUObjectItem* Objects;
    int32_t MaxElements;
    int32_t NumElements;
};

// FUObjectArray
class FUObjectArray {
public:
    int32_t ObjFirstGCIndex;
    int32_t ObjLastNonGCIndex;
    int32_t MaxObjectsNotConsideredByGC;
    int32_t OpenForDisregardForGC;
    TUObjectArray ObjObjects;
};

// UObject — minimal
class UClass;
class UFunction;

class UObject {
public:
    static FUObjectArray* GUObjectArray;
    static TNameEntryArray* GNames;
    
    void* VTable;
    int32_t ObjectFlags;
    int32_t InternalIndex;
    UClass* ClassPrivate;
    FName NamePrivate;
    UObject* OuterPrivate;
    
    static TUObjectArray& GetGlobalObjects() { return GUObjectArray->ObjObjects; }
    
    std::string GetName() const;
    std::string GetFullName() const;
    bool IsA(UClass* cmp) const;
    
    void ProcessEvent(UFunction* func, void* params) {
        GetVFunction<void(*)(UObject*, UFunction*, void*)>(this, 76)(this, func, params);
    }
    
    template<typename T>
    static T* FindObject(const std::string& name) {
        for (int i = 0; i < GetGlobalObjects().Num(); ++i) {
            auto object = GetGlobalObjects().GetByIndex(i);
            if (!object) continue;
            if (object->GetFullName() == name)
                return static_cast<T*>(object);
        }
        return nullptr;
    }
    
    static UClass* FindClass(const std::string& name) {
        return FindObject<UClass>(name);
    }
};

// UField/UStruct/UClass chain
class UField : public UObject {
public:
    UField* Next;
};

class UStruct : public UField {
public:
    unsigned char pad_UStruct[0x70]; // padding to get to SuperField
};

class UClass : public UStruct {};

class UFunction : public UStruct {
public:
    uint32_t FunctionFlags;
    // ... more fields we don't need
};

// UCanvas
struct UCanvas_K2_DrawLine_Params {
    FVector2D ScreenPositionA;
    FVector2D ScreenPositionB;
    float Thickness;
    FLinearColor RenderColor;
};

class UCanvas : public UObject {
public:
    unsigned char pad0[0x1C]; // 0x28 to 0x44
    int32_t SizeX; // 0x44
    int32_t SizeY; // 0x48
    
    void K2_DrawLine(const FVector2D& A, const FVector2D& B, float Thickness, const FLinearColor& Color);
};

// Verify UCanvas layout: VTable(8) + ObjectFlags(4) + InternalIndex(4) + ClassPrivate(8) + NamePrivate(8) + OuterPrivate(8) = 0x28 base
// Then pad to 0x44 = 0x1C bytes of unknown data, then SizeX at 0x44, SizeY at 0x48

// UGameViewportClient
class UWorld;
class UGameViewportClient : public UObject {
public:
    unsigned char pad[0x78]; // padding — World is deep in the class
    UWorld* World; // approximate — we'll find it via GetFullWorld instead
    
    static UClass* StaticClass() {
        static UClass* c = nullptr;
        if (!c) c = UObject::FindClass("Class Engine.GameViewportClient");
        return c;
    }
};

// UEngine  
class UGameViewportClient;
class UEngine : public UObject {
public:
    unsigned char pad_engine[0x888]; // large padding
    UGameViewportClient* GameViewport; // offset varies, found via iteration
    
    static UClass* StaticClass() {
        static UClass* c = nullptr;
        if (!c) c = UObject::FindClass("Class Engine.Engine");
        return c;
    }
};

// UWorld
class ULevel;
class UGameInstance;
class UWorld : public UObject {
public:
    // 0x28 (UObject) + 0x08 padding = 0x30
    unsigned char pad_to_level[0x08]; 
    ULevel* PersistentLevel; // 0x30
    unsigned char pad_to_gi[0x43C]; // 0x38 to 0x470
    UGameInstance* OwningGameInstance; // 0x470
    // Net driver check
    unsigned char pad_to_net[0x08];
    void* NetDriver; // approximate
};

// UGameInstance
class UGameInstance : public UObject {
public:
    unsigned char pad_to_lp[0x20]; // 0x28 to 0x48
    TArray<UObject*> LocalPlayers; // 0x48
};

// APlayerController
class APlayerController : public UObject {
public:
    unsigned char pad_pc[0x4B8]; // 0x28 to 0x4E0
    FRotator ControlRotation; // 0x4E0
    unsigned char pad2[0x3C]; // 0x4EC to 0x528
    UObject* AcknowledgedPawn; // 0x528
    unsigned char pad3[0x18]; // 0x530 to 0x548
    UObject* PlayerCameraManager; // 0x548
    
    bool ProjectWorldLocationToScreen(FVector WorldLocation, bool bPlayerViewportRelative, FVector2D& ScreenPosition);
};

// APlayerCameraManager  
struct FMinimalViewInfo {
    FVector Location;       // +0x00
    FVector LocationLocal;  // +0x0C
    FRotator Rotation;      // +0x18
    float FOV;              // +0x24
};

class APlayerCameraManager : public UObject {
public:
    // CameraCache at 0x520, POV at +0x10 inside FCameraCacheEntry
    unsigned char pad_cam[0x4F8]; // 0x28 to 0x520
    unsigned char CacheEntry[0x10]; // 0x520: timestamp etc
    FMinimalViewInfo CachePOV; // 0x530: the actual POV
};

// ULevel  
class ULevel : public UObject {
public:
    unsigned char pad_lev[0x78]; // 0x28 to 0xA0
    TArray<UObject*> Actors; // 0xA0 — NOT USED, we call GetActorArray function instead
};

// ASTExtraPlayerCharacter (BGMI player character)
class ASTExtraPlayerCharacter : public UObject {
public:
    // We access these via offsets from base
    // TeamID at 0x0998
    // Health at 0x0E60
    // HealthMax at 0x0E64
    // bDead at 0x0E7C
    // Mesh at 0x0510
    
    int32_t GetTeamID() { return *(int32_t*)((uintptr_t)this + 0x0998); }
    float GetHealth() { return *(float*)((uintptr_t)this + 0x0E60); }
    float GetHealthMax() { return *(float*)((uintptr_t)this + 0x0E64); }
    uint8_t IsDead() { return *(uint8_t*)((uintptr_t)this + 0x0E7C); }
    
    FVector GetBonePos(const char* BoneName, FVector DefaultPos);
    
    static UClass* StaticClass() {
        static UClass* c = nullptr;
        if (!c) c = UObject::FindClass("Class ShadowTrackerExtra.STExtraPlayerCharacter");
        return c;
    }
};

// ULocalPlayer
class ULocalPlayer : public UObject {
public:
    unsigned char pad_ulp[0x08]; // 0x28 to 0x30
    APlayerController* PlayerController; // 0x30
    unsigned char pad2[0x20]; // 0x38 to 0x58  
    UGameViewportClient* ViewportClient; // 0x58
};

// Globals
extern uintptr_t UE4;
extern TNameEntryArray* (*GetGNamesFunc)();

// World access
UWorld* GetFullWorld();

// Offsets
namespace Off {
    constexpr uintptr_t GNames = 0x8394964;
    constexpr uintptr_t GUObjectArray = 0xe22f8d0;
    constexpr uintptr_t GetActorArray = 0xa1018ac;
    constexpr uintptr_t GNativeAndroidApp = 0xdf74398;
}
