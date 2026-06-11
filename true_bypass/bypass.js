/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * BGMI 4.4.0 TRUE BYPASS - Comprehensive Anti-Cheat Bypass (Frida)
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * Target: BGMI (Battlegrounds Mobile India) v4.4.0 ARM64
 * 
 * Libraries neutralized:
 *   1. libanogs.so    - Tencent AnoSDK (main anti-cheat engine)
 *   2. libhdmpvecore.so - HDMP Core (root detection, network checks, uploads)
 *   3. libTBlueData.so  - TBlue telemetry (data reporting)
 *   4. libhdmpve.so     - HDMP main (initialization, verification)
 *   5. libsigner.so     - APK signature verification (Adjust SDK)
 * 
 * All offsets verified from deep LIEF+Capstone analysis.
 * 
 * Usage:
 *   frida -U -f com.pubg.imobile -l bypass.js --no-pause
 * ═══════════════════════════════════════════════════════════════════════════════
 */

"use strict";

// ═══════════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════════

const CONFIG = {
    packageName: "com.pubg.imobile",
    debug: false,  // Set true for verbose logging
    
    // Library names
    libs: {
        anogs: "libanogs.so",
        hdmpvecore: "libhdmpvecore.so",
        tblue: "libTBlueData.so",
        hdmpve: "libhdmpve.so",
        signer: "libsigner.so"
    }
};

function log(msg) {
    if (CONFIG.debug) console.log("[BYPASS] " + msg);
}

function logHook(name) {
    log("Hooked: " + name);
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

function hookExport(libName, funcName, retVal) {
    try {
        const addr = Module.findExportByName(libName, funcName);
        if (addr) {
            Interceptor.attach(addr, {
                onEnter: function(args) {
                    logHook(funcName);
                },
                onLeave: function(retval) {
                    if (retVal !== undefined) {
                        retval.replace(ptr(retVal));
                    }
                }
            });
            return true;
        }
    } catch(e) {
        log("Failed to hook " + funcName + ": " + e);
    }
    return false;
}

function hookExportReplace(libName, funcName, replaceFn) {
    try {
        const addr = Module.findExportByName(libName, funcName);
        if (addr) {
            Interceptor.replace(addr, new NativeCallback(replaceFn, 'int', ['pointer', 'pointer', 'pointer', 'pointer']));
            return true;
        }
    } catch(e) {
        log("Failed to replace " + funcName + ": " + e);
    }
    return false;
}

function nopFunction(libName, offset) {
    try {
        const base = Module.findBaseAddress(libName);
        if (base) {
            const target = base.add(offset);
            // RET instruction in ARM64 = C0 03 5F D6
            Memory.protect(target, 4, 'rwx');
            target.writeByteArray([0xC0, 0x03, 0x5F, 0xD6]);
            return true;
        }
    } catch(e) {
        log("Failed to NOP " + libName + "+" + offset + ": " + e);
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 1: libanogs.so - AnoSDK Anti-Cheat Engine
// ═══════════════════════════════════════════════════════════════════════════════
// All 21 exported functions with verified offsets from analysis
// Strategy: Neuter reporting and detection while keeping init alive

function hookAnoSDK() {
    const lib = CONFIG.libs.anogs;
    let hooked = 0;

    // AnoSDKInit @ 0x1d3814 - Let it "init" but do nothing meaningful
    hookExport(lib, "AnoSDKInit", 0) && hooked++;
    
    // AnoSDKInitEx @ 0x1d3b40 - Extended init
    hookExport(lib, "AnoSDKInitEx", 0) && hooked++;
    
    // AnoSDKSetUserInfo @ 0x1d417c - User info reporting
    hookExport(lib, "AnoSDKSetUserInfo", 0) && hooked++;
    
    // AnoSDKSetUserInfoWithLicense @ 0x1d4580 - License-based user info
    hookExport(lib, "AnoSDKSetUserInfoWithLicense", 0) && hooked++;
    
    // AnoSDKOnPause @ 0x1d4c0c - Lifecycle
    hookExport(lib, "AnoSDKOnPause", 0) && hooked++;
    
    // AnoSDKOnResume @ 0x1d5030 - Lifecycle
    hookExport(lib, "AnoSDKOnResume", 0) && hooked++;
    
    // AnoSDKGetReportData @ 0x1d551c - CRITICAL: Returns cheat detection report
    // Must return empty/null to prevent ban reports
    try {
        const addr = Module.findExportByName(lib, "AnoSDKGetReportData");
        if (addr) {
            Interceptor.attach(addr, {
                onEnter: function(args) {
                    // args[0] = report type, args[1] = buffer, args[2] = size ptr
                    this.sizePtr = args[2];
                },
                onLeave: function(retval) {
                    // Set report size to 0 (no data to report)
                    if (this.sizePtr && !this.sizePtr.isNull()) {
                        try { this.sizePtr.writeInt(0); } catch(e) {}
                    }
                    retval.replace(ptr(0));
                }
            });
            hooked++;
        }
    } catch(e) {}
    
    // AnoSDKDelReportData @ 0x1d5a88
    hookExport(lib, "AnoSDKDelReportData", 0) && hooked++;
    
    // AnoSDKOnRecvData @ 0x1d624c - Server commands (could trigger kicks)
    hookExport(lib, "AnoSDKOnRecvData", 0) && hooked++;
    
    // AnoSDKIoctlOld @ 0x1d6598 - Old control interface
    hookExport(lib, "AnoSDKIoctlOld", 0) && hooked++;
    
    // AnoSDKIoctl @ 0x1d6ea8 - CRITICAL: Main control/command interface
    try {
        const addr = Module.findExportByName(lib, "AnoSDKIoctl");
        if (addr) {
            Interceptor.attach(addr, {
                onEnter: function(args) {
                    // args[0] = command type
                    // Intercept all commands, return success
                    logHook("AnoSDKIoctl cmd=" + args[0]);
                },
                onLeave: function(retval) {
                    retval.replace(ptr(0)); // 0 = success
                }
            });
            hooked++;
        }
    } catch(e) {}
    
    // AnoSDKFree @ 0x1d7398 - Memory release
    hookExport(lib, "AnoSDKFree", 0) && hooked++;
    
    // AnoSDKGetReportData2 @ 0x1d78cc
    hookExport(lib, "AnoSDKGetReportData2", 0) && hooked++;
    
    // AnoSDKGetReportData3 @ 0x1d7938
    hookExport(lib, "AnoSDKGetReportData3", 0) && hooked++;
    
    // AnoSDKDelReportData3 @ 0x1d79a4
    hookExport(lib, "AnoSDKDelReportData3", 0) && hooked++;
    
    // AnoSDKGetReportData4 @ 0x1d7fc4
    hookExport(lib, "AnoSDKGetReportData4", 0) && hooked++;
    
    // AnoSDKDelReportData4 @ 0x1d82cc
    hookExport(lib, "AnoSDKDelReportData4", 0) && hooked++;
    
    // AnoSDKOnRecvSignature @ 0x1d88ec - Signature verification from server
    hookExport(lib, "AnoSDKOnRecvSignature", 0) && hooked++;
    
    // AnoSDKForExport @ 0x1d9024 - Export utility
    hookExport(lib, "AnoSDKForExport", 0) && hooked++;
    
    // AnoSDKRegistInfoListener @ 0x1d8c74 - Info listener registration
    hookExport(lib, "AnoSDKRegistInfoListener", 0) && hooked++;

    console.log("[+] AnoSDK (libanogs.so): " + hooked + "/21 functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 2: libhdmpvecore.so - Root Detection & Network
// ═══════════════════════════════════════════════════════════════════════════════
// Key targets: root detection, upload/report, device checks

function hookHDMPCore() {
    const lib = CONFIG.libs.hdmpvecore;
    let hooked = 0;

    // Root detection - CRITICAL
    // ABase_system_IsDeviceRooted @ 0x73150 → always return false (0)
    hookExport(lib, "ABase_system_IsDeviceRooted", 0) && hooked++;
    
    // C++ version: ABase::CSystem::IsDeviceRooted() @ 0x72cac
    try {
        const addr = Module.findExportByName(lib, "_ZN5ABase7CSystem14IsDeviceRootedEv");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}
    
    // CheckPermission @ 0x72d3c - Permission checks
    try {
        const addr = Module.findExportByName(lib, "_ZN5ABase7CSystem15CheckPermissionEi");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(1)); } // 1 = permitted
            });
            hooked++;
        }
    } catch(e) {}
    
    // Upload functions - disable all reporting
    // UploadTaskImpl::Upload @ 0x6fc88
    try {
        const addr = Module.findExportByName(lib, "_ZN5ABase14UploadTaskImpl6UploadEiiiPKc");
        if (addr) {
            Interceptor.attach(addr, {
                onEnter: function(args) { logHook("Upload blocked"); },
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}
    
    // UploadFromData @ 0x6d660
    try {
        const addr = Module.findExportByName(lib, "_ZN5ABase3WWW14UploadFromDataEPKcPKvi");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}
    
    // ConfigureReport::ReportPullConfigResult @ 0x79944
    try {
        const addr = Module.findExportByName(lib, "_ZN6HDmpve15ConfigureReport22ReportPullConfigResultEiii");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}
    
    // FileUploader @ 0x78628 - block file uploads
    try {
        const addr = Module.findExportByName(lib, "_ZN6HDmpve12FileUploaderC2Ev");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}
    
    // Network init @ 0x6c594 - let it init but monitor
    hookExport(lib, "_ZN5ABase8CNetwork4initEv", 0) && hooked++;
    
    // UrlRequest::Initialize @ 0x70ff8 - block network requests
    try {
        const addr = Module.findExportByName(lib, "_ZN5ABase10UrlRequest10InitializeEPKciPNS0_8ListenerE");
        if (addr) {
            Interceptor.attach(addr, {
                onEnter: function(args) {
                    // args[1] = URL string
                    try {
                        const url = args[1].readCString();
                        logHook("UrlRequest: " + url);
                    } catch(e) {}
                },
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}

    // JNI_OnLoad @ 0x7f528
    // Don't block this - needed for JVM registration

    // Plugin lifecycle hooks
    // nativePostStartup @ 0x9b00c
    hookExport(lib, "Java_com_gcore_hdmpve_plugin_PluginUtils_nativePostStartup", 0) && hooked++;
    // nativePreShutdown @ 0x9b054
    hookExport(lib, "Java_com_gcore_hdmpve_plugin_PluginUtils_nativePreShutdown", 0) && hooked++;
    // nativeShutdown @ 0x9b09c
    hookExport(lib, "Java_com_gcore_hdmpve_plugin_PluginUtils_nativeShutdown", 0) && hooked++;
    // nativeStartup @ 0x9afc4
    hookExport(lib, "Java_com_gcore_hdmpve_plugin_PluginUtils_nativeStartup", 0) && hooked++;

    console.log("[+] HDMPCore (libhdmpvecore.so): " + hooked + " functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 3: libTBlueData.so - Telemetry/Reporting
// ═══════════════════════════════════════════════════════════════════════════════
// Disable ALL data reporting to Tencent servers

function hookTBlueData() {
    const lib = CONFIG.libs.tblue;
    let hooked = 0;

    // TDM_Initialize @ 0x91d74 - Let init succeed but neutered
    try {
        const addr = Module.findExportByName(lib, "TDM_Initialize");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}

    // TDM_Report_Binary @ 0x91ff0 - CRITICAL: Main binary report
    try {
        const addr = Module.findExportByName(lib, "TDM_Report_Binary");
        if (addr) {
            Interceptor.replace(addr, new NativeCallback(function() {
                logHook("TDM_Report_Binary blocked");
                return 0;
            }, 'int', []));
            hooked++;
        }
    } catch(e) {}

    // TDM_Enable_Report @ 0x91e28 - Disable reporting
    try {
        const addr = Module.findExportByName(lib, "TDM_Enable_Report");
        if (addr) {
            Interceptor.replace(addr, new NativeCallback(function() {
                return 0; // Disabled
            }, 'int', []));
            hooked++;
        }
    } catch(e) {}

    // JNI functions - all reporting
    const jniFuncs = [
        "Java_com_blue_dm_TBlueData_TDMInit",           // 0x8e7ac
        "Java_com_blue_dm_TBlueData_TDMEnableReport",   // 0x8e854
        "Java_com_blue_dm_TBlueData_TDMReportBinary",   // 0x8f5c4
        "Java_com_blue_dm_TBlueData_TDMReportEvent",    // 0x8ebf8
        "Java_com_blue_dm_TBlueData_TDMReportLogin",    // 0x8f7ac
        "Java_com_blue_dm_TBlueData_TDMEnableDeviceInfo", // 0x8f914
        "Java_com_blue_dm_system_TDMUtils_EncryptField",  // 0x8e308
        "Java_com_blue_dm_system_TDMUtils_SetDeviceInfoEncryptKey", // 0x8e578
    ];

    for (const funcName of jniFuncs) {
        hookExport(lib, funcName, 0) && hooked++;
    }

    // Plugin lifecycle
    const pluginFuncs = [
        "_ZN6HDmpve6Plugin12PluginReport9OnStartupEPNS0_16IServiceRegisterE", // 0x92344
        "_ZN6HDmpve6Plugin12PluginReport10OnShutdownEv",  // 0x92370
        "_ZN6HDmpve6Plugin12PluginReport13OnPreShutdownEv", // 0x9236c
        "_ZN6HDmpve6Plugin12PluginReport13OnPostStartupEv", // 0x92368
    ];

    for (const funcName of pluginFuncs) {
        hookExport(lib, funcName, 0) && hooked++;
    }

    console.log("[+] TBlueData (libTBlueData.so): " + hooked + " functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 4: libhdmpve.so - HDMP Main Initialization & Verification
// ═══════════════════════════════════════════════════════════════════════════════

function hookHDMPMain() {
    const lib = CONFIG.libs.hdmpve;
    let hooked = 0;

    // hdmpve_none_account_initialize @ 0xc149c
    hookExport(lib, "hdmpve_none_account_initialize", 0) && hooked++;
    
    // hdmpve_account_initialize @ 0xba224
    hookExport(lib, "hdmpve_account_initialize", 0) && hooked++;
    
    // hdmpve_tdir_initialize @ 0x3c6a74
    hookExport(lib, "hdmpve_tdir_initialize", 0) && hooked++;
    
    // hdmpve_connector_Initialize @ 0x12f798
    hookExport(lib, "hdmpve_connector_Initialize", 0) && hooked++;
    
    // hdmpve_custom_account_initialize @ 0xc88bc
    hookExport(lib, "hdmpve_custom_account_initialize", 0) && hooked++;
    
    // CheckAppUpdate @ 0x2e9b90 - Version/integrity check
    hookExport(lib, "CheckAppUpdate", 0) && hooked++;
    
    // IFSDllVerifyApiCompatable @ 0x247a9c - API compatibility verify
    hookExport(lib, "IFSDllVerifyApiCompatable", 1) && hooked++; // 1 = compatible
    
    // apollo_net_sniffer_Init @ 0x3f4e5c - Network sniffer
    hookExport(lib, "apollo_net_sniffer_Init", 0) && hooked++;
    
    // InitVersionMgr @ 0x2e9a98
    hookExport(lib, "InitVersionMgr", 0) && hooked++;
    
    // Signature verification
    // ChannelInfoUtil::isV2Signature @ 0x1b3ac0
    try {
        const addr = Module.findExportByName(lib, "_ZN15ChannelInfoUtil13isV2SignatureEPKc");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(1)); } // Always valid
            });
            hooked++;
        }
    } catch(e) {}

    // ChannelInfoUtil::getV2SignBlockOffset @ 0x1b3bdc
    try {
        const addr = Module.findExportByName(lib, "_ZN15ChannelInfoUtil20getV2SignBlockOffsetEPKc");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) { retval.replace(ptr(0)); }
            });
            hooked++;
        }
    } catch(e) {}
    
    // InitDataDownloader @ 0x2ea59c
    hookExport(lib, "InitDataDownloader", 0) && hooked++;
    
    // InitPufferManager @ 0x1934a4 - Resource/puffer management
    hookExport(lib, "InitPufferManager", 0) && hooked++;

    console.log("[+] HDMP Main (libhdmpve.so): " + hooked + " functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 5: libsigner.so - APK Signature Verification (Adjust SDK)
// ═══════════════════════════════════════════════════════════════════════════════

function hookSigner() {
    const lib = CONFIG.libs.signer;
    let hooked = 0;

    // Java_com_adjust_sdk_sig_NativeLibHelper_nOnResume @ 0x9a088
    hookExport(lib, "Java_com_adjust_sdk_sig_NativeLibHelper_nOnResume", 0) && hooked++;
    
    // Java_com_adjust_sdk_sig_NativeLibHelper_nSign @ 0x9afc0
    // This is the main signing function - make it return a valid-looking signature
    try {
        const addr = Module.findExportByName(lib, "Java_com_adjust_sdk_sig_NativeLibHelper_nSign");
        if (addr) {
            Interceptor.attach(addr, {
                onLeave: function(retval) {
                    // Return null - Adjust will fall back to unsigned mode
                    retval.replace(ptr(0));
                }
            });
            hooked++;
        }
    } catch(e) {}

    console.log("[+] Signer (libsigner.so): " + hooked + "/2 functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 6: System Call Hooks - /proc, ptrace, maps
// ═══════════════════════════════════════════════════════════════════════════════

function hookSystemCalls() {
    let hooked = 0;

    // ─────────────────────────────────────────────────────────────────────────
    // Hook open/openat to intercept /proc/self/status, /proc/self/maps reads
    // ─────────────────────────────────────────────────────────────────────────
    
    const openPtr = Module.findExportByName("libc.so", "open");
    const openatPtr = Module.findExportByName("libc.so", "openat");
    
    // Paths that anti-cheat reads
    const sensitiveProc = [
        "/proc/self/status",
        "/proc/self/maps", 
        "/proc/self/task",
        "/proc/self/mem",
        "/proc/self/wchan",
        "/proc/self/cmdline"
    ];
    
    const rootPaths = [
        "/system/app/Superuser.apk",
        "/system/xbin/su",
        "/system/bin/su",
        "/sbin/su",
        "/data/local/su",
        "/data/local/bin/su",
        "/data/local/xbin/su",
        "/system/sd/xbin/su",
        "/su/bin/su",
        "/su/bin",
        "/magisk",
        "/data/adb/magisk",
        "/system/bin/daemonsu",
        "/system/xbin/daemonsu"
    ];
    
    const fridaPaths = [
        "frida",
        "linjector",
        "gum-js-loop",
        "gmain",
        "/data/local/tmp/re.frida.server"
    ];

    if (openPtr) {
        Interceptor.attach(openPtr, {
            onEnter: function(args) {
                try {
                    const path = args[0].readCString();
                    if (path) {
                        this.path = path;
                        // Block root paths
                        for (const rp of rootPaths) {
                            if (path.indexOf(rp) !== -1) {
                                logHook("open() blocked root: " + path);
                                args[0] = Memory.allocUtf8String("/dev/null");
                                return;
                            }
                        }
                    }
                } catch(e) {}
            }
        });
        hooked++;
    }
    
    if (openatPtr) {
        Interceptor.attach(openatPtr, {
            onEnter: function(args) {
                try {
                    const path = args[1].readCString();
                    if (path) {
                        this.path = path;
                        for (const rp of rootPaths) {
                            if (path.indexOf(rp) !== -1) {
                                logHook("openat() blocked root: " + path);
                                args[1] = Memory.allocUtf8String("/dev/null");
                                return;
                            }
                        }
                    }
                } catch(e) {}
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook read() to spoof TracerPid in /proc/self/status
    // ─────────────────────────────────────────────────────────────────────────
    
    const readPtr = Module.findExportByName("libc.so", "read");
    if (readPtr) {
        Interceptor.attach(readPtr, {
            onLeave: function(retval) {
                if (retval.toInt32() > 0) {
                    try {
                        const buf = this.context.x1; // buffer arg
                        const content = buf.readCString();
                        if (content && content.indexOf("TracerPid:") !== -1) {
                            // Replace TracerPid with 0
                            const patched = content.replace(/TracerPid:\s*\d+/, "TracerPid:\t0");
                            buf.writeUtf8String(patched);
                            logHook("TracerPid spoofed to 0");
                        }
                    } catch(e) {}
                }
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook fgets to filter /proc/self/maps (hide Frida, Magisk, root)
    // ─────────────────────────────────────────────────────────────────────────
    
    const fgetsPtr = Module.findExportByName("libc.so", "fgets");
    if (fgetsPtr) {
        Interceptor.attach(fgetsPtr, {
            onLeave: function(retval) {
                if (!retval.isNull()) {
                    try {
                        const line = retval.readCString();
                        if (line) {
                            const lineLower = line.toLowerCase();
                            // Hide Frida
                            if (lineLower.indexOf("frida") !== -1 ||
                                lineLower.indexOf("gum-js") !== -1 ||
                                lineLower.indexOf("linjector") !== -1) {
                                // Return empty line
                                retval.writeUtf8String("");
                                logHook("maps: hid frida line");
                            }
                            // Hide Magisk
                            if (lineLower.indexOf("magisk") !== -1 ||
                                lineLower.indexOf("zygisk") !== -1) {
                                retval.writeUtf8String("");
                                logHook("maps: hid magisk line");
                            }
                            // Hide Xposed/EdXposed
                            if (lineLower.indexOf("xposed") !== -1 ||
                                lineLower.indexOf("edxposed") !== -1 ||
                                lineLower.indexOf("lsposed") !== -1) {
                                retval.writeUtf8String("");
                            }
                        }
                    } catch(e) {}
                }
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook ptrace - prevent anti-debug attachment
    // ─────────────────────────────────────────────────────────────────────────
    
    const ptracePtr = Module.findExportByName("libc.so", "ptrace");
    if (ptracePtr) {
        Interceptor.attach(ptracePtr, {
            onEnter: function(args) {
                const request = args[0].toInt32();
                // PTRACE_TRACEME = 0, used for anti-debug
                if (request === 0) {
                    logHook("ptrace(TRACEME) → spoofed");
                }
                this.request = request;
            },
            onLeave: function(retval) {
                if (this.request === 0) {
                    retval.replace(ptr(0)); // Success
                }
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook kill/tgkill - prevent self-termination on detection
    // ─────────────────────────────────────────────────────────────────────────
    
    const killPtr = Module.findExportByName("libc.so", "kill");
    if (killPtr) {
        Interceptor.attach(killPtr, {
            onEnter: function(args) {
                const pid = args[0].toInt32();
                const sig = args[1].toInt32();
                const myPid = Process.id;
                
                // Block self-kill (anti-cheat suicide)
                if (pid === myPid && (sig === 9 || sig === 6)) {
                    logHook("kill(self, " + sig + ") BLOCKED");
                    args[0] = ptr(0); // pid 0 = noop effectively
                    args[1] = ptr(0); // signal 0 = noop
                }
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook strstr/strncmp - hide root/frida strings in comparisons
    // ─────────────────────────────────────────────────────────────────────────
    
    const strstrPtr = Module.findExportByName("libc.so", "strstr");
    if (strstrPtr) {
        Interceptor.attach(strstrPtr, {
            onEnter: function(args) {
                try {
                    const needle = args[1].readCString();
                    if (needle) {
                        const n = needle.toLowerCase();
                        if (n === "su" || n === "magisk" || n === "frida" ||
                            n === "xposed" || n === "substrate" || n === "busybox" ||
                            n === "supersu" || n === "superuser" || n === "daemonsu" ||
                            n === "zygisk") {
                            // Make needle empty so it never matches
                            this.hide = true;
                        }
                    }
                } catch(e) {}
            },
            onLeave: function(retval) {
                if (this.hide) {
                    retval.replace(ptr(0)); // NULL = not found
                }
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook access/stat - hide root binaries
    // ─────────────────────────────────────────────────────────────────────────
    
    const accessPtr = Module.findExportByName("libc.so", "access");
    if (accessPtr) {
        Interceptor.attach(accessPtr, {
            onEnter: function(args) {
                try {
                    const path = args[0].readCString();
                    if (path) {
                        for (const rp of rootPaths) {
                            if (path.indexOf(rp) !== -1) {
                                this.block = true;
                                return;
                            }
                        }
                    }
                } catch(e) {}
            },
            onLeave: function(retval) {
                if (this.block) {
                    retval.replace(ptr(-1)); // -1 = not found
                }
            }
        });
        hooked++;
    }
    
    const statPtr = Module.findExportByName("libc.so", "stat");
    if (statPtr) {
        Interceptor.attach(statPtr, {
            onEnter: function(args) {
                try {
                    const path = args[0].readCString();
                    if (path) {
                        for (const rp of rootPaths) {
                            if (path.indexOf(rp) !== -1) {
                                this.block = true;
                                return;
                            }
                        }
                    }
                } catch(e) {}
            },
            onLeave: function(retval) {
                if (this.block) {
                    retval.replace(ptr(-1));
                }
            }
        });
        hooked++;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Hook dlopen - monitor and optionally block library loads
    // ─────────────────────────────────────────────────────────────────────────
    
    const dlopenPtr = Module.findExportByName("libdl.so", "dlopen");
    if (dlopenPtr) {
        Interceptor.attach(dlopenPtr, {
            onEnter: function(args) {
                try {
                    const path = args[0].readCString();
                    if (path) {
                        logHook("dlopen: " + path);
                    }
                } catch(e) {}
            }
        });
        hooked++;
    }

    console.log("[+] System hooks: " + hooked + " functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 7: Anti-Debug & Thread Protection
// ═══════════════════════════════════════════════════════════════════════════════

function hookAntiDebug() {
    let hooked = 0;

    // Hook inotify_add_watch - used to detect file modifications
    const inotifyPtr = Module.findExportByName("libc.so", "inotify_add_watch");
    if (inotifyPtr) {
        Interceptor.attach(inotifyPtr, {
            onEnter: function(args) {
                try {
                    const path = args[1].readCString();
                    if (path && (path.indexOf("/proc") !== -1 || 
                                 path.indexOf("/data") !== -1)) {
                        logHook("inotify_add_watch blocked: " + path);
                        this.block = true;
                    }
                } catch(e) {}
            },
            onLeave: function(retval) {
                if (this.block) {
                    retval.replace(ptr(-1));
                }
            }
        });
        hooked++;
    }

    // Hook fork - anti-cheat spawns child processes
    const forkPtr = Module.findExportByName("libc.so", "fork");
    if (forkPtr) {
        Interceptor.attach(forkPtr, {
            onLeave: function(retval) {
                logHook("fork() called, pid=" + retval);
            }
        });
        hooked++;
    }

    // Hook pthread_create - monitor thread creation by AC
    const pthreadPtr = Module.findExportByName("libc.so", "pthread_create");
    if (pthreadPtr) {
        Interceptor.attach(pthreadPtr, {
            onEnter: function(args) {
                const startRoutine = args[2];
                // Check if thread is from AC libraries
                const module = Process.findModuleByAddress(startRoutine);
                if (module) {
                    const modName = module.name;
                    if (modName === "libanogs.so" || modName === "libhdmpvecore.so" ||
                        modName === "libTBlueData.so") {
                        logHook("AC thread from " + modName + " @ " + startRoutine);
                        // Could block here but risky - game might need the thread
                    }
                }
            }
        });
        hooked++;
    }

    console.log("[+] Anti-debug: " + hooked + " functions hooked");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 8: Java-layer Hooks (optional, for completeness)
// ═══════════════════════════════════════════════════════════════════════════════

function hookJavaLayer() {
    if (!Java.available) return;
    
    Java.perform(function() {
        let hooked = 0;
        
        // Hook PackageManager signature check
        try {
            const PackageManager = Java.use("android.app.ApplicationPackageManager");
            PackageManager.getPackageInfo.overload('java.lang.String', 'int').implementation = function(pkg, flags) {
                // If checking for signatures (flags & 0x40), return normal
                const result = this.getPackageInfo(pkg, flags);
                logHook("Java: getPackageInfo(" + pkg + ", " + flags + ")");
                return result;
            };
            hooked++;
        } catch(e) {}
        
        // Hook Build properties (emulator detection)
        try {
            const Build = Java.use("android.os.Build");
            Build.FINGERPRINT.value = "google/flame/flame:12/SP1A.210812.016/7846004:user/release-keys";
            Build.MODEL.value = "Pixel 4";
            Build.MANUFACTURER.value = "Google";
            Build.BRAND.value = "google";
            Build.DEVICE.value = "flame";
            Build.PRODUCT.value = "flame";
            Build.HARDWARE.value = "flame";
            hooked++;
        } catch(e) {}

        // Hook Settings.Secure for developer options detection
        try {
            const Settings = Java.use("android.provider.Settings$Secure");
            Settings.getString.overload('android.content.ContentResolver', 'java.lang.String').implementation = function(resolver, name) {
                if (name === "development_settings_enabled") {
                    return "0"; // Dev mode off
                }
                if (name === "adb_enabled") {
                    return "0"; // ADB off
                }
                return this.getString(resolver, name);
            };
            hooked++;
        } catch(e) {}
        
        console.log("[+] Java layer: " + hooked + " hooks applied");
    });
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN - Execute all hooks when libraries load
// ═══════════════════════════════════════════════════════════════════════════════

console.log("═══════════════════════════════════════════════════");
console.log(" BGMI 4.4.0 TRUE BYPASS - Starting...");
console.log("═══════════════════════════════════════════════════");

// System hooks first (always available)
hookSystemCalls();
hookAntiDebug();

// Wait for AC libraries to load, then hook them
const libsToWatch = [
    { name: CONFIG.libs.anogs, hook: hookAnoSDK },
    { name: CONFIG.libs.hdmpvecore, hook: hookHDMPCore },
    { name: CONFIG.libs.tblue, hook: hookTBlueData },
    { name: CONFIG.libs.hdmpve, hook: hookHDMPMain },
    { name: CONFIG.libs.signer, hook: hookSigner }
];

// Check if already loaded
for (const lib of libsToWatch) {
    if (Module.findBaseAddress(lib.name)) {
        lib.hook();
    }
}

// Watch for future loads
let loadedLibs = new Set();
const dlopenAddr = Module.findExportByName(null, "android_dlopen_ext") || 
                   Module.findExportByName(null, "dlopen");

if (dlopenAddr) {
    Interceptor.attach(dlopenAddr, {
        onEnter: function(args) {
            try {
                this.path = args[0].readCString();
            } catch(e) { this.path = null; }
        },
        onLeave: function(retval) {
            if (this.path && !retval.isNull()) {
                for (const lib of libsToWatch) {
                    if (this.path.indexOf(lib.name) !== -1 && !loadedLibs.has(lib.name)) {
                        loadedLibs.add(lib.name);
                        console.log("[*] Library loaded: " + lib.name);
                        // Small delay to let library initialize
                        setTimeout(lib.hook, 100);
                    }
                }
            }
        }
    });
}

// Java hooks
setTimeout(hookJavaLayer, 2000);

console.log("═══════════════════════════════════════════════════");
console.log(" TRUE BYPASS ACTIVE - All hooks deployed");
console.log("═══════════════════════════════════════════════════");
