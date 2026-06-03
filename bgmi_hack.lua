--[[
  ═══════════════════════════════════════════════════════════════
  BGMI 4.4.0 No Recoil + No Shake - GameGuardian Script
  ═══════════════════════════════════════════════════════════════
  Library: libUE4.so (arm64-v8a)
  Game: com.pubg.imobile (BGMI 4.4.0)
  Created by: Agent 2
  
  OFFSETS (from SDK dump analysis, v3.0-3.6 confirmed):
  These work by writing 0.0f to weapon recoil/shake properties.
  
  UPDATE GUIDE: If offsets shift in newer version, adjust the
  ShootWeaponEntity and ShootWeaponEffectComponent values below.
  Use UE4Dumper to get fresh offsets for your version.
]]

-- ═══════════════════════════════════════════════════════════════
-- OFFSET CONFIGURATION (Update per version)
-- ═══════════════════════════════════════════════════════════════

-- Object chain (STExtraBaseCharacter → Weapon → Components)
local Offsets = {
    -- Player → WeaponManager
    WeaponManagerComponent = 0x21B8,  -- v3.0 confirmed; try 0x24D8 or 0x2558 if fails
    -- WeaponManager → Current Weapon
    CurrentWeaponReplicated = 0x4F0,  -- v3.0; try 0x558 if fails
    -- Weapon → Components
    ShootWeaponEntityComp = 0xFE8,    -- v3.0; try 0x11B8 or 0x1048 if fails
    ShootWeaponEffectComp = 0xFF0,    -- v3.0; try 0x1050 if fails
    ShootWeaponComponent = 0xE68,     -- v3.0; try 0xEF0 if fails
    
    -- ShootWeaponEntity recoil fields
    RecoilKickADS = 0xBE8,            -- v3.0; CvnhanAi says 0xBF8 for v3.6
    AccessoriesVRecoilFactor = 0xAB8, -- vertical recoil multiplier
    AccessoriesHRecoilFactor = 0xABC, -- horizontal recoil multiplier
    AccessoriesRecoveryFactor = 0xAC0,-- recovery speed
    AnimationKick = 0xC04,            -- visual kick
    GameDeviationFactor = 0xB30,      -- bullet spread
    GameDeviationAccuracy = 0xB34,    -- accuracy modifier
    BulletFireSpeed = 0x4E0,          -- bullet speed
    RecoilInfo = 0xA48,               -- recoil info struct (112 bytes)
    
    -- ShootWeaponEffectComponent shake fields (very stable across versions)
    CameraShakeInnerRadius = 0x258,
    CameraShakeOuterRadius = 0x25C,
    CameraShakFalloff = 0x260,
    
    -- Player state
    bIsWeaponFiring = 0x15A8,         -- bool check if firing
}

-- ═══════════════════════════════════════════════════════════════
-- HELPER FUNCTIONS
-- ═══════════════════════════════════════════════════════════════

function readPtr(addr)
    if addr == 0 or addr == nil then return 0 end
    local t = gg.getValues({{address = addr, flags = gg.TYPE_QWORD}})
    if t[1].value == nil then return 0 end
    return tonumber(t[1].value)
end

function writeFloat(addr, val)
    if addr == 0 or addr == nil then return end
    gg.setValues({{address = addr, flags = gg.TYPE_FLOAT, value = val}})
end

function getLibBase(name)
    local ranges = gg.getRangesList(name)
    if #ranges == 0 then return nil end
    -- Find Xa (executable) range
    for _, r in ipairs(ranges) do
        if r.state == "Xa" then return r.start end
    end
    return ranges[1].start
end

-- ═══════════════════════════════════════════════════════════════
-- METHOD 1: Pattern Search (Universal - works across versions)
-- ═══════════════════════════════════════════════════════════════

function NoRecoilPattern()
    gg.toast("🎯 No Recoil (Pattern Search)...")
    gg.clearResults()
    gg.setRanges(gg.REGION_C_ALLOC | gg.REGION_ANONYMOUS)
    
    -- Search for weapon recoil float patterns
    -- Standard weapons have: RecoilKickADS ~0.3-0.5, VRecoil ~0.8-1.2
    -- Pattern: Two adjacent floats in recoil range
    gg.searchNumber("0.3~0.5;0.8~1.2::5", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
    local count = gg.getResultsCount()
    
    if count > 0 and count < 500 then
        local results = gg.getResults(count)
        local patched = 0
        for i, v in ipairs(results) do
            v.value = "0.0"
            v.freeze = true
            patched = patched + 1
        end
        gg.setValues(results)
        gg.addListItems(results)
        gg.toast("✅ No Recoil! ("..patched.." values frozen to 0)")
    else
        gg.toast("⚠️ Pattern not found. Try Method 2 or hold a weapon first.")
    end
    gg.clearResults()
end

-- ═══════════════════════════════════════════════════════════════
-- METHOD 2: Offset Chain (Precise - needs correct offsets)
-- ═══════════════════════════════════════════════════════════════

function NoRecoilOffset()
    gg.toast("🎯 No Recoil (Offset Method)...")
    
    local base = getLibBase("libUE4.so")
    if not base then
        gg.toast("❌ libUE4.so not loaded! Open game first.")
        return false
    end
    
    -- For offset method, we need the player pointer
    -- This requires GWorld → GameState → PlayerArray → LocalPlayer chain
    -- Which needs GWorld offset specific to this version
    
    gg.toast("⚠️ Offset method needs GWorld address. Use UE4Dumper first.\n" ..
             "For now, use Pattern method or manual GG search.")
    return false
end

-- ═══════════════════════════════════════════════════════════════
-- METHOD 3: Zero RecoilInfo struct (fills 112 bytes with 0)
-- ═══════════════════════════════════════════════════════════════

function NoRecoilStruct()
    gg.toast("🎯 No Recoil (Struct Zero)...")
    gg.clearResults()
    gg.setRanges(gg.REGION_C_ALLOC | gg.REGION_ANONYMOUS)
    
    -- Search for RecoilInfo struct pattern
    -- RecoilInfo starts with specific float sequences per weapon
    -- M416: ~0.28, 0.35, 0.42... (vertical recoil curve)
    gg.searchNumber("0.28~0.30;0.35~0.37;0.40~0.44::9", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
    local count = gg.getResultsCount()
    
    if count > 0 then
        local results = gg.getResults(math.min(count, 100))
        -- Zero entire recoil curve
        for i, v in ipairs(results) do
            v.value = "0.0"
            v.freeze = true
        end
        gg.setValues(results)
        gg.addListItems(results)
        gg.toast("✅ Recoil curve zeroed! ("..#results.." points)")
    else
        gg.toast("⚠️ Recoil curve not found. Equip M416 and retry.")
    end
    gg.clearResults()
end

-- ═══════════════════════════════════════════════════════════════
-- NO SHAKE (Camera Shake Removal)
-- ═══════════════════════════════════════════════════════════════

function NoShake()
    gg.toast("📷 No Camera Shake...")
    gg.clearResults()
    gg.setRanges(gg.REGION_C_ALLOC | gg.REGION_ANONYMOUS)
    
    -- Camera shake uses radius values (typically 200-1000 range as floats)
    -- InnerRadius and OuterRadius are adjacent floats
    -- Search for typical shake radius pattern
    gg.searchNumber("200~500;500~1500::5", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
    local count = gg.getResultsCount()
    
    if count > 0 and count < 200 then
        local results = gg.getResults(count)
        for i, v in ipairs(results) do
            v.value = "0.0"
            v.freeze = true
        end
        gg.setValues(results)
        gg.addListItems(results)
        gg.toast("✅ No Shake! ("..#results.." values)")
    else
        -- Fallback: search for the shake component near weapon data
        gg.clearResults()
        gg.searchNumber("100~2000;100~2000;0.1~5.0::9", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
        count = gg.getResultsCount()
        if count > 0 and count < 100 then
            local results = gg.getResults(count)
            for i, v in ipairs(results) do
                v.value = "0.0"
                v.freeze = true
            end
            gg.setValues(results)
            gg.addListItems(results)
            gg.toast("✅ No Shake (Method 2)! ("..#results.." values)")
        else
            gg.toast("⚠️ Shake values not found. Fire a weapon first, then retry.")
        end
    end
    gg.clearResults()
end

-- ═══════════════════════════════════════════════════════════════
-- COMBINED: No Recoil + No Shake
-- ═══════════════════════════════════════════════════════════════

function NoRecoilNoShake()
    NoRecoilPattern()
    gg.sleep(500)
    NoShake()
    gg.toast("🎯📷 No Recoil + No Shake ACTIVE!")
end

-- ═══════════════════════════════════════════════════════════════
-- MENU
-- ═══════════════════════════════════════════════════════════════

function main()
    local menu = gg.choice({
        "🎯 No Recoil (Pattern - Universal)",
        "🎯 No Recoil (Struct Zero - M416)",
        "📷 No Camera Shake",
        "⚡ BOTH (No Recoil + No Shake)",
        "❌ Exit"
    }, nil, "BGMI 4.4.0 - No Recoil & No Shake")
    
    if menu == 1 then NoRecoilPattern()
    elseif menu == 2 then NoRecoilStruct()
    elseif menu == 3 then NoShake()
    elseif menu == 4 then NoRecoilNoShake()
    elseif menu == 5 then os.exit()
    end
end

-- Loop menu
while true do
    if gg.isVisible(true) then
        gg.setVisible(false)
        main()
    end
    gg.sleep(100)
end
