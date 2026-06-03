#pragma once
#include "../Includes/Logger.h"
#include "../Includes/Utils.h"
#include "../Includes/obfuscate.h"
#include <sys/mman.h>
#include <chrono>

// ============= Globals for shadow memory CRC evasion =============
DWORD libanogsBase  = 0;
DWORD libanogsAlloc = 0;
DWORD libanogsSize  = 0;
DWORD libUE4Base    = 0;
DWORD libUE4Alloc   = 0;
DWORD libUE4Size    = 0;

int (*osub_3766E0)(int a1);
int hsub_3766E0(int a1) { return 0; }

int (*oAnoSDKGetReportData3)(void);
int hAnoSDKGetReportData3(void) { return 0; }
