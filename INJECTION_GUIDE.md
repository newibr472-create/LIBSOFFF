# 💉 libbypass.so Injection Guide — BGMI 4.4.0

## Method: APK Repack (Game ke saath load)

### Step 1: APK Decompile
```bash
apktool d com.pubg.imobile.apk -o bgmi_decompiled
```

### Step 2: Add libbypass.so
```bash
cp libbypass.so bgmi_decompiled/lib/arm64-v8a/libbypass.so
```

### Step 3: Smali Inject — Load Library Before Game
Find main activity: `bgmi_decompiled/AndroidManifest.xml`
Look for: `<activity ... android:name="...">`

Open that activity's smali file and add this in `.method static constructor <clinit>()V`:
```smali
.method static constructor <clinit>()V
    .registers 1
    
    const-string v0, "bypass"
    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
    
    return-void
.end method
```

**OR** if `<clinit>` already exists, add BEFORE existing code:
```smali
    const-string v0, "bypass"
    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
```

> ⚠️ Note: Library name is "bypass" (without "lib" prefix and ".so" suffix)

### Step 4: Recompile & Sign
```bash
apktool b bgmi_decompiled -o bgmi_mod.apk
# Sign with your key
jarsigner -keystore my.keystore bgmi_mod.apk alias
# Or use apksigner
apksigner sign --ks my.keystore bgmi_mod.apk
```

### Step 5: Install
```bash
adb install bgmi_mod.apk
```

---

## Method 2: Virtual Space (No Root)

Use apps like:
- **Parallel Space**
- **VirtualXposed**
- **Dual Space**

These can load custom .so files alongside the game.

---

## Method 3: Direct /data/data Injection (Root)

```bash
# Copy .so to game's lib folder
adb push libbypass.so /data/data/com.pubg.imobile/lib/libbypass.so
chmod 755 /data/data/com.pubg.imobile/lib/libbypass.so

# Then modify game's native lib loading to also load libbypass.so
```

---

## Method 4: LD_PRELOAD (Root)

```bash
# Set LD_PRELOAD before game starts
# This loads your .so before everything else
export LD_PRELOAD=/data/local/tmp/libbypass.so
am start -n com.pubg.imobile/com.epicgames.ue4.SplashActivity
```

---

## How It Works (Once Loaded):

```
Game starts → Android loads libbypass.so → __attribute__((constructor)) fires
  └── pthread_create(Chameli) → new thread starts
       ├── Wait for libUE4.so to load (poll /proc/self/maps)
       ├── Wait for GNativeAndroidApp
       ├── Initialize GNames + GUObjectArray  
       ├── Apply 809 bypass patches (mprotect + memcpy)
       └── Hook PostRender VTable (index 134)
            └── ESP + Aimbot + Hitbox active every frame
```

## Important Notes:
- Library MUST be loaded BEFORE or WITH the game (not after)
- `__attribute__((constructor))` runs immediately on dlopen
- The mod then WAITS for libUE4.so in a loop — so even if loaded early, it won't crash
- No timing issues: proper polling until each component ready
- No Zygisk needed — standard Android library loading
