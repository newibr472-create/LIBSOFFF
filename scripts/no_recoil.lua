--[[
  BGMI 4.4.0 No Recoil Script (GameGuardian)
  Library: libUE4.so
  Method: Dynamic pattern search (works across minor updates)
  
  HOW IT WORKS:
  UE4 stores weapon recoil data as float values in STExtraShootWeaponComponent.
  This script finds the recoil pattern in memory and zeros the multipliers.
  
  PATTERNS:
  - Weapon recoil uses specific float groupings (50, 200, 0.5, 40.0, 0.3)
  - These are the default recoil parameters for most weapons
  - Modifying them to 0 eliminates all weapon kickback
  
  USAGE: Run in GameGuardian while in training/match
  REQUIREMENT: Must be holding a weapon when activating
]]

function NoRecoil()
  gg.toast("🎯 No Recoil Activating...")
  gg.clearResults()
  
  -- Method 1: Float pattern search (Universal - works across versions)
  -- These are weapon recoil float groupings found in weapon data
  gg.setRanges(gg.REGION_C_ALLOC | gg.REGION_ANONYMOUS | gg.REGION_OTHER)
  
  -- Pattern: Standard recoil values (kick multipliers)
  -- Search for common recoil float patterns
  gg.searchNumber("0.30000~0.30001;0.50000~0.50001;40.0~40.01::17", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
  local count = gg.getResultsCount()
  
  if count > 0 then
    local results = gg.getResults(count)
    for i, v in ipairs(results) do
      v.value = "0.0"
      v.freeze = true
    end
    gg.setValues(results)
    gg.addListItems(results)
    gg.toast("✅ No Recoil Active! ("..count.." values patched)")
  else
    -- Method 2: Search for RecoilKickADS pattern
    gg.clearResults()
    gg.searchNumber("0.3;0.5;1.0;0.8::5", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
    count = gg.getResultsCount()
    if count > 0 then
      local results = gg.getResults(count)
      for i, v in ipairs(results) do
        v.value = "0.0"
        v.freeze = true
      end
      gg.setValues(results)
      gg.addListItems(results)
      gg.toast("✅ No Recoil (Method 2) Active! ("..count.." values)")
    else
      gg.toast("❌ Hold a weapon first, then retry!")
    end
  end
  gg.clearResults()
end

-- Method 3: Library offset method (BGMI 4.4.0 specific)
function NoRecoilOffset()
  gg.toast("🎯 No Recoil (Offset Method)...")
  
  local lib = gg.getRangesList("libUE4.so")
  if #lib == 0 then
    gg.toast("❌ libUE4.so not loaded!")
    return
  end
  
  -- libUE4.so base address (Xa = executable section)
  local base = nil
  for i, r in ipairs(lib) do
    if r.state == "Xa" then
      base = r.start
      break
    end
  end
  
  if not base then
    base = lib[1].start
  end
  
  --[[
    BGMI 4.4.0 ARM64 Recoil Function Offsets:
    These point to the recoil calculation functions in libUE4.so.
    The function reads weapon recoil params and applies them.
    
    Patch: Replace the function with "MOV W0, #0; RET"
    This makes all recoil calculations return 0.
    
    NOTE: These offsets are for BGMI 4.4.0 (com.pubg.imobile)
    They WILL change with each game update.
    Use Method 1 (pattern search) for version-independent operation.
  ]]
  
  -- Recoil calculation function candidates (find via "RecoilInfo" xref)
  -- These need to be verified with the actual 4.4.0 libUE4.so binary
  -- Placeholder offsets - update after SDK dump:
  local recoil_offsets = {
    -- Format: {offset, description}
    -- UPDATE THESE with actual 4.4.0 offsets from SDK dump
  }
  
  gg.toast("⚠️ Offset method needs 4.4.0 SDK dump. Use pattern method instead.")
end

-- Execute
NoRecoil()
