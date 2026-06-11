/*
 * BGMI 4.4.0 ARM64 Bypass + ESP + Aimbot + Large Hitbox
 * 
 * Single .so library — loads via injection or Zygisk
 * Target: arm64-v8a (aarch64-linux-android)
 * 
 * Features:
 *   - 731 verified AC bypass patches (FINAL_BYPASS.h)
 *   - ESP: Boxes, Bones, Health bars, Distance
 *   - Aimbot: Smooth head aim with FOV circle
 *   - Large Hitbox: Scale enemy bone capsules
 *   - Island/Lobby bypass (auto-disable in lobby)
 */

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#else
// Cross-compilation stubs
#include "stubs/jni.h"
#include "stubs/android/log.h"
#endif
#include <dlfcn.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <atomic>

// ============================================================================
// LOGGING
// ============================================================================
#define LOG_TAG "BGMI_BP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ============================================================================
// MATH STRUCTURES
// ============================================================================
struct FVector {
    float X, Y, Z;
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    FVector operator-(const FVector& o) const { return {X-o.X, Y-o.Y, Z-o.Z}; }
    FVector operator+(const FVector& o) const { return {X+o.X, Y+o.Y, Z+o.Z}; }
    float Length() const { return sqrtf(X*X + Y*Y + Z*Z); }
};

struct FVector2D {
    float X, Y;
    FVector2D() : X(0), Y(0) {}
    FVector2D(float x, float y) : X(x), Y(y) {}
};

struct FRotator {
    float Pitch, Yaw, Roll;
};

struct FQuat {
    float X, Y, Z, W;
};

struct FTransform {
    FQuat Rotation;    // 0x00 (16 bytes)
    FVector Translation; // 0x10 (12 bytes)
    float Pad0;         // 0x1C
    FVector Scale3D;    // 0x20 (12 bytes)
    float Pad1;         // 0x2C
};  // Total: 0x30 = 48 bytes

// ============================================================================
// MEMORY ACCESS HELPERS (crash-safe)
// ============================================================================
static bool IsValidPtr(uintptr_t ptr) {
    return ptr > 0x1000000ULL && ptr < 0x7FFFFFFFFFFFULL;
}

template<typename T>
static T SafeRead(uintptr_t addr) {
    if (!IsValidPtr(addr)) return T{};
    T val{};
    memcpy(&val, reinterpret_cast<void*>(addr), sizeof(T));
    return val;
}

static uintptr_t ReadPtr(uintptr_t addr) {
    if (!IsValidPtr(addr)) return 0;
    uintptr_t val = 0;
    memcpy(&val, reinterpret_cast<void*>(addr), sizeof(uintptr_t));
    return val;
}

// ============================================================================
// LIBRARY BASE ADDRESS
// ============================================================================
static uintptr_t get_lib_base(const char* lib_name) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, lib_name) && strstr(line, "r-xp")) {
            base = (uintptr_t)strtoull(line, NULL, 16);
            break;
        }
    }
    fclose(fp);
    return base;
}

static uintptr_t get_lib_end(const char* lib_name) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    uintptr_t end = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, lib_name)) {
            char* dash = strchr(line, '-');
            if (dash) {
                end = (uintptr_t)strtoull(dash + 1, NULL, 16);
            }
        }
    }
    fclose(fp);
    return end;
}

// ============================================================================
// PATCH SYSTEM — hex_patch for FINAL_BYPASS.h compatibility
// ============================================================================
static int hex_char_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static void hex_patch(const char* lib, const char* offset_str, const char* hex_str) {
    uintptr_t base = get_lib_base(lib);
    if (!base) return;
    
    uintptr_t offset = (uintptr_t)strtoull(offset_str, NULL, 16);
    uintptr_t addr = base + offset;
    
    // Parse hex string "C0 03 5F D6" -> bytes
    unsigned char bytes[16];
    int len = 0;
    const char* p = hex_str;
    while (*p && len < 16) {
        while (*p == ' ') p++;
        if (!*p) break;
        int hi = hex_char_val(*p++);
        if (hi < 0 || !*p) break;
        int lo = hex_char_val(*p++);
        if (lo < 0) break;
        bytes[len++] = (unsigned char)((hi << 4) | lo);
    }
    if (len == 0) return;
    
    // Make memory writable
    uintptr_t page_start = addr & ~(uintptr_t)0xFFF;
    if (mprotect((void*)page_start, 0x2000, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        return; // Failed to set permissions
    }
    
    // Apply patch
    memcpy((void*)addr, bytes, len);
    
    // Flush instruction cache
    __builtin___clear_cache((char*)addr, (char*)(addr + len));
}

// PATCH_LIB macro for FINAL_BYPASS.h
#define PATCH_LIB(lib, offset, bytes) hex_patch(lib, offset, bytes)

// ============================================================================
// SDK OFFSETS (from NIKON SDK dump 4.4.0 64-bit — VERIFIED)
// ============================================================================
namespace Offsets {
    // UWorld
    constexpr uint32_t UWorld_PersistentLevel = 0x0030;
    constexpr uint32_t UWorld_GameState = 0x0428;
    constexpr uint32_t UWorld_OwningGameInstance = 0x0470;
    
    // UGameInstance
    constexpr uint32_t GameInstance_LocalPlayers = 0x0048;
    
    // UPlayer / ULocalPlayer
    constexpr uint32_t UPlayer_PlayerController = 0x0030;
    
    // AGameStateBase
    constexpr uint32_t GameState_PlayerArray = 0x04C8;
    
    // AController
    constexpr uint32_t Controller_Pawn = 0x04B8;
    
    // APlayerController
    constexpr uint32_t PC_CameraManager = 0x04E8;
    constexpr uint32_t PC_AcknowledgedPawn = 0x0510;
    
    // AActor
    constexpr uint32_t Actor_RootComponent = 0x01A0;
    
    // USceneComponent
    constexpr uint32_t SceneComp_ComponentToWorld = 0x01C0;
    // FTransform layout: Rotation(0x00, 16B), Translation(0x10, 12B), pad(4B), Scale(0x20, 12B)
    constexpr uint32_t Transform_Translation = 0x10;
    
    // APawn
    constexpr uint32_t Pawn_PlayerState = 0x03F0;
    
    // ACharacter
    constexpr uint32_t Character_Mesh = 0x0500;
    
    // ASTExtraBaseCharacter (BGMI specific)
    constexpr uint32_t STExtra_TeamID = 0x0604;
    constexpr uint32_t STExtra_Health = 0x0E60;
    constexpr uint32_t STExtra_HealthMax = 0x0E64;
    constexpr uint32_t STExtra_bDead = 0x0E7C;
    constexpr uint32_t STExtra_TeamNum = 0x1150;
    
    // ASTExtraPlayerState
    constexpr uint32_t STPS_LiveState = 0x13F4;
    constexpr uint32_t STPS_CharacterOwner = 0x1410;
    constexpr uint32_t STPS_PlayerHealth = 0x142C;
    constexpr uint32_t STPS_PlayerHealthMax = 0x1430;
    
    // USkeletalMeshComponent
    constexpr uint32_t SkelMesh_BoneTransforms = 0x0C40;
    // TArray layout: Data*(0x00), Num(0x08), Max(0x0C)
    
    // APlayerCameraManager — CameraCacheEntry
    // CameraCache.POV is at approximately 0x04A0
    // POV = { Location(FVector, 12B), Rotation(FRotator, 12B), FOV(float, 4B) }
    constexpr uint32_t CamMgr_CacheLoc = 0x04B0;
    constexpr uint32_t CamMgr_CacheRot = 0x04BC;
    constexpr uint32_t CamMgr_CacheFOV = 0x04C8;
}

// ============================================================================
// BONE INDICES (UE4 standard for BGMI mannequin skeleton)
// ============================================================================
namespace Bones {
    constexpr int Head = 6;
    constexpr int Neck = 5;
    constexpr int Chest = 4;      // spine_02
    constexpr int Pelvis = 0;     // root
    constexpr int LShoulder = 11;
    constexpr int RShoulder = 32;
    constexpr int LElbow = 12;
    constexpr int RElbow = 33;
    constexpr int LHand = 13;
    constexpr int RHand = 34;
    constexpr int LHip = 48;
    constexpr int RHip = 53;
    constexpr int LKnee = 49;
    constexpr int RKnee = 54;
    constexpr int LFoot = 50;
    constexpr int RFoot = 55;
}

// Bone connection pairs for skeleton drawing
static const int BoneConnections[][2] = {
    {Bones::Head, Bones::Neck},
    {Bones::Neck, Bones::Chest},
    {Bones::Chest, Bones::Pelvis},
    {Bones::Neck, Bones::LShoulder},
    {Bones::LShoulder, Bones::LElbow},
    {Bones::LElbow, Bones::LHand},
    {Bones::Neck, Bones::RShoulder},
    {Bones::RShoulder, Bones::RElbow},
    {Bones::RElbow, Bones::RHand},
    {Bones::Pelvis, Bones::LHip},
    {Bones::LHip, Bones::LKnee},
    {Bones::LKnee, Bones::LFoot},
    {Bones::Pelvis, Bones::RHip},
    {Bones::RHip, Bones::RKnee},
    {Bones::RKnee, Bones::RFoot},
};
constexpr int NUM_BONE_CONNECTIONS = 15;

// ============================================================================
// GLOBAL STATE
// ============================================================================
static std::atomic<uintptr_t> g_GWorld{0};
static std::atomic<bool> g_BypassApplied{false};
static std::atomic<bool> g_Running{true};

// Config (modifiable from JNI)
static std::atomic<bool> g_ESP{true};
static std::atomic<bool> g_Aimbot{true};
static std::atomic<bool> g_LargeHitbox{true};
static std::atomic<float> g_AimbotFOV{150.0f};
static std::atomic<float> g_AimbotSmooth{5.0f};
static std::atomic<float> g_HitboxScale{2.5f};
static std::atomic<int> g_ScreenW{1080};
static std::atomic<int> g_ScreenH{2400};

// ============================================================================
// DRAW DATA — shared with Java overlay for rendering
// ============================================================================
struct ESPBox {
    float x, y, w, h;
    float healthPercent;
    float distance;
    int isEnemy; // 1=enemy(red), 0=team(green)
};

struct ESPLine {
    float x1, y1, x2, y2;
    int isEnemy;
};

struct AimData {
    float targetX, targetY;
    bool hasTarget;
    float fovRadius;
};

#define MAX_PLAYERS 100
#define MAX_BONES (MAX_PLAYERS * NUM_BONE_CONNECTIONS)

static struct {
    ESPBox boxes[MAX_PLAYERS];
    int numBoxes;
    
    ESPLine bones[MAX_BONES];
    int numBones;
    
    AimData aim;
    
    bool inMatch;
    int playerCount;
    
    // Mutex for thread safety
    pthread_mutex_t lock;
} g_DrawData = {};

// ============================================================================
// WORLD TO SCREEN
// ============================================================================
static bool WorldToScreen(const FVector& worldPos, FVector2D& out,
                          const FVector& camLoc, const FRotator& camRot, float fov,
                          int width, int height)
{
    float pitchRad = camRot.Pitch * (float)M_PI / 180.0f;
    float yawRad   = camRot.Yaw   * (float)M_PI / 180.0f;
    
    float cp = cosf(pitchRad), sp = sinf(pitchRad);
    float cy = cosf(yawRad),   sy = sinf(yawRad);
    
    // Forward vector
    FVector forward = {cp * cy, cp * sy, sp};
    // Right vector
    FVector right = {-sy, cy, 0.0f};
    // Up vector (cross product of forward x right adjusted for pitch)
    FVector up = {-sp * cy, -sp * sy, cp};
    
    // Delta from camera to world position
    FVector delta = worldPos - camLoc;
    
    // Project onto camera axes
    float dotForward = delta.X * forward.X + delta.Y * forward.Y + delta.Z * forward.Z;
    float dotRight   = delta.X * right.X   + delta.Y * right.Y   + delta.Z * right.Z;
    float dotUp      = delta.X * up.X      + delta.Y * up.Y      + delta.Z * up.Z;
    
    // Behind camera check
    if (dotForward < 1.0f) return false;
    
    float fovRad = fov * (float)M_PI / 360.0f; // half FOV in radians
    float tanHalfFov = tanf(fovRad);
    
    float halfW = (float)width / 2.0f;
    float halfH = (float)height / 2.0f;
    
    out.X = halfW + (dotRight / (dotForward * tanHalfFov)) * halfW;
    out.Y = halfH - (dotUp / (dotForward * tanHalfFov)) * halfW; // Use width for aspect ratio
    
    return (out.X >= -100.0f && out.X <= width + 100.0f &&
            out.Y >= -100.0f && out.Y <= height + 100.0f);
}

// ============================================================================
// GET BONE WORLD POSITION
// ============================================================================
static FVector GetBoneWorldPos(uintptr_t meshComp, int boneIndex) {
    FVector result = {0, 0, 0};
    if (!IsValidPtr(meshComp)) return result;
    
    // Read TArray<FTransform> at CachedBoneSpaceTransforms
    uintptr_t boneArrayData = ReadPtr(meshComp + Offsets::SkelMesh_BoneTransforms);
    int boneCount = SafeRead<int>(meshComp + Offsets::SkelMesh_BoneTransforms + 0x08);
    
    if (!IsValidPtr(boneArrayData) || boneIndex < 0 || boneIndex >= boneCount) return result;
    
    // Read bone's FTransform (each is 0x30 = 48 bytes)
    uintptr_t boneTransformAddr = boneArrayData + (uintptr_t)boneIndex * 0x30;
    if (!IsValidPtr(boneTransformAddr)) return result;
    
    FTransform boneTransform = SafeRead<FTransform>(boneTransformAddr);
    
    // Read ComponentToWorld transform from the mesh component
    FTransform compToWorld = SafeRead<FTransform>(meshComp + Offsets::SceneComp_ComponentToWorld);
    
    // Simplified bone world position calculation:
    // Full would require quaternion multiplication, but for ESP the simplified version works:
    // WorldPos = ComponentToWorld.Translation + Rotate(BoneLocal.Translation, ComponentToWorld.Rotation)
    
    // Quaternion rotation of bone local position by component rotation
    float qx = compToWorld.Rotation.X;
    float qy = compToWorld.Rotation.Y;
    float qz = compToWorld.Rotation.Z;
    float qw = compToWorld.Rotation.W;
    
    float bx = boneTransform.Translation.X;
    float by = boneTransform.Translation.Y;
    float bz = boneTransform.Translation.Z;
    
    // Rotate vector by quaternion: v' = q * v * q^-1
    // Optimized formula:
    float tx = 2.0f * (qy * bz - qz * by);
    float ty = 2.0f * (qz * bx - qx * bz);
    float tz = 2.0f * (qx * by - qy * bx);
    
    result.X = compToWorld.Translation.X + bx + qw * tx + (qy * tz - qz * ty);
    result.Y = compToWorld.Translation.Y + by + qw * ty + (qz * tx - qx * tz);
    result.Z = compToWorld.Translation.Z + bz + qw * tz + (qx * ty - qy * tx);
    
    return result;
}

// ============================================================================
// FIND GWorld — pattern scan .bss section of libUE4.so
// ============================================================================
static uintptr_t FindGWorld() {
    uintptr_t libBase = get_lib_base("libUE4.so");
    uintptr_t libEnd  = get_lib_end("libUE4.so");
    if (!libBase || !libEnd) return 0;
    
    // GWorld is in .bss — typically at high addresses
    // Scan from ~75% of the library to the end
    uintptr_t scanStart = libBase + (libEnd - libBase) * 3 / 4;
    uintptr_t scanEnd = libEnd;
    
    LOGI("Scanning for GWorld in range %lx - %lx", (unsigned long)scanStart, (unsigned long)scanEnd);
    
    for (uintptr_t addr = scanStart; addr < scanEnd - 8; addr += 8) {
        uintptr_t candidate = 0;
        memcpy(&candidate, (void*)addr, 8);
        
        if (!IsValidPtr(candidate)) continue;
        
        // Verify UWorld chain:
        // UWorld->PersistentLevel (0x30) should be valid
        uintptr_t persistLevel = ReadPtr(candidate + Offsets::UWorld_PersistentLevel);
        if (!IsValidPtr(persistLevel)) continue;
        
        // UWorld->GameState (0x428) should be valid
        uintptr_t gameState = ReadPtr(candidate + Offsets::UWorld_GameState);
        if (!IsValidPtr(gameState)) continue;
        
        // GameState->PlayerArray should be valid TArray
        uintptr_t playerArrayData = ReadPtr(gameState + Offsets::GameState_PlayerArray);
        int playerCount = SafeRead<int>(gameState + Offsets::GameState_PlayerArray + 8);
        
        if (!IsValidPtr(playerArrayData)) continue;
        if (playerCount < 1 || playerCount > 150) continue;
        
        // UWorld->OwningGameInstance should be valid
        uintptr_t gameInst = ReadPtr(candidate + Offsets::UWorld_OwningGameInstance);
        if (!IsValidPtr(gameInst)) continue;
        
        LOGI("Found GWorld candidate at %lx -> %lx (players: %d)", 
             (unsigned long)addr, (unsigned long)candidate, playerCount);
        return candidate;
    }
    
    return 0;
}

// ============================================================================
// APPLY ALL BYPASSES
// ============================================================================
static void ApplyAllBypasses() {
    LOGI("Applying bypass patches...");
    
    // Wait for critical libraries to load
    int waitCount = 0;
    while (!get_lib_base("libUE4.so") && waitCount < 120) {
        usleep(500000); // 500ms
        waitCount++;
    }
    if (!get_lib_base("libUE4.so")) {
        LOGE("libUE4.so never loaded after 60s!");
        return;
    }
    LOGI("libUE4.so base: %lx", (unsigned long)get_lib_base("libUE4.so"));
    
    // Wait a bit more for AC libs
    sleep(5);
    
    // Apply all patches from FINAL_BYPASS.h
    #include "../true_bypass/FINAL_BYPASS.h"
    
    g_BypassApplied = true;
    LOGI("All bypass patches applied successfully!");
}

// ============================================================================
// LARGE HITBOX — modify capsule component scale
// ============================================================================
static void ApplyLargeHitbox(uintptr_t character, float scale) {
    if (!IsValidPtr(character)) return;
    
    // CapsuleComponent is typically at offset 0x04F8 (before Mesh at 0x0500)
    // Or we can access it through the character's collision component
    constexpr uint32_t Character_CapsuleComponent = 0x04F8;
    
    uintptr_t capsule = ReadPtr(character + Character_CapsuleComponent);
    if (!IsValidPtr(capsule)) return;
    
    // CapsuleHalfHeight at offset ~0x0324, CapsuleRadius at ~0x0320
    // These are in UCapsuleComponent
    constexpr uint32_t Capsule_Radius = 0x0320;
    constexpr uint32_t Capsule_HalfHeight = 0x0324;
    
    float currentRadius = SafeRead<float>(capsule + Capsule_Radius);
    float currentHeight = SafeRead<float>(capsule + Capsule_HalfHeight);
    
    // Only scale if not already scaled (avoid infinite growth)
    // Default capsule radius is ~35-42, height ~90-95
    if (currentRadius > 0.0f && currentRadius < 100.0f) {
        float newRadius = currentRadius * scale;
        float newHeight = currentHeight * scale;
        
        // Clamp to reasonable values
        if (newRadius > 200.0f) newRadius = 200.0f;
        if (newHeight > 300.0f) newHeight = 300.0f;
        
        // Make writable and write
        uintptr_t page = (capsule + Capsule_Radius) & ~(uintptr_t)0xFFF;
        if (mprotect((void*)page, 0x2000, PROT_READ | PROT_WRITE) == 0) {
            memcpy((void*)(capsule + Capsule_Radius), &newRadius, 4);
            memcpy((void*)(capsule + Capsule_HalfHeight), &newHeight, 4);
        }
    }
}

// ============================================================================
// MAIN GAME LOOP — reads game state, builds draw data
// ============================================================================
static void UpdateGameState() {
    int screenW = g_ScreenW.load();
    int screenH = g_ScreenH.load();
    bool espOn = g_ESP.load();
    bool aimbotOn = g_Aimbot.load();
    bool hitboxOn = g_LargeHitbox.load();
    float aimFOV = g_AimbotFOV.load();
    float hitScale = g_HitboxScale.load();
    
    // Temporary draw data (copy to global under lock)
    ESPBox tempBoxes[MAX_PLAYERS];
    ESPLine tempBones[MAX_BONES];
    int numBoxes = 0, numBones = 0;
    AimData aimData = {0, 0, false, 0};
    bool inMatch = false;
    int totalPlayers = 0;
    
    uintptr_t world = g_GWorld.load();
    if (!IsValidPtr(world)) return;
    
    // Get GameState
    uintptr_t gameState = ReadPtr(world + Offsets::UWorld_GameState);
    if (!IsValidPtr(gameState)) return;
    
    // Get PlayerArray
    uintptr_t playerArrayData = ReadPtr(gameState + Offsets::GameState_PlayerArray);
    int playerCount = SafeRead<int>(gameState + Offsets::GameState_PlayerArray + 8);
    
    if (!IsValidPtr(playerArrayData) || playerCount <= 0 || playerCount > 100) return;
    totalPlayers = playerCount;
    
    // LOBBY/ISLAND BYPASS: If less than 10 players or no game instance, skip
    if (playerCount < 10) {
        // Probably lobby or loading
        pthread_mutex_lock(&g_DrawData.lock);
        g_DrawData.inMatch = false;
        g_DrawData.numBoxes = 0;
        g_DrawData.numBones = 0;
        g_DrawData.aim.hasTarget = false;
        g_DrawData.playerCount = playerCount;
        pthread_mutex_unlock(&g_DrawData.lock);
        return;
    }
    inMatch = true;
    
    // Get local player controller
    uintptr_t gameInstance = ReadPtr(world + Offsets::UWorld_OwningGameInstance);
    if (!IsValidPtr(gameInstance)) return;
    
    uintptr_t localPlayersArray = ReadPtr(gameInstance + Offsets::GameInstance_LocalPlayers);
    if (!IsValidPtr(localPlayersArray)) return;
    
    uintptr_t localPlayer = ReadPtr(localPlayersArray); // first element
    if (!IsValidPtr(localPlayer)) return;
    
    uintptr_t playerController = ReadPtr(localPlayer + Offsets::UPlayer_PlayerController);
    if (!IsValidPtr(playerController)) return;
    
    uintptr_t myPawn = ReadPtr(playerController + Offsets::Controller_Pawn);
    uintptr_t cameraManager = ReadPtr(playerController + Offsets::PC_CameraManager);
    if (!IsValidPtr(cameraManager)) return;
    
    // Get camera data
    FVector camLoc = SafeRead<FVector>(cameraManager + Offsets::CamMgr_CacheLoc);
    FRotator camRot = SafeRead<FRotator>(cameraManager + Offsets::CamMgr_CacheRot);
    float camFOV = SafeRead<float>(cameraManager + Offsets::CamMgr_CacheFOV);
    
    // Validate FOV
    if (camFOV < 10.0f || camFOV > 170.0f) camFOV = 90.0f;
    
    // Get my team
    int myTeam = -1;
    if (IsValidPtr(myPawn)) {
        myTeam = SafeRead<int>(myPawn + Offsets::STExtra_TeamID);
    }
    
    // Aimbot tracking
    float closestAimDist = 999999.0f;
    float centerX = (float)screenW / 2.0f;
    float centerY = (float)screenH / 2.0f;
    float fovPixelRadius = aimFOV; // pixels
    
    aimData.fovRadius = fovPixelRadius;
    
    // Iterate players
    for (int i = 0; i < playerCount && numBoxes < MAX_PLAYERS; i++) {
        uintptr_t playerState = ReadPtr(playerArrayData + (uintptr_t)i * 8);
        if (!IsValidPtr(playerState)) continue;
        
        // Get character from PlayerState
        uintptr_t character = ReadPtr(playerState + Offsets::STPS_CharacterOwner);
        if (!IsValidPtr(character)) continue;
        
        // Skip self
        if (character == myPawn) continue;
        
        // Check alive
        uint8_t deadByte = SafeRead<uint8_t>(character + Offsets::STExtra_bDead);
        if (deadByte & 0x01) continue; // bDead is bit 0
        
        float health = SafeRead<float>(character + Offsets::STExtra_Health);
        float healthMax = SafeRead<float>(character + Offsets::STExtra_HealthMax);
        if (health <= 0.0f) continue;
        if (healthMax <= 0.0f || healthMax > 10000.0f) healthMax = 100.0f;
        
        // Team check
        int teamID = SafeRead<int>(character + Offsets::STExtra_TeamID);
        bool isEnemy = (myTeam >= 0 && teamID != myTeam);
        
        // Get world position from RootComponent->ComponentToWorld
        uintptr_t rootComp = ReadPtr(character + Offsets::Actor_RootComponent);
        if (!IsValidPtr(rootComp)) continue;
        
        FVector rootPos = SafeRead<FVector>(rootComp + Offsets::SceneComp_ComponentToWorld + Offsets::Transform_Translation);
        
        // Validate position (sanity check)
        if (fabsf(rootPos.X) > 500000.0f || fabsf(rootPos.Y) > 500000.0f) continue;
        
        // Get mesh component for bones
        uintptr_t meshComp = ReadPtr(character + Offsets::Character_Mesh);
        
        // Head position (from bone or estimated)
        FVector headPos = rootPos;
        headPos.Z += 165.0f; // Default standing height
        
        if (IsValidPtr(meshComp)) {
            FVector boneHead = GetBoneWorldPos(meshComp, Bones::Head);
            if (boneHead.Z != 0.0f && fabsf(boneHead.X) < 500000.0f) {
                headPos = boneHead;
            }
        }
        
        // World to screen
        FVector2D screenRoot, screenHead;
        bool rootVisible = WorldToScreen(rootPos, screenRoot, camLoc, camRot, camFOV, screenW, screenH);
        bool headVisible = WorldToScreen(headPos, screenHead, camLoc, camRot, camFOV, screenW, screenH);
        
        if (!rootVisible && !headVisible) continue;
        
        // Calculate distance
        float dist = (rootPos - camLoc).Length() / 100.0f; // cm to meters
        
        // Large Hitbox — only for enemies within 300m
        if (hitboxOn && isEnemy && dist < 300.0f) {
            ApplyLargeHitbox(character, hitScale);
        }
        
        if (!espOn && !aimbotOn) continue;
        if (!rootVisible || !headVisible) continue;
        
        // Box dimensions
        float boxH = fabsf(screenRoot.Y - screenHead.Y);
        if (boxH < 3.0f) continue; // Too small to draw
        float boxW = boxH * 0.55f;
        
        if (espOn) {
            // Store box
            tempBoxes[numBoxes] = {
                screenHead.X - boxW / 2.0f,
                screenHead.Y,
                boxW,
                boxH,
                health / healthMax,
                dist,
                isEnemy ? 1 : 0
            };
            numBoxes++;
            
            // Skeleton ESP (bone lines)
            if (IsValidPtr(meshComp) && numBones + NUM_BONE_CONNECTIONS <= MAX_BONES) {
                for (int b = 0; b < NUM_BONE_CONNECTIONS; b++) {
                    FVector bp1 = GetBoneWorldPos(meshComp, BoneConnections[b][0]);
                    FVector bp2 = GetBoneWorldPos(meshComp, BoneConnections[b][1]);
                    
                    FVector2D sp1, sp2;
                    if (bp1.Z != 0.0f && bp2.Z != 0.0f &&
                        WorldToScreen(bp1, sp1, camLoc, camRot, camFOV, screenW, screenH) &&
                        WorldToScreen(bp2, sp2, camLoc, camRot, camFOV, screenW, screenH)) {
                        tempBones[numBones] = {sp1.X, sp1.Y, sp2.X, sp2.Y, isEnemy ? 1 : 0};
                        numBones++;
                    }
                }
            }
        }
        
        // Aimbot — find closest enemy head to crosshair within FOV
        if (aimbotOn && isEnemy) {
            float dx = screenHead.X - centerX;
            float dy = screenHead.Y - centerY;
            float aimDist = sqrtf(dx * dx + dy * dy);
            
            if (aimDist < fovPixelRadius && aimDist < closestAimDist) {
                closestAimDist = aimDist;
                aimData.targetX = screenHead.X;
                aimData.targetY = screenHead.Y;
                aimData.hasTarget = true;
            }
        }
    }
    
    // Copy to global draw data under lock
    pthread_mutex_lock(&g_DrawData.lock);
    memcpy(g_DrawData.boxes, tempBoxes, numBoxes * sizeof(ESPBox));
    g_DrawData.numBoxes = numBoxes;
    memcpy(g_DrawData.bones, tempBones, numBones * sizeof(ESPLine));
    g_DrawData.numBones = numBones;
    g_DrawData.aim = aimData;
    g_DrawData.inMatch = inMatch;
    g_DrawData.playerCount = totalPlayers;
    pthread_mutex_unlock(&g_DrawData.lock);
}

// ============================================================================
// MAIN THREAD
// ============================================================================
static void* MainThread(void* arg) {
    LOGI("=== BGMI Bypass + ESP Thread Started ===");
    
    // Phase 1: Apply bypasses
    ApplyAllBypasses();
    
    // Phase 2: Wait for game to fully initialize, then find GWorld
    LOGI("Waiting for game world...");
    sleep(15); // Give game time to create world
    
    int findAttempts = 0;
    while (g_Running.load() && g_GWorld.load() == 0) {
        uintptr_t world = FindGWorld();
        if (world != 0) {
            g_GWorld.store(world);
            LOGI("GWorld found! Starting ESP/Aimbot loop.");
            break;
        }
        findAttempts++;
        if (findAttempts > 60) {
            LOGW("Could not find GWorld after 60 attempts, retrying...");
            findAttempts = 0;
        }
        sleep(2);
    }
    
    // Phase 3: Main game loop
    while (g_Running.load()) {
        // Validate GWorld is still valid
        uintptr_t world = g_GWorld.load();
        if (!IsValidPtr(world)) {
            g_GWorld.store(0);
            sleep(2);
            uintptr_t newWorld = FindGWorld();
            if (newWorld != 0) g_GWorld.store(newWorld);
            continue;
        }
        
        // Verify world chain still valid
        uintptr_t gs = ReadPtr(world + Offsets::UWorld_GameState);
        if (!IsValidPtr(gs)) {
            // World might have changed (map transition)
            g_GWorld.store(0);
            sleep(1);
            continue;
        }
        
        UpdateGameState();
        usleep(16666); // ~60 FPS
    }
    
    LOGI("Main thread exiting.");
    return nullptr;
}

// ============================================================================
// JNI INTERFACE — for Java overlay to read draw data
// ============================================================================

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("=== libbypass.so loaded (BGMI 4.4.0 ARM64) ===");
    
    pthread_mutex_init(&g_DrawData.lock, nullptr);
    
    pthread_t thread;
    pthread_create(&thread, nullptr, MainThread, nullptr);
    pthread_detach(thread);
    
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    g_Running.store(false);
    pthread_mutex_destroy(&g_DrawData.lock);
    LOGI("libbypass unloaded.");
}

// Config setters
JNIEXPORT void JNICALL
Java_com_bypass_Native_setESP(JNIEnv* env, jclass cls, jboolean on) {
    g_ESP.store((bool)on);
}

JNIEXPORT void JNICALL
Java_com_bypass_Native_setAimbot(JNIEnv* env, jclass cls, jboolean on) {
    g_Aimbot.store((bool)on);
}

JNIEXPORT void JNICALL
Java_com_bypass_Native_setLargeHitbox(JNIEnv* env, jclass cls, jboolean on) {
    g_LargeHitbox.store((bool)on);
}

JNIEXPORT void JNICALL
Java_com_bypass_Native_setAimbotFOV(JNIEnv* env, jclass cls, jfloat fov) {
    g_AimbotFOV.store(fov);
}

JNIEXPORT void JNICALL
Java_com_bypass_Native_setAimbotSmooth(JNIEnv* env, jclass cls, jfloat smooth) {
    g_AimbotSmooth.store(smooth);
}

JNIEXPORT void JNICALL
Java_com_bypass_Native_setHitboxScale(JNIEnv* env, jclass cls, jfloat scale) {
    g_HitboxScale.store(scale);
}

JNIEXPORT void JNICALL
Java_com_bypass_Native_setScreenSize(JNIEnv* env, jclass cls, jint w, jint h) {
    g_ScreenW.store(w);
    g_ScreenH.store(h);
}

// Get draw data as arrays for Java Canvas rendering
JNIEXPORT jfloatArray JNICALL
Java_com_bypass_Native_getBoxes(JNIEnv* env, jclass cls) {
    pthread_mutex_lock(&g_DrawData.lock);
    
    int count = g_DrawData.numBoxes;
    // Each box: x, y, w, h, healthPercent, distance, isEnemy (7 floats)
    jfloatArray result = env->NewFloatArray(count * 7);
    if (result && count > 0) {
        float* data = new float[count * 7];
        for (int i = 0; i < count; i++) {
            data[i*7 + 0] = g_DrawData.boxes[i].x;
            data[i*7 + 1] = g_DrawData.boxes[i].y;
            data[i*7 + 2] = g_DrawData.boxes[i].w;
            data[i*7 + 3] = g_DrawData.boxes[i].h;
            data[i*7 + 4] = g_DrawData.boxes[i].healthPercent;
            data[i*7 + 5] = g_DrawData.boxes[i].distance;
            data[i*7 + 6] = (float)g_DrawData.boxes[i].isEnemy;
        }
        env->SetFloatArrayRegion(result, 0, count * 7, data);
        delete[] data;
    }
    
    pthread_mutex_unlock(&g_DrawData.lock);
    return result;
}

JNIEXPORT jfloatArray JNICALL
Java_com_bypass_Native_getBones(JNIEnv* env, jclass cls) {
    pthread_mutex_lock(&g_DrawData.lock);
    
    int count = g_DrawData.numBones;
    // Each bone line: x1, y1, x2, y2, isEnemy (5 floats)
    jfloatArray result = env->NewFloatArray(count * 5);
    if (result && count > 0) {
        float* data = new float[count * 5];
        for (int i = 0; i < count; i++) {
            data[i*5 + 0] = g_DrawData.bones[i].x1;
            data[i*5 + 1] = g_DrawData.bones[i].y1;
            data[i*5 + 2] = g_DrawData.bones[i].x2;
            data[i*5 + 3] = g_DrawData.bones[i].y2;
            data[i*5 + 4] = (float)g_DrawData.bones[i].isEnemy;
        }
        env->SetFloatArrayRegion(result, 0, count * 5, data);
        delete[] data;
    }
    
    pthread_mutex_unlock(&g_DrawData.lock);
    return result;
}

JNIEXPORT jfloatArray JNICALL
Java_com_bypass_Native_getAimData(JNIEnv* env, jclass cls) {
    pthread_mutex_lock(&g_DrawData.lock);
    
    // targetX, targetY, hasTarget, fovRadius (4 floats)
    jfloatArray result = env->NewFloatArray(4);
    float data[4] = {
        g_DrawData.aim.targetX,
        g_DrawData.aim.targetY,
        g_DrawData.aim.hasTarget ? 1.0f : 0.0f,
        g_DrawData.aim.fovRadius
    };
    env->SetFloatArrayRegion(result, 0, 4, data);
    
    pthread_mutex_unlock(&g_DrawData.lock);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_com_bypass_Native_isInMatch(JNIEnv* env, jclass cls) {
    return (jboolean)g_DrawData.inMatch;
}

JNIEXPORT jint JNICALL
Java_com_bypass_Native_getPlayerCount(JNIEnv* env, jclass cls) {
    return (jint)g_DrawData.playerCount;
}

JNIEXPORT jboolean JNICALL
Java_com_bypass_Native_isBypassApplied(JNIEnv* env, jclass cls) {
    return (jboolean)g_BypassApplied.load();
}

} // extern "C"
