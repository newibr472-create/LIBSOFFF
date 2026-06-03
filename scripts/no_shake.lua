--[[
  BGMI 4.4.0 No Camera Shake Script (GameGuardian)
  Library: libUE4.so
  Method: Dynamic pattern search + offset patching
  
  HOW IT WORKS:
  UE4's camera shake system uses UCameraModifier_CameraShake.
  When firing, explosions, or vehicle bumps happen, shake is applied.
  This script zeros the shake scale/amplitude values.
  
  CAMERA SHAKE FLOATS:
  - ShakeScale (master multiplier)
  - OscillationDuration
  - RotOscillation (Pitch/Yaw/Roll amplitudes)
  - LocOscillation (X/Y/Z amplitudes)
  
  USAGE: Run in GameGuardian anytime during match
]]

function NoShake()
  gg.toast("📷 No Camera Shake Activating...")
  gg.clearResults()
  
  -- Camera shake uses specific float patterns
  -- Shake amplitude values are typically: 1.0, 2.0, 0.5 (grouped)
  gg.setRanges(gg.REGION_C_ALLOC | gg.REGION_ANONYMOUS | gg.REGION_OTHER)
  
  -- Method 1: Search for camera shake oscillation patterns
  -- Shake typically has Pitch/Yaw/Roll amplitude + frequency pairs
  -- Pattern: amplitude;frequency;amplitude;frequency (repeated for 3 axes)
  
  -- First, find the shake scale multiplier
  -- In UE4, CameraShakeScale is often stored near PlayCameraShake
  gg.searchNumber("1.0;0.0;1.0;0.0;1.0;0.0::9", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
  local count = gg.getResultsCount()
  
  if count > 0 and count < 5000 then
    local results = gg.getResults(math.min(count, 200))
    -- Zero out shake amplitudes (every other value is amplitude)
    for i, v in ipairs(results) do
      if v.value ~= "0.0" then
        v.value = "0.0"
        v.freeze = true
      end
    end
    gg.setValues(results)
    gg.addListItems(results)
    gg.toast("✅ No Shake Active! ("..#results.." values)")
  else
    -- Method 2: Search for weapon-specific shake values
    gg.clearResults()
    -- Weapon fire shake typically uses these ranges
    gg.searchNumber("0.5~3.0", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
    gg.refineNumber("0.5~3.0", gg.TYPE_FLOAT, false, gg.SIGN_EQUAL, 0, -1)
    count = gg.getResultsCount()
    gg.toast("Found "..count.." potential shake values. Manual filtering needed.")
  end
  
  gg.clearResults()
end

-- Method 2: Patch the PlayCameraShake function directly
function NoShakeOffset()
  gg.toast("📷 No Shake (Offset Patch)...")
  
  local lib = gg.getRangesList("libUE4.so")
  if #lib == 0 then
    gg.toast("❌ libUE4.so not loaded!")
    return
  end
  
  local base = nil
  for i, r in ipairs(lib) do
    if r.state == "Xa" then
      base = r.start
      break
    end
  end
  if not base then base = lib[1].start end
  
  --[[
    BGMI 4.4.0 Camera Shake Function Offsets:
    
    In UE4, camera shake is applied via:
    1. APlayerCameraManager::StartCameraShake()
    2. UCameraModifier_CameraShake::ModifyCamera()
    
    Patching ModifyCamera to return immediately = no shake ever applied.
    
    ARM64 Patch: RET (0xC0035FD6) at function entry
    
    NOTE: Offsets need verification with actual 4.4.0 binary.
    Use Method 1 (float search) for immediate results.
  ]]
  
  gg.toast("⚠️ Offset method needs 4.4.0 verification. Use pattern method.")
end

-- Execute
NoShake()
