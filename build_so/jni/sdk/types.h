// sdk/types.h — BGMI 4.4.0 verified from NIKON SDK dump
#pragma once
#include <cstdint>

struct FVector { 
    float X, Y, Z; 
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    FVector operator-(const FVector &o) const { return {X-o.X, Y-o.Y, Z-o.Z}; }
    FVector operator+(const FVector &o) const { return {X+o.X, Y+o.Y, Z+o.Z}; }
    float Length() const { return __builtin_sqrtf(X*X + Y*Y + Z*Z); }
};
struct FVector2D { float X, Y; };
struct FRotator { float Pitch, Yaw, Roll; };
struct FLinearColor { float R, G, B, A; };

// FMinimalViewInfo (verified from SDK — has LocationLocalSpace between Location and Rotation!)
struct FMinimalViewInfo {
    FVector  Location;           // +0x00
    FVector  LocationLocalSpace; // +0x0C  ← THIS WAS MISSING!
    FRotator Rotation;           // +0x18
    float    FOV;                // +0x24
};

struct alignas(16) FTransform {
    float Rotation[4];
    float Translation[4];
    float Scale3D[4];
    FVector GetLocation() const { return {Translation[0], Translation[1], Translation[2]}; }
};

template <typename T>
struct TArray {
    T *Data;
    int32_t Count;
    int32_t Max;
    bool IsValid() const { return Data != nullptr && Count > 0; }
};

namespace Off {
    // Global offsets (relative to libUE4.so base)
    constexpr uintptr_t GNames         = 0x8394964;
    constexpr uintptr_t GUObjectArray  = 0xe22f8d0;
    constexpr uintptr_t GNativeApp     = 0xdf74398;
    constexpr uintptr_t GWorld         = 0xa1018ac; // This is GetActorArray function — NOT UWorld pointer!

    // UWorld (from UObject base at 0x28)
    constexpr uint32_t World_PersistentLevel     = 0x0030;
    constexpr uint32_t World_OwningGameInstance  = 0x0470; // FIXED! Was 0x01A8

    // UGameInstance
    constexpr uint32_t GI_LocalPlayers           = 0x0048; // FIXED! Was 0x0038 (that's EncryptedLocalPlayers)

    // UPlayer / ULocalPlayer  
    constexpr uint32_t Player_PlayerController   = 0x0030; // Correct
    constexpr uint32_t LocalPlayer_ViewportClient= 0x0058; // FIXED! Was 0x0078

    // AController
    constexpr uint32_t ControlRotation           = 0x04E0; // Correct

    // APlayerController
    constexpr uint32_t PC_AcknowledgedPawn       = 0x0528; // Correct
    constexpr uint32_t PC_PlayerCameraManager    = 0x0548; // Correct

    // APlayerCameraManager
    // CameraCache at 0x0520, POV at +0x10 inside FCameraCacheEntry
    constexpr uint32_t CamMgr_CachePOV           = 0x0530; // 0x520 + 0x10 = Location starts here

    // AActor
    constexpr uint32_t Actor_RootComponent       = 0x0198; // Standard UE4
    
    // USceneComponent
    constexpr uint32_t Comp_RelativeLocation     = 0x0164;
    constexpr uint32_t Comp_ComponentToWorld     = 0x01D0;

    // ACharacter
    constexpr uint32_t Char_Mesh                 = 0x0510;

    // ASTExtraCharacter (BGMI specific)
    constexpr uint32_t Health                    = 0x0E60;
    constexpr uint32_t HealthMax                 = 0x0E64;
    constexpr uint32_t bDead                     = 0x0E7C;
    constexpr uint32_t TeamID                    = 0x0998;

    // USkeletalMeshComponent
    constexpr uint32_t BoneArray                 = 0x0600;

    // ULevel
    constexpr uint32_t Level_Actors              = 0x00A0; // TArray<AActor*> inside UnknownData00

    // UCanvas
    constexpr uint32_t Canvas_SizeX              = 0x0044;
    constexpr uint32_t Canvas_SizeY              = 0x0048;
}
