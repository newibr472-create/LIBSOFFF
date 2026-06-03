# Critical Bugs & Required Fixes

## Bug #1: Wrong Source File Compiled (CRITICAL)

**File:** `jni/Android.mk`  
**Line:** `LOCAL_SRC_FILES := ../BYPASS.cpp`  
**Problem:** This compiles the root `BYPASS.cpp` which has ALL hooks commented out. The production version with active hooks is in `jni/BYPASS.cpp` and is NEVER compiled.  
**Fix:** Change to `LOCAL_SRC_FILES := BYPASS.cpp`

## Bug #2: Libc Hooks Never Installed (CRITICAL)

**Files:** `BYPASS.cpp`, `jni/BYPASS.cpp`  
**Problem:** The hooks for `__system_property_get`, `access`, `fopen`, and `stat` are DEFINED but never installed with `DobbyHook()`. The emulator detection bypass (property spoofing + file path blocking) is completely non-functional.  
**Fix:** Add DobbyHook calls in `lib_main()`:
```cpp
// In lib_main(), before spawning threads:
DobbyHook((void *)__system_property_get, (void *)hooked_system_property_get, (void **)&orig_system_property_get);
DobbyHook((void *)access, (void *)hooked_access, (void **)&orig_access);
DobbyHook((void *)fopen, (void *)fopen_t, (void **)&orig_fopen);
DobbyHook((void *)stat, (void *)stat_t, (void **)&orig_stat);
```

## Bug #3: Shadow Memory CRC Evasion Non-Functional (CRITICAL)

**Files:** `BYPASS.cpp`, `jni/BYPASS.cpp`  
**Problem:** Shadow memory is allocated and the .so is copied to it, but the `memcpy`/`memcmp` hooks that redirect reads to shadow memory are commented out. The anti-cheat's CRC checks will still read the original (modified) memory and detect the hooks.  
**Fix:** Implement and install CRC redirect hooks:
```cpp
// Hook memcpy to redirect reads from .so regions to shadow memory
// Hook memcmp to redirect CRC comparisons to shadow memory
```

## Bug #4: 32-bit Pointer Types (HIGH — ARM64 Migration)

**Files:** `anogs_hooks.h`, `Utils.h`, `BYPASS.cpp`  
**Problem:** All base addresses and allocations use `DWORD` (uint32_t) which truncates 64-bit pointers.  
**Fix:** Change to `uintptr_t`:
```cpp
// anogs_hooks.h
extern uintptr_t libanogsBase;
extern uintptr_t libanogsAlloc;
extern uintptr_t libanogsSize;
extern uintptr_t libUE4Base;
extern uintptr_t libUE4Alloc;
extern uintptr_t libUE4Size;
```

## Bug #5: Vtable Array Indexing for ARM64 (HIGH)

**File:** `ue4_hooks.h` — `hsub_79DFEEC`  
**Problem:** Uses `_DWORD*` (4-byte) array indexing like `a1[84]` through `a1[127]`. On ARM64, vtable entries are 8 bytes each.  
**Fix:** Change to `uint64_t*` and halve all indices:
```cpp
uint64_t *a1 = (uint64_t *)arg1;
// a1[84] at 4 bytes = offset 336 = a1[42] at 8 bytes
// a1[127] at 4 bytes = offset 508 = a1[63] at 8 bytes
```

## Bug #6: Hook Signatures Use `int` for Pointers (HIGH)

**Files:** `tdm_hooks.h`, `ue4_hooks.h`  
**Problem:** Hook function signatures use `int` for pointer-sized parameters which truncates on ARM64.  
**Fix:** Change to `uintptr_t` or proper pointer types.

## Bug #7: KR Button Handler Bug (MEDIUM)

**File:** `WOLF/Form1.cs`  
**Problem:** Korean checkbox handler checks `this.gl.Checked` instead of `this.kr.Checked`. Korean cleanup never executes.  
**Fix:** Change `gl.Checked` to `kr.Checked` in the Korean checkbox handler.

## Bug #8: Self-Deletion Uses Wrong Name (MEDIUM)

**File:** `WOLF/Form1.cs`  
**Problem:** `RemoveFile("WOLVES.exe")` uses old executable name. Assembly is now `RADAHN BP`.  
**Fix:** Use `Process.GetCurrentProcess().MainModule.FileName` for dynamic path.

## Bug #9: No Dobby ARM64 Build (HIGH)

**Path:** `jni/Dobby_inc/`  
**Problem:** Only `armeabi-v7a/libdobby.a` exists. No ARM64 prebuilt.  
**Fix:** Build Dobby from `Dobby_repo/` source for `arm64-v8a`:
```bash
cd jni/Dobby_repo
mkdir build-android-arm64 && cd build-android-arm64
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-21 \
      ..
make -j$(nproc)
cp libdobby.a ../Dobby_inc/arm64-v8a/
```

## Bug #10: PatchMonitor Has Nothing to Monitor (LOW)

**File:** `jni/BYPASS.cpp`  
**Problem:** `g_patches` vector is declared but never populated with any `PatchInfo` entries. The PatchMonitor thread runs but checks nothing.  
**Fix:** Add patch entries after each hook installation using the `PATCH_LIB` macro or manual `PatchInfo` population.
