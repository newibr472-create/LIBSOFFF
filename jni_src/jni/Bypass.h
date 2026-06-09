#include <jni.h>
#include <string>
#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <unwind.h>
#include <libgen.h>
#include <stdint.h>
#include <functional>
#include <sys/stat.h>
#include <fcntl.h>
#include <android/log.h>
#include <random>

#include "Main/Tools.h"
#include "Main/Logger.h"
#include "Main/oxorany.h"
#include "Main/Utils.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include "Main/Macros.h"

#define targetLibName oxorany("libUE4.so")

//#define <OWERHUBEE>
class LODWORD;
#define LODWORD long
#define _QWORD long
#define _DWORD long
#define _BYTE long
#define _WORD long
using namespace std;
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long

typedef long long int64;
typedef short int16;
uintptr_t ANOGS;
uintptr_t UE5;
DWORD EGLBase = 0;
DWORD EGLSize = 0;
DWORD EGLAlloc = 0;
DWORD libcBase = 0;
DWORD libcSize = 0;
DWORD libcAlloc = 0;
DWORD libUE4Base = 0;
DWORD UE4Base = 0;
DWORD libanogsBase = 0;
DWORD libanortBase = 0;
DWORD libanogsAlloc = 0;
DWORD libUE4Alloc = 0;
unsigned int libanogsSize = 0;
unsigned int libUE4Size = 0;
DWORD NewBase = 0;
unsigned char shellcode[] = {0x00, 0x00, 0x80, 0xD2, 0xC0, 0x03, 0x5F, 0xD6};

#define anogs OBFUSCATE("libanogs.so")

// ============================================================
// Include Integrated Modules
// ============================================================
#include "AntiCheat_Bypass.h"
#include "GuestLogin.h"
#include "LuaSettingsMenu.h"

// ============================================================
// Permission Restore
// ============================================================
void chRestore() {
    char mode[] = "0777";
    char *path = "/data/data/com.pubg.imobile/files/ano_tmp";
    char *path2 = "/data/data/com.pubg.imobile/files";
    int m = strtol(mode, 0, 8);
    chmod(path, m);
    chmod(path2, m);
    LOGI(OBFUSCATE("permissions restored"));
}

// ============================================================
// Skin Changer (SRCWALAA)
// ============================================================
size_t SRCWALAA(char *s) {
    static const std::unordered_map<std::string, std::string> replacements = {
       
        {"1405388", "1407758"}, //new ultimet 
        {"1405586", "1405870"},    // BLOOD RAVEN
        {"1405647", "1406971"},    //  Marmoris X-Suit 
        {"1405196", "1407841"},   // new hani bagger 
        {"1405807", "1405983"},    // Poseidon X-Suit
        {"1405548", "1406469"},   //Golden Pharaoh X-Suit (7-Star) | 
        {"1405113", "1407512"},   // Arcane Jester X-Suit (6-Star)
        {"403041", "1407142"},    // Silvanus X-Suit (7-Star)
        {"403224","1406727"}, //pri
        {"403006", "1406152"},    // Avalanche X-Suit
        {"1402154", "1405934"},   // Godzilla Set
        {"1402155", "1410826"},    // Mask
        {"403042", "1405092"},   // Vampyra Set
        {"403033", "1407512"},//Dravion X-Suit (7-Star)
        {"403211","1407695"},// Winter Highness Set   
        {"403163", "1406573"}, // Wukong Prime
        {"403042", "1407695"}, // Macabre Valentine Set
        
       {"502001","1502001014"}, // helmet
       {"502002","1502002014"}, // helmet
       {"502003","1502003014"}, // helmet
      
	   
{"501001", "1501003688"}, // Backpack (Lv. 1) 
{"501002", "1501003688"}, // Backpack (Lv. 2) 
{"501003", "1501003688"}, // Backpack (Lv. 3) 

// --- DACIA (Sedan) ---
{"1903001", "1903200"}, // Bentley Flying Spur Mulliner (Nebula)
{"1903002", "1903200"},
{"1903003", "1903200"},
// --- COUPE RB (Sports Car) ---
{"1961001", "1961151"}, // Bugatti Bolide (Chromium - Mirror Finish)
{"1961002", "1961151"},
{"1961003", "1961151"},

// --- UAZ (Off-Road) ---
{"1908001", "1908094"}, // Bentley Bentayga Azure (Galaxy Glitter)
{"1908002", "1908094"}, 
{"1908002", "1908094"}, 
// --- MIRADO (Open Top) ---
{"1915001", "1915008"}, // Bentley Continental GTC Mulliner (Holocrystal)
{"1915002", "1915008"}, 
{"1915003", "1915008"}, 
{"1915004", "1915008"}, 
{"1915005", "1915008"}, 


        {"1907001","1907054"},//Buggy
        {"1907002","1907054"},//Buggy
        {"1907003","1907054"},//Buggy
        {"1907004","1907054"},//Buggy
        {"1907005","1907054"},//Buggy
        {"1907006","1907054"},//Buggy
		//{"3001023","1601071"},
		
		
// --- MOTORCYCLE (2-Seat) ---
{"1901001", "1901073"}, // DUCATI Panigale V4S
{"1901002", "1901092"}, // Indian FTR R Carbon
{"1901003", "1901077"}, // Bulma's Motorcycle (Dragon Ball)

// --- MONSTER TRUCK ---
{"1953001", "1953012"}, 
{"1953002", "1953012"}, 
{"1953003", "1953012"}, 

// --- MINI BUS ---
{"1904001", "1904016"}, // NewJeans Minibus
{"1904001", "1904018"}, // Frieren: Beyond Journey's End Bus

/*
    {"10100400", "1101004236"}, // M416
    {"10100100", "1101001265"}, // AKM
    {"10100300", "1101003219"}, // SCAR-L
    {"10100500", "1101005098"}, // GROZA
    {"10100600", "1101006075"}, // AUG
    {"10100200", "1101002056"}, // M16A4
    {"10100800", "1101008154"}, // M762
    {"10110200", "1101102025"}, // ACE32
    {"10200200", "1102002424"}, // UMP45
    {"10200100", "1102001120"}, // UZI
    {"10200300", "1102003100"}, // VECTOR
    {"10200500", "1102005064"}, // BIZON
    {"10200400", "1102004018"}, // TOMMY
    {"10210500", "1102105012"}, // P90
    {"10300300", "1103003079"}, // AWM
    {"10300100", "1103001202"}, // Kar98K
    {"10300200", "1103002030"}, // M24
    {"10300700", "1103007020"}, // MK14
    {"10500200", "1105002091"}, // DP-28
    {"10500100", "1105001069"}, // M249
    {"10501000", "1105010019"}, // MG3
    {"10400400", "1104004035"}, // DBS
    {"10400300", "1104003037"}, // S12K
    {"10800400", "1108004356"}, // PAN
    {"10101200", "1101012033"}, // HONEY BADGER
    {"10100900", "1101009012"}, // FAMAS
    {"10301200", "1103012031"}, // AMR
    {"10300500", "1103005037"}, // MINI14
    {"10300400", "1103004058"}, // SKS
    {"10300900", "1103009028"}, // SLR
    {"10410100", "1104101001"},  // M1014
*/
//GLIDER SKIN//
         {"703001", "1401619"},//Glider parachute 🔅
         {"703001", "4151100"},// Glider 🔅
    };

auto it = replacements.find(s);
    if (it != replacements.end()) {
        strcpy(s, it->second.c_str());
        return strlen(s);
    }
    return strlen(s);
}



// Non-blocking sleep - doesn't freeze game process
void safe_sleep_ms(int ms) {
    int chunks = ms / 100;
    for (int i = 0; i < chunks; i++) {
        usleep(100000); // 100ms chunks
    }
}

void safe_sleep(int seconds) {
    safe_sleep_ms(seconds * 1000);
}

// ============================================================
// MAIN THREAD - Initialization Sequence
// ============================================================
void *KAMLESH_thread(void *) {
    // STEP 1: Load Settings
    loadSettings();
    LOGI("[@OWNERHUBEE] Settings loaded, starting bypass sequence...");
    
    // STEP 2: Wait for libraries to load
    UE5 = Tools::GetBaseAddress("libUE4.so");
    while (!UE5) {
        UE5 = Tools::GetBaseAddress("libUE4.so");
        usleep(500000); // 500ms non-blocking
    }
    
    ANOGS = Tools::GetBaseAddress("libanogs.so");
    while (!ANOGS) {
        ANOGS = Tools::GetBaseAddress("libanogs.so");
        usleep(500000); // 500ms non-blocking
    }
    
    do {
        usleep(500000); // 500ms non-blocking
    } while (!isLibraryLoaded(targetLibName));
    
    libanogsBase = findLibrary("libanogs.so");
    libUE4Base = findLibrary("libUE4.so");
    
    while (!isLibraryLoaded("libUE4.so")) { sleep(1); }
    while (!isLibraryLoaded("libanogs.so")) { sleep(1); }
    while (!isLibraryLoaded("libanort.so")) { sleep(1); }
    
    libanogsAlloc = (DWORD)malloc(libanogsSize);
    libUE4Alloc = (DWORD)malloc(libUE4Size);
    memcpy((void *)libanogsAlloc, (void *)libanogsBase, libanogsSize);
    memcpy((void *)libUE4Alloc, (void *)libUE4Base, libUE4Size);
    
    LOGI("[@OWNERHUBEE] ====================================================");
    LOGI("[@OWNERHUBEE] =====< LIBRARY LOADED - STARTING BYPASS >======");
    LOGI("[@OWNERHUBEE] ====================================================");
    LOGI("UE4 size: 0x%04X | ANOGS size: 0x%04X", libUE4Size, libanogsSize);

#if defined(__aarch64__)
    
    // STEP 3: Apply ALL anti-cheat bypasses FIRST
    initAllBypasses();
    
    // STEP 4: Wait for AC to settle
    safe_sleep(5);
    LOGI("[@OWNERHUBEE] AC settle period complete, applying game mods...");
    
    // STEP 5: Skin Changer Hook
    if (g_Settings.bSkinChanger) {
        HOOK_LIB_NO_ORIG("libUE4.so", "0xc4dfb90", SRCWALAA);
        LOGI("[@OWNERHUBEE] Skin changer hook installed");
        logStatus("[OK] Skin Changer HOOK @ 0xc4dfb90");
    }
    
    // STEP 6: Guest Login
    initGuestLogin();
    initSettingsMenu();
    
    // STEP 7: Restore permissions
    chRestore();
    
    // STEP 8: Wait for lobby + finalize
    safe_sleep(55);
    
    // STEP 9: Close status log
    closeStatusLog();
    
    LOGI("[@OWNERHUBEE] ====================================================");
    LOGI("[@OWNERHUBEE] ALL INITIALIZATION COMPLETE");
    extern bool g_BypassDone;
    g_BypassDone = true;
    LOGI("[@OWNERHUBEE] ====================================================");
    
    LOGI("[@OWNERHUBEE] Bypass complete! Chameli already running.");
    
#endif
    
    return NULL;
}

// ============================================================
// CONSTRUCTOR - Entry Point (.so load)
// ============================================================
__attribute__((constructor)) void mainload() {
    pthread_t ptid1, ptid2;
    
    // Step 1: Start Chameli FIRST (PostRender/ESP setup)
    extern void *Chameli(void *);
    pthread_create(&ptid1, NULL, Chameli, NULL);
    LOGI("[@OWNERHUBEE] Chameli thread started FIRST");
    
    // Step 2: Start KAMLESH_thread (bypass) after 30s delay
    // Using a lambda-style wrapper thread for the delay
    pthread_create(&ptid2, NULL, [](void*) -> void* {
        safe_sleep(30); // 30 sec non-blocking wait
        LOGI("[@OWNERHUBEE] 30s passed - starting KAMLESH_thread (bypass)");
        pthread_t kamlesh_tid;
        pthread_create(&kamlesh_tid, NULL, KAMLESH_thread, NULL);
        return NULL;
    }, NULL);
}
