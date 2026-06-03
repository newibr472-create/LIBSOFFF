/*
 * NoShake.h - Zero Camera Shake for BGMI 4.4.0
 * Library: libUE4.so (arm64-v8a)
 * Created by: Agent 2
 * 
 * ═══════════════════════════════════════════════════════════════
 * HOW CAMERA SHAKE WORKS IN BGMI (Unreal Engine 4):
 * ═══════════════════════════════════════════════════════════════
 *
 * When firing, the game triggers camera shake via:
 *   ShootWeaponEffectComponent → CameraShakeInnerRadius
 *   ShootWeaponEffectComponent → CameraShakeOuterRadius
 *   ShootWeaponEffectComponent → CameraShakFalloff
 *
 * Object Chain:
 *   STExtraShootWeapon (CurrentWeapon)
 *     → ShootWeaponEffectComp (+0xFF0 in v3.0, estimated +0x1050 in 4.4.0)
 *       → CameraShakeInnerRadius (float, +0x258)
 *       → CameraShakeOuterRadius (float, +0x25C) 
 *       → CameraShakFalloff (float, +0x260)
 *
 * Writing 0.0f to ALL THREE = NO camera shake from gunfire.
 *
 * ═══════════════════════════════════════════════════════════════
 * OFFSET TABLE:
 * ═══════════════════════════════════════════════════════════════
 *
 * ShootWeaponEffectComponent offsets (stable across versions):
 *
 * | Field                  | v3.0(2024) | v3.6(2025) | v4.4(2026) EST |
 * |------------------------|------------|------------|----------------|
 * | CameraShakeInnerRadius | 0x258      | 0x258      | 0x268~0x288    |
 * | CameraShakeOuterRadius | 0x25C      | 0x25C      | 0x26C~0x28C    |
 * | CameraShakFalloff      | 0x260      | 0x260      | 0x270~0x290    |
 *
 * Parent chain:
 * | Field                  | v3.0(2024) | v3.6(2025) | v4.4(2026) EST |
 * |------------------------|------------|------------|----------------|
 * | ShootWeaponEffectComp  | 0xFF0      | 0xFF0      | 0x1050~0x1080  |
 *
 * NOTE: CameraShake sub-offsets (0x258-0x260) are remarkably stable
 * across versions since they're in a small component class.
 *
 * ═══════════════════════════════════════════════════════════════
 * IMPLEMENTATION:
 * ═══════════════════════════════════════════════════════════════
 */

// ShootWeaponEffectComponent field offsets
#define CAMERA_SHAKE_INNER_RADIUS   0x258   // float - inner shake radius
#define CAMERA_SHAKE_OUTER_RADIUS   0x25C   // float - outer shake radius  
#define CAMERA_SHAKE_FALLOFF        0x260   // float - shake falloff curve

// Estimated v4.4.0 (if offsets shifted):
#define CAMERA_SHAKE_INNER_RADIUS_V44   0x278   // float
#define CAMERA_SHAKE_OUTER_RADIUS_V44   0x27C   // float
#define CAMERA_SHAKE_FALLOFF_V44        0x280   // float

// Parent offset (STExtraShootWeapon → ShootWeaponEffectComp)
#define SHOOT_WEAPON_EFFECT_COMP    0xFF0   // v3.0 confirmed
#define SHOOT_WEAPON_EFFECT_COMP_V44 0x1050 // v4.4 estimated

/*
 * PATCH USAGE (memory writer):
 *
 * // Get current weapon's effect component
 * auto CurrentWeapon = Read(WeaponManager + CURRENT_WEAPON_REP);
 * auto EffectComp = Read(CurrentWeapon + SHOOT_WEAPON_EFFECT_COMP);
 * 
 * // Zero all shake values
 * Write<float>(EffectComp + CAMERA_SHAKE_INNER_RADIUS, 0.0f);
 * Write<float>(EffectComp + CAMERA_SHAKE_OUTER_RADIUS, 0.0f);
 * Write<float>(EffectComp + CAMERA_SHAKE_FALLOFF, 0.0f);
 *
 * // Also zero recoil visual kick (enhances no-shake feel)
 * auto EntityComp = Read(CurrentWeapon + SHOOT_WEAPON_ENTITY);
 * Write<float>(EntityComp + ANIMATION_KICK, 0.0f);  // 0xC04/0xC54
 */

/*
 * ALTERNATIVE: Patch UCameraModifier_CameraShake::ModifyCamera()
 * This is a function-level patch that disables ALL camera shake globally.
 * 
 * In libUE4.so, find xref to "CameraShake" string → ModifyCamera function
 * Patch function entry with: RET (0xC0035FD6)
 * 
 * This is more aggressive but catches ALL shake sources:
 * - Gunfire shake
 * - Explosion shake  
 * - Vehicle shake
 * - Landing shake
 */
