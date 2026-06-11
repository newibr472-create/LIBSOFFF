/*
 * TRUE_BYPASS_UE4.h - BGMI 4.4.0 ARM64 libUE4.so Bypass (CRASH-SAFE)
 *
 * Generated from Ghidra deep analysis (597,805 functions, 30,388 AC strings)
 * 
 * CRITICAL FINDINGS:
 * - Old bypasses (AntiBan.h etc) used WRONG offsets not in Ghidra's function DB
 * - AC logic is EMBEDDED inside game functions, not separate AC functions
 * - Cannot RET game functions (crash!) - must NOP specific check branch points
 * - AC components: WeaponAntiCheatComp, MoveAntiCheatComponent, 
 *   EntityAntiCheatComponent, DefaultAntiCheatComponent, LagCompensation,
 *   FTableGuard, NetworkReportActor, EventReportComponent
 *
 * STRATEGY:
 * 1. NOP branch calls to AC verification subroutines (skip the check)
 * 2. NOP the "Is Cheat!" flag-set instructions (never flag as cheat)
 * 3. RET only pure AC-only functions (report, network report actors)
 * 4. Never touch game-critical init/tick/render functions
 *
 * Format: PATCH_LIB("libUE4.so", "0xOFFSET", "HEX");  // comment
 * NOP = 1F 20 03 D5, RET = C0 03 5F D6, MOV W0,#0;RET = 00 00 80 52 C0 03 5F D6
 *
 * Architecture: ARM64 (AARCH64)
 */

#ifndef TRUE_BYPASS_UE4_H
#define TRUE_BYPASS_UE4_H

// ============================================================================
// SECTION 1: CharMove TimeSpeed Anti-Cheat (Movement speed hack detection)
// Function at 0x5B2E464 (8444 bytes, 14x called) - contains "Is Cheat!" logic
// This is a large ServerMove handler - CANNOT RET it!
// NOP the specific instruction that sets the cheat flag
// String ref "CharMove TimeSpeed ServerMove, Is Cheat!" at code 0x5B2F240
// ============================================================================

PATCH_LIB("libUE4.so", "0x5B2F240", "1F 20 03 D5");  // NOP cheat flag in CharMove speed check

// ============================================================================
// SECTION 2: ShootVerify - Server-side shot validation
// These are INSIDE large weapon/combat functions - NOP the fail branches
// Each ref is where the "ShootVerify failed!" string is loaded before reporting
// NOP = skip the report/punishment, let shot through
// ============================================================================

PATCH_LIB("libUE4.so", "0x5AB8298", "1F 20 03 D5");  // ShootVerify DistOffset deviation
PATCH_LIB("libUE4.so", "0x5AC01AC", "1F 20 03 D5");  // ShootVerify DistOffset check2
PATCH_LIB("libUE4.so", "0x5ABB11C", "1F 20 03 D5");  // ShootVerify ray intersect static
PATCH_LIB("libUE4.so", "0x5ABB5F4", "1F 20 03 D5");  // ShootVerify nullptr weapon
PATCH_LIB("libUE4.so", "0x5ABB6BC", "1F 20 03 D5");  // ShootVerify range exceeded
PATCH_LIB("libUE4.so", "0x5ABA394", "1F 20 03 D5");  // ShootVerifyEx damagefreeze
PATCH_LIB("libUE4.so", "0x5ABA9AC", "1F 20 03 D5");  // ShootVerify DistOffset big v2
PATCH_LIB("libUE4.so", "0x5AC0780", "1F 20 03 D5");  // ShootVerify TolerateShootPoint
PATCH_LIB("libUE4.so", "0x5A9C868", "1F 20 03 D5");  // ShootVerify VerifyShooterMuzzle
PATCH_LIB("libUE4.so", "0x5A9D32C", "1F 20 03 D5");  // ShootVerify ray box failed
PATCH_LIB("libUE4.so", "0x5A9D5E4", "1F 20 03 D5");  // ShootVerify HeadBody verify
PATCH_LIB("libUE4.so", "0x5C9A1AC", "1F 20 03 D5");  // ShootVerifyEx ShootData failed
PATCH_LIB("libUE4.so", "0x5C9A380", "1F 20 03 D5");  // ShootVerifyEx Shoottimedata
PATCH_LIB("libUE4.so", "0x5C9A3EC", "1F 20 03 D5");  // ShootVerifyEx ReplayStartFire

// ============================================================================
// SECTION 3: LagCompensation Verification (anti-aimbot checks)
// NOP the points where verification FAILS are logged
// ============================================================================

PATCH_LIB("libUE4.so", "0x5ABD2E8", "1F 20 03 D5");  // LagComp VerifyMuzzleImpactDir
PATCH_LIB("libUE4.so", "0x5ABDE18", "1F 20 03 D5");  // LagComp VerifyShootPosInHistory
PATCH_LIB("libUE4.so", "0x5ABE400", "1F 20 03 D5");  // LagComp VerifyMuzzleLocus Y
PATCH_LIB("libUE4.so", "0x5ABE498", "1F 20 03 D5");  // LagComp VerifyMuzzleLocus Z
PATCH_LIB("libUE4.so", "0x5ABE7D8", "1F 20 03 D5");  // LagComp VerifyShooterHeadLocus
PATCH_LIB("libUE4.so", "0x5ABE868", "1F 20 03 D5");  // LagComp HeadLocus dist
PATCH_LIB("libUE4.so", "0x5ABECCC", "1F 20 03 D5");  // LagComp VerifyShooterGunLocus
PATCH_LIB("libUE4.so", "0x5ABED70", "1F 20 03 D5");  // LagComp GunLocus distance
PATCH_LIB("libUE4.so", "0x5ABF110", "1F 20 03 D5");  // LagComp VerifyGunTail2Muzzle
PATCH_LIB("libUE4.so", "0x5ABF4F4", "1F 20 03 D5");  // LagComp empty AntiCheatComp
PATCH_LIB("libUE4.so", "0x5ABF744", "1F 20 03 D5");  // LagComp Head2ShooterPos block
PATCH_LIB("libUE4.so", "0x5ABFD80", "1F 20 03 D5");  // LagComp ReverseVerityBlock
PATCH_LIB("libUE4.so", "0x5ABD43C", "1F 20 03 D5");  // LagComp MuzzleDir skip punish
PATCH_LIB("libUE4.so", "0x5ABD774", "1F 20 03 D5");  // LagComp ShootPos empty data
PATCH_LIB("libUE4.so", "0x5ABCDA8", "1F 20 03 D5");  // MuzzleHeightTimes check

// ============================================================================
// SECTION 4: MoveAntiCheat (speed/teleport detection)
// NOP cheat-flag and report points inside movement handler
// ============================================================================

PATCH_LIB("libUE4.so", "0x5AC0F04", "1F 20 03 D5");  // MoveAC BeginPlay start
PATCH_LIB("libUE4.so", "0x5AC1060", "1F 20 03 D5");  // MoveAC MaxTotalMoveCheat config
PATCH_LIB("libUE4.so", "0x5AC11D4", "1F 20 03 D5");  // MoveAC Strategy3 open
PATCH_LIB("libUE4.so", "0x5AC2560", "1F 20 03 D5");  // MoveAC cheat flag add
PATCH_LIB("libUE4.so", "0x5AC296C", "1F 20 03 D5");  // MoveAC SweepTest
PATCH_LIB("libUE4.so", "0x5AC2CF8", "1F 20 03 D5");  // MoveAC SweepTest v2
PATCH_LIB("libUE4.so", "0x5AC2EC4", "1F 20 03 D5");  // MoveAC PassWallTimes check
PATCH_LIB("libUE4.so", "0x5AC3FD0", "1F 20 03 D5");  // MoveAC Strategy3 Cheat detected

// ============================================================================
// SECTION 5: RealtimeVerify (periodic integrity checks)
// ============================================================================

PATCH_LIB("libUE4.so", "0x5BA3CB4", "1F 20 03 D5");  // RealtimeVerify trigger point

// ============================================================================
// SECTION 6: WeaponAntiCheatComp (fire rate, ammo validation)
// ============================================================================

PATCH_LIB("libUE4.so", "0x6AAE36C", "1F 20 03 D5");  // WeaponAC VerifyStartFireInfo fail
PATCH_LIB("libUE4.so", "0x6AAE414", "1F 20 03 D5");  // WeaponAC fire interval check
PATCH_LIB("libUE4.so", "0x6AADF74", "1F 20 03 D5");  // RequestStartFire AC verify fails
PATCH_LIB("libUE4.so", "0x6AADBDC", "1F 20 03 D5");  // RequestStartFire no AntiCheatComp
PATCH_LIB("libUE4.so", "0x6AAE758", "1F 20 03 D5");  // VerifyShootID VaildShootNum<=0
PATCH_LIB("libUE4.so", "0x6AAEB3C", "1F 20 03 D5");  // VerifyShootID ClipInfo fail
PATCH_LIB("libUE4.so", "0x6AAED6C", "1F 20 03 D5");  // VerifyShootID ClipInfoIdx fail
PATCH_LIB("libUE4.so", "0x6AC381C", "1F 20 03 D5");  // WeaponAC ClearOldDatas
PATCH_LIB("libUE4.so", "0x6AC3A0C", "1F 20 03 D5");  // WeaponAC ClearOnReconnect
PATCH_LIB("libUE4.so", "0x6AC3EEC", "1F 20 03 D5");  // WeaponAC ShootRate SkipError

// ============================================================================
// SECTION 7: EntityAntiCheat + DefaultAntiCheat (hit validation)
// ============================================================================

PATCH_LIB("libUE4.so", "0x69D9E20", "1F 20 03 D5");  // EntityAC CheckCoronaLabHit result
PATCH_LIB("libUE4.so", "0x69DA274", "1F 20 03 D5");  // EntityAC CheckCoronaLabHit invalid
PATCH_LIB("libUE4.so", "0x650E6D0", "1F 20 03 D5");  // DefaultAC Verify_Projectile WOI
PATCH_LIB("libUE4.so", "0x650E75C", "1F 20 03 D5");  // DefaultAC Verify_Projectile LagComp
PATCH_LIB("libUE4.so", "0x69C137C", "1F 20 03 D5");  // ShootProjectile no WeaponAC

// ============================================================================
// SECTION 8: FTableGuard (data table integrity - detects memory edits)
// NOP the tamper detection report, let tables load normally
// ============================================================================

PATCH_LIB("libUE4.so", "0x7BCBCD0", "1F 20 03 D5");  // TableGuard tamper detected!
PATCH_LIB("libUE4.so", "0x7BCBC54", "1F 20 03 D5");  // TableGuard tamper detected! v2

// ============================================================================
// SECTION 9: NetworkReportActor (sends cheat reports to server)
// These are report-sending functions - safe to NOP their trigger points
// ============================================================================

PATCH_LIB("libUE4.so", "0x77E8478", "1F 20 03 D5");  // NetworkReport SyncPkgLoss
PATCH_LIB("libUE4.so", "0x77E80C8", "1F 20 03 D5");  // NetworkReport UnbindDelegates
PATCH_LIB("libUE4.so", "0x77E8C7C", "1F 20 03 D5");  // NetworkReport ResetAllData

// ============================================================================
// SECTION 10: EventReportComponent + AntiCheat data collection
// ============================================================================

PATCH_LIB("libUE4.so", "0x7E504E0", "1F 20 03 D5");  // EventReport ReportCollectedData
PATCH_LIB("libUE4.so", "0x78E0020", "1F 20 03 D5");  // AntiCheatMovementRawData
PATCH_LIB("libUE4.so", "0x78DE48C", "1F 20 03 D5");  // CharacterShootVerifyStat
PATCH_LIB("libUE4.so", "0x62592E4", "1F 20 03 D5");  // ServerHandleHitDataArray AC data
PATCH_LIB("libUE4.so", "0x605DB44", "1F 20 03 D5");  // GetPlayerRealtimeVerifyInfo

// ============================================================================
// SECTION 11: AntiCheat component registration (from string "anticheat")
// ============================================================================

PATCH_LIB("libUE4.so", "0x64A3D28", "1F 20 03 D5");  // "anticheat" ref 1
PATCH_LIB("libUE4.so", "0x649F060", "1F 20 03 D5");  // "anticheat" ref 2
PATCH_LIB("libUE4.so", "0x649ED48", "1F 20 03 D5");  // "anticheat" ref 3
PATCH_LIB("libUE4.so", "0x6E98710", "1F 20 03 D5");  // "AntiCheat" ref
PATCH_LIB("libUE4.so", "0x6E22D58", "1F 20 03 D5");  // MoveCheatAntiStrategy ref
PATCH_LIB("libUE4.so", "0x6E2B548", "1F 20 03 D5");  // MoveAntiCheatComponent ref 1
PATCH_LIB("libUE4.so", "0x6E230B0", "1F 20 03 D5");  // MoveAntiCheatComponent ref 2
PATCH_LIB("libUE4.so", "0x6E22FEC", "1F 20 03 D5");  // MoveAntiCheatComponent ref 3

#endif // TRUE_BYPASS_UE4_H
