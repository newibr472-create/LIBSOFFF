/*
 * BGMI 4.4.0 ARM64 — Modern Minimal Library
 * 
 * Architecture:
 * - Entry: __attribute__((constructor)) → pthread (instant return)
 * - Memory: raw syscalls (270/271) — no libc hooks visible to AC
 * - Library detection: dl_iterate_phdr (no /proc/self/maps)
 * - VTable hook: Shadow VTable technique (original vtable untouched)
 * - ESP: K2_DrawLine via game's own canvas (no overlay)
 * - Aimbot: Humanized acceleration curve + jitter
 * - Bypass: NOP strategy for libUE4, RET0 for libanogs
 * 
 * NO login, NO GUI menu, NO curl, NO external deps
 */

#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include <cstring>
#include <cstdlib>

#include "core/memory.h"
#include "core/bypass.h"
#include "sdk/types.h"
#include "features/esp.h"
#include "features/aimbot.h"

#define TAG "M"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ============================================================================
// Global state
// ============================================================================
static uintptr_t g_UE4 = 0;
static void    **g_ShadowVTable = nullptr;
static bool      g_Hooked = false;

// ============================================================================
// PostRender hook — this is where ESP/Aimbot run every frame
// ============================================================================
typedef void *(*PostRender_t)(void *viewport, void *canvas);
static PostRender_t g_OrigPostRender = nullptr;

static void *hook_PostRender(void *viewport, void *canvas) {
    if (canvas && mem::IsValid(canvas) && g_UE4) {
        // ESP renders enemies
        esp::Render((uintptr_t)canvas, g_UE4);
        
        // Aimbot applies rotation (uses data from ESP pass)
        // We need myPC and camMgr — get them fresh
        uintptr_t pWorld = 0;
        mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
        if (pWorld && mem::IsValid((void *)pWorld)) {
            uintptr_t gameInst = 0;
            mem::Read((void *)(pWorld + Off::OwningGameInstance), &gameInst, sizeof(uintptr_t));
            if (gameInst && mem::IsValid((void *)gameInst)) {
                TArray<uintptr_t> lp;
                mem::Read((void *)(gameInst + Off::LocalPlayers), &lp, sizeof(lp));
                if (lp.IsValid()) {
                    uintptr_t localP = 0;
                    mem::Read(&lp.Data[0], &localP, sizeof(uintptr_t));
                    if (localP && mem::IsValid((void *)localP)) {
                        uintptr_t myPC = 0;
                        mem::Read((void *)(localP + Off::PlayerController), &myPC, sizeof(uintptr_t));
                        if (myPC && mem::IsValid((void *)myPC)) {
                            uintptr_t camMgr = 0;
                            mem::Read((void *)(myPC + Off::PlayerCameraMgr), &camMgr, sizeof(uintptr_t));
                            if (camMgr && mem::IsValid((void *)camMgr)) {
                                aimbot::Apply(myPC, camMgr, g_UE4);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Call original
    if (g_OrigPostRender) return g_OrigPostRender(viewport, canvas);
    return nullptr;
}

// ============================================================================
// Shadow VTable hook installation
// 
// TECHNIQUE: Instead of modifying the game's VTable directly (which AC scans),
// we CLONE the vtable, hook our entry on the clone, then point the viewport
// object to our clone. The original vtable stays pristine.
// ============================================================================
static bool InstallPostRenderHook() {
    // Find GameViewportClient via GWorld chain
    uintptr_t pWorld = 0;
    mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
    if (!pWorld || !mem::IsValid((void *)pWorld)) return false;
    
    uintptr_t gameInst = 0;
    mem::Read((void *)(pWorld + Off::OwningGameInstance), &gameInst, sizeof(uintptr_t));
    if (!gameInst || !mem::IsValid((void *)gameInst)) return false;
    
    TArray<uintptr_t> localPlayers;
    mem::Read((void *)(gameInst + Off::LocalPlayers), &localPlayers, sizeof(localPlayers));
    if (!localPlayers.IsValid()) return false;
    
    uintptr_t localPlayer = 0;
    mem::Read(&localPlayers.Data[0], &localPlayer, sizeof(uintptr_t));
    if (!localPlayer || !mem::IsValid((void *)localPlayer)) return false;
    
    uintptr_t myPC = 0;
    mem::Read((void *)(localPlayer + Off::PlayerController), &myPC, sizeof(uintptr_t));
    if (!myPC || !mem::IsValid((void *)myPC)) return false;
    
    // PlayerController → GameViewportClient is not directly accessible
    // Walk: UWorld → GameViewportClient (UEngine→GameViewport from SDK)
    // Alternative: We can find it via UEngine in GUObjectArray
    // Simplest: From GWorld → OwningGameInstance → ...
    // Actually in UE4 mobile: GEngine->GameViewport
    // Let's try the direct path:
    // GEngine is at GWorld+0x-relative... 
    // Better approach: scan the vtable holder directly from GNativeAndroidApp
    
    // The reference code uses: UObject::FindObject<UGameViewportClient>(...)
    // We don't have FindObject compiled — let's use pointer chain:
    // GWorld → LevelOwner → GameViewport (not standard but works)
    
    // SAFEST: Read viewport from GWorld + known offset for BGMI 4.4.0
    // In BGMI, GEngine→GameViewport→Viewport pattern:
    // Let's walk from GameInstance → LocalPlayers[0] → ViewportClient
    // ULocalPlayer::ViewportClient = 0x0078 (standard UE4)
    
    uintptr_t viewportClient = 0;
    mem::Read((void *)(localPlayer + 0x0078), &viewportClient, sizeof(uintptr_t));
    if (!viewportClient || !mem::IsValid((void *)viewportClient)) {
        // Fallback: try 0x0070
        mem::Read((void *)(localPlayer + 0x0070), &viewportClient, sizeof(uintptr_t));
        if (!viewportClient || !mem::IsValid((void *)viewportClient)) return false;
    }
    
    LOGI("[+] ViewportClient = 0x%lx", (unsigned long)viewportClient);
    
    // Read original VTable pointer
    void **origVTable = nullptr;
    mem::Read((void *)viewportClient, &origVTable, sizeof(void **));
    if (!origVTable || !mem::IsValid(origVTable)) return false;
    
    // Clone VTable (allocate 256 entries — more than enough)
    static const int VTABLE_SIZE = 256;
    g_ShadowVTable = mem::CloneVTable(origVTable, VTABLE_SIZE);
    if (!g_ShadowVTable) return false;
    
    // PostRender index = 134 (200 - 66)
    static const int POST_RENDER_IDX = 134;
    
    // Hook on shadow
    g_OrigPostRender = (PostRender_t)g_ShadowVTable[POST_RENDER_IDX];
    g_ShadowVTable[POST_RENDER_IDX] = (void *)hook_PostRender;
    
    // Point object to shadow vtable
    mem::SwapVTable((void *)viewportClient, g_ShadowVTable);
    
    LOGI("[+] PostRender shadow-hooked at [%d]", POST_RENDER_IDX);
    return true;
}

// ============================================================================
// Large Hitbox (write to all enemies each iteration)
// Applied once per match entry — increases collision angles
// ============================================================================
static void ApplyLargeHitbox() {
    // This is applied per-actor in ESP loop via the bone/collision system
    // The actual hitbox expansion is done by writing collision angles
    // We set this up in the config — ESP handles it per-actor
    LOGI("[+] Large hitbox enabled (handled per-actor)");
}

// ============================================================================
// Main worker thread
// ============================================================================
static void *worker_thread(void *) {
    LOGI("[*] Worker started");
    
    // ========== Phase 1: Wait for ALL critical libraries ==========
    int waitCount = 0;
    while (!mem::GetBase("libUE4.so")) {
        usleep(500000); // 500ms
        if (++waitCount > 120) { LOGE("[-] libUE4 timeout"); return nullptr; }
    }
    g_UE4 = mem::GetBase("libUE4.so");
    LOGI("[+] libUE4 = 0x%lx", (unsigned long)g_UE4);
    
    waitCount = 0;
    while (!mem::GetBase("libanogs.so")) {
        usleep(500000);
        if (++waitCount > 60) { LOGE("[-] libanogs timeout"); return nullptr; }
    }
    LOGI("[+] libanogs found");
    
    // libanort — optional, don't block forever
    for (int i = 0; i < 10; i++) {
        if (mem::GetBase("libanort.so")) { LOGI("[+] libanort found"); break; }
        usleep(500000);
    }
    
    // ========== Phase 2: Let libraries FULLY initialize ==========
    // This is CRITICAL — libraries run their own constructors after dlopen
    // If we patch too early, we corrupt half-initialized state → crash
    sleep(10);
    LOGI("[*] Post-init delay done");
    
    // ========== Phase 3: Apply anti-cheat bypasses ==========
    bypass::Apply();
    LOGI("[+] Bypasses active");
    
    // ========== Phase 4: Wait for game engine to be ready ==========
    // Check GWorld pointer becomes valid (game has loaded a map)
    waitCount = 0;
    while (waitCount < 120) { // Max 2 minutes
        uintptr_t pWorld = 0;
        mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
        if (pWorld && mem::IsValid((void *)pWorld)) {
            LOGI("[+] GWorld valid");
            break;
        }
        sleep(1);
        waitCount++;
    }
    
    // ========== Phase 5: Wait for lobby/match ==========
    // Game needs time to fully initialize player controllers
    // Reference uses sleep(55) — we use 45 with verification
    sleep(45);
    
    // Verify LocalPlayer exists
    waitCount = 0;
    while (waitCount < 30) {
        uintptr_t pWorld = 0;
        mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
        if (!pWorld || !mem::IsValid((void *)pWorld)) { sleep(2); waitCount++; continue; }
        
        uintptr_t gi = 0;
        mem::Read((void *)(pWorld + Off::OwningGameInstance), &gi, sizeof(uintptr_t));
        if (!gi || !mem::IsValid((void *)gi)) { sleep(2); waitCount++; continue; }
        
        TArray<uintptr_t> lp;
        mem::Read((void *)(gi + Off::LocalPlayers), &lp, sizeof(lp));
        if (lp.IsValid() && lp.Count > 0) {
            LOGI("[+] LocalPlayer ready");
            break;
        }
        sleep(2);
        waitCount++;
    }
    
    // ========== Phase 6: Install PostRender hook ==========
    int hookAttempts = 0;
    while (!g_Hooked && hookAttempts < 15) {
        if (InstallPostRenderHook()) {
            g_Hooked = true;
            LOGI("[+] ===== ESP ACTIVE =====");
        } else {
            sleep(3);
            hookAttempts++;
        }
    }
    
    if (!g_Hooked) {
        LOGE("[-] Hook failed after all attempts");
    }
    
    // Large hitbox config
    ApplyLargeHitbox();
    
    LOGI("[*] Worker done — features running in PostRender");
    return nullptr;
}

// ============================================================================
// ENTRY POINT — minimum work, instant return
// IBox Loader / APK injection loads this .so → constructor runs
// ============================================================================
__attribute__((constructor))
void __attribute__((used)) entry() {
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, worker_thread, nullptr);
    pthread_attr_destroy(&attr);
}
