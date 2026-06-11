/*
 * BGMI 4.4.0 ARM64 — ESP + Aimbot + Large Hitbox + 809 Bypass Patches
 * 
 * Uses NIKON SDK class layouts (game_sdk.h) for type-safe memory access.
 * Entry: __attribute__((constructor)) → pthread_create(main_thread)
 * All pointer chains are null-checked for crash safety.
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
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <cfloat>
#include <climits>

#include "game_sdk.h"

// ============================================================================
// LOGGING
// ============================================================================
#define TAG "BGMI_MOD"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ============================================================================
// CONFIGURATION (can be changed at runtime via /data/local/tmp/bgmi_config.txt)
// ============================================================================
struct Config {
    // Screen
    float screen_width  = 2340.0f;
    float screen_height = 1080.0f;
    
    // ESP
    bool  esp_enabled   = true;
    bool  esp_box       = true;
    bool  esp_skeleton  = true;
    bool  esp_health    = true;
    bool  esp_distance  = true;
    float esp_max_dist  = 500.0f; // meters
    
    // Aimbot
    bool  aim_enabled   = true;
    float aim_fov       = 60.0f;  // pixels from crosshair
    float aim_smooth    = 5.0f;   // higher = smoother
    int   aim_bone      = BONE_HEAD;
    
    // Large Hitbox
    bool  hitbox_enabled = true;
    float hitbox_scale   = 2.5f;
    
    // Lobby bypass
    int   min_players    = 10; // min players to enable features
};

static Config g_cfg;
static std::atomic<bool> g_running(false);
static uintptr_t g_ue4_base = 0;
static uintptr_t g_gworld_ptr = 0;

// ============================================================================
// MEMORY UTILITIES
// ============================================================================

static bool is_valid_ptr(void* ptr) {
    if (!ptr) return false;
    uintptr_t addr = (uintptr_t)ptr;
    // Valid Android user-space: above 0x1000, below kernel space
    return (addr > 0x10000 && addr < 0x7FFFFFFFFFFF);
}

template<typename T>
static T safe_read(uintptr_t addr) {
    if (!is_valid_ptr((void*)addr)) return T{};
    return *(T*)addr;
}

template<typename T>
static T* safe_ptr(uintptr_t addr) {
    if (!is_valid_ptr((void*)addr)) return nullptr;
    T* p = *(T**)addr;
    if (!is_valid_ptr(p)) return nullptr;
    return p;
}

static uintptr_t get_lib_base(const char* lib_name) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, lib_name) && strstr(line, "r-xp")) {
            base = strtoull(line, nullptr, 16);
            break;
        }
    }
    fclose(fp);
    return base;
}

static void get_lib_range(const char* lib_name, uintptr_t& start, uintptr_t& end) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) { start = end = 0; return; }
    
    start = UINT64_MAX; end = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, lib_name)) {
            uintptr_t s, e;
            sscanf(line, "%lx-%lx", &s, &e);
            if (s < start) start = s;
            if (e > end) end = e;
        }
    }
    fclose(fp);
    if (start == UINT64_MAX) start = 0;
}

// ============================================================================
// MEMORY PATCHING
// ============================================================================

static int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static int parse_hex_bytes(const char* hex_str, uint8_t* out, int max_len) {
    int count = 0;
    while (*hex_str && count < max_len) {
        // Skip spaces
        while (*hex_str == ' ') hex_str++;
        if (!*hex_str) break;
        
        int hi = hex_char_to_int(*hex_str++);
        if (hi < 0) break;
        int lo = hex_char_to_int(*hex_str++);
        if (lo < 0) break;
        
        out[count++] = (uint8_t)((hi << 4) | lo);
    }
    return count;
}

static void hex_patch(const char* lib, const char* offset_str, const char* hex_bytes) {
    uintptr_t base = get_lib_base(lib);
    if (!base) return;
    
    uintptr_t offset = strtoull(offset_str, nullptr, 16);
    uintptr_t target = base + offset;
    
    uint8_t patch[16];
    int patch_len = parse_hex_bytes(hex_bytes, patch, sizeof(patch));
    if (patch_len <= 0) return;
    
    // Make memory writable
    uintptr_t page = target & ~0xFFF;
    if (mprotect((void*)page, 0x2000, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) return;
    
    // Apply patch
    memcpy((void*)target, patch, patch_len);
    
    // Flush instruction cache
    __builtin___clear_cache((char*)target, (char*)(target + patch_len));
}

// Redefine PATCH_LIB macro for including FINAL_BYPASS.h
#define PATCH_LIB(lib, offset, hex) hex_patch(lib, offset, hex)

static void apply_all_bypasses() {
    LOGI("[*] Applying 809 verified bypass patches...");
    
    // Wait for all libraries to load
    int retries = 0;
    while (retries < 60) {
        if (get_lib_base("libUE4.so") != 0 &&
            get_lib_base("libanogs.so") != 0) {
            break;
        }
        usleep(500000); // 500ms
        retries++;
    }
    
    if (retries >= 60) {
        LOGW("[!] Timeout waiting for libraries, applying what's available");
    }
    
    // Include all 809 patches
    #include "../true_bypass/FINAL_BYPASS.h"
    
    LOGI("[+] Bypass patches applied!");
}

#undef PATCH_LIB

// ============================================================================
// GWORLD FINDER (Pattern scan .bss section of libUE4.so)
// ============================================================================

static uintptr_t find_gworld() {
    uintptr_t ue4_start, ue4_end;
    get_lib_range("libUE4.so", ue4_start, ue4_end);
    if (!ue4_start || !ue4_end) return 0;
    
    g_ue4_base = ue4_start;
    
    // GWorld is a global pointer in .bss section (high addresses of the lib)
    // It points to a valid UWorld* which has:
    //   PersistentLevel at +0x30 (non-null pointer)
    //   GameState at +0x428 (non-null pointer)
    //   OwningGameInstance at +0x470 (non-null pointer)
    
    // Scan the last 20% of the library mapping (where .bss/.data are)
    uintptr_t scan_start = ue4_start + (ue4_end - ue4_start) * 80 / 100;
    uintptr_t scan_end   = ue4_end - 8;
    
    for (uintptr_t addr = scan_start; addr < scan_end; addr += 8) {
        if (!is_valid_ptr((void*)addr)) continue;
        
        uintptr_t candidate = *(uintptr_t*)addr;
        if (!is_valid_ptr((void*)candidate)) continue;
        
        // Check if candidate looks like UWorld*
        UWorld* world = (UWorld*)candidate;
        
        // PersistentLevel must be valid
        if (!is_valid_ptr(world->PersistentLevel)) continue;
        
        // GameState must be valid
        if (!is_valid_ptr(world->GameState)) continue;
        
        // OwningGameInstance must be valid
        UGameInstance* gi = (UGameInstance*)world->OwningGameInstance;
        if (!is_valid_ptr(gi)) continue;
        
        // GameInstance->LocalPlayers.Data must be valid, Count > 0
        if (!is_valid_ptr(gi->LocalPlayers.Data)) continue;
        if (gi->LocalPlayers.Count < 1 || gi->LocalPlayers.Count > 4) continue;
        
        // LocalPlayers[0] must be valid
        ULocalPlayer* lp = (ULocalPlayer*)gi->LocalPlayers.Data[0];
        if (!is_valid_ptr(lp)) continue;
        
        // LocalPlayer->PlayerController must be valid
        if (!is_valid_ptr(lp->PlayerController)) continue;
        
        // PlayerController->PlayerCameraManager must be valid
        APlayerController* pc = lp->PlayerController;
        if (!is_valid_ptr(pc->PlayerCameraManager)) continue;
        
        LOGI("[+] Found GWorld at 0x%lx (value=0x%lx)", addr - ue4_start, candidate);
        return addr;
    }
    
    LOGW("[!] GWorld not found via pattern scan");
    return 0;
}

// ============================================================================
// WORLD TO SCREEN PROJECTION
// ============================================================================

static bool WorldToScreen(const FVector& worldPos, const FVector& camLoc, 
                          const FRotator& camRot, float fov, FVector2D& out) {
    // Convert rotation to radians
    float pitch = camRot.Pitch * (M_PI / 180.0f);
    float yaw   = camRot.Yaw   * (M_PI / 180.0f);
    float roll  = camRot.Roll  * (M_PI / 180.0f);
    
    // Camera forward/right/up vectors from rotation
    float cp = cosf(pitch), sp = sinf(pitch);
    float cy = cosf(yaw),   sy = sinf(yaw);
    float cr = cosf(roll),  sr = sinf(roll);
    
    FVector forward = { cp * cy, cp * sy, sp };
    FVector right   = { -(sr * sp * cy - cr * sy), -(sr * sp * sy + cr * cy), sr * cp };
    FVector up      = { -(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp };
    
    // Vector from camera to world point
    FVector delta = worldPos - camLoc;
    
    // Dot products
    float dotForward = delta.X * forward.X + delta.Y * forward.Y + delta.Z * forward.Z;
    float dotRight   = delta.X * right.X   + delta.Y * right.Y   + delta.Z * right.Z;
    float dotUp      = delta.X * up.X      + delta.Y * up.Y      + delta.Z * up.Z;
    
    // Behind camera
    if (dotForward < 1.0f) return false;
    
    // FOV scaling
    float fovRad = fov * (M_PI / 180.0f);
    float screenCenterX = g_cfg.screen_width  * 0.5f;
    float screenCenterY = g_cfg.screen_height * 0.5f;
    float tanHalfFov = tanf(fovRad * 0.5f);
    
    out.X = screenCenterX + (dotRight / dotForward / tanHalfFov) * screenCenterX;
    out.Y = screenCenterY - (dotUp    / dotForward / tanHalfFov) * screenCenterY;
    
    return true;
}

// ============================================================================
// BONE POSITION (from CachedBoneSpaceTransforms + ComponentToWorld)
// ============================================================================

static FVector GetBoneWorldPos(USkeletalMeshComponent* mesh, int boneIndex) {
    FVector result = {0, 0, 0};
    if (!is_valid_ptr(mesh)) return result;
    
    // Get CachedBoneSpaceTransforms array
    TArray<FTransform>* bones = &mesh->CachedBoneSpaceTransforms;
    if (!is_valid_ptr(bones->Data)) return result;
    if (boneIndex < 0 || boneIndex >= bones->Count) return result;
    
    // Get bone transform (relative to component)
    FTransform* boneTransform = &bones->Data[boneIndex];
    if (!is_valid_ptr(boneTransform)) return result;
    
    // Get component-to-world transform
    FTransform* c2w = &mesh->ComponentToWorld;
    
    // Simplified: bone world pos = ComponentToWorld.Translation + BoneRelative.Translation
    // (Ignoring rotation for now — good enough for ESP box/line positions)
    // Full transform would require quaternion multiplication
    
    // For proper bone position: apply component rotation to bone translation
    FQuat& rot = c2w->Rotation;
    FVector& boneLoc = boneTransform->Translation;
    
    // Quaternion rotation: v' = q * v * q^-1
    // Simplified using quaternion-vector multiplication:
    float qx = rot.X, qy = rot.Y, qz = rot.Z, qw = rot.W;
    float vx = boneLoc.X, vy = boneLoc.Y, vz = boneLoc.Z;
    
    // t = 2 * cross(q.xyz, v)
    float tx = 2.0f * (qy * vz - qz * vy);
    float ty = 2.0f * (qz * vx - qx * vz);
    float tz = 2.0f * (qx * vy - qy * vx);
    
    // result = v + w * t + cross(q.xyz, t)
    result.X = vx + qw * tx + (qy * tz - qz * ty);
    result.Y = vy + qw * ty + (qz * tx - qx * tz);
    result.Z = vz + qw * tz + (qx * ty - qy * tx);
    
    // Add component world location
    result.X += c2w->Translation.X;
    result.Y += c2w->Translation.Y;
    result.Z += c2w->Translation.Z;
    
    return result;
}

// ============================================================================
// GET ACTOR WORLD LOCATION (from RootComponent→ComponentToWorld)
// ============================================================================

static FVector GetActorLocation(AActor* actor) {
    if (!is_valid_ptr(actor)) return {0, 0, 0};
    USceneComponent* root = actor->RootComponent;
    if (!is_valid_ptr(root)) return {0, 0, 0};
    return root->ComponentToWorld.Translation;
}

// ============================================================================
// CAMERA INFO
// ============================================================================

struct CameraData {
    FVector  location;
    FRotator rotation;
    float    fov;
    bool     valid;
};

static CameraData GetCameraData(APlayerController* pc) {
    CameraData cam = {};
    cam.valid = false;
    
    if (!is_valid_ptr(pc)) return cam;
    
    APlayerCameraManager* mgr = (APlayerCameraManager*)pc->PlayerCameraManager;
    if (!is_valid_ptr(mgr)) return cam;
    
    cam.location = mgr->CameraCache.POV.Location;
    cam.rotation = mgr->CameraCache.POV.Rotation;
    cam.fov      = mgr->CameraCache.POV.FOV;
    
    // Sanity check FOV
    if (cam.fov < 1.0f || cam.fov > 180.0f) cam.fov = 90.0f;
    
    cam.valid = true;
    return cam;
}

// ============================================================================
// LOCAL PLAYER INFO
// ============================================================================

struct LocalPlayerInfo {
    APlayerController*      playerController;
    ASTExtraBaseCharacter*  myPawn;
    int32_t                 myTeamID;
    bool                    valid;
};

static LocalPlayerInfo GetLocalPlayer(UWorld* world) {
    LocalPlayerInfo info = {};
    info.valid = false;
    
    if (!is_valid_ptr(world)) return info;
    
    // World → GameInstance → LocalPlayers[0] → PlayerController
    UGameInstance* gi = (UGameInstance*)world->OwningGameInstance;
    if (!is_valid_ptr(gi)) return info;
    if (!is_valid_ptr(gi->LocalPlayers.Data)) return info;
    if (gi->LocalPlayers.Count < 1) return info;
    
    ULocalPlayer* lp = (ULocalPlayer*)gi->LocalPlayers.Data[0];
    if (!is_valid_ptr(lp)) return info;
    
    info.playerController = lp->PlayerController;
    if (!is_valid_ptr(info.playerController)) return info;
    
    // Get our pawn through Controller→Pawn
    AController* ctrl = (AController*)info.playerController;
    APawn* pawn = ctrl->Pawn;
    if (is_valid_ptr(pawn)) {
        info.myPawn = (ASTExtraBaseCharacter*)pawn;
        // Get TeamID from AUAECharacter offset
        AUAECharacter* uaeChar = (AUAECharacter*)info.myPawn;
        info.myTeamID = uaeChar->TeamID;
    }
    
    info.valid = true;
    return info;
}

// ============================================================================
// ESP DATA (written to file for overlay app to render)
// ============================================================================

struct PlayerEspData {
    float screenX, screenY;          // Center position
    float headScreenX, headScreenY;  // Head position
    float footScreenX, footScreenY;  // Foot position
    float health, healthMax;
    float distance;                   // in meters (UE4 units / 100)
    int   teamId;
    bool  isEnemy;
    // Skeleton bone screen positions
    float boneScreen[58][2];         // indexed by bone ID
    bool  boneValid[58];
};

// ============================================================================
// AIMBOT
// ============================================================================

static void DoAimbot(APlayerController* pc, const FVector& targetHeadWorld, 
                     const CameraData& cam) {
    if (!g_cfg.aim_enabled) return;
    if (!is_valid_ptr(pc)) return;
    
    // Calculate target rotation
    FVector delta = targetHeadWorld - cam.location;
    float dist = delta.Size();
    if (dist < 1.0f) return;
    
    FRotator targetRot;
    targetRot.Yaw   = atan2f(delta.Y, delta.X) * (180.0f / M_PI);
    targetRot.Pitch = atan2f(delta.Z, sqrtf(delta.X*delta.X + delta.Y*delta.Y)) * (180.0f / M_PI);
    targetRot.Roll  = 0.0f;
    
    // Get current ControlRotation from AController
    AController* ctrl = (AController*)pc;
    FRotator& current = ctrl->ControlRotation;
    
    // Calculate delta with wrapping
    float deltaYaw = targetRot.Yaw - current.Yaw;
    float deltaPitch = targetRot.Pitch - current.Pitch;
    
    // Normalize angles to [-180, 180]
    while (deltaYaw > 180.0f)  deltaYaw -= 360.0f;
    while (deltaYaw < -180.0f) deltaYaw += 360.0f;
    while (deltaPitch > 180.0f)  deltaPitch -= 360.0f;
    while (deltaPitch < -180.0f) deltaPitch += 360.0f;
    
    // Check if within FOV (in screen space)
    FVector2D targetScreen;
    if (!WorldToScreen(targetHeadWorld, cam.location, cam.rotation, cam.fov, targetScreen))
        return;
    
    float screenCenterX = g_cfg.screen_width  * 0.5f;
    float screenCenterY = g_cfg.screen_height * 0.5f;
    float dx = targetScreen.X - screenCenterX;
    float dy = targetScreen.Y - screenCenterY;
    float screenDist = sqrtf(dx*dx + dy*dy);
    
    if (screenDist > g_cfg.aim_fov) return; // Outside FOV circle
    
    // Apply smooth aim
    float smooth = g_cfg.aim_smooth;
    if (smooth < 1.0f) smooth = 1.0f;
    
    current.Yaw   += deltaYaw / smooth;
    current.Pitch += deltaPitch / smooth;
}

// ============================================================================
// LARGE HITBOX
// ============================================================================

static void ApplyLargeHitbox(ASTExtraBaseCharacter* character) {
    if (!g_cfg.hitbox_enabled) return;
    if (!is_valid_ptr(character)) return;
    
    ACharacter* ch = (ACharacter*)character;
    void* capsule = ch->CapsuleComponent;
    if (!is_valid_ptr(capsule)) return;
    
    // UCapsuleComponent: CapsuleHalfHeight at ~0x02E8, CapsuleRadius at ~0x02EC
    // These offsets are from UShapeComponent base
    // Safe: only scale up, and only for enemies (already filtered)
    float* halfHeight = (float*)((uintptr_t)capsule + 0x02E8);
    float* radius = (float*)((uintptr_t)capsule + 0x02EC);
    
    if (is_valid_ptr(halfHeight) && is_valid_ptr(radius)) {
        float currentRadius = *radius;
        // Only scale if not already scaled (avoid compound scaling)
        if (currentRadius > 0.0f && currentRadius < 100.0f) {
            *radius = currentRadius * g_cfg.hitbox_scale;
            *halfHeight = (*halfHeight) * g_cfg.hitbox_scale;
        }
    }
}

// ============================================================================
// MAIN THREAD
// ============================================================================

static void* main_thread(void* arg) {
    LOGI("[*] Main thread started, waiting for game...");
    
    // Wait for libUE4.so to load
    while (get_lib_base("libUE4.so") == 0) {
        usleep(1000000); // 1 second
    }
    LOGI("[+] libUE4.so loaded at 0x%lx", get_lib_base("libUE4.so"));
    
    // Apply all bypass patches
    apply_all_bypasses();
    
    // Wait a bit for game to initialize
    sleep(5);
    
    // Find GWorld
    LOGI("[*] Scanning for GWorld...");
    g_gworld_ptr = find_gworld();
    
    g_running = true;
    LOGI("[+] Main loop starting");
    
    // ESP output file
    const char* esp_output = "/data/local/tmp/esp_data.bin";
    
    while (g_running) {
        usleep(16666); // ~60 fps
        
        // Re-find GWorld if lost
        if (!g_gworld_ptr) {
            g_gworld_ptr = find_gworld();
            if (!g_gworld_ptr) continue;
        }
        
        // Read GWorld pointer
        UWorld* world = safe_read<UWorld*>(g_gworld_ptr);
        if (!is_valid_ptr(world)) {
            g_gworld_ptr = 0; // Lost it, re-scan next frame
            continue;
        }
        
        // Get local player
        LocalPlayerInfo local = GetLocalPlayer(world);
        if (!local.valid) continue;
        
        // Get camera
        CameraData cam = GetCameraData(local.playerController);
        if (!cam.valid) continue;
        
        // Get GameState → PlayerArray
        AGameStateBase* gameState = world->GameState;
        if (!is_valid_ptr(gameState)) continue;
        
        TArray<void*>& playerArray = gameState->PlayerArray;
        if (!is_valid_ptr(playerArray.Data)) continue;
        if (playerArray.Count < 1 || playerArray.Count > 200) continue;
        
        // Lobby/Island check
        if (playerArray.Count < g_cfg.min_players) continue;
        
        // Process players
        std::vector<PlayerEspData> espPlayers;
        float closestAimDist = FLT_MAX;
        FVector closestHeadWorld = {0, 0, 0};
        bool hasAimTarget = false;
        
        for (int i = 0; i < playerArray.Count; i++) {
            ASTExtraPlayerState* ps = (ASTExtraPlayerState*)playerArray.Data[i];
            if (!is_valid_ptr(ps)) continue;
            
            // Get CharacterOwner from PlayerState
            ASTExtraBaseCharacter* character = ps->CharacterOwner;
            if (!is_valid_ptr(character)) continue;
            
            // Skip self
            if (character == local.myPawn) continue;
            
            // Check alive (from ASTExtraCharacter parent)
            ASTExtraCharacter* stChar = (ASTExtraCharacter*)character;
            if (stChar->IsDead()) continue;
            if (stChar->Health <= 0.0f) continue;
            
            // Get TeamID (from AUAECharacter)
            AUAECharacter* uaeChar = (AUAECharacter*)character;
            int32_t theirTeam = uaeChar->TeamID;
            bool isEnemy = (theirTeam != local.myTeamID);
            
            // Get character position
            FVector actorLoc = GetActorLocation((AActor*)character);
            float distance = (actorLoc - cam.location).Size() / 100.0f; // Convert to meters
            
            // Skip if too far
            if (distance > g_cfg.esp_max_dist) continue;
            
            // Get mesh for bone positions
            ACharacter* ch = (ACharacter*)character;
            USkeletalMeshComponent* mesh = ch->Mesh;
            if (!is_valid_ptr(mesh)) continue;
            
            // Get head and foot positions
            FVector headWorld = GetBoneWorldPos(mesh, BONE_HEAD);
            FVector footWorld = GetBoneWorldPos(mesh, BONE_ROOT);
            
            // WorldToScreen for head and foot
            FVector2D headScreen, footScreen, centerScreen;
            bool headOnScreen = WorldToScreen(headWorld, cam.location, cam.rotation, cam.fov, headScreen);
            bool footOnScreen = WorldToScreen(footWorld, cam.location, cam.rotation, cam.fov, footScreen);
            
            if (!headOnScreen && !footOnScreen) continue;
            
            // Build ESP data
            PlayerEspData esp = {};
            esp.headScreenX = headScreen.X;
            esp.headScreenY = headScreen.Y;
            esp.footScreenX = footScreen.X;
            esp.footScreenY = footScreen.Y;
            esp.screenX = (headScreen.X + footScreen.X) / 2.0f;
            esp.screenY = (headScreen.Y + footScreen.Y) / 2.0f;
            esp.health = stChar->Health;
            esp.healthMax = stChar->HealthMax;
            esp.distance = distance;
            esp.teamId = theirTeam;
            esp.isEnemy = isEnemy;
            
            // Calculate all bone positions for skeleton ESP
            memset(esp.boneValid, 0, sizeof(esp.boneValid));
            for (int b = 0; b < 58; b++) {
                FVector boneWorld = GetBoneWorldPos(mesh, b);
                FVector2D boneScreen;
                if (WorldToScreen(boneWorld, cam.location, cam.rotation, cam.fov, boneScreen)) {
                    esp.boneScreen[b][0] = boneScreen.X;
                    esp.boneScreen[b][1] = boneScreen.Y;
                    esp.boneValid[b] = true;
                }
            }
            
            espPlayers.push_back(esp);
            
            // Aimbot: find closest enemy head to crosshair
            if (isEnemy && g_cfg.aim_enabled && headOnScreen) {
                float screenCenterX = g_cfg.screen_width  * 0.5f;
                float screenCenterY = g_cfg.screen_height * 0.5f;
                float dx = headScreen.X - screenCenterX;
                float dy = headScreen.Y - screenCenterY;
                float aimDist = sqrtf(dx*dx + dy*dy);
                
                if (aimDist < closestAimDist && aimDist < g_cfg.aim_fov) {
                    closestAimDist = aimDist;
                    closestHeadWorld = headWorld;
                    hasAimTarget = true;
                }
            }
            
            // Apply large hitbox to enemies
            if (isEnemy && g_cfg.hitbox_enabled) {
                ApplyLargeHitbox(character);
            }
        }
        
        // Execute aimbot on closest target
        if (hasAimTarget) {
            DoAimbot(local.playerController, closestHeadWorld, cam);
        }
        
        // Write ESP data to binary file (overlay app reads this)
        int fd = open(esp_output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd >= 0) {
            int32_t count = (int32_t)espPlayers.size();
            write(fd, &count, sizeof(count));
            if (count > 0) {
                write(fd, espPlayers.data(), count * sizeof(PlayerEspData));
            }
            close(fd);
        }
    }
    
    LOGI("[*] Main thread exiting");
    return nullptr;
}

// ============================================================================
// SCREEN SIZE DETECTION
// ============================================================================

static void detect_screen_size() {
    FILE* fp = popen("wm size 2>/dev/null", "r");
    if (fp) {
        char buf[128];
        if (fgets(buf, sizeof(buf), fp)) {
            int w, h;
            if (sscanf(buf, "Physical size: %dx%d", &w, &h) == 2) {
                g_cfg.screen_width  = (float)w;
                g_cfg.screen_height = (float)h;
                LOGI("[+] Screen: %dx%d", w, h);
            }
        }
        pclose(fp);
    }
}

// ============================================================================
// CONFIG LOADER
// ============================================================================

static void load_config() {
    FILE* fp = fopen("/data/local/tmp/bgmi_config.txt", "r");
    if (!fp) return;
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        float val;
        int ival;
        if (sscanf(line, "aim_fov=%f", &val) == 1) g_cfg.aim_fov = val;
        else if (sscanf(line, "aim_smooth=%f", &val) == 1) g_cfg.aim_smooth = val;
        else if (sscanf(line, "aim_bone=%d", &ival) == 1) g_cfg.aim_bone = ival;
        else if (sscanf(line, "hitbox_scale=%f", &val) == 1) g_cfg.hitbox_scale = val;
        else if (sscanf(line, "esp_max_dist=%f", &val) == 1) g_cfg.esp_max_dist = val;
        else if (sscanf(line, "aim_enabled=%d", &ival) == 1) g_cfg.aim_enabled = (ival != 0);
        else if (sscanf(line, "esp_enabled=%d", &ival) == 1) g_cfg.esp_enabled = (ival != 0);
        else if (sscanf(line, "hitbox_enabled=%d", &ival) == 1) g_cfg.hitbox_enabled = (ival != 0);
    }
    fclose(fp);
    LOGI("[+] Config loaded");
}

// ============================================================================
// ENTRY POINT — __attribute__((constructor))
// ============================================================================

__attribute__((constructor))
void _init() {
    LOGI("[*] ====================================");
    LOGI("[*] BGMI 4.4.0 Mod Loaded");
    LOGI("[*] SDK: NIKON 4.4.0 64-bit");
    LOGI("[*] Features: ESP + Aimbot + LargeHitbox");
    LOGI("[*] Bypasses: 809 verified patches");
    LOGI("[*] ====================================");
    
    detect_screen_size();
    load_config();
    
    pthread_t ptid;
    pthread_create(&ptid, nullptr, main_thread, nullptr);
    pthread_detach(ptid);
}
