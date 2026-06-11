/*
 * FINAL_BYPASS.h - BGMI 4.4.0 ARM64 Complete Bypass (VERIFIED)
 *
 * EVERY offset verified from:
 *   1. Ghidra/LIEF analysis JSON (function exists at offset)
 *   2. Capstone disassembly (valid ARM64 instruction at offset)
 *
 * CRASH-SAFE rules applied:
 *   - No init/constructor functions patched
 *   - No high-xref core engine functions touched
 *   - No JNI_OnLoad patched
 *   - libUE4.so uses NOP only (game functions keep running)
 *   - Other libs use RET only on report/upload/scan functions
 *
 * Architecture: ARM64 (AARCH64)
 * Target: com.pubg.imobile (BGMI 4.4.0)
 * Total verified patches: 230
 *
 * Requires: Macros.h with PATCH_LIB macro (KittyMemory)
 */

#ifndef FINAL_BYPASS_H
#define FINAL_BYPASS_H


// ======================================================================
// libanogs.so
// ======================================================================

PATCH_LIB("libanogs.so", "0x1d7938", "00 00 80 52 C0 03 5F D6");  // return 0 (no data)
PATCH_LIB("libanogs.so", "0x1d551c", "00 00 80 52 C0 03 5F D6");  // return 0 (no data)
PATCH_LIB("libanogs.so", "0x1d624c", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d6598", "00 00 80 52 C0 03 5F D6");  // return 0 (no data)
PATCH_LIB("libanogs.so", "0x1d6ea8", "00 00 80 52 C0 03 5F D6");  // return 0 (no data)
PATCH_LIB("libanogs.so", "0x1d79a4", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d7fc4", "00 00 80 52 C0 03 5F D6");  // return 0 (no data)
PATCH_LIB("libanogs.so", "0x1d88ec", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d417c", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d5a88", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d82cc", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d4580", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d7398", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d9024", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d4c0c", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d5030", "C0 03 5F D6");  // RET
PATCH_LIB("libanogs.so", "0x1d78cc", "00 00 80 52 C0 03 5F D6");  // return 0 (no data)
PATCH_LIB("libanogs.so", "0x1d8c74", "C0 03 5F D6");  // RET

// ======================================================================
// libhdmpvecore.so
// ======================================================================

PATCH_LIB("libhdmpvecore.so", "0x6fc88", "C0 03 5F D6");  // RET (block upload)
PATCH_LIB("libhdmpvecore.so", "0x72d3c", "20 00 80 52 C0 03 5F D6");  // return 1 (permitted)
PATCH_LIB("libhdmpvecore.so", "0x73150", "00 00 80 52 C0 03 5F D6");  // return 0 (not rooted)
PATCH_LIB("libhdmpvecore.so", "0x6d660", "C0 03 5F D6");  // RET (block upload)
PATCH_LIB("libhdmpvecore.so", "0x6d900", "C0 03 5F D6");  // RET (block finish)
PATCH_LIB("libhdmpvecore.so", "0x79944", "C0 03 5F D6");  // RET (block report)
PATCH_LIB("libhdmpvecore.so", "0x72cac", "00 00 80 52 C0 03 5F D6");  // return 0 (not rooted)
PATCH_LIB("libhdmpvecore.so", "0x7314c", "20 00 80 52 C0 03 5F D6");  // return 1 (permitted)
PATCH_LIB("libhdmpvecore.so", "0x95b44", "C0 03 5F D6");  // RET (block trace)
PATCH_LIB("libhdmpvecore.so", "0x79bd0", "C0 03 5F D6");  // RET (block report)

// ======================================================================
// libTBlueData.so
// ======================================================================

PATCH_LIB("libTBlueData.so", "0x8e854", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x8f5c4", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x91fb8", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x8e308", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x8e578", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x8ebf8", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x91ff0", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x91ef8", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x91e74", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x8f7ac", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x8f914", "C0 03 5F D6");  // RET (block telemetry)
PATCH_LIB("libTBlueData.so", "0x91e28", "C0 03 5F D6");  // RET (block telemetry)

// ======================================================================
// libhdmpve.so
// ======================================================================

PATCH_LIB("libhdmpve.so", "0x247a9c", "20 00 80 52 C0 03 5F D6");  // return 1 (compatible)
PATCH_LIB("libhdmpve.so", "0x3f4e5c", "C0 03 5F D6");  // RET (kill sniffer)

// ======================================================================
// libsigner.so
// ======================================================================

PATCH_LIB("libsigner.so", "0x9a088", "C0 03 5F D6");  // RET (skip signature)
PATCH_LIB("libsigner.so", "0x9afc0", "C0 03 5F D6");  // RET (skip signature)

// ======================================================================
// libUE4.so
// ======================================================================

PATCH_LIB("libUE4.so", "0x5BA3CB4", "1F 20 03 D5");  // NOP (skip AC check: [RealtimeVerify] Trigger verification [P)
PATCH_LIB("libUE4.so", "0x5C9A3EC", "1F 20 03 D5");  // NOP (skip AC check: ShootVerifyEx::Failed ReplayStartFireInf)
PATCH_LIB("libUE4.so", "0x69DA274", "1F 20 03 D5");  // NOP (skip AC check: UEntityAntiCheatComponent::_CheckCoronaL)
PATCH_LIB("libUE4.so", "0x69D9E20", "1F 20 03 D5");  // NOP (skip AC check: UEntityAntiCheatComponent::_CheckCoronaL)
PATCH_LIB("libUE4.so", "0x6AAEB3C", "1F 20 03 D5");  // NOP (skip AC check: USTExtraShootWeaponComponent::VerifyShoo)
PATCH_LIB("libUE4.so", "0x6AC3A0C", "1F 20 03 D5");  // NOP (skip AC check: UWeaponAntiCheatComp::Clear_AntiCheatOnR)
PATCH_LIB("libUE4.so", "0x6AC381C", "1F 20 03 D5");  // NOP (skip AC check: UWeaponAntiCheatComp::ClearOldDatas: Rem)
PATCH_LIB("libUE4.so", "0x7B9925C", "1F 20 03 D5");  // NOP (skip AC check: FTableGuard::RegisterTable - Invalid Dat)
PATCH_LIB("libUE4.so", "0x5AB8298", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! DistOffset Deviation)
PATCH_LIB("libUE4.so", "0x5AC01AC", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! DistOffset Deviation)
PATCH_LIB("libUE4.so", "0x5ABB11C", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! ray intersect with w)
PATCH_LIB("libUE4.so", "0x5ABF744", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x5B2F240", "1F 20 03 D5");  // NOP (skip AC check: CharMove TimeSpeed ServerMove, Is Cheat!)
PATCH_LIB("libUE4.so", "0x6AAE758", "1F 20 03 D5");  // NOP (skip AC check: USTExtraShootWeaponComponent::VerifyShoo)
PATCH_LIB("libUE4.so", "0x6AAED6C", "1F 20 03 D5");  // NOP (skip AC check: USTExtraShootWeaponComponent::VerifyShoo)
PATCH_LIB("libUE4.so", "0x7883F4C", "1F 20 03 D5");  // NOP (skip AC check: AUAEPlayerController::CleanupNetworkRepo)
PATCH_LIB("libUE4.so", "0x7B99424", "1F 20 03 D5");  // NOP (skip AC check: FTableGuard::RegisterTable - Registered )
PATCH_LIB("libUE4.so", "0x7E504E0", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::ReportCollectedEv)
PATCH_LIB("libUE4.so", "0x6E2B548", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheatComponent)
PATCH_LIB("libUE4.so", "0x6E230B0", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheatComponent)
PATCH_LIB("libUE4.so", "0x6E22FEC", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheatComponent)
PATCH_LIB("libUE4.so", "0x77E8478", "1F 20 03 D5");  // NOP (skip AC check: ANetworkReportActor RPC_Server_SyncClien)
PATCH_LIB("libUE4.so", "0x7E4D9E8", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventCircleRun )
PATCH_LIB("libUE4.so", "0x5ABFD80", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::ReverseVe)
PATCH_LIB("libUE4.so", "0x69C144C", "1F 20 03 D5");  // NOP (skip AC check: RPC_Server_ShootProjectileBullet_Impleme)
PATCH_LIB("libUE4.so", "0x7B9BA8C", "1F 20 03 D5");  // NOP (skip AC check: FTableGuard::HandleModNameChanged - Mod )
PATCH_LIB("libUE4.so", "0x5ABB5F4", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! nullptr == Weapon)
PATCH_LIB("libUE4.so", "0x5ABD2E8", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyMuz)
PATCH_LIB("libUE4.so", "0x5ABDE18", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x5ABE400", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyWea)
PATCH_LIB("libUE4.so", "0x5AC296C", "1F 20 03 D5");  // NOP (skip AC check: UMoveAntiCheatComponent::MoveSweepTest S)
PATCH_LIB("libUE4.so", "0x5C99574", "1F 20 03 D5");  // NOP (skip AC check: UVehicleLagCompensationComponent BeginPl)
PATCH_LIB("libUE4.so", "0x7E4FDB8", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventWeaponUsag)
PATCH_LIB("libUE4.so", "0x792F14C", "1F 20 03 D5");  // NOP (skip AC check: NetworkReportActor)
PATCH_LIB("libUE4.so", "0x791F180", "1F 20 03 D5");  // NOP (skip AC check: NetworkReportActor)
PATCH_LIB("libUE4.so", "0x791F0C8", "1F 20 03 D5");  // NOP (skip AC check: NetworkReportActor)
PATCH_LIB("libUE4.so", "0x791EE00", "1F 20 03 D5");  // NOP (skip AC check: NetworkReportActor)
PATCH_LIB("libUE4.so", "0x791ED3C", "1F 20 03 D5");  // NOP (skip AC check: NetworkReportActor)
PATCH_LIB("libUE4.so", "0x5A9D5E4", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify HeadBody verify failed! Hist)
PATCH_LIB("libUE4.so", "0x5ABF110", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyGun)
PATCH_LIB("libUE4.so", "0x5AC092C", "1F 20 03 D5");  // NOP (skip AC check: VerifyShootPointPassWall failed! %s <> %)
PATCH_LIB("libUE4.so", "0x5ABA394", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponent::ShootVerifyEx)
PATCH_LIB("libUE4.so", "0x5ABE498", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyWea)
PATCH_LIB("libUE4.so", "0x6D2E3D8", "1F 20 03 D5");  // NOP (skip AC check: ShootVerifyConfig)
PATCH_LIB("libUE4.so", "0x5ABF4F4", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x5AC11D4", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheat Strategy3, bIsOpen:%d, Ser)
PATCH_LIB("libUE4.so", "0x5AC3FD0", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheat Strategy3 Cheat, PlayerKey)
PATCH_LIB("libUE4.so", "0x7B72FBC", "1F 20 03 D5");  // NOP (skip AC check: FileCRCGuard tamper detected)
PATCH_LIB("libUE4.so", "0x5ABA9AC", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! DistOffset Deviation)
PATCH_LIB("libUE4.so", "0x650EBD0", "1F 20 03 D5");  // NOP (skip AC check: UVehicleWeaponACComp::_VerifyShootTimeGu)
PATCH_LIB("libUE4.so", "0x650ED54", "1F 20 03 D5");  // NOP (skip AC check: UVehicleWeaponACComp::_VerifyShootTimeGu)
PATCH_LIB("libUE4.so", "0x69C137C", "1F 20 03 D5");  // NOP (skip AC check: RPC_Server_ShootProjectileBullet_Impleme)
PATCH_LIB("libUE4.so", "0x6AAE36C", "1F 20 03 D5");  // NOP (skip AC check: UWeaponAntiCheatComp::VerifyStartFireInf)
PATCH_LIB("libUE4.so", "0x5A9C484", "1F 20 03 D5");  // NOP (skip AC check: VerifyShootPointPassWall blocks shooting)
PATCH_LIB("libUE4.so", "0x5ABE7D8", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x5AC1060", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheat MaxTotalMoveCheatTimes:%d,)
PATCH_LIB("libUE4.so", "0x6AAE414", "1F 20 03 D5");  // NOP (skip AC check: UWeaponAntiCheatComp::VerifyStartFireInf)
PATCH_LIB("libUE4.so", "0x77E80C8", "1F 20 03 D5");  // NOP (skip AC check: ANetworkReportActor::UnbindDelegates)
PATCH_LIB("libUE4.so", "0x7E4E854", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventDrivingVeh)
PATCH_LIB("libUE4.so", "0x5C9A380", "1F 20 03 D5");  // NOP (skip AC check: ShootVerifyEx::Failed Shoottimedata = [%)
PATCH_LIB("libUE4.so", "0x78DE48C", "1F 20 03 D5");  // NOP (skip AC check: CharacterShootVerifyStat)
PATCH_LIB("libUE4.so", "0x7BCBCD0", "1F 20 03 D5");  // NOP (skip AC check: FTableGuard::GlobalGuard - Table tamper )
PATCH_LIB("libUE4.so", "0x7BCBC54", "1F 20 03 D5");  // NOP (skip AC check: FTableGuard::GlobalGuard - Table tamper )
PATCH_LIB("libUE4.so", "0x5AC2CF8", "1F 20 03 D5");  // NOP (skip AC check: UMoveAntiCheatComponent::MoveSweepTest S)
PATCH_LIB("libUE4.so", "0x9C4C844", "1F 20 03 D5");  // NOP (skip AC check: Pak file has been corrupted or tampered )
PATCH_LIB("libUE4.so", "0x5ABD43C", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyMuz)
PATCH_LIB("libUE4.so", "0x5ABECCC", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x77E8C7C", "1F 20 03 D5");  // NOP (skip AC check: ANetworkReportActor::ResetAllNetworkData)
PATCH_LIB("libUE4.so", "0x7B9A8D0", "1F 20 03 D5");  // NOP (skip AC check: Table=[%s], TamperedRows=%d)
PATCH_LIB("libUE4.so", "0x7E4F230", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventPlayerPos )
PATCH_LIB("libUE4.so", "0x5ABED70", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x605DB44", "1F 20 03 D5");  // NOP (skip AC check: ABattleRoyaleGameModeBase::GetPlayerReal)
PATCH_LIB("libUE4.so", "0x5AECAB0", "1F 20 03 D5");  // NOP (skip AC check: lagcomp.DSLagServerTimeOpt)
PATCH_LIB("libUE4.so", "0x5AB8168", "1F 20 03 D5");  // NOP (skip AC check: VerifyClientProjectileBulletImpact_Verif)
PATCH_LIB("libUE4.so", "0x5ABE868", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x815A930", "1F 20 03 D5");  // NOP (skip AC check: EventReportComponent)
PATCH_LIB("libUE4.so", "0x5AECAB8", "1F 20 03 D5");  // NOP (skip AC check: lagcomp.DSLagServerTimeOpt
lagcomp.DSLag)
PATCH_LIB("libUE4.so", "0x5ABB6BC", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! DistSquared(%f) > Sh)
PATCH_LIB("libUE4.so", "0x5ABD774", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x5AC2560", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheat PlayerKey:%u Move Cheat ad)
PATCH_LIB("libUE4.so", "0x5AC2EC4", "1F 20 03 D5");  // NOP (skip AC check: UMoveAntiCheatComponent::Check_PassWallT)
PATCH_LIB("libUE4.so", "0x5ABCDA8", "1F 20 03 D5");  // NOP (skip AC check: UMoveAntiCheatComponent::Check_MuzzleHei)
PATCH_LIB("libUE4.so", "0x5C9A1AC", "1F 20 03 D5");  // NOP (skip AC check: ShootVerifyEx::Failed ShootData = [%s] )
PATCH_LIB("libUE4.so", "0x6F2825C", "1F 20 03 D5");  // NOP (skip AC check: EShootVerifyStatType)
PATCH_LIB("libUE4.so", "0x650E75C", "1F 20 03 D5");  // NOP (skip AC check: UDefaultAntiCheatComponent::_Verify_Proj)
PATCH_LIB("libUE4.so", "0x7E4F1EC", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventPlayerPos )
PATCH_LIB("libUE4.so", "0x5AC0784", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! DistSquared(%f) > To)
PATCH_LIB("libUE4.so", "0x5A9942C", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponent::StartLagCompe)
PATCH_LIB("libUE4.so", "0x5A9C868", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! VerifyShooterMuzzle )
PATCH_LIB("libUE4.so", "0x5A9D32C", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! ray intersect box fa)
PATCH_LIB("libUE4.so", "0x5AC0F04", "1F 20 03 D5");  // NOP (skip AC check: UMoveAntiCheatComponent::BeginPlay=====>)
PATCH_LIB("libUE4.so", "0x650E9D8", "1F 20 03 D5");  // NOP (skip AC check: UVehicleWeaponACComp::_VerifyShootTimeGu)
PATCH_LIB("libUE4.so", "0x71FD684", "1F 20 03 D5");  // NOP (skip AC check: VehicleLagCompensationComponent)
PATCH_LIB("libUE4.so", "0x71FD27C", "1F 20 03 D5");  // NOP (skip AC check: VehicleLagCompensationComponent)
PATCH_LIB("libUE4.so", "0x71FD1B8", "1F 20 03 D5");  // NOP (skip AC check: VehicleLagCompensationComponent)
PATCH_LIB("libUE4.so", "0x78DE564", "1F 20 03 D5");  // NOP (skip AC check: CharacterShootVerifyData)
PATCH_LIB("libUE4.so", "0x7E4D514", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventTakingDama)
PATCH_LIB("libUE4.so", "0x6810B54", "1F 20 03 D5");  // NOP (skip AC check: VehicleLagCompensationComponentBase)
PATCH_LIB("libUE4.so", "0x69D9FF8", "1F 20 03 D5");  // NOP (skip AC check: UEntityAntiCheatComponent::_CheckCoronaL)
PATCH_LIB("libUE4.so", "0x7E48684", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventRankingSco)
PATCH_LIB("libUE4.so", "0x5AC3F28", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheat Strategy3, PlayerKey:%u De)
PATCH_LIB("libUE4.so", "0x6A7A740", "1F 20 03 D5");  // NOP (skip AC check: >>>>>>>>>>>>>>>ServerHandleHitDataArray_)
PATCH_LIB("libUE4.so", "0x6AAE458", "1F 20 03 D5");  // NOP (skip AC check: UWeaponAntiCheatComp::VerifyStartFireInf)
PATCH_LIB("libUE4.so", "0x792096C", "1F 20 03 D5");  // NOP (skip AC check: RealtimeVerifyInfo)
PATCH_LIB("libUE4.so", "0x7B9B100", "1F 20 03 D5");  // NOP (skip AC check: FTableGuard::DumpTamperReport - Failed t)
PATCH_LIB("libUE4.so", "0x69DA0AC", "1F 20 03 D5");  // NOP (skip AC check: UEntityAntiCheatComponent::_CheckCoronaL)
PATCH_LIB("libUE4.so", "0x7B9A8A4", "1F 20 03 D5");  // NOP (skip AC check: TableGuard tamper detected)
PATCH_LIB("libUE4.so", "0x7EEE450", "1F 20 03 D5");  // NOP (skip AC check: UBioVehicleLagCompensationComponent::Sho)
PATCH_LIB("libUE4.so", "0x5ABB188", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! nullptr == MyWorld)
PATCH_LIB("libUE4.so", "0x5ABB1FC", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! ray intersect with w)
PATCH_LIB("libUE4.so", "0x7E4E1F8", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventRescue Res)
PATCH_LIB("libUE4.so", "0x69DA1B4", "1F 20 03 D5");  // NOP (skip AC check: UEntityAntiCheatComponent::_CheckCoronaL)
PATCH_LIB("libUE4.so", "0x6AAE904", "1F 20 03 D5");  // NOP (skip AC check: USTExtraShootWeaponComponent::VerifyShoo)
PATCH_LIB("libUE4.so", "0x7E4A0F0", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventUnderEnerm)
PATCH_LIB("libUE4.so", "0x7E4D558", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventTakingDama)
PATCH_LIB("libUE4.so", "0x7E4E244", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventRescue Res)
PATCH_LIB("libUE4.so", "0x5A98324", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponent::GetRecordyInd)
PATCH_LIB("libUE4.so", "0x650EC48", "1F 20 03 D5");  // NOP (skip AC check: UVehicleWeaponACComp::_VerifyShootTimeGu)
PATCH_LIB("libUE4.so", "0x5A994B0", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponent::StartLagCompe)
PATCH_LIB("libUE4.so", "0x5ABA454", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! CharacterImpactOffse)
PATCH_LIB("libUE4.so", "0x5C9B068", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! CharacterImpactOffse)
PATCH_LIB("libUE4.so", "0x5A9C5D8", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! CharacterImpactOffse)
PATCH_LIB("libUE4.so", "0x5A9C76C", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! VerifyShooterMuzzle )
PATCH_LIB("libUE4.so", "0x5ABD6EC", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifySho)
PATCH_LIB("libUE4.so", "0x6AC37CC", "1F 20 03 D5");  // NOP (skip AC check: UWeaponAntiCheatComp::ClearOldDatas: Rem)
PATCH_LIB("libUE4.so", "0x6F28550", "1F 20 03 D5");  // NOP (skip AC check: TraceShootData)
PATCH_LIB("libUE4.so", "0x78414E0", "1F 20 03 D5");  // NOP (skip AC check: LogNetworkReportActor)
PATCH_LIB("libUE4.so", "0x7E4E8A0", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventDrivingVeh)
PATCH_LIB("libUE4.so", "0x5A9DE04", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyHis)
PATCH_LIB("libUE4.so", "0x5ABE368", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerifyWea)
PATCH_LIB("libUE4.so", "0x5ABFA30", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! CharacterImpactOffse)
PATCH_LIB("libUE4.so", "0x5C9A27C", "1F 20 03 D5");  // NOP (skip AC check: ShootVerifyEx::Failed VictimHistotyData )
PATCH_LIB("libUE4.so", "0x5C9B318", "1F 20 03 D5");  // NOP (skip AC check: UVehicleLagCompensationComponent::Verify)
PATCH_LIB("libUE4.so", "0x7883FA0", "1F 20 03 D5");  // NOP (skip AC check: AUAEPlayerController::CleanupNetworkRepo)
PATCH_LIB("libUE4.so", "0x7B72FD0", "1F 20 03 D5");  // NOP (skip AC check: TamperedFiles=%d)
PATCH_LIB("libUE4.so", "0x7E48638", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventRankingSco)
PATCH_LIB("libUE4.so", "0x7E4FD74", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventWeaponUsag)
PATCH_LIB("libUE4.so", "0x5A98258", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponent::GetRecordyInd)
PATCH_LIB("libUE4.so", "0x7E50494", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::ReportCollectedEv)
PATCH_LIB("libUE4.so", "0x7E493C8", "1F 20 03 D5");  // NOP (skip AC check: UEventReportComponent::OnEventItemOperat)
PATCH_LIB("libUE4.so", "0x5AC0470", "1F 20 03 D5");  // NOP (skip AC check: ShootVerify failed! MuzzleDistSquared(%f)
PATCH_LIB("libUE4.so", "0x5ABFC04", "1F 20 03 D5");  // NOP (skip AC check: ULagCompensationComponentBase::VerityBlo)
PATCH_LIB("libUE4.so", "0x5AC25C4", "1F 20 03 D5");  // NOP (skip AC check: MoveAntiCheat PlayerKey:%u Detacted Move)
PATCH_LIB("libUE4.so", "0x69D9C24", "1F 20 03 D5");  // NOP (skip AC check: UEntityAntiCheatComponent::_CheckCoronaL)
PATCH_LIB("libUE4.so", "0x77E81D4", "1F 20 03 D5");  // NOP (skip AC check: ANetworkReportActor::UnbindDelegates fin)
PATCH_LIB("libUE4.so", "0x7883FD4", "1F 20 03 D5");  // NOP (skip AC check: AUAEPlayerController::CleanupNetworkRepo)
PATCH_LIB("libUE4.so", "0x5AC3A34", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5AC3F1C", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5AC3FCC", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5AC3240", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5AC2EC0", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABFA2C", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABFC00", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABFD7C", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABFCF4", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABFCB4", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABBB10", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABC284", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABC158", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABBD38", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5ABBD84", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x77E8C78", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Gamepl)
PATCH_LIB("libUE4.so", "0x77E838C", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Gamepl)
PATCH_LIB("libUE4.so", "0x77E8474", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Gamepl)
PATCH_LIB("libUE4.so", "0x77E80C4", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Gamepl)
PATCH_LIB("libUE4.so", "0x77E81D0", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Gamepl)
PATCH_LIB("libUE4.so", "0x7BCBCCC", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Securi)
PATCH_LIB("libUE4.so", "0x7BCBC50", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Securi)
PATCH_LIB("libUE4.so", "0x7B9BA80", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Securi)
PATCH_LIB("libUE4.so", "0x7B99250", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Securi)
PATCH_LIB("libUE4.so", "0x7B99420", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Securi)
PATCH_LIB("libUE4.so", "0x7E505A8", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Addons)
PATCH_LIB("libUE4.so", "0x7E50694", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Addons)
PATCH_LIB("libUE4.so", "0x7E505F4", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Addons)
PATCH_LIB("libUE4.so", "0x7E504D8", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Addons)
PATCH_LIB("libUE4.so", "0x7E50490", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Addons)
PATCH_LIB("libUE4.so", "0x6AC3B84", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x6AC3A08", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x6AC37C8", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x6AC3818", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x6AAE368", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5C9B064", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5C9B314", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5C99EBC", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5C997FC", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)
PATCH_LIB("libUE4.so", "0x5C997A8", "1F 20 03 D5");  // NOP (skip AC check: F:\Release4.4.0\AS\Survive\Source\Shadow)

#endif // FINAL_BYPASS_H
