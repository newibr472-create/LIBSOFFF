#pragma once
// ============================================================
// Settings.h - Mod Configuration System
// Loads/saves settings from OBB folder config file
// ============================================================

#include <fstream>
#include <string>
#include <unordered_map>
#include <sys/stat.h>

#define SETTINGS_PATH "/sdcard/Android/data/com.pubg.imobile/files/mod_settings.cfg"
#define STATUS_PATH "/sdcard/Android/data/com.pubg.imobile/files/bypass_status.txt"

struct ModSettings {
    // Anti-Cheat Bypass Categories
    bool bAntiCheatBypass = true;
    bool bAnogsPatches = true;
    bool bHdmpvePatches = true;
    bool bHdmpvecorePatches = true;
    bool bTDataMasterPatches = true;
    bool bSignerPatches = true;
    bool bUE4ACPatches = true;
    
    // Game Feature Hooks
    bool bSkinChanger = true;
    bool bFPSUnlock = true;
    bool bNoRecoil = false;
    bool bNoShake = false;
    bool bNoGrass = false;
    bool bFlashSpeed = false;
    bool bAimAssist = false;
    bool bHDRUnlock = true;
    bool bIPadFOV = false;
    
    // Lua Features
    bool bGuestLogin = true;
    
    // Debug
    bool bVerboseLog = false;
};

static ModSettings g_Settings;
static FILE* g_StatusFile = nullptr;

void logStatus(const char* fmt, ...) {
    if (!g_StatusFile) {
        g_StatusFile = fopen(STATUS_PATH, "w");
        if (!g_StatusFile) return;
        fprintf(g_StatusFile, "=== BYPASS STATUS LOG ===\n");
        fprintf(g_StatusFile, "Package: com.pubg.imobile\n");
        fprintf(g_StatusFile, "Build: arm64-v8a\n\n");
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(g_StatusFile, fmt, args);
    fprintf(g_StatusFile, "\n");
    fflush(g_StatusFile);
    va_end(args);
}

void closeStatusLog() {
    if (g_StatusFile) {
        fprintf(g_StatusFile, "\n=== END LOG ===\n");
        fclose(g_StatusFile);
        g_StatusFile = nullptr;
    }
}

void createDefaultSettings() {
    // Create directory if needed
    mkdir("/sdcard/Android/data/com.pubg.imobile/files", 0777);
    
    FILE* f = fopen(SETTINGS_PATH, "w");
    if (!f) return;
    
    fprintf(f, "# ========================================\n");
    fprintf(f, "# BGMI Mod Settings - Edit to toggle features\n");
    fprintf(f, "# 1 = ON, 0 = OFF\n");
    fprintf(f, "# ========================================\n\n");
    fprintf(f, "[AntiCheat]\n");
    fprintf(f, "AntiCheatBypass=1\n");
    fprintf(f, "AnogsPatches=1\n");
    fprintf(f, "HdmpvePatches=1\n");
    fprintf(f, "HdmpvecorePatches=1\n");
    fprintf(f, "TDataMasterPatches=1\n");
    fprintf(f, "SignerPatches=1\n");
    fprintf(f, "UE4ACPatches=1\n\n");
    fprintf(f, "[GameFeatures]\n");
    fprintf(f, "SkinChanger=1\n");
    fprintf(f, "FPSUnlock=1\n");
    fprintf(f, "NoRecoil=0\n");
    fprintf(f, "NoShake=0\n");
    fprintf(f, "NoGrass=0\n");
    fprintf(f, "FlashSpeed=0\n");
    fprintf(f, "AimAssist=0\n");
    fprintf(f, "HDRUnlock=1\n");
    fprintf(f, "IPadFOV=0\n\n");
    fprintf(f, "[Lua]\n");
    fprintf(f, "GuestLogin=1\n\n");
    fprintf(f, "[Debug]\n");
    fprintf(f, "VerboseLog=0\n");
    fclose(f);
}

bool parseBool(const std::string& val) {
    return val == "1" || val == "true" || val == "yes";
}

void loadSettings() {
    FILE* f = fopen(SETTINGS_PATH, "r");
    if (!f) {
        createDefaultSettings();
        LOGI("[@OWNERHUBEE] Created default settings at %s", SETTINGS_PATH);
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        std::string l(line);
        // Skip comments and empty lines
        if (l.empty() || l[0] == '#' || l[0] == '[' || l[0] == '\n') continue;
        
        size_t eq = l.find('=');
        if (eq == std::string::npos) continue;
        
        std::string key = l.substr(0, eq);
        std::string val = l.substr(eq + 1);
        // Trim newline
        while (!val.empty() && (val.back() == '\n' || val.back() == '\r'))
            val.pop_back();
        
        if (key == "AntiCheatBypass") g_Settings.bAntiCheatBypass = parseBool(val);
        else if (key == "AnogsPatches") g_Settings.bAnogsPatches = parseBool(val);
        else if (key == "HdmpvePatches") g_Settings.bHdmpvePatches = parseBool(val);
        else if (key == "HdmpvecorePatches") g_Settings.bHdmpvecorePatches = parseBool(val);
        else if (key == "TDataMasterPatches") g_Settings.bTDataMasterPatches = parseBool(val);
        else if (key == "SignerPatches") g_Settings.bSignerPatches = parseBool(val);
        else if (key == "UE4ACPatches") g_Settings.bUE4ACPatches = parseBool(val);
        else if (key == "SkinChanger") g_Settings.bSkinChanger = parseBool(val);
        else if (key == "FPSUnlock") g_Settings.bFPSUnlock = parseBool(val);
        else if (key == "NoRecoil") g_Settings.bNoRecoil = parseBool(val);
        else if (key == "NoShake") g_Settings.bNoShake = parseBool(val);
        else if (key == "NoGrass") g_Settings.bNoGrass = parseBool(val);
        else if (key == "FlashSpeed") g_Settings.bFlashSpeed = parseBool(val);
        else if (key == "AimAssist") g_Settings.bAimAssist = parseBool(val);
        else if (key == "HDRUnlock") g_Settings.bHDRUnlock = parseBool(val);
        else if (key == "IPadFOV") g_Settings.bIPadFOV = parseBool(val);
        else if (key == "GuestLogin") g_Settings.bGuestLogin = parseBool(val);
        else if (key == "VerboseLog") g_Settings.bVerboseLog = parseBool(val);
    }
    fclose(f);
    LOGI("[@OWNERHUBEE] Settings loaded from %s", SETTINGS_PATH);
}
