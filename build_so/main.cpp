/*
 * BGMI 4.4.0 ARM64 - ESP + Aimbot + Large Hitbox + Bypass
 * 
 * Entry: __attribute__((constructor)) → pthread main loop
 * SDK: NIKON 4.4.0 64-bit offsets (verified from .hpp)
 * Bypass: 809 verified patches from Ghidra analysis
 * 
 * Architecture: aarch64-linux-android
 */

#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <android/log.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <atomic>
#include <errno.h>
#include <sys/syscall.h>
#include <fcntl.h>

// ============================================================================
// LOGGING
// ============================================================================
#define TAG "BGMI_ESP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ============================================================================
// MATH TYPES
// ============================================================================
struct FVector {
    float X, Y, Z;
    
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    
    FVector operator-(const FVector& v) const { return {X - v.X, Y - v.Y, Z - v.Z}; }
    FVector operator+(const FVector& v) const { return {X + v.X, Y + v.Y, Z + v.Z}; }
    FVector operator*(float s) const { return {X * s, Y * s, Z * s}; }
    
    float Length() const { return sqrtf(X*X + Y*Y + Z*Z); }
    float Distance(const FVector& v) const { return (*this - v).Length(); }
};

struct FVector2D {
    float X, Y;
    FVector2D() : X(0), Y(0) {}
    FVector2D(float x, float y) : X(x), Y(y) {}
};

struct FRotator {
    float Pitch, Yaw, Roll;
    FRotator() : Pitch(0), Yaw(0), Roll(0) {}
};

struct FTransform {
    float Rotation[4]; // Quaternion: X,Y,Z,W
    FVector Translation;
    float pad0;
    FVector Scale3D;
    float pad1;
};

struct FMatrix {
    float M[4][4];
};

// ============================================================================
// CONFIG
// ============================================================================
struct Config {
    // ESP
    bool esp_enabled = true;
    bool esp_box = true;
    bool esp_health = true;
    bool esp_distance = true;
    bool esp_bones = true;
    
    // Aimbot
    bool aimbot_enabled = true;
    float aimbot_fov = 120.0f;          // pixels radius
    float aimbot_smooth = 6.0f;         // higher = smoother
    int   aimbot_bone = 6;              // head
    bool  aimbot_visible_only = false;
    
    // Large Hitbox
    bool large_hitbox_enabled = true;
    float hitbox_scale = 2.5f;
    
    // General
    bool lobby_bypass = true;           // disable in lobby
    int  min_players_for_match = 10;
    
    // Screen
    float screen_width = 1080.0f;
    float screen_height = 2400.0f;
};

static Config g_config;

// ============================================================================
// GAME OFFSETS (from NIKON SDK 4.4.0 64-bit)
// ============================================================================
namespace Offsets {
    // UWorld
    constexpr uintptr_t UWorld_PersistentLevel       = 0x0030;
    constexpr uintptr_t UWorld_GameState             = 0x0428;
    constexpr uintptr_t UWorld_OwningGameInstance    = 0x0470;
    
    // UGameInstance
    constexpr uintptr_t GameInstance_LocalPlayers    = 0x0048;
    
    // UPlayer / ULocalPlayer
    constexpr uintptr_t UPlayer_PlayerController    = 0x0030;
    
    // AController
    constexpr uintptr_t Controller_Pawn             = 0x04B8;
    constexpr uintptr_t Controller_Character        = 0x04C8;
    constexpr uintptr_t Controller_PlayerState      = 0x04D0;
    constexpr uintptr_t Controller_ControlRotation  = 0x04E0;
    
    // APlayerController (extends AController)
    constexpr uintptr_t PC_Player                   = 0x0518;
    constexpr uintptr_t PC_AcknowledgedPawn         = 0x0528;
    constexpr uintptr_t PC_MyHUD                    = 0x0540;
    constexpr uintptr_t PC_PlayerCameraManager      = 0x0548;
    
    // APlayerCameraManager
    // CameraCache at 0x0520, inside FCameraCacheEntry POV at +0x10
    constexpr uintptr_t CameraManager_CameraCache   = 0x0520;
    constexpr uintptr_t CacheEntry_POV              = 0x0010;
    constexpr uintptr_t POV_Location                = 0x0000;
    constexpr uintptr_t POV_Rotation                = 0x0018;
    constexpr uintptr_t POV_FOV                     = 0x0024;
    // Combined: CameraManager + 0x0530 = Location, +0x0548 = Rotation, +0x0554 = FOV
    
    // AGameStateBase
    constexpr uintptr_t GameState_PlayerArray       = 0x04C8;
    
    // AActor
    constexpr uintptr_t Actor_RootComponent         = 0x01A0;
    
    // APawn
    constexpr uintptr_t Pawn_PlayerState            = 0x04D0;
    constexpr uintptr_t Pawn_Controller             = 0x04E8;
    
    // ACharacter
    constexpr uintptr_t Character_Mesh              = 0x0510;
    constexpr uintptr_t Character_Movement          = 0x0518;
    constexpr uintptr_t Character_Capsule           = 0x0520;
    
    // ASTExtraBaseCharacter (game-specific)
    constexpr uintptr_t STChar_TeamID               = 0x0604;
    constexpr uintptr_t STChar_Health               = 0x0E60;
    constexpr uintptr_t STChar_HealthMax            = 0x0E64;
    constexpr uintptr_t STChar_bDead                = 0x0E7C; // bit 0
    constexpr uintptr_t STChar_TeamNum              = 0x1150;
    
    // ASTExtraPlayerState
    constexpr uintptr_t STState_LiveState           = 0x13F4;
    constexpr uintptr_t STState_CharacterOwner      = 0x1410;
    constexpr uintptr_t STState_PlayerHealth        = 0x142C;
    constexpr uintptr_t STState_PlayerHealthMax     = 0x1430;
    
    // USceneComponent
    constexpr uintptr_t SceneComp_ComponentToWorld  = 0x01C0;
    
    // USkeletalMeshComponent
    constexpr uintptr_t SkelMesh_CachedBoneTransforms = 0x0C40;
    
    // UCapsuleComponent
    constexpr uintptr_t Capsule_HalfHeight          = 0x02E8;
    constexpr uintptr_t Capsule_Radius              = 0x02EC;
    
    // TArray layout
    constexpr uintptr_t TArray_Data                 = 0x0000;
    constexpr uintptr_t TArray_Count                = 0x0008;
}

// ============================================================================
// BONE INDICES (UE4 standard for BGMI humanoid skeleton)
// ============================================================================
namespace Bones {
    constexpr int Root          = 0;
    constexpr int Pelvis        = 1;
    constexpr int Spine01       = 3;
    constexpr int Spine02       = 4;
    constexpr int Neck          = 5;
    constexpr int Head          = 6;
    constexpr int LeftShoulder  = 11;
    constexpr int LeftElbow     = 12;
    constexpr int LeftHand      = 13;
    constexpr int RightShoulder = 32;
    constexpr int RightElbow    = 33;
    constexpr int RightHand     = 34;
    constexpr int LeftHip       = 49;
    constexpr int LeftKnee      = 50;
    constexpr int LeftFoot      = 51;
    constexpr int RightHip      = 55;
    constexpr int RightKnee     = 56;
    constexpr int RightFoot     = 57;
    
    // Skeleton connections for drawing
    static const int skeleton_pairs[][2] = {
        {Head, Neck}, {Neck, Spine02}, {Spine02, Spine01}, {Spine01, Pelvis},
        {Neck, LeftShoulder}, {LeftShoulder, LeftElbow}, {LeftElbow, LeftHand},
        {Neck, RightShoulder}, {RightShoulder, RightElbow}, {RightElbow, RightHand},
        {Pelvis, LeftHip}, {LeftHip, LeftKnee}, {LeftKnee, LeftFoot},
        {Pelvis, RightHip}, {RightHip, RightKnee}, {RightKnee, RightFoot}
    };
    constexpr int skeleton_pair_count = 16;
}

// ============================================================================
// ESP DATA OUTPUT
// ============================================================================
struct PlayerESP {
    FVector2D screenPos;        // feet screen pos
    FVector2D headScreen;       // head screen pos
    float health;
    float healthMax;
    float distance;             // meters
    int teamId;
    bool isEnemy;
    FVector2D bones[20];        // key bone screen positions
    bool bonesValid[20];
    FVector worldPos;           // 3D position for aimbot
    FVector headWorld;          // head 3D for aimbot
};

static std::vector<PlayerESP> g_espData;
static std::atomic<bool> g_running(false);
static uintptr_t g_libUE4Base = 0;
static uintptr_t g_gworldPtr = 0;

// ============================================================================
// MEMORY UTILITIES
// ============================================================================

static bool is_valid_ptr(void* ptr) {
    if (ptr == nullptr) return false;
    uintptr_t addr = (uintptr_t)ptr;
    // Valid user-space addresses on ARM64 Android
    if (addr < 0x1000) return false;
    if (addr > 0x7FFFFFFFFFFF) return false;
    return true;
}

template<typename T>
static T read_mem(uintptr_t addr) {
    if (!is_valid_ptr((void*)addr)) return T{};
    return *(T*)addr;
}

template<typename T>
static T read_chain(uintptr_t base, std::initializer_list<uintptr_t> offsets) {
    uintptr_t addr = base;
    auto it = offsets.begin();
    // Follow pointer chain except last offset
    for (size_t i = 0; i < offsets.size() - 1; i++, it++) {
        addr = read_mem<uintptr_t>(addr + *it);
        if (!is_valid_ptr((void*)addr)) return T{};
    }
    return read_mem<T>(addr + *it);
}

static uintptr_t read_ptr_chain(uintptr_t base, std::initializer_list<uintptr_t> offsets) {
    uintptr_t addr = base;
    for (auto off : offsets) {
        addr = read_mem<uintptr_t>(addr + off);
        if (!is_valid_ptr((void*)addr)) return 0;
    }
    return addr;
}

// ============================================================================
// LIBRARY BASE ADDRESS
// ============================================================================

static uintptr_t get_lib_base(const char* libName) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, libName) && strstr(line, "r-xp")) {
            base = strtoull(line, nullptr, 16);
            break;
        }
        // Fallback: also check r--p for some loaders
        if (!base && strstr(line, libName) && strstr(line, "r--p")) {
            base = strtoull(line, nullptr, 16);
        }
    }
    fclose(fp);
    return base;
}

static size_t get_lib_size(const char* libName) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    
    uintptr_t start = 0, end = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, libName)) {
            uintptr_t s = strtoull(line, nullptr, 16);
            char* dash = strchr(line, '-');
            uintptr_t e = strtoull(dash + 1, nullptr, 16);
            if (start == 0 || s < start) start = s;
            if (e > end) end = e;
        }
    }
    fclose(fp);
    return (size_t)(end - start);
}

// ============================================================================
// MEMORY PATCHING
// ============================================================================

static bool mem_protect(uintptr_t addr, size_t len, int prot) {
    uintptr_t page = addr & ~(uintptr_t)0xFFF;
    size_t size = (addr - page) + len + 0xFFF;
    size &= ~(size_t)0xFFF;
    return mprotect((void*)page, size, prot) == 0;
}

static void hex_patch(const char* lib, const char* offsetStr, const char* hexStr) {
    uintptr_t base = get_lib_base(lib);
    if (!base) return;
    
    uintptr_t offset = strtoull(offsetStr, nullptr, 16);
    uintptr_t addr = base + offset;
    
    // Parse hex string "C0 03 5F D6" into bytes
    unsigned char bytes[32];
    int byteCount = 0;
    
    const char* p = hexStr;
    while (*p && byteCount < 32) {
        while (*p == ' ') p++;
        if (!*p) break;
        
        char hi = *p++;
        if (!*p) break;
        char lo = *p++;
        
        auto hexval = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return 0;
        };
        
        bytes[byteCount++] = (hexval(hi) << 4) | hexval(lo);
    }
    
    if (byteCount == 0) return;
    
    // Make writable, patch, restore
    if (!mem_protect(addr, byteCount, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        return;
    }
    
    memcpy((void*)addr, bytes, byteCount);
    
    // Clear instruction cache
    __builtin___clear_cache((char*)addr, (char*)(addr + byteCount));
}

// ============================================================================
// BYPASS APPLICATION (from FINAL_BYPASS.h)
// ============================================================================

// Redefine PATCH_LIB macro to call hex_patch
#define PATCH_LIB(lib, offset, hex) hex_patch(lib, offset, hex)

static void apply_all_bypasses() {
    LOGI("[*] Applying 809 verified bypass patches...");
    
    // Wait for libraries to load
    int attempts = 0;
    while (!get_lib_base("libUE4.so") && attempts < 60) {
        usleep(500000); // 500ms
        attempts++;
    }
    
    if (!get_lib_base("libUE4.so")) {
        LOGE("[-] libUE4.so not loaded after 30s, skipping patches");
        return;
    }
    
    // Small delay for other libs to finish loading
    usleep(2000000); // 2s
    
    // Include all verified patches
    #include "../true_bypass/FINAL_BYPASS.h"
    
    LOGI("[+] All bypass patches applied successfully");
}

#undef PATCH_LIB

// ============================================================================
// FIND GWORLD
// ============================================================================

static uintptr_t find_gworld() {
    uintptr_t base = get_lib_base("libUE4.so");
    if (!base) return 0;
    
    g_libUE4Base = base;
    size_t size = get_lib_size("libUE4.so");
    if (size == 0) size = 234 * 1024 * 1024; // fallback: 234MB
    
    // Pattern scan for GWorld pointer
    // GWorld is typically referenced early in tick functions
    // Look for ADRP+LDR pattern that loads GWorld
    // Alternative: scan .bss/.data for a pointer that points to a valid UWorld
    
    // Method 1: Look for known string "GWorld" reference
    // Method 2: Scan data section for pointer patterns
    
    // Use the reliable method: find GWorld via GUObjectArray iteration
    // Actually, simplest reliable method on Android:
    // GWorld is at a fixed offset in libUE4.so's .bss section
    // We can find it by scanning for a pointer that, when dereferenced,
    // gives a valid chain: UWorld->GameState->PlayerArray with reasonable count
    
    // Scan .bss region (high offsets in the library, after .text and .rodata)
    // libUE4.so is 234MB, .bss starts around offset 0x0C000000+
    
    uintptr_t scan_start = base + 0x0C000000;
    uintptr_t scan_end = base + size;
    
    // Limit scan to avoid taking too long
    if (scan_end - scan_start > 0x4000000) {
        scan_end = scan_start + 0x4000000; // scan 64MB max
    }
    
    LOGI("[*] Scanning for GWorld in range %p - %p", (void*)scan_start, (void*)scan_end);
    
    for (uintptr_t addr = scan_start; addr < scan_end - 8; addr += 8) {
        uintptr_t candidate = 0;
        
        // Safe read
        if (!is_valid_ptr((void*)addr)) continue;
        candidate = *(uintptr_t*)addr;
        
        if (!is_valid_ptr((void*)candidate)) continue;
        
        // Check if candidate looks like a UWorld*
        // UWorld->PersistentLevel should be a valid pointer
        uintptr_t persistentLevel = read_mem<uintptr_t>(candidate + Offsets::UWorld_PersistentLevel);
        if (!is_valid_ptr((void*)persistentLevel)) continue;
        
        // UWorld->GameState should be valid
        uintptr_t gameState = read_mem<uintptr_t>(candidate + Offsets::UWorld_GameState);
        if (!is_valid_ptr((void*)gameState)) continue;
        
        // GameState->PlayerArray count should be 1-100
        int32_t playerCount = read_mem<int32_t>(gameState + Offsets::GameState_PlayerArray + 8);
        if (playerCount < 1 || playerCount > 150) continue;
        
        // PlayerArray data pointer should be valid
        uintptr_t playerArrayData = read_mem<uintptr_t>(gameState + Offsets::GameState_PlayerArray);
        if (!is_valid_ptr((void*)playerArrayData)) continue;
        
        // UWorld->OwningGameInstance should be valid
        uintptr_t gameInstance = read_mem<uintptr_t>(candidate + Offsets::UWorld_OwningGameInstance);
        if (!is_valid_ptr((void*)gameInstance)) continue;
        
        // If we get here, this is very likely GWorld
        LOGI("[+] Found GWorld at offset 0x%lX (addr=%p, value=%p)", 
             (unsigned long)(addr - base), (void*)addr, (void*)candidate);
        LOGI("[+] PlayerCount=%d, GameState=%p", playerCount, (void*)gameState);
        
        return addr;
    }
    
    LOGW("[-] GWorld not found via scan");
    return 0;
}

// ============================================================================
// WORLD TO SCREEN
// ============================================================================

static bool WorldToScreen(FVector worldPos, FVector cameraLoc, FRotator cameraRot, float fov, FVector2D& screenOut) {
    // Convert rotation to radians
    float pitch = cameraRot.Pitch * (M_PI / 180.0f);
    float yaw   = cameraRot.Yaw   * (M_PI / 180.0f);
    float roll  = cameraRot.Roll  * (M_PI / 180.0f);
    
    float cp = cosf(pitch), sp = sinf(pitch);
    float cy = cosf(yaw),   sy = sinf(yaw);
    float cr = cosf(roll),  sr = sinf(roll);
    
    // Build view matrix axes (UE4 coordinate system)
    FVector forward = {cp * cy, cp * sy, sp};
    FVector right   = {
        -(sr * sp * cy - cr * sy),
        -(sr * sp * sy + cr * cy),
        sr * cp
    };
    FVector up = {
        cr * sp * cy + sr * sy,
        cr * sp * sy - sr * cy,
        -(cr * cp)
    };
    
    // Transform world position to view space
    FVector delta = worldPos - cameraLoc;
    
    float x = delta.X * right.X + delta.Y * right.Y + delta.Z * right.Z;
    float y = delta.X * up.X    + delta.Y * up.Y    + delta.Z * up.Z;
    float z = delta.X * forward.X + delta.Y * forward.Y + delta.Z * forward.Z;
    
    // Behind camera check
    if (z < 1.0f) return false;
    
    // Project to screen
    float fovRad = fov * (M_PI / 180.0f);
    float tanHalfFov = tanf(fovRad * 0.5f);
    
    float screenX = (g_config.screen_width * 0.5f) + (x / z / tanHalfFov) * (g_config.screen_width * 0.5f);
    float screenY = (g_config.screen_height * 0.5f) - (y / z / tanHalfFov) * (g_config.screen_width * 0.5f);
    
    screenOut.X = screenX;
    screenOut.Y = screenY;
    
    return true;
}

// ============================================================================
// BONE POSITION FROM MESH
// ============================================================================

static FVector GetBoneWorldPosition(uintptr_t mesh, int boneIndex) {
    FVector result = {0, 0, 0};
    
    if (!is_valid_ptr((void*)mesh)) return result;
    
    // Get CachedBoneSpaceTransforms TArray
    uintptr_t boneArrayData = read_mem<uintptr_t>(mesh + Offsets::SkelMesh_CachedBoneTransforms);
    int32_t boneCount = read_mem<int32_t>(mesh + Offsets::SkelMesh_CachedBoneTransforms + 8);
    
    if (!is_valid_ptr((void*)boneArrayData)) return result;
    if (boneIndex < 0 || boneIndex >= boneCount) return result;
    
    // Each FTransform is 48 bytes (0x30) on ARM64 with padding
    // Actually: Quat(16) + Translation(16) + Scale(16) = 48
    // But with alignment it's typically 0x30 = 48 bytes
    size_t transformSize = 0x30; // sizeof(FTransform) = 48
    
    uintptr_t boneTransformAddr = boneArrayData + (boneIndex * transformSize);
    if (!is_valid_ptr((void*)boneTransformAddr)) return result;
    
    // FTransform: Rotation(Quat 16 bytes), Translation(Vector+pad 16 bytes), Scale(Vector+pad 16 bytes)
    // Translation is at offset 0x10 inside FTransform
    FVector boneLocalPos = read_mem<FVector>(boneTransformAddr + 0x10);
    
    // Get ComponentToWorld transform from the mesh's SceneComponent
    // USkeletalMeshComponent inherits from USceneComponent
    FTransform compToWorld = read_mem<FTransform>(mesh + Offsets::SceneComp_ComponentToWorld);
    
    // Transform bone position to world space
    // Simplified: just add component world translation + bone local
    // Full transform would involve rotation, but for ESP this is sufficient
    // Actually we need proper transform:
    
    // Quaternion rotation
    float qx = compToWorld.Rotation[0];
    float qy = compToWorld.Rotation[1];
    float qz = compToWorld.Rotation[2];
    float qw = compToWorld.Rotation[3];
    
    // Rotate boneLocalPos by quaternion
    // v' = q * v * q^-1 (using quaternion multiplication)
    float vx = boneLocalPos.X, vy = boneLocalPos.Y, vz = boneLocalPos.Z;
    
    // Optimized quaternion rotation
    float tx = 2.0f * (qy * vz - qz * vy);
    float ty = 2.0f * (qz * vx - qx * vz);
    float tz = 2.0f * (qx * vy - qy * vx);
    
    result.X = vx + qw * tx + (qy * tz - qz * ty);
    result.Y = vy + qw * ty + (qz * tx - qx * tz);
    result.Z = vz + qw * tz + (qx * ty - qy * tx);
    
    // Scale
    result.X *= compToWorld.Scale3D.X;
    result.Y *= compToWorld.Scale3D.Y;
    result.Z *= compToWorld.Scale3D.Z;
    
    // Translate
    result.X += compToWorld.Translation.X;
    result.Y += compToWorld.Translation.Y;
    result.Z += compToWorld.Translation.Z;
    
    return result;
}

// ============================================================================
// GET ACTOR LOCATION (from RootComponent)
// ============================================================================

static FVector GetActorLocation(uintptr_t actor) {
    if (!is_valid_ptr((void*)actor)) return {0, 0, 0};
    
    uintptr_t rootComp = read_mem<uintptr_t>(actor + Offsets::Actor_RootComponent);
    if (!is_valid_ptr((void*)rootComp)) return {0, 0, 0};
    
    // ComponentToWorld.Translation = world position
    FVector pos = read_mem<FVector>(rootComp + Offsets::SceneComp_ComponentToWorld + 0x10);
    return pos;
}

// ============================================================================
// CAMERA INFO
// ============================================================================

struct CameraInfo {
    FVector location;
    FRotator rotation;
    float fov;
    bool valid;
};

static CameraInfo GetCameraInfo(uintptr_t playerController) {
    CameraInfo cam = {};
    cam.valid = false;
    
    if (!is_valid_ptr((void*)playerController)) return cam;
    
    uintptr_t cameraManager = read_mem<uintptr_t>(playerController + Offsets::PC_PlayerCameraManager);
    if (!is_valid_ptr((void*)cameraManager)) return cam;
    
    // CameraCache.POV.Location
    uintptr_t povBase = cameraManager + Offsets::CameraManager_CameraCache + Offsets::CacheEntry_POV;
    
    cam.location = read_mem<FVector>(povBase + Offsets::POV_Location);
    cam.rotation = read_mem<FRotator>(povBase + Offsets::POV_Rotation);
    cam.fov = read_mem<float>(povBase + Offsets::POV_FOV);
    
    // Validate
    if (cam.fov < 10.0f || cam.fov > 170.0f) cam.fov = 90.0f;
    if (cam.location.Length() < 1.0f) return cam; // probably invalid
    
    cam.valid = true;
    return cam;
}

// ============================================================================
// LOCAL PLAYER INFO
// ============================================================================

struct LocalPlayerInfo {
    uintptr_t playerController;
    uintptr_t pawn;
    uintptr_t playerState;
    int teamId;
    FVector location;
    bool valid;
};

static LocalPlayerInfo GetLocalPlayer(uintptr_t world) {
    LocalPlayerInfo info = {};
    info.valid = false;
    
    if (!is_valid_ptr((void*)world)) return info;
    
    // UWorld -> OwningGameInstance -> LocalPlayers[0] -> PlayerController
    uintptr_t gameInstance = read_mem<uintptr_t>(world + Offsets::UWorld_OwningGameInstance);
    if (!is_valid_ptr((void*)gameInstance)) return info;
    
    // LocalPlayers TArray
    uintptr_t localPlayersData = read_mem<uintptr_t>(gameInstance + Offsets::GameInstance_LocalPlayers);
    int32_t localPlayersCount = read_mem<int32_t>(gameInstance + Offsets::GameInstance_LocalPlayers + 8);
    
    if (!is_valid_ptr((void*)localPlayersData) || localPlayersCount < 1) return info;
    
    // LocalPlayers[0]
    uintptr_t localPlayer = read_mem<uintptr_t>(localPlayersData);
    if (!is_valid_ptr((void*)localPlayer)) return info;
    
    // UPlayer::PlayerController
    info.playerController = read_mem<uintptr_t>(localPlayer + Offsets::UPlayer_PlayerController);
    if (!is_valid_ptr((void*)info.playerController)) return info;
    
    // AController::Pawn
    info.pawn = read_mem<uintptr_t>(info.playerController + Offsets::Controller_Pawn);
    if (!is_valid_ptr((void*)info.pawn)) return info;
    
    // TeamID from pawn (ASTExtraBaseCharacter)
    info.teamId = read_mem<int32_t>(info.pawn + Offsets::STChar_TeamID);
    info.location = GetActorLocation(info.pawn);
    
    info.valid = true;
    return info;
}

// ============================================================================
// AIMBOT
// ============================================================================

static void DoAimbot(uintptr_t playerController, const CameraInfo& cam, const std::vector<PlayerESP>& players) {
    if (!g_config.aimbot_enabled) return;
    if (!is_valid_ptr((void*)playerController)) return;
    
    float screenCenterX = g_config.screen_width / 2.0f;
    float screenCenterY = g_config.screen_height / 2.0f;
    
    float closestDist = g_config.aimbot_fov;
    FVector bestTarget = {0, 0, 0};
    bool foundTarget = false;
    
    for (const auto& player : players) {
        if (!player.isEnemy) continue;
        if (player.health <= 0) continue;
        
        // Check if head is on screen
        if (!player.bonesValid[Bones::Head]) continue;
        
        float headScreenX = player.bones[Bones::Head].X;
        float headScreenY = player.bones[Bones::Head].Y;
        
        // Distance from crosshair
        float dx = headScreenX - screenCenterX;
        float dy = headScreenY - screenCenterY;
        float dist = sqrtf(dx * dx + dy * dy);
        
        if (dist < closestDist) {
            closestDist = dist;
            bestTarget = player.headWorld;
            foundTarget = true;
        }
    }
    
    if (!foundTarget) return;
    
    // Calculate aim angle
    FVector delta = bestTarget - cam.location;
    float dist = delta.Length();
    if (dist < 1.0f) return;
    
    FRotator targetRot;
    targetRot.Yaw = atan2f(delta.Y, delta.X) * (180.0f / M_PI);
    targetRot.Pitch = -atan2f(delta.Z, sqrtf(delta.X * delta.X + delta.Y * delta.Y)) * (180.0f / M_PI);
    targetRot.Roll = 0;
    
    // Read current control rotation
    FRotator* controlRot = (FRotator*)((uintptr_t)playerController + Offsets::Controller_ControlRotation);
    if (!is_valid_ptr((void*)controlRot)) return;
    
    // Smooth interpolation
    float smooth = g_config.aimbot_smooth;
    if (smooth < 1.0f) smooth = 1.0f;
    
    // Normalize angle difference
    auto normAngle = [](float angle) -> float {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    };
    
    float diffYaw = normAngle(targetRot.Yaw - controlRot->Yaw);
    float diffPitch = normAngle(targetRot.Pitch - controlRot->Pitch);
    
    controlRot->Yaw += diffYaw / smooth;
    controlRot->Pitch += diffPitch / smooth;
}

// ============================================================================
// LARGE HITBOX
// ============================================================================

static void ApplyLargeHitbox(uintptr_t character) {
    if (!g_config.large_hitbox_enabled) return;
    if (!is_valid_ptr((void*)character)) return;
    
    uintptr_t capsule = read_mem<uintptr_t>(character + Offsets::Character_Capsule);
    if (!is_valid_ptr((void*)capsule)) return;
    
    float currentRadius = read_mem<float>(capsule + Offsets::Capsule_Radius);
    
    // Only scale if it's a reasonable base value (hasn't been scaled yet)
    // Normal capsule radius is around 30-45 units
    if (currentRadius > 10.0f && currentRadius < 60.0f) {
        float newRadius = currentRadius * g_config.hitbox_scale;
        
        if (mem_protect(capsule + Offsets::Capsule_Radius, 4, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            *(float*)(capsule + Offsets::Capsule_Radius) = newRadius;
        }
    }
}

// ============================================================================
// ESP DATA OUTPUT (write to file for overlay app to read)
// ============================================================================

static void WriteESPData(const std::vector<PlayerESP>& players, const CameraInfo& cam) {
    // Write ESP data to shared memory file for overlay rendering
    FILE* fp = fopen("/data/local/tmp/bgmi_esp.dat", "wb");
    if (!fp) {
        // Try alternative path
        fp = fopen("/sdcard/Android/data/com.pubg.imobile/bgmi_esp.dat", "wb");
        if (!fp) return;
    }
    
    // Header: player count, screen size
    int32_t count = (int32_t)players.size();
    fwrite(&count, 4, 1, fp);
    fwrite(&g_config.screen_width, 4, 1, fp);
    fwrite(&g_config.screen_height, 4, 1, fp);
    fwrite(&cam.location, sizeof(FVector), 1, fp);
    
    // Aimbot FOV circle center
    float cx = g_config.screen_width / 2.0f;
    float cy = g_config.screen_height / 2.0f;
    float fovRadius = g_config.aimbot_fov;
    fwrite(&cx, 4, 1, fp);
    fwrite(&cy, 4, 1, fp);
    fwrite(&fovRadius, 4, 1, fp);
    
    // Per-player data
    for (const auto& p : players) {
        fwrite(&p, sizeof(PlayerESP), 1, fp);
    }
    
    fclose(fp);
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

static void* main_thread(void* arg) {
    LOGI("[*] BGMI Bypass + ESP thread started");
    
    // Phase 1: Apply bypasses
    apply_all_bypasses();
    
    // Phase 2: Wait for game to fully load
    LOGI("[*] Waiting for game to initialize...");
    usleep(5000000); // 5 seconds
    
    // Phase 3: Find GWorld
    int retries = 0;
    while (g_gworldPtr == 0 && retries < 30) {
        g_gworldPtr = find_gworld();
        if (g_gworldPtr == 0) {
            LOGI("[*] GWorld not found, retry %d/30...", retries + 1);
            usleep(3000000); // 3s
        }
        retries++;
    }
    
    if (g_gworldPtr == 0) {
        LOGE("[-] Failed to find GWorld after 30 retries. ESP disabled.");
        // Bypasses still active
        while (true) { sleep(60); }
        return nullptr;
    }
    
    LOGI("[+] ESP/Aimbot loop starting");
    g_running = true;
    
    // Main loop
    while (g_running) {
        usleep(16666); // ~60 FPS
        
        // Read GWorld pointer
        uintptr_t world = read_mem<uintptr_t>(g_gworldPtr);
        if (!is_valid_ptr((void*)world)) {
            // World may have been destroyed (loading screen, etc)
            usleep(1000000);
            continue;
        }
        
        // Get local player
        LocalPlayerInfo local = GetLocalPlayer(world);
        if (!local.valid) continue;
        
        // Get camera
        CameraInfo cam = GetCameraInfo(local.playerController);
        if (!cam.valid) continue;
        
        // Get GameState -> PlayerArray
        uintptr_t gameState = read_mem<uintptr_t>(world + Offsets::UWorld_GameState);
        if (!is_valid_ptr((void*)gameState)) continue;
        
        uintptr_t playerArrayData = read_mem<uintptr_t>(gameState + Offsets::GameState_PlayerArray);
        int32_t playerCount = read_mem<int32_t>(gameState + Offsets::GameState_PlayerArray + 8);
        
        if (!is_valid_ptr((void*)playerArrayData)) continue;
        if (playerCount < 1 || playerCount > 150) continue;
        
        // Lobby/Island bypass
        if (g_config.lobby_bypass && playerCount < g_config.min_players_for_match) {
            continue;
        }
        
        // Process players
        std::vector<PlayerESP> espPlayers;
        espPlayers.reserve(playerCount);
        
        for (int i = 0; i < playerCount; i++) {
            uintptr_t playerState = read_mem<uintptr_t>(playerArrayData + i * 8);
            if (!is_valid_ptr((void*)playerState)) continue;
            
            // Get character from ASTExtraPlayerState::CharacterOwner
            uintptr_t character = read_mem<uintptr_t>(playerState + Offsets::STState_CharacterOwner);
            if (!is_valid_ptr((void*)character)) continue;
            
            // Skip self
            if (character == local.pawn) continue;
            
            // Check alive
            float health = read_mem<float>(character + Offsets::STChar_Health);
            float healthMax = read_mem<float>(character + Offsets::STChar_HealthMax);
            uint8_t deadByte = read_mem<uint8_t>(character + Offsets::STChar_bDead);
            bool isDead = (deadByte & 0x01) != 0;
            
            if (isDead || health <= 0.0f) continue;
            if (healthMax <= 0.0f) healthMax = 100.0f;
            
            // Team check
            int teamId = read_mem<int32_t>(character + Offsets::STChar_TeamID);
            bool isEnemy = (teamId != local.teamId);
            
            // Get position
            FVector actorPos = GetActorLocation(character);
            float distance = actorPos.Distance(local.location) / 100.0f; // cm to meters
            
            // Get mesh for bones
            uintptr_t mesh = read_mem<uintptr_t>(character + Offsets::Character_Mesh);
            
            // Build ESP data
            PlayerESP esp = {};
            esp.health = health;
            esp.healthMax = healthMax;
            esp.distance = distance;
            esp.teamId = teamId;
            esp.isEnemy = isEnemy;
            esp.worldPos = actorPos;
            
            // World to screen for position
            FVector2D screenPos;
            if (!WorldToScreen(actorPos, cam.location, cam.rotation, cam.fov, screenPos)) continue;
            esp.screenPos = screenPos;
            
            // Get bone positions
            if (is_valid_ptr((void*)mesh)) {
                // Key bones to process
                static const int bonesToProcess[] = {
                    Bones::Head, Bones::Neck, Bones::Spine02, Bones::Spine01,
                    Bones::Pelvis, Bones::LeftShoulder, Bones::LeftElbow, 
                    Bones::LeftHand, Bones::RightShoulder, Bones::RightElbow,
                    Bones::RightHand, Bones::LeftHip, Bones::LeftKnee,
                    Bones::LeftFoot, Bones::RightHip, Bones::RightKnee,
                    Bones::RightFoot
                };
                
                for (int b = 0; b < 17; b++) {
                    int boneIdx = bonesToProcess[b];
                    FVector boneWorld = GetBoneWorldPosition(mesh, boneIdx);
                    
                    if (boneWorld.Length() < 1.0f) {
                        esp.bonesValid[boneIdx] = false;
                        continue;
                    }
                    
                    FVector2D boneScreen;
                    if (WorldToScreen(boneWorld, cam.location, cam.rotation, cam.fov, boneScreen)) {
                        esp.bones[boneIdx] = boneScreen;
                        esp.bonesValid[boneIdx] = true;
                        
                        // Store head world position for aimbot
                        if (boneIdx == Bones::Head) {
                            esp.headWorld = boneWorld;
                            esp.headScreen = boneScreen;
                        }
                    } else {
                        esp.bonesValid[boneIdx] = false;
                    }
                }
                
                // Apply large hitbox on enemies
                if (isEnemy && g_config.large_hitbox_enabled) {
                    ApplyLargeHitbox(character);
                }
            }
            
            espPlayers.push_back(esp);
        }
        
        // Aimbot
        if (g_config.aimbot_enabled && !espPlayers.empty()) {
            DoAimbot(local.playerController, cam, espPlayers);
        }
        
        // Write ESP data for overlay
        if (g_config.esp_enabled && !espPlayers.empty()) {
            WriteESPData(espPlayers, cam);
        }
        
        // Update global for external access
        g_espData = espPlayers;
    }
    
    return nullptr;
}

// ============================================================================
// SCREEN RESOLUTION DETECTION
// ============================================================================

static void detect_screen_size() {
    // Try to read from /sys or wm
    FILE* fp = popen("wm size 2>/dev/null", "r");
    if (fp) {
        char buf[128];
        if (fgets(buf, sizeof(buf), fp)) {
            int w, h;
            if (sscanf(buf, "Physical size: %dx%d", &w, &h) == 2) {
                g_config.screen_width = (float)w;
                g_config.screen_height = (float)h;
                LOGI("[+] Screen: %dx%d", w, h);
            }
        }
        pclose(fp);
    }
}

// ============================================================================
// ENTRY POINT
// ============================================================================

__attribute__((constructor))
void lib_init() {
    LOGI("[*] ====================================");
    LOGI("[*] BGMI 4.4.0 Bypass + ESP loaded");
    LOGI("[*] ====================================");
    
    detect_screen_size();
    
    pthread_t ptid;
    pthread_create(&ptid, nullptr, main_thread, nullptr);
    pthread_detach(ptid);
}
