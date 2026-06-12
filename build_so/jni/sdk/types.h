/*
 * sdk/types.h — Minimal UE4 types for BGMI 4.4.0
 * Offsets verified from NIKON SDK dump
 */
#pragma once
#include <cstdint>
#include <cstring>

// ============================================================================
// Basic Math Types
// ============================================================================
struct FVector {
    float X, Y, Z;
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    FVector operator-(const FVector &o) const { return {X-o.X, Y-o.Y, Z-o.Z}; }
    FVector operator+(const FVector &o) const { return {X+o.X, Y+o.Y, Z+o.Z}; }
    FVector operator*(float s) const { return {X*s, Y*s, Z*s}; }
    float Length() const { return __builtin_sqrtf(X*X + Y*Y + Z*Z); }
    float Length2D() const { return __builtin_sqrtf(X*X + Y*Y); }
};

struct FVector2D {
    float X, Y;
    FVector2D() : X(0), Y(0) {}
    FVector2D(float x, float y) : X(x), Y(y) {}
};

struct FRotator {
    float Pitch, Yaw, Roll;
    FRotator() : Pitch(0), Yaw(0), Roll(0) {}
    FRotator(float p, float y, float r) : Pitch(p), Yaw(y), Roll(r) {}
};

struct FLinearColor {
    float R, G, B, A;
    FLinearColor() : R(0), G(0), B(0), A(1) {}
    FLinearColor(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {}
};

struct FMinimalViewInfo {
    FVector  Location;    // 0x0000
    FRotator Rotation;    // 0x000C
    float    FOV;         // 0x0018
};

// ============================================================================
// BGMI 4.4.0 Offsets (from NIKON SDK)
// ============================================================================
namespace Off {
    // Global pointers (relative to libUE4.so base)
    constexpr uintptr_t GNames         = 0x8394964;
    constexpr uintptr_t GUObjectArray  = 0xe22f8d0;
    constexpr uintptr_t GNativeApp     = 0xdf74398;
    constexpr uintptr_t GWorld         = 0xa1018ac;  // UWorld**

    // UObject
    constexpr uint32_t ObjIndex        = 0x000C;
    constexpr uint32_t ObjClass        = 0x0010;
    constexpr uint32_t ObjName         = 0x0018;

    // AActor
    constexpr uint32_t RootComponent   = 0x0198;
    
    // AController / APlayerController
    constexpr uint32_t ControlRotation = 0x04E0;
    constexpr uint32_t PlayerCameraMgr = 0x0548;
    constexpr uint32_t AcknowledgedPawn= 0x0528;
    
    // ACharacter
    constexpr uint32_t Mesh            = 0x0510;
    
    // ASTExtraCharacter (BGMI specific)
    constexpr uint32_t Health          = 0x0E60;
    constexpr uint32_t HealthMax       = 0x0E64;
    constexpr uint32_t bDead           = 0x0E7C;
    constexpr uint32_t TeamID          = 0x0998;
    
    // USceneComponent
    constexpr uint32_t RelativeLocation= 0x0164;
    constexpr uint32_t ComponentToWorld= 0x01D0;
    
    // USkeletalMeshComponent
    constexpr uint32_t BoneArray       = 0x0600;  // TArray<FTransform>
    constexpr uint32_t BoneSpaceTransforms = 0x0600;
    
    // APlayerCameraManager
    constexpr uint32_t CameraCachePOV  = 0x0530;  // CameraCache.POV.Location
    
    // UWorld
    constexpr uint32_t PersistentLevel = 0x0030;
    constexpr uint32_t OwningGameInstance = 0x01A8;
    
    // ULevel
    constexpr uint32_t Actors          = 0x00A0;  // TArray<AActor*>
    
    // UGameInstance
    constexpr uint32_t LocalPlayers    = 0x0038;
    
    // ULocalPlayer
    constexpr uint32_t PlayerController= 0x0030;
    
    // UCanvas
    constexpr uint32_t SizeX           = 0x0044;
    constexpr uint32_t SizeY           = 0x0048;
    
    // Canvas drawing functions (offsets in libUE4.so)
    constexpr uintptr_t K2_DrawLine    = 0x8DEE10C;
    constexpr uintptr_t K2_DrawText    = 0x8DEDF50;
    constexpr uintptr_t K2_DrawBox     = 0x8DEE2D0;
    
    // ProjectWorldToScreen
    constexpr uintptr_t ProjectWorldToScreen = 0x8C91D48;
    
    // GetBoneMatrix (USkeletalMeshComponent)
    constexpr uintptr_t GetBoneMatrix  = 0x89F4A10;
}

// ============================================================================
// TArray template
// ============================================================================
template <typename T>
struct TArray {
    T      *Data;
    int32_t Count;
    int32_t Max;
    
    int Num() const { return Count; }
    T &operator[](int i) { return Data[i]; }
    const T &operator[](int i) const { return Data[i]; }
    bool IsValid() const { return Data != nullptr && Count > 0; }
};

// ============================================================================
// FTransform (bone transforms)
// ============================================================================
struct alignas(16) FTransform {
    float Rotation[4];   // Quaternion (X,Y,Z,W)
    float Translation[4]; // XYZ + pad (Translation[0]=X, [1]=Y, [2]=Z)
    float Scale3D[4];    // XYZ + pad
    
    FVector GetLocation() const {
        return {Translation[0], Translation[1], Translation[2]};
    }
};
