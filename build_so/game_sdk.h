#pragma once
/*
 * game_sdk.h — Minimal type-safe SDK for BGMI 4.4.0 ARM64
 * 
 * All offsets extracted from NIKON SDK dump (verified .hpp files):
 *   - SDK/NIKON_Engine_classes.hpp
 *   - SDK/NIKON_Engine_structs.hpp
 *   - SDK/NIKON_Gameplay_classes.hpp
 *   - SDK/NIKON_ShadowTrackerExtra_classes.hpp
 *   - SDK/NIKON_slua_unreal_classes.hpp
 *
 * Class sizes from SDK comments: "// 0xNEW (0xTOTAL - 0xBASE)"
 *
 * Inheritance chain (with total sizes):
 *   UObject(0x28) → AActor(0x4B0) → APawn(0x510) → ACharacter(0x8F0)
 *     → AUAECharacter(0xAE0) → ASTExtraCharacter(0x12C0) → ASTExtraBaseCharacter(0x44D0)
 *   
 *   UObject(0x28) → AActor(0x4B0) → AInfo(0x4B0) → APlayerState(0x540)
 *     → ALuaPlayerState(0x5F8) → AUAEPlayerState(0x13F0) → ASTExtraPlayerState(0x1CC8)
 *   
 *   UObject(0x28) → AActor(0x4B0) → AController(0x518) → APlayerController(0x828)
 *   
 *   UObject(0x28) → UWorld(0xE38)
 *   UObject(0x28) → UGameInstance(0x2E8)
 *   UObject(0x28) → UPlayer(0x48) → ULocalPlayer(0x290)
 *   
 *   UObject(0x28) → AActor(0x4B0) → AInfo(0x4B0) → AGameStateBase(0x4F0)
 *   
 *   UObject(0x28) → AActor(0x4B0) → APlayerCameraManager(0x1D70)
 *   
 *   UObject(0x28) → UActorComponent(0x178) → USceneComponent(0x3A0)
 *     → UPrimitiveComponent(0x940) → UMeshComponent(0x9A0)
 *     → USkinnedMeshComponent(0xBB0) → USkeletalMeshComponent(0x1280)
 */

#include <cstdint>
#include <cstring>

// ============================================================================
// MATH TYPES (UE4 standard layout)
// ============================================================================

struct FVector {
    float X, Y, Z;
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    FVector operator-(const FVector& v) const { return {X-v.X, Y-v.Y, Z-v.Z}; }
    FVector operator+(const FVector& v) const { return {X+v.X, Y+v.Y, Z+v.Z}; }
    FVector operator*(float s) const { return {X*s, Y*s, Z*s}; }
    float Size() const { return sqrtf(X*X + Y*Y + Z*Z); }
    float SizeSquared() const { return X*X + Y*Y + Z*Z; }
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

struct FQuat {
    float X, Y, Z, W;
};

// FTransform in UE4 ARM64: Quat(16) + Translation(12) + pad(4) + Scale(12) + pad(4) = 48 bytes (0x30)
struct FTransform {
    FQuat   Rotation;       // 0x00
    FVector Translation;    // 0x10
    float   pad0;           // 0x1C
    FVector Scale3D;        // 0x20
    float   pad1;           // 0x2C
};
static_assert(sizeof(FTransform) == 0x30, "FTransform must be 48 bytes");

// ============================================================================
// TArray (from SDK/NIKON_Basic.hpp)
// ============================================================================

template<class T>
struct TArray {
    T*      Data;
    int32_t Count;
    int32_t Max;
    
    bool IsValidIndex(int32_t i) const { return i >= 0 && i < Count; }
    T& operator[](int32_t i) { return Data[i]; }
    const T& operator[](int32_t i) const { return Data[i]; }
    int32_t Num() const { return Count; }
};

// ============================================================================
// FMinimalViewInfo (from SDK/NIKON_Engine_structs.hpp line 5890)
// Size: 0x05B0
// ============================================================================

struct FMinimalViewInfo {
    FVector  Location;           // 0x0000
    FVector  LocationLocalSpace; // 0x000C
    FRotator Rotation;           // 0x0018
    float    FOV;                // 0x0024
    float    OrthoWidth;         // 0x0028
    float    OrthoNearClipPlane; // 0x002C
    float    OrthoFarClipPlane;  // 0x0030
    float    AspectRatio;        // 0x0034
    char     pad[0x578];         // 0x0038 to 0x05B0
};
static_assert(sizeof(FMinimalViewInfo) == 0x05B0, "FMinimalViewInfo size mismatch");

// ============================================================================
// FCameraCacheEntry (from SDK/NIKON_Engine_structs.hpp line 6392)
// Size: 0x05C0
// ============================================================================

struct FCameraCacheEntry {
    float            Timestamp;  // 0x0000
    char             pad0[0x0C]; // 0x0004 to 0x0010
    FMinimalViewInfo POV;        // 0x0010 (size 0x05B0)
};
static_assert(sizeof(FCameraCacheEntry) == 0x05C0, "FCameraCacheEntry size mismatch");

// ============================================================================
// USceneComponent (base 0x178, total 0x3A0)
// ComponentToWorld at 0x01C0 from SDK/NIKON_Engine_classes.hpp line 425
// ============================================================================

class USceneComponent {
public:
    char          pad_000[0x01C0];            // 0x0000 to 0x01C0
    FTransform    ComponentToWorld;           // 0x01C0 (FTransform, 0x30 bytes)
    char          pad_1F0[0x3A0 - 0x01F0];   // rest to end
};

// ============================================================================
// USkeletalMeshComponent (total 0x1280)
// CachedBoneSpaceTransforms (TArray<FTransform>) at 0x0C40
// ============================================================================

class USkeletalMeshComponent {
public:
    char                   pad_000[0x01C0];
    FTransform             ComponentToWorld;          // 0x01C0
    char                   pad_1F0[0x0C40 - 0x01F0];
    TArray<FTransform>     CachedBoneSpaceTransforms; // 0x0C40
    char                   pad_C50[0x1280 - 0x0C50]; // (TArray is 16 bytes on 64-bit)
};

// ============================================================================
// AActor (base 0x28, total 0x4B0)
// RootComponent at 0x01A0 from SDK
// ============================================================================

class AActor {
public:
    char              pad_000[0x01A0];        // 0x0000 to 0x01A0
    USceneComponent*  RootComponent;          // 0x01A0
    char              pad_1A8[0x04B0 - 0x01A8]; // rest
};

// ============================================================================
// APawn (base 0x4B0, total 0x510)
// PlayerState at 0x04D0 (from AController, but APawn has it too in Engine)
// ============================================================================

class APawn : public AActor {
public:
    char   pad_4B0[0x0510 - 0x04B0]; // APawn adds 0x60 bytes
};

// ============================================================================
// ACharacter (base 0x510, total 0x8F0)
// Mesh at 0x0510, CharacterMovement at 0x0518, CapsuleComponent at 0x0520
// ============================================================================

class ACharacter : public APawn {
public:
    USkeletalMeshComponent* Mesh;               // 0x0510
    void*                   CharacterMovement;  // 0x0518
    void*                   CapsuleComponent;   // 0x0520
    char                    pad_528[0x08F0 - 0x0528]; // rest
};

// ============================================================================
// AUAECharacter (base 0x8F0, total 0xAE0)
// TeamID at 0x0998, PlayerKey at 0x0980, PlayerName at 0x0960
// From SDK/NIKON_Gameplay_classes.hpp line 15
// ============================================================================

class AUAECharacter : public ACharacter {
public:
    char     pad_8F0[0x0960 - 0x08F0];      // 0x08F0 to 0x0960
    // FString PlayerName at 0x0960 (skip, 0x10 bytes)
    char     playerName_pad[0x10];            // 0x0960
    // FString Nation at 0x0970 (skip)
    char     nation_pad[0x10];                // 0x0970
    uint32_t PlayerKey;                       // 0x0980
    char     pad_984[0x04];                   // 0x0984
    // FString PlayerUID at 0x0988
    char     playerUID_pad[0x10];             // 0x0988
    int32_t  TeamID;                          // 0x0998
    char     pad_99C[0x0AE0 - 0x099C];       // rest to end
};

// ============================================================================
// ASTExtraCharacter (base 0xAE0, total 0x12C0)
// Health at 0x0E60, HealthMax at 0x0E64, bDead at 0x0E7C bit 0
// From SDK/NIKON_ShadowTrackerExtra_classes.hpp line 6596
// ============================================================================

class ASTExtraCharacter : public AUAECharacter {
public:
    char     pad_AE0[0x0E60 - 0x0AE0];      // 0x0AE0 to 0x0E60
    float    Health;                          // 0x0E60
    float    HealthMax;                       // 0x0E64
    char     pad_E68[0x0E7C - 0x0E68];       // 0x0E68 to 0x0E7C
    uint8_t  bDeadBitfield;                   // 0x0E7C (bit 0 = bDead)
    char     pad_E7D[0x12C0 - 0x0E7D];       // rest

    bool IsDead() const { return (bDeadBitfield & 0x01) != 0; }
};

// ============================================================================
// ASTExtraBaseCharacter (base 0x12C0, total 0x44D0)
// Inherits Health, TeamID from parents
// From SDK/NIKON_ShadowTrackerExtra_classes.hpp line 49277
// ============================================================================

class ASTExtraBaseCharacter : public ASTExtraCharacter {
public:
    char     pad_12C0[0x44D0 - 0x12C0];     // 0x12C0 to end
};

// ============================================================================
// AController (base 0x4B0, total 0x518)
// Pawn at 0x04B8, Character at 0x04C8, PlayerState at 0x04D0
// ControlRotation at 0x04E0
// ============================================================================

class AController : public AActor {
public:
    char         pad_4B0[0x08];              // 0x04B0 UnknownData
    APawn*       Pawn;                       // 0x04B8
    char         pad_4C0[0x08];              // 0x04C0 UnknownData
    ACharacter*  Character;                  // 0x04C8
    void*        PlayerState;                // 0x04D0
    void*        TransformComponent;         // 0x04D8
    FRotator     ControlRotation;            // 0x04E0
    char         pad_4EC[0x0518 - 0x04EC];   // rest
};

// ============================================================================
// APlayerController (base 0x518, total 0x828)
// Player at 0x0518, AcknowledgedPawn at 0x0528, MyHUD at 0x0540
// PlayerCameraManager at 0x0548
// ============================================================================

class APlayerController : public AController {
public:
    void*  Player;                           // 0x0518 (UPlayer*)
    char   pad_520[0x08];                    // 0x0520 UnknownData
    APawn* AcknowledgedPawn;                 // 0x0528
    char   pad_530[0x10];                    // 0x0530 to 0x0540
    void*  MyHUD;                            // 0x0540
    void*  PlayerCameraManager;              // 0x0548 (APlayerCameraManager*)
    char   pad_550[0x0828 - 0x0550];         // rest
};

// ============================================================================
// APlayerCameraManager (base 0x4B0, total 0x1D70)
// CameraCache (FCameraCacheEntry) at 0x0520
// LastFrameCameraCache at 0x0AE0
// ============================================================================

class APlayerCameraManager : public AActor {
public:
    char              pad_4B0[0x0520 - 0x04B0]; // 0x04B0 to 0x0520
    FCameraCacheEntry CameraCache;              // 0x0520 (size 0x05C0)
    // CameraCache.POV.Location = this + 0x0520 + 0x10 + 0x00 = this + 0x0530
    // CameraCache.POV.Rotation = this + 0x0520 + 0x10 + 0x18 = this + 0x0548
    // CameraCache.POV.FOV      = this + 0x0520 + 0x10 + 0x24 = this + 0x0554
    char              pad_AE0[0x1D70 - 0x0AE0]; // rest
};

// ============================================================================
// AGameStateBase (base 0x4B0, total 0x4F0)
// PlayerArray (TArray<APlayerState*>) at 0x04C8
// ============================================================================

class AGameStateBase : public AActor {
public:
    char                    pad_4B0[0x04C8 - 0x04B0]; // 0x04B0 to 0x04C8
    TArray<void*>           PlayerArray;               // 0x04C8 (TArray<APlayerState*>)
    char                    pad_4D8[0x04F0 - 0x04D8]; // rest
};

// ============================================================================
// ASTExtraPlayerState (base 0x13F0, total 0x1CC8)
// LiveState at 0x13F4, CharacterOwner at 0x1410
// PlayerHealth at 0x142C, PlayerHealthMax at 0x1430
// ============================================================================

class ASTExtraPlayerState {
public:
    char                     pad_000[0x13F4];         // 0x0000 to 0x13F4
    uint8_t                  LiveState;               // 0x13F4 (ExtraPlayerLiveState enum)
    char                     pad_13F5[0x1B];          // 0x13F5 to 0x1410
    ASTExtraBaseCharacter*   CharacterOwner;          // 0x1410
    void*                    SpectatingCharacterOwner;// 0x1418
    char                     pad_1420[0x0C];          // 0x1420 to 0x142C
    float                    PlayerHealth;            // 0x142C
    float                    PlayerHealthMax;         // 0x1430
    char                     pad_1434[0x1CC8 - 0x1434]; // rest
};

// ============================================================================
// UWorld (base 0x28, total 0xE38)
// PersistentLevel at 0x0030, GameState at 0x0428
// OwningGameInstance at 0x0470
// ============================================================================

class UWorld {
public:
    char               pad_000[0x0030];            // 0x0000 to 0x0030
    void*              PersistentLevel;            // 0x0030
    char               pad_038[0x0428 - 0x0038];  // 0x0038 to 0x0428
    AGameStateBase*    GameState;                  // 0x0428
    char               pad_430[0x0470 - 0x0430];  // 0x0430 to 0x0470
    void*              OwningGameInstance;         // 0x0470 (UGameInstance*)
    char               pad_478[0x0E38 - 0x0478];  // rest
};

// ============================================================================
// UGameInstance (base 0x28, total 0x2E8)
// LocalPlayers (TArray<ULocalPlayer*>) at 0x0048
// ============================================================================

class UGameInstance {
public:
    char              pad_000[0x0048];             // 0x0000 to 0x0048
    TArray<void*>     LocalPlayers;               // 0x0048 (TArray<ULocalPlayer*>)
    char              pad_058[0x02E8 - 0x0058];   // rest
};

// ============================================================================
// ULocalPlayer (UPlayer base at 0x28→0x48, ULocalPlayer total 0x290)
// UPlayer::PlayerController at 0x0030
// ============================================================================

class ULocalPlayer {
public:
    char                pad_000[0x0030];           // 0x0000 to 0x0030
    APlayerController*  PlayerController;          // 0x0030
    char                pad_038[0x0290 - 0x0038];  // rest
};

// ============================================================================
// BONE INDICES (UE4 standard humanoid skeleton)
// ============================================================================

enum EBoneIndex : int32_t {
    BONE_ROOT          = 0,
    BONE_PELVIS        = 1,
    BONE_SPINE_01      = 2,
    BONE_SPINE_02      = 3,
    BONE_SPINE_03      = 4,
    BONE_NECK          = 5,
    BONE_HEAD          = 6,
    BONE_LEFT_SHOULDER = 11,
    BONE_LEFT_ELBOW    = 12,
    BONE_LEFT_HAND     = 13,
    BONE_RIGHT_SHOULDER= 32,
    BONE_RIGHT_ELBOW   = 33,
    BONE_RIGHT_HAND    = 34,
    BONE_LEFT_THIGH    = 49,
    BONE_LEFT_KNEE     = 50,
    BONE_LEFT_FOOT     = 51,
    BONE_RIGHT_THIGH   = 55,
    BONE_RIGHT_KNEE    = 56,
    BONE_RIGHT_FOOT    = 57,
};

// Skeleton pairs for drawing bone lines
static const int SKELETON_CONNECTIONS[][2] = {
    {BONE_HEAD, BONE_NECK},
    {BONE_NECK, BONE_SPINE_03},
    {BONE_SPINE_03, BONE_SPINE_02},
    {BONE_SPINE_02, BONE_SPINE_01},
    {BONE_SPINE_01, BONE_PELVIS},
    // Left arm
    {BONE_NECK, BONE_LEFT_SHOULDER},
    {BONE_LEFT_SHOULDER, BONE_LEFT_ELBOW},
    {BONE_LEFT_ELBOW, BONE_LEFT_HAND},
    // Right arm
    {BONE_NECK, BONE_RIGHT_SHOULDER},
    {BONE_RIGHT_SHOULDER, BONE_RIGHT_ELBOW},
    {BONE_RIGHT_ELBOW, BONE_RIGHT_HAND},
    // Left leg
    {BONE_PELVIS, BONE_LEFT_THIGH},
    {BONE_LEFT_THIGH, BONE_LEFT_KNEE},
    {BONE_LEFT_KNEE, BONE_LEFT_FOOT},
    // Right leg
    {BONE_PELVIS, BONE_RIGHT_THIGH},
    {BONE_RIGHT_THIGH, BONE_RIGHT_KNEE},
    {BONE_RIGHT_KNEE, BONE_RIGHT_FOOT},
};
#define SKELETON_PAIR_COUNT 17
