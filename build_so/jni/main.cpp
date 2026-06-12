/*
 * BGMI 4.4.0 ARM64 — Modern Minimal Library v2.1
 * 
 * CRASH FIX: Match loading crash
 * - PostRender hook now has world-transition detection
 * - K2_DrawLine wrapped with signal safety
 * - Frame skip during map transitions
 * - ESP only activates when match is FULLY loaded
 */

#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <setjmp.h>

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

// Safety: track world pointer to detect map transitions
static uintptr_t g_LastWorld = 0;
static int       g_FramesSinceWorldChange = 0;
static int       g_TotalFrames = 0;

// Crash recovery
static volatile bool g_CrashInHook = false;
static sigjmp_buf g_JmpBuf;

// ============================================================================
// Signal handler for crash recovery inside hook
// If our ESP code crashes, we recover and disable ESP instead of killing game
// ============================================================================
static struct sigaction g_OldSigsegv;
static struct sigaction g_OldSigbus;

static void crash_handler(int sig, siginfo_t *info, void *ctx) {
    if (g_CrashInHook) {
        // We crashed inside our hook — recover
        g_CrashInHook = false;
        esp::cfg.enabled = false; // Disable ESP permanently
        siglongjmp(g_JmpBuf, 1);
    }
    // Not our crash — pass to original handler
    if (sig == SIGSEGV && g_OldSigsegv.sa_sigaction) {
        g_OldSigsegv.sa_sigaction(sig, info, ctx);
    } else if (sig == SIGBUS && g_OldSigbus.sa_sigaction) {
        g_OldSigbus.sa_sigaction(sig, info, ctx);
    }
}

static void install_crash_guard() {
    struct sigaction sa{};
    sa.sa_sigaction = crash_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, &g_OldSigsegv);
    sigaction(SIGBUS, &sa, &g_OldSigbus);
}

// ============================================================================
// PostRender hook — SAFE version
// ============================================================================
typedef void *(*PostRender_t)(void *viewport, void *canvas);
static PostRender_t g_OrigPostRender = nullptr;

static void *hook_PostRender(void *viewport, void *canvas) {
    g_TotalFrames++;
    
    // Skip first 60 frames after hook install (let rendering stabilize)
    if (g_TotalFrames < 60) goto call_original;
    
    // Check if canvas is valid
    if (!canvas || !mem::IsValid(canvas)) goto call_original;
    
    // Check world stability (detect map transitions)
    {
        uintptr_t curWorld = 0;
        mem::Read((void *)(g_UE4 + Off::GWorld), &curWorld, sizeof(uintptr_t));
        
        if (curWorld != g_LastWorld) {
            // World changed! Map transition happening
            g_LastWorld = curWorld;
            g_FramesSinceWorldChange = 0;
            goto call_original; // Don't render during transition
        }
        
        g_FramesSinceWorldChange++;
        
        // Wait 300 frames (~5 seconds at 60fps) after world change before rendering
        // This ensures match is FULLY loaded
        if (g_FramesSinceWorldChange < 300) goto call_original;
        
        // World must be valid
        if (!curWorld || !mem::IsValid((void *)curWorld)) goto call_original;
    }
    
    // ESP rendering with crash guard
    if (esp::cfg.enabled || aimbot::cfg.enabled) {
        g_CrashInHook = true;
        if (sigsetjmp(g_JmpBuf, 1) == 0) {
            // Normal path — render ESP
            esp::Render((uintptr_t)canvas, g_UE4);
            
            // Aimbot (uses data from ESP)
            if (aimbot::cfg.enabled && aimbot::g_ClosestTarget) {
                uintptr_t pWorld = 0;
                mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
                if (pWorld && mem::IsValid((void *)pWorld)) {
                    uintptr_t gi = 0;
                    mem::Read((void *)(pWorld + Off::OwningGameInstance), &gi, sizeof(uintptr_t));
                    if (gi && mem::IsValid((void *)gi)) {
                        TArray<uintptr_t> lp;
                        mem::Read((void *)(gi + Off::LocalPlayers), &lp, sizeof(lp));
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
        } else {
            // Crash recovered — ESP disabled
            LOGE("[!] Crash in ESP — disabled permanently");
        }
        g_CrashInHook = false;
    }

call_original:
    if (g_OrigPostRender) return g_OrigPostRender(viewport, canvas);
    return nullptr;
}

// ============================================================================
// Shadow VTable hook
// ============================================================================
static bool InstallPostRenderHook() {
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
    
    // ULocalPlayer::ViewportClient — try multiple offsets
    uintptr_t viewportClient = 0;
    const uint32_t vpOffsets[] = {0x0078, 0x0070, 0x0080, 0x0068};
    for (uint32_t off : vpOffsets) {
        uintptr_t tmp = 0;
        mem::Read((void *)(localPlayer + off), &tmp, sizeof(uintptr_t));
        if (tmp && mem::IsValid((void *)tmp)) {
            // Verify it has a vtable
            void **vt = nullptr;
            mem::Read((void *)tmp, &vt, sizeof(void **));
            if (vt && mem::IsValid(vt)) {
                viewportClient = tmp;
                LOGI("[+] ViewportClient at offset 0x%x = 0x%lx", off, (unsigned long)tmp);
                break;
            }
        }
    }
    
    if (!viewportClient) return false;
    
    // Read original VTable pointer
    void **origVTable = nullptr;
    mem::Read((void *)viewportClient, &origVTable, sizeof(void **));
    if (!origVTable || !mem::IsValid(origVTable)) return false;
    
    // Verify vtable entry at index 134 looks like a function pointer
    void *postRenderFunc = nullptr;
    mem::Read(&origVTable[134], &postRenderFunc, sizeof(void *));
    if (!postRenderFunc || !mem::IsValid(postRenderFunc)) {
        LOGE("[-] VTable[134] invalid, trying nearby indices...");
        // Try nearby indices (version differences)
        for (int idx = 130; idx <= 140; idx++) {
            mem::Read(&origVTable[idx], &postRenderFunc, sizeof(void *));
            if (postRenderFunc && mem::IsValid(postRenderFunc)) {
                LOGI("[?] VTable[%d] valid: %p", idx, postRenderFunc);
            }
        }
        return false;
    }
    
    // Clone VTable (256 entries)
    g_ShadowVTable = mem::CloneVTable(origVTable, 256);
    if (!g_ShadowVTable) return false;
    
    // Hook PostRender on shadow
    g_OrigPostRender = (PostRender_t)g_ShadowVTable[134];
    g_ShadowVTable[134] = (void *)hook_PostRender;
    
    // Point object to shadow vtable
    mem::SwapVTable((void *)viewportClient, g_ShadowVTable);
    
    // Record initial world
    g_LastWorld = pWorld;
    g_FramesSinceWorldChange = 0;
    
    LOGI("[+] PostRender shadow-hooked at [134], orig=%p", (void *)g_OrigPostRender);
    return true;
}

// ============================================================================
// Main worker thread
// ============================================================================
static void *worker_thread(void *) {
    LOGI("[*] Worker started");
    
    // Install crash guard
    install_crash_guard();
    
    // Phase 1: Wait for ALL critical libraries
    int waitCount = 0;
    while (!mem::GetBase("libUE4.so")) {
        usleep(500000);
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
    
    // libanort — optional
    for (int i = 0; i < 10; i++) {
        if (mem::GetBase("libanort.so")) { LOGI("[+] libanort found"); break; }
        usleep(500000);
    }
    
    // Phase 2: Let libraries fully initialize
    sleep(12);
    LOGI("[*] Post-init delay done");
    
    // Phase 3: Apply bypasses
    bypass::Apply();
    LOGI("[+] Bypasses active");
    
    // Phase 4: Wait for game world to be ready
    waitCount = 0;
    while (waitCount < 180) { // Max 3 minutes
        uintptr_t pWorld = 0;
        mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
        if (pWorld && mem::IsValid((void *)pWorld)) {
            LOGI("[+] GWorld valid = 0x%lx", (unsigned long)pWorld);
            break;
        }
        sleep(1);
        waitCount++;
    }
    
    // Phase 5: Wait for LocalPlayer to exist (means we're in lobby)
    sleep(30); // Minimum wait
    
    waitCount = 0;
    while (waitCount < 60) {
        uintptr_t pWorld = 0;
        mem::Read((void *)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
        if (!pWorld || !mem::IsValid((void *)pWorld)) { sleep(2); waitCount++; continue; }
        
        uintptr_t gi = 0;
        mem::Read((void *)(pWorld + Off::OwningGameInstance), &gi, sizeof(uintptr_t));
        if (!gi || !mem::IsValid((void *)gi)) { sleep(2); waitCount++; continue; }
        
        TArray<uintptr_t> lp;
        mem::Read((void *)(gi + Off::LocalPlayers), &lp, sizeof(lp));
        if (lp.IsValid() && lp.Count > 0) {
            LOGI("[+] LocalPlayer ready (waited %d extra sec)", waitCount * 2);
            break;
        }
        sleep(2);
        waitCount++;
    }
    
    // Extra stability wait
    sleep(5);
    
    // Phase 6: Install PostRender hook
    int hookAttempts = 0;
    while (!g_Hooked && hookAttempts < 20) {
        if (InstallPostRenderHook()) {
            g_Hooked = true;
            LOGI("[+] ===== HOOK ACTIVE =====");
        } else {
            LOGI("[.] Hook attempt %d failed, retrying...", hookAttempts + 1);
            sleep(3);
            hookAttempts++;
        }
    }
    
    if (!g_Hooked) {
        LOGE("[-] Hook failed after %d attempts", hookAttempts);
    }
    
    LOGI("[*] Worker done");
    return nullptr;
}

// ============================================================================
// ENTRY
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
