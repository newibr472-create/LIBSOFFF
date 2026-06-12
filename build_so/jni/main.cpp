// JOIN = TG >> @JKVIPOWNER
#include "Offset/SDK.hpp"
using namespace SDK;
using namespace std;
#include <android/native_window_jni.h>
#include <android/log.h>
#include "Main/Tools.h"
#include "Main/Dobby/dobby.h"
#include "Main/Includes.h"
#include "Main/StrEnc.h"
#include "Main/Vector3.hpp"
#include "Main/Vector2.hpp"
#include "Main/MemoryTools.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include "Main/android_native_app_glue.h"
#include "Main/obfuscate.h"
#include "Main/Eagle_GUI.h"
#include <thread>
#include <chrono>
#include "Bypass.h"
bool bValid = true;
bool isLogin = true;


bool XD = false;
bool NARTUO = false;
android_app *g_App = 0;
ASTExtraPlayerCharacter *g_LocalPlayer=0;
ASTExtraPlayerController *g_PlayerController =0;
#define SLEEP_TIME 1000LL / 120LL
#define TSL_FONT_DEFAULT_SIZE 12

#define PI 3.14159265358979323846
#define RAD2DEG(x) ((float)(x) * (float)(180.f / PI))

uintptr_t GNames_Offset = 0x8394964;
uintptr_t GUObject_Offset = 0xe22f8d0;
uintptr_t GetActorArray = 0xa1018ac;
uintptr_t GNativeAndroidApp_Offset = 0xdf74398;
uintptr_t Actors_Offset = 0xA0;
// JOIN = TG >> @JKVIPOWNER
int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
float density = -1;
uintptr_t UE4;

float BulletTrackFov = 120.0f;
bool Bullet[20];
// JOIN = TG >> @JKVIPOWNER

// JOIN = TG >> @JKVIPOWNER
float screenSizeX = 0;
float screenSizeY = 0;
float FOVsize = 150;
float Speed_Aim = 1.2;
float recoilCompensationFactor = 1.4;
float Range = 200;
bool head;
bool AimHead = false;
bool AimBody = true;
int trackingType = 1;
int scopeAndFire = 0;
float FOVSize;

enum EAimMode {
    AimBullet = 0,
    Pbullet = 1,
    AimBot = 2
};
enum EAimTarget {
    Chest = 0,
    Head = 1
};
enum EAimBy {
    FOV = 0,
    Distance = 1
};
enum EAimTrigger {
    Shooting = 0,
    None = 1,
    Scoping = 2,
    Both = 3,
    Any = 4
};// JOIN = TG >> @JKVIPOWNER

// JOIN = TG >> @JKVIPOWNER



std::map<int, bool> itemConfig;

struct sConfig {
    bool Bypass;
    bool Enable;
    struct sPlayerESP {
        bool Line;
        bool Health;
        bool Skeleton;
        bool Name;
        bool Distance;
        bool TeamID;
        bool Grenade;
        bool Alert;
        bool Weapon;
        bool ItemEsp;
        bool Vehicle;
        bool OneClickEsp;
        bool MessageBox;
    };
    sPlayerESP PlayerESP{0};

    struct sVehicleESP {
        bool ShowVehicle;
        bool ShowDistance;
    };
    sVehicleESP VehicleESP{0};

    struct sAimMenu {// JOIN = TG >> @JKVIPOWNER
        bool Enable;
        bool AimBot;
        EAimMode Mode;
        EAimBy AimBy;
        EAimTarget Target;
        EAimTrigger Trigger;
        bool RecoilComparison;
        float Recc;
        bool RecoilSet;
        bool Radius;
        float Line;
        bool Prediction;
        float Cross;
        float Crosss;
        bool Wide;
        bool IgnoreKnocked;
        bool VisCheck;
        bool IgnoreBot;
        float AimSmooth = 3.0f;
    };
    sAimMenu SilentAim{0};
    sAimMenu AimBot{0};
};
sConfig Config{0};

int (*MessageBoxExt)(unsigned int, const char16_t *, const char16_t *);
 
std::u16string convertToUtf16(const std::string &utf8str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(utf8str);
}

std::string Fuck = "BATTLE ROYALE MODZ";
std::u16string f2 = convertToUtf16(Fuck);

std::string Fuck1 = "FEEDBACK DE DO AB";
std::u16string f3 = convertToUtf16(Fuck1);

std::string Op4 = "Winner Winner Chicken Dinner!\nFeedback: To Owner - Thanks for Playing ❤";
std::u16string xxx4 = convertToUtf16(Op4);

std::string Filepath = "/sdcard/Android/obb/com.pubg.imobile/key.lic";
const char *Gamepackage = "com.pubg.imobile";

static bool isHead, isNeck, isPelvis, isLeftClavicle, isRightClavicle, 
            isLeftUpperArm, isLeftLowerArm, isLeftHand, isLeftThigh, 
            isLeftCalf, isLeftFoot, isRightUpperArm, isRightLowerArm, 
            isRightHand, isRightThigh, isRightCalf, isRightFoot, 
            isSpine1, isSpine2, isSpine3;
static int algorithm = 0;

static int frameCount = 0;
static float currentFPS = 0.0f;
static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
void UpdateFPS()
{
    frameCount++;
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - lastTime;

    if (elapsed.count() >= 1.0f)
    {
        currentFPS = frameCount / elapsed.count();
        frameCount = 0;
        lastTime = now;
    }
}



  std::string getDayName() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    const char* weekday[] = { "VALIDITY,", "VALIDITY,", "VALIDITY,", "VALIDITY,", "VALIDITY,", "VALIDITY,", "VALIDITY," };
    return weekday[timeinfo->tm_wday];
}

// Function to get AM/PM
std::string getAMPM() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    return (timeinfo->tm_hour < 12) ? "AM " : "PM ";
}// JOIN = TG >> @JKVIPOWNER





struct sRegion
{
    uintptr_t start, end;
};

std::vector<sRegion> trapRegions;

bool isEqual(std::string s1, const char* check) {
    std::string s2(check);
    return (s1 == s2);
}

bool isObjectInvalid(UObject *obj)
{
    if (!Tools::IsPtrValid(obj))
    {
        return true;
    }
    if (!Tools::IsPtrValid(obj->ClassPrivate))
    {
        return true;
    }
    if (obj->InternalIndex <= 0)
    {
        return true;
    }
    if (obj->NamePrivate.ComparisonIndex <= 0)
    {
        return true;
    }
    if ((uintptr_t)(obj) % sizeof(uintptr_t) != 0x0 && (uintptr_t)(obj) % sizeof(uintptr_t) != 0x4)
    {
        return true;
    }
    if (std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t)obj) >= region.start && ((uintptr_t)obj) <= region.end; }) ||
        std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t)obj->ClassPrivate) >= region.start && ((uintptr_t)obj->ClassPrivate) <= region.end; }))
    {
        return true;
    }
    return false;
}

bool UrlLink;
int OpenURL(const char* url)
{
    JavaVM* java_vm = g_App->activity->vm;
    JNIEnv* java_env = NULL;

    jint jni_return = java_vm->GetEnv((void**)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;

    jni_return = java_vm->AttachCurrentThread(&java_env, NULL);
    if (jni_return != JNI_OK)
        return -2;

    jclass native_activity_clazz = java_env->GetObjectClass(g_App->activity->clazz);
    if (native_activity_clazz == NULL)
        return -3;

    jmethodID method_id = java_env->GetMethodID(native_activity_clazz, "AndroidThunkJava_LaunchURL", "(Ljava/lang/String;)V");
    if (method_id == NULL)
        return -4;
        
    jstring retStr = java_env->NewStringUTF(url);
    java_env->CallVoidMethod(g_App->activity->clazz, method_id, retStr);

    jni_return = java_vm->DetachCurrentThread();
    if (jni_return != JNI_OK)
        return -5;

    return 0;
}// JOIN = TG >> @JKVIPOWNER

template<typename T>
void Write(uintptr_t addr, T value) {
WriteAddr((void *) addr, &value, sizeof(T));
}
void MemoryD_type(uintptr_t addr,int var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),4);
}
void MemoryQ_type(uintptr_t addr,int var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),32);
}

UWorld *GEWorld;
int GWorldNum = 0;
TUObjectArray gobjects;
UWorld *GetFullWorld()
{
    if(GWorldNum == 0) {
        gobjects = UObject::GUObjectArray->ObjObjects;
        for (int i=0; i< gobjects.Num(); i++)
            if (auto obj = gobjects.GetByIndex(i)) {
                if(obj->IsA(UEngine::StaticClass())) {
                    auto GEngine = (UEngine *) obj;
                    if(GEngine) {
                        auto ViewPort = GEngine->GameViewport;
                        if (ViewPort)
                        {
                            GEWorld = ViewPort->World;
                            GWorldNum = i;
                            if (!GEWorld || !GEWorld->NetDriver) {
                              g_LocalPlayer = nullptr;
                              g_PlayerController = nullptr;
                              return nullptr;
                              }
                            return ViewPort->World;
                        }
                    }
                }
            }
    }else {
        auto GEngine = (UEngine *) (gobjects.GetByIndex(GWorldNum));
        if(GEngine) {
            auto ViewPort = GEngine->GameViewport;
            if(ViewPort) {
                GEWorld = ViewPort->World;
                                            if (!GEWorld || !GEWorld->NetDriver) {
                              g_LocalPlayer = nullptr;
                              g_PlayerController = nullptr;
                              return nullptr;
                              }
                return ViewPort->World;
            }
        }
    }
    return 0;
}

static UGameViewportClient *GameViewport = 0;
UGameViewportClient *GetGameViewport() {
    while (!GameViewport) {
        GameViewport = UObject::FindObject<UGameViewportClient>("GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
        sleep(1);
    }
    if (GameViewport) {
        return GameViewport;
    }
    return 0;
}

std::vector<AActor *> GetActors() {
    auto World = GetFullWorld();
    if (!World)
    return std::vector<AActor *>();
    auto PersistentLevel = World->PersistentLevel;
    if (!PersistentLevel)
    return std::vector<AActor *>();
    struct GovnoArray {
    uintptr_t base;
    int32_t count;
    int32_t max;
    };
    static thread_local GovnoArray Actors{};
    Actors = *(((GovnoArray*(*)(uintptr_t))(UE4 + GetActorArray))(reinterpret_cast<uintptr_t>(PersistentLevel)));
    if (Actors.count <= 0) {
    return {};
    }
    std::vector<AActor *> actors;
    for (int i = 0; i < Actors.count; i++) {
    auto Actor = *(uintptr_t *) (Actors.base + (i * sizeof(uintptr_t)));
    if (Actor) {
    actors.push_back(reinterpret_cast<AActor *const>(Actor));
    }
    }
  return actors;
}// JOIN = TG >> @JKVIPOWNER








const char *GetVehicleName(ASTExtraVehicleBase *Vehicle) {
    switch (Vehicle->VehicleShapeType) {
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Motorbike:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Motorbike_SideCart:
            return "Motorbike";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Dacia:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyDacia:
            return "Dacia";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_MiniBus:
            return "Mini Bus";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PickUp:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PickUp01:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyPickup:
            return "Pick Up";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Buggy:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyBuggy:
            return "Buggy";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ01:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ02:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ03:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyUAZ:
            return "UAZ";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PG117:
            return "PG117";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Aquarail:
            return "Aquarail";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Mirado:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Mirado01:
            return "Mirado";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Rony:
            return "Rony";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Scooter:
            return "Scooter";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_SnowMobile:
            return "Snow Mobile";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_TukTukTuk:
            return "Tuk Tuk";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_SnowBike:
            return "Snow Bike";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Surfboard:
            return "Surf Board";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Snowboard:
            return "Snow Board";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Amphibious:
            return "Amphibious";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_LadaNiva:
            return "Lada Niva";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAV:
            return "UAV";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_MegaDrop:
            return "Mega Drop";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Lamborghini:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Lamborghini01:
            return "Lamborghini";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_GoldMirado:
            return "Gold Mirado";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_BigFoot:
            return "Big Foot";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyUH60:
            return "UH60";
            break;
        default:
            return "Vehicle";
            break;
    }
    return "Vehicle";
}



TNameEntryArray *GetGNames()
{
    return ((TNameEntryArray * (*)()) (UE4 + GNames_Offset))();
}// JOIN = TG >> @JKVIPOWNER

template <class T>
void GetAllActors(std::vector<T *> &Actors)
{
    UGameplayStatics *gGameplayStatics = (UGameplayStatics *)gGameplayStatics->StaticClass();
    auto GWorld = GetFullWorld();
    if (GWorld)
    {
        TArray<AActor *> Actors2;
        gGameplayStatics->GetAllActorsOfClass((UObject *)GWorld, T::StaticClass(), &Actors2);
        for (int i = 0; i < Actors2.Num(); i++)
        {
            Actors.push_back((T *)Actors2[i]);
        }
    }
}
////==========================================================================================================//
FVector GetBoneLocationByName(ASTExtraPlayerCharacter *Actor, const char *BoneName)
{
    return Actor->GetBonePos(BoneName, FVector());
}

#define COLOR_IN FLinearColor(0, 0, 0, 0)
#define COLOR_BLACK FLinearColor(0, 0, 0, 1.f)
#define COLOR_WHITE FLinearColor(1.f, 1.f, 1.f, 1.f)
#define COLOR_RED FLinearColor(1.f, 0, 0, 1.f)
#define COLOR_CAR FLinearColor(1.f, 0.5f, 1.f, 1.f)
#define COLOR_GREEN FLinearColor(0.0f, 1.0f, .0f, 1.0f)
#define COLOR_ORANGE FLinearColor(1.f, 0.4f, 0, 1.f)
#define COLOR_ROSE FLinearColor(0.929f, 0.682f, 0.753f, 1.0f)
#define COLOR_YELLOW FLinearColor(1.f, 1.f, 0, 1.f)
#define COLOR_LIME FLinearColor(0, 1.f, 0, 1.f)
#define COLOR_BLUE FLinearColor(0, 0, 1.f, 1.f)
#define COLOR_THISTLE FLinearColor(1.0f, 0.74f, 0.84f, 1.0f)
#define COLOR_PINK FLinearColor(1.0f, 0.75f, 0.8f, 1.0f)
#define COLOR_CYAN FLinearColor(0.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_OUTLINE FLinearColor(0.0f, 0.0f, 0.0f, 0.25f)
#define Black FLinearColor(0, 0, 0, 1.f)
#define White FLinearColor(1.f, 1.f, 1.f, 1.f)
#define Red   FLinearColor(1.f, 0, 0, 1.f)
#define Gray  FLinearColor(0, 1.f, 0, 1.f)
#define Blue  FLinearColor(0, 0, 1.f, 1.f)
#define Pink   FLinearColor(1.f, 0.5f, 1.f, 1.f)
#define Red FLinearColor(1.f, 0, 0, 0.7f)
#define Green FLinearColor(0, 0.5f, 0, 1.f)
#define Orange FLinearColor(1.f, 0.4f, 0, 1.f)
#define Purple FLinearColor(0.67f, 0.57f, 0.81f, 1.0f)
#define Yellow FLinearColor(1.f, 1.f, 0, 1.f)
#define BB FLinearColor(36 / 255.f, 249 / 255.f, 217 / 255.f, 0.0f);



SDK::FVector SubtractVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X - b.X;
    result.Y = a.Y - b.Y;
    result.Z = a.Z - b.Z;
    return result;
}// JOIN = TG >> @JKVIPOWNER

SDK::FVector AddVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
    return result;
}// JOIN = TG >> @JKVIPOWNER

SDK::FVector MultiplyVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X * b.X;
    result.Y = a.Y * b.Y;
    result.Z = a.Z * b.Z;
    return result;
}// JOIN = TG >> @JKVIPOWNER

SDK::FVector DivideVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X / b.X;
    result.Y = a.Y / b.Y;
    result.Z = a.Z / b.Z;
    return result;
}// JOIN = TG >> @JKVIPOWNER

SDK::FVector MultiplyVectorFloat(SDK::FVector a, float scalar) {
    SDK::FVector result;
    result.X = a.X * scalar;
    result.Y = a.Y * scalar;
    result.Z = a.Z * scalar;
    return result;
}

FRotator ToRotator2(FVector local, FVector target) {
    FVector Lund = SubtractVectors(target, local);
    FRotator newViewAngle;
    newViewAngle.Pitch =
            -std::atan2(Lund.Z, std::sqrt(Lund.X * Lund.X + Lund.Y * Lund.Y)) * (180.f / M_PI);
    newViewAngle.Yaw = std::atan2(Lund.Y, Lund.X) * (180.f / M_PI);
    newViewAngle.Roll = 0.f;
    if (newViewAngle.Yaw < 0.f) {
        newViewAngle.Yaw += 360.f;
    }
    return newViewAngle;
}



void VectorAnglesRadar(Vector3 &forward, FVector &angles) {
    if (forward.X == 0.f && forward.Y == 0.f) {
        angles.X = forward.Z > 0.f ? -360.f : 360.f;
        angles.Y = 0.f;
    } else {
        angles.X = RAD2DEG(atan2(-forward.Z, forward.Magnitude(forward)));
        angles.Y = RAD2DEG(atan2(forward.Y, forward.X));
    }
    angles.Z = 360.f;
}



// Hex-style glowing corner made only with rectangles
void DrawCornerHex(SDK::UCanvas* Canvas, SDK::FVector2D pos, float size, SDK::FLinearColor color)
{
    // main short L-shape
    DrawFilledRect(Canvas, pos, size * 0.6f, 3.0f, color);                    // horizontal
    DrawFilledRect(Canvas, SDK::FVector2D(pos.X, pos.Y), 3.0f, size * 0.6f, color); // vertical

    // small inner block to fake a cut/angle
    DrawFilledRect(Canvas,
                   SDK::FVector2D(pos.X + (size * 0.45f), pos.Y + (size * 0.45f)),
                   3.0f, 3.0f, color);

    // soft glow around corner (bigger, low alpha)
    SDK::FLinearColor glow(color.R, color.G, color.B, 0.22f);
    DrawFilledRect(Canvas,
                   SDK::FVector2D(pos.X - 2.0f, pos.Y - 2.0f),
                   (size * 0.6f) + 4.0f, 3.0f + 4.0f,
                   glow);
    DrawFilledRect(Canvas,
                   SDK::FVector2D(pos.X - 2.0f, pos.Y - 2.0f),
                   3.0f + 4.0f, (size * 0.6f) + 4.0f,
                   glow);
}




FRotator ToRotator(FVector local, FVector target) {
FVector rotation = UKismetMathLibrary::Subtract_VectorVector(local, target);

float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);

FRotator newViewAngle = {0};
newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float) 3.14159265358979323846);
newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float) 3.14159265358979323846);
newViewAngle.Roll = (float) 0.f;

if (rotation.X >= 0.f)
newViewAngle.Yaw += 180.0f;

return newViewAngle;
}// JOIN = TG >> @JKVIPOWNER
int GetIndex(int currentIndex, int arrayNum) {
    if (arrayNum <= 0) {
        return 0;
    }// JOIN = TG >> @JKVIPOWNER
    return (currentIndex + 1) % arrayNum;
}// JOIN = TG >> @JKVIPOWNER

void clampAngles(FRotator &angles)
{
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;
    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;
    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}

void AimAngle(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}

bool W2S2(FVector worldPos, FVector2D *screenPos) {
return g_PlayerController->ProjectWorldLocationToScreen(worldPos, true, screenPos);
}








////==========================================================================================================//
void *LoadFont(void *) {
    while (!tslFont || !robotoTinyFont) {
        tslFont = UObject::FindObject<UFont>("Font TSLFont.TSLFont");
        robotoTinyFont = UObject::FindObject<UFont>("Font RobotoTiny.RobotoTiny");
        sleep(1);
    }
    return 0;
}

void NekoHook(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}// JOIN = TG >> @JKVIPOWNER
void NekoHook(float *angles) {
    if (angles[0] > 180)
        angles[0] -= 360;
    if (angles[0] < -180)
        angles[0] += 360;

    if (angles[0] < -75.f)
        angles[0] = -75.f;
    else if (angles[0] > 75.f)
        angles[0] = 75.f;

    while (angles[1] < -180.0f)
        angles[1] += 360.0f;
    while (angles[1] > 180.0f)
        angles[1] -= 360.0f;
}

void NekoHook(FVector2D angles) {
    if (angles.X > 180)
        angles.X -= 360;
    if (angles.X < -180)
        angles.X += 360;

    if (angles.X < -75.f)
        angles.X = -75.f;
    else if (angles.X > 75.f)
        angles.X = 75.f;

    while (angles.Y < -180.0f)
        angles.Y += 360.0f;
    while (angles.Y > 180.0f)
        angles.Y -= 360.0f;
}
////==========================================================================================================//
#define W2S(w, s) UGameplayStatics::ProjectWorldToScreen(g_PlayerController, w, true, s)


bool isInsideFOV(int x, int y) {
    int circle_x = screenWidth / 2;
    int circle_y = screenHeight / 2;
    int rad = (int) FOVsize;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

auto GetTargetForAim()
{
    ASTExtraPlayerCharacter *result = 0;
    float max = std::numeric_limits<float>::infinity();
    auto Actors = GetActors();
    
    auto localPlayer = g_LocalPlayer;
    auto localPlayerController = g_PlayerController;
    
    if (localPlayer)
    {
        for (auto Actor : Actors)
        {
            if (isObjectInvalid(Actor))
                continue;
            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
            {
                auto Player = (ASTExtraPlayerCharacter *)Actor;
                
                float dist = localPlayer->GetDistanceTo(Player) / 100.0f;    
                   if (dist > Range)
                    continue;
                    
                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;

                if (Player->TeamID == localPlayer->TeamID)
                    continue;

                if (Player->bDead)
                    continue;

                if (Config.AimBot.IgnoreKnocked) {
                if (Player->Health == 0.0f)
                continue;
                }
                                                  
                if (Config.AimBot.IgnoreBot) {
                if (Player->bEnsure)
                continue;
                }
                        
                if (Config.AimBot.VisCheck) {
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("Head", {0, 0, 0}), false))//头
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("neck_01", {0, 0, 0}), false))//Neck
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("upperarm_r", {0, 0, 0}), false))//上面的肩膀右
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("upperarm_l", {0, 0, 0}), false))//上面的肩膀左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("lowerarm_r", {0, 0, 0}), false))//上面的手臂右
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("lowerarm_l", {0, 0, 0}), false))//上面的手臂左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_03", {0, 0, 0}), false))//脊柱3
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_02", {0, 0, 0}), false))//脊柱2
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_01", {0, 0, 0}), false))//脊柱2
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("pelvis", {0, 0, 0}), false))//骨盆
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("thigh_l", {0, 0, 0}), false))//大腿左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("thigh_r", {0, 0, 0}), false))//大腿右
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("calf_l", {0, 0, 0}), false))//小腿左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("calf_r", {0, 0, 0}), false))//小腿右
                continue;

                static bool isSelected = false;
                algorithm = 0;
                isSelected = false;
                 

                
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("Head", {0, 0, 0}),  false)) {//头
                 isHead = false;
                }else{
                 isHead = true;
                }
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("pelvis", {0, 0, 0}),  false))
                {//骨盆
                 isPelvis = false;
                }else{
                 isPelvis = true;
                }
                                               
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("neck_01", {0, 0, 0}),  false))
                {//Neck
                 isNeck = false;
                }else{
                 isNeck = true;
                }
                                                
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("hand_l", {0, 0, 0}),  false))
                {//左手
                 isLeftHand = false;
                }else{
                 isLeftHand = true;
                }
                                                
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("hand_r", {0, 0, 0}),  false))
                {//右手
                 isRightHand = false;
                }else{
                 isRightHand = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("foot_l", {0, 0, 0}),  false))
                {//左脚
                 isLeftFoot = false;
                }else{
                 isLeftFoot = true;
                }
                     
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("foot_r", {0, 0, 0}),  false))
                {//右脚
                 isRightFoot = false;
                }else{
                 isRightFoot = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("calf_l", {0, 0, 0}),  false))
                {//左小腿
                 isLeftCalf = false;
                }else{
                 isLeftCalf = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("calf_r", {0, 0, 0}),  false))
                {//右小腿
                 isRightCalf = false;
                }else{
                 isRightCalf = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("lowerarm_l", {0, 0, 0}),  false))
                {//左小臂
                 isLeftLowerArm = false;
                }else{
                 isLeftLowerArm = true;
                }
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("lowerarm_r", {0, 0, 0}),  false))
                {//右小臂
                 isRightLowerArm = false;
                }else{
                 isRightLowerArm = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("thigh_l", {0, 0, 0}),  false))
                {//左上臂
                 isLeftThigh = false;
                }else{
                 isLeftThigh = true;
                }
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("thigh_r", {0, 0, 0}),  false))
                {//左上臂
                 isRightThigh = false;
                }else{
                 isRightThigh = true;
                }
                                                                                                
                if (!isSelected)
                if(isHead)
                {
                  algorithm = 1;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isPelvis)
                {// JOIN = TG >> @JKVIPOWNER
                  algorithm = 2;
                  isSelected = true;
                }else{
                  isSelected = false;
                }// JOIN = TG >> @JKVIPOWNER
                if (!isSelected)
                if(isLeftCalf)
                {// JOIN = TG >> @JKVIPOWNER
                  algorithm = 3;
                  isSelected = true;
                }else{
                  isSelected = false;
                }// JOIN = TG >> @JKVIPOWNER
                if (!isSelected)
                if(isRightCalf)
                {// JOIN = TG >> @JKVIPOWNER
                  algorithm = 4;   
                  isSelected = true;
                }else{
                  isSelected = false;
                }// JOIN = TG >> @JKVIPOWNER
                if (!isSelected)
                if(isLeftLowerArm)
                {// JOIN = TG >> @JKVIPOWNER
                  algorithm = 5;
                  isSelected = true;
                }else{
                  isSelected = false;
                }// JOIN = TG >> @JKVIPOWNER
                if (!isSelected)
                if(isRightLowerArm)
                {
                  algorithm = 6;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftUpperArm)
                {
                  algorithm = 7;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightUpperArm)
                {
                  algorithm = 8;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftThigh)
                {
                  algorithm = 9;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightThigh)
                {
                  algorithm = 10;
                  isSelected = true;
                 }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftFoot)
                {
                  algorithm = 11;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightFoot)
                {
                  algorithm = 12;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                }

                if (trackingType == 0) {
                   float dist = localPlayer->GetDistanceTo(Player);
                   if (dist < max) {
                      max = dist;
                      result = Player;
                    }
                }
                
                if (trackingType == 1) {
                   auto Root = Player->GetBonePos("Root", {});
                   auto Head = Player->GetBonePos("Head", {});

                    FVector2D RootSc, HeadSc;
                     if (W2S(Root, &RootSc) && W2S(Head, &HeadSc))
                      {
                         float height = abs(HeadSc.Y - RootSc.Y);
                         float width = height * 0.65f;
                        FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),0};
                        if ((middlePoint.X >= 0 && middlePoint.X <= screenWidth) && (middlePoint.Y >= 0 && middlePoint.Y <= screenHeight))
                        {
                        FVector2D v2Middle = FVector2D((float)(screenWidth / 2), (float)(screenHeight / 2));
                        FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);
                        if(isInsideFOV((int)middlePoint.X, (int)middlePoint.Y)) {
                           float dist = FVector2D::Distance(v2Middle, v2Loc);
                           if (dist < max) {
                                max = dist;
                                result = Player;
                               }                           
                            }
                        }
                    }
                }                
            }
        }
    }    
    return result;
}


bool isInsideFOVs(int x, int y)
{
    if (!Config.AimBot.Radius)
        return true;

    int circle_x = glWidth / 2;
    int circle_y = glHeight / 2;
    int rad = Config.AimBot.Radius;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

auto GetTargetForAimBot()
{
    ASTExtraPlayerCharacter *result = nullptr;
    float max = std::numeric_limits<float>::infinity();
    auto Actors = GetActors();
    if (g_LocalPlayer)
    {
        for (int i = 0; i < Actors.size(); i++)
        {
            auto Actor = Actors[i];
            if (isObjectInvalid(Actor))
                continue;

            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
            {
                auto Player = (ASTExtraPlayerCharacter *)Actor;

                float dist = g_LocalPlayer->GetDistanceTo(Player) / 100.0f;
                if (dist > 150.0f)
                    continue;

                if (Player->PlayerKey == g_PlayerController->PlayerKey)
                    continue;
                if (Player->TeamID == g_PlayerController->TeamID)
                    continue;
                if (Player->bDead)
                    continue;

                if (Config.AimBot.IgnoreKnocked)
                {
                    if (Player->Health == 0.0f)
                        continue;
                }

                if (Config.AimBot.VisCheck)
                {
                    if (!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("Head", {}), true))
                        continue;
                }

                if (trackingType == 0) {
                   float dist = g_LocalPlayer->GetDistanceTo(Player);
                   if (dist < max) {
                      max = dist;
                      result = Player;
                    }
                }
                
                if (trackingType == 1) {
                   auto Root = Player->GetBonePos("Root", {});
                   auto Head = Player->GetBonePos("Head", {});

                    FVector2D RootSc, HeadSc;
                     if (W2S(Root, &RootSc) && W2S(Head, &HeadSc))
                      {
                         float height = abs(HeadSc.Y - RootSc.Y);
                         float width = height * 0.65f;
                        FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),0};
                        if ((middlePoint.X >= 0 && middlePoint.X <= screenWidth) && (middlePoint.Y >= 0 && middlePoint.Y <= screenHeight))
                        {
                        FVector2D v2Middle = FVector2D((float)(screenWidth / 2), (float)(screenHeight / 2));
                        FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);
                        if(isInsideFOVs((int)middlePoint.X, (int)middlePoint.Y)) {
                           float dist = FVector2D::Distance(v2Middle, v2Loc);
                           if (dist < max) {
                                max = dist;
                                result = Player;
                               }                           
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}


auto GetTargetByPussy() {
    ASTExtraPlayerCharacter *result = 0;
    float max = std::numeric_limits<float>::infinity();

    auto GWorld = GetFullWorld();
    if (GWorld) {
        ULevel *PersistentLevel = GWorld->PersistentLevel;
        if (PersistentLevel) {
            TArray<AActor *> Actors = *(TArray<AActor *> *) ((uintptr_t) PersistentLevel +
                                                             Actors_Offset);


            auto localPlayer = g_LocalPlayer;
            auto localController = g_PlayerController;

            if (localPlayer) {
                for (int i = 0; i < Actors.Num(); i++) {
                    auto Actor = Actors[i];
                    if (isObjectInvalid(Actor))
                        continue;

            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {
                auto Player = (ASTExtraPlayerCharacter *) Actor;

                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;

                if (Player->TeamID == localPlayer->TeamID)
                    continue;

                if (Player->bDead)
                    continue;

                if (Config.SilentAim.IgnoreKnocked) {
                    if (Player->Health == 0.0f)
                        continue;
                }
        
                if (Config.SilentAim.IgnoreBot) {
                    if (Player->bIsAI)
                        continue;
                }
                  
                 
                    if (!localController->LineOfSightTo(Player, {0, 0, 0}, true))
                              continue;
      
                        float dist = g_LocalPlayer->GetDistanceTo(Player);
                             if (dist < max) {
                                 max = dist;
                              result = Player;
                        
							  }
                            }
                        }
                    }
                }
            }
        

    return result;
}



void RenderESP(UCanvas* Canvas, int ScreenWidth, int ScreenHeight)
{




std::string datePart; // Declare outside so it's accessible
// ---- TOP UI TEXT DRAW ---- //
    // ===== DATE / EXPIRY =====
    std::string dayName = getDayName();
    std::string ampm    = getAMPM();
    datePart = dayName + " ACTIVE " + ampm;   // No new declaration

    tslFont->LegacyFontSize = 15;
    DrawOutlinedTextFPS(Canvas,
        FString(datePart.c_str()),
        { (float)screenWidth / 10 + screenWidth / 25.2f, 680 },
        COLOR_RED, COLOR_IN, true);


    ASTExtraPlayerCharacter *localPlayer = 0;
    ASTExtraPlayerController *localPlayerController = 0;

    screenWidth = ScreenWidth;
    screenHeight = ScreenHeight;


            auto Actors = GetActors();
            UGameplayStatics *gGameplayStatics = (UGameplayStatics *)UGameplayStatics::StaticClass();
            auto GWorld = GetFullWorld();
            if (GWorld)
            {
                UNetDriver *NetDriver = GWorld->NetDriver;
                if (NetDriver)
                {
                    UNetConnection *ServerConnection = NetDriver->ServerConnection;
                if (ServerConnection)
                {
                    localPlayerController = (ASTExtraPlayerController *)ServerConnection->PlayerController;
                }
            }
            }
            if (localPlayerController) {
                std::vector<ASTExtraPlayerCharacter *> PlayerCharacter;             
                GetAllActors(PlayerCharacter);
            for (auto actor = PlayerCharacter.begin();
                actor != PlayerCharacter.end(); actor++) {
                 auto Actor = *actor;
            if (Actor->PlayerKey ==((ASTExtraPlayerController *) localPlayerController)->PlayerKey) {
                 localPlayer = Actor;
                 break;
                }
            }
            if (localPlayer) {
            
if (localPlayer->PartHitComponent) {
auto ConfigCollisionDistSqAngles = localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
for (int j = 0; j < ConfigCollisionDistSqAngles.Num(); j++) {
ConfigCollisionDistSqAngles[j].Angle = 90.0f;
}
localPlayer->PartHitComponent->ConfigCollisionDistSqAngles = ConfigCollisionDistSqAngles;
}



// JOIN = TG >> @JKVIPOWNER
			           
          if (Config.AimBot.Enable) {// JOIN = TG >> @JKVIPOWNER
        DrawCircle(Canvas, (screenWidth / 2), (screenHeight / 2), FOVsize, 200, COLOR_CYAN);
                
            ASTExtraPlayerCharacter *Target = GetTargetForAim();
                        if (Target) {
                            bool triggerOk = false;
                            if (Config.AimBot.Trigger != EAimTrigger::None) {
                                if (Config.AimBot.Trigger == EAimTrigger::Shooting) {
                                    triggerOk = g_LocalPlayer->bIsWeaponFiring;
                                } else if (Config.AimBot.Trigger == EAimTrigger::Scoping) {
                                    triggerOk = g_LocalPlayer->bIsGunADS;
                                } else if (Config.AimBot.Trigger == EAimTrigger::Both) {
                                    triggerOk = g_LocalPlayer->bIsWeaponFiring && g_LocalPlayer->bIsGunADS;
                                } else if (Config.AimBot.Trigger == EAimTrigger::Any) {
                                    triggerOk = g_LocalPlayer->bIsWeaponFiring || g_LocalPlayer->bIsGunADS;
                                }
                            } else triggerOk = true;
                            if (triggerOk) {
                                FVector targetAimPos ;
                                                    if (AimHead) {
                       if(algorithm == 0) {
                        targetAimPos=Target->GetBonePos("Head", {});
                       } else if(algorithm == 1) {
                        targetAimPos = Target->GetBonePos("Head", {});
                       }else if(algorithm == 2){
                        targetAimPos = Target->GetBonePos("pelvis", {});//锁骨
                       }else if(algorithm == 3){
                        targetAimPos = Target->GetBonePos("calf_l", {});//左小腿
                       }else if(algorithm == 4){
                        targetAimPos = Target->GetBonePos("calf_r", {});//右小腿
                       }else if(algorithm == 5){
                        targetAimPos = Target->GetBonePos("lowerarm_l", {});//左小臂
                       }else if(algorithm == 6){
                        targetAimPos = Target->GetBonePos("lowerarm_r", {});//右小臂
                       }else if(algorithm == 7){
                        targetAimPos = Target->GetBonePos("upperarm_l", {});//左上臂
                       }else if(algorithm == 8){
                        targetAimPos = Target->GetBonePos("upperarm_r", {});//右上臂
                       }else if(algorithm == 9) {
                        targetAimPos = Target->GetBonePos("thigh_l", {});//左大腿
                       }else if(algorithm == 10) {
                        targetAimPos = Target->GetBonePos("thigh_r", {});//右大腿
                       }else if(algorithm == 11) {
                        targetAimPos = Target->GetBonePos("foot_l", {});//左脚
                       }else if(algorithm == 12){
                        targetAimPos = Target->GetBonePos("foot_r", {});//右脚
                       }
                     }
                     if(AimBody){
                       if(algorithm == 0) {
                        targetAimPos = Target->GetBonePos("Head", {});//头
                       }else if(algorithm == 1) {
                        targetAimPos = Target->GetBonePos("neck_01", {});//Neck
                       }else if(algorithm == 2){
                        targetAimPos = Target->GetBonePos("pelvis", {});//屁股
                       }else if(algorithm == 3){
                        targetAimPos = Target->GetBonePos("calf_l", {});//左小腿
                       }else if(algorithm == 4){
                        targetAimPos = Target->GetBonePos("calf_r", {});//右小腿
                       }else if(algorithm == 5){
                        targetAimPos = Target->GetBonePos("lowerarm_l", {});//左小臂
                       }else if(algorithm == 6){
                        targetAimPos = Target->GetBonePos("lowerarm_r", {});//右小臂
                       }else if(algorithm == 7){
                        targetAimPos = Target->GetBonePos("upperarm_l", {});//左上臂
                       }else if(algorithm == 8){
                        targetAimPos = Target->GetBonePos("upperarm_r", {});//右上臂
                       }else if(algorithm == 9) {
                        targetAimPos = Target->GetBonePos("thigh_l", {});//左大腿
                       }else if(algorithm == 10) {
                        targetAimPos = Target->GetBonePos("thigh_r", {});//右大腿
                       }else if(algorithm == 11) {
                        targetAimPos = Target->GetBonePos("foot_l", {});//左脚
                       }else if(algorithm == 12){
                        targetAimPos = Target->GetBonePos("foot_r", {});//右脚
                        }
                     }// JOIN = TG >> @JKVIPOWNER

                                auto WeaponManagerComponent = g_LocalPlayer->WeaponManagerComponent;
                                if (WeaponManagerComponent) {
                                    auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
                                    if ((int) propSlot.GetValue() >= 1 && (int) propSlot.GetValue() <= 3) {
                                        auto CurrentWeaponReplicated = (ASTExtraShootWeapon *) WeaponManagerComponent->CurrentWeaponReplicated;
                                        if (CurrentWeaponReplicated) {
                                            auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
                                            if (ShootWeaponComponent) {
                                                UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
                                                if (ShootWeaponEntityComponent) {
                                                    ASTExtraVehicleBase *CurrentVehicle = Target->CurrentVehicle;
                                                    if (CurrentVehicle) {
                                                        FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;

                                                        float dist = g_LocalPlayer->GetDistanceTo(Target);
                                                        auto timeToTravel = dist / ShootWeaponEntityComponent->BulletRange;

                                                        targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(LinearVelocity, timeToTravel));
                                                    } else {
                                                        FVector Velocity = Target->GetVelocity();

                                                        float dist = g_LocalPlayer->GetDistanceTo(Target);
                                                        auto timeToTravel = dist / ShootWeaponEntityComponent->BulletRange;

                                                        targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(Velocity, timeToTravel));
                                                        }
                                                    g_PlayerController->SetControlRotation(ToRotator(g_PlayerController->PlayerCameraManager->CameraCache.POV.Location, targetAimPos), "");
                                                }
                                                            

                                                
                                                if (Config.AimBot.RecoilSet) {
                                                    if (g_LocalPlayer->bIsGunADS) {
                                                        if (g_LocalPlayer->bIsWeaponFiring) {
                                                            float dist = g_LocalPlayer->GetDistanceTo(Target) / 100.f;                                                                                 
                                                            targetAimPos.Z -= dist * Config.AimBot.RecoilSet;
                                                        }  
                                                    }
                                                }
                                                g_PlayerController->SetControlRotation(ToRotator(g_PlayerController->PlayerCameraManager->CameraCache.POV.Location, targetAimPos), "");
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    
                    
               
                int totalEnemies = 0, totalBots = 0;
                std::vector<ASTExtraPlayerCharacter *> PlayerCharacter;
                GetAllActors(PlayerCharacter);
                for (auto actor = PlayerCharacter.begin(); actor != PlayerCharacter.end(); actor++)
                {

                auto Player = *actor;
                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;
                if (Player->TeamID == localPlayer->TeamID)
                    continue;
                if (Player->bDead)
                    continue;
                if (Player->bHidden)
                    continue;
                                                                
                if (!Player->RootComponent)
                    continue;
                
                if (Player->bEnsure)
                 totalBots++;
                 else totalEnemies++;
                        
                float Distance = localPlayer->GetDistanceTo(Player) / 100.0f;
                if (Distance > 500)
                continue;            
                                                
                FVector HeadPos = GetBoneLocationByName(Player,"Head");
                FVector2D HeadPosSC;
                FVector RootPos = GetBoneLocationByName(Player,"Root");
                FVector2D RootPosSC;
                FVector Root = GetBoneLocationByName(Player,"Root");
                FVector Spin = GetBoneLocationByName(Player,"pelvis");
                FVector Spin2 = GetBoneLocationByName(Player,"spine_03");
                FVector pelvis = GetBoneLocationByName(Player,"pelvis");
                FVector2D pelvisPoSC;
                FVector upper_r = GetBoneLocationByName(Player,"upperarm_r");
                FVector2D upper_rPoSC;
                FVector lowerarm_r = GetBoneLocationByName(Player,"lowerarm_r");
                FVector2D lowerarm_rPoSC;
                FVector lowerarm_l = GetBoneLocationByName(Player,"lowerarm_l");
                FVector2D lowerarm_lSC;
                FVector hand_r = GetBoneLocationByName(Player,"hand_r");
                FVector2D hand_rPoSC;
                FVector upper_l = GetBoneLocationByName(Player,"upperarm_l");
                FVector2D upper_lPoSC;
                FVector hand_l = GetBoneLocationByName(Player,"hand_l");
                FVector2D hand_lPoSC;
                FVector thigh_l = GetBoneLocationByName(Player,"thigh_l");
                FVector2D thigh_lPoSC;
                FVector calf_l = GetBoneLocationByName(Player,"calf_l");
                FVector2D calf_lPoSC;
                FVector foot_l = GetBoneLocationByName(Player,"foot_l");
                FVector2D foot_lPoSC;
                FVector thigh_r = GetBoneLocationByName(Player,"thigh_r");
                FVector2D thigh_rPoSC;
                FVector calf_r = GetBoneLocationByName(Player,"calf_r");
                FVector2D calf_rPoSC;
                FVector foot_r = GetBoneLocationByName(Player,"foot_r");
                FVector2D foot_rPoSC;
                FVector neck_01 = GetBoneLocationByName(Player,"neck_01");
                FVector2D neck_01PoSC;
                FVector spine_01 = GetBoneLocationByName(Player,"spine_01");
                FVector2D spine_01PoSC;
                FVector spine_02 = GetBoneLocationByName(Player,"spine_02");
                FVector2D spine_02PoSC;
                FVector spine_03 = GetBoneLocationByName(Player,"spine_03");
                FVector2D spine_03PoSC;               
////==========================================================================================================//
                if (gGameplayStatics->ProjectWorldToScreen(g_PlayerController, HeadPos, false, &HeadPosSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, lowerarm_l, false, &lowerarm_lSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, upper_r, false, &upper_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, upper_l, false, &upper_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, lowerarm_r, false, &lowerarm_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, hand_r, false, &hand_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, hand_l, false, &hand_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, thigh_l, false, &thigh_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, calf_l, false, &calf_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, foot_l, false, &foot_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, thigh_r, false, &thigh_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, calf_r, false, &calf_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, foot_r, false, &foot_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, neck_01, false, &neck_01PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, pelvis, false, &pelvisPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, RootPos, false, &RootPosSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_01, false, &spine_01PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_02, false, &spine_02PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_03, false, &spine_03PoSC)) {
////==========================================================================================================//

                bool IsVisible = g_PlayerController->LineOfSightTo(Player, {0,0,0}, true);
                

if (Config.PlayerESP.Skeleton) {
    static std::vector<std::string> right_arm{"neck_01", "clavicle_r", "upperarm_r", "lowerarm_r", "hand_r"};
    static std::vector<std::string> left_arm{"neck_01", "clavicle_l", "upperarm_l", "lowerarm_l", "hand_l"};
    static std::vector<std::string> spine{"Head", "neck_01", "spine_01", "pelvis"};
    static std::vector<std::string> lower_right{"pelvis", "thigh_r", "calf_r", "foot_r"};
    static std::vector<std::string> lower_left{"pelvis", "thigh_l", "calf_l", "foot_l"};
    static std::vector<std::vector<std::string>> skeleton{right_arm, left_arm, spine, lower_right, lower_left};
    if (!Player || !Canvas || !localPlayerController) {
        return;
    }

    static std::unordered_map<std::string, int> boneToIndex;
    static std::vector<std::string> boneNames;
    static bool bInitialized = false;
    if (!bInitialized) {
        int index = 0;
        boneNames.reserve(32);
        for (const auto& boneStructure : skeleton) {
            for (const auto& bone : boneStructure) {
                if (boneToIndex.emplace(bone, index).second) {
                    boneNames.push_back(bone);
                    ++index;
                }
            }
        }
        bInitialized = true;
    }

    std::vector<FVector> boneLocations(boneNames.size());
    for (auto &v : boneLocations) {
        v.X = 0.0f;
        v.Y = 0.0f;
        v.Z = 0.0f;
    }
    for (size_t i = 0; i < boneNames.size(); ++i) {
        boneLocations[i] = GetBoneLocationByName(Player, boneNames[i].c_str());
    }

    std::vector<std::pair<FVector2D, bool>> screenPositions(boneNames.size(), {FVector2D(0, 0), false});
    for (size_t i = 0; i < boneNames.size(); ++i) {
        FVector2D screenPos;
        bool bProjected = gGameplayStatics->ProjectWorldToScreen(localPlayerController, boneLocations[i], false, &screenPos);
        bool bOnScreen = bProjected &&
        screenPos.X >= -200 && screenPos.X <= Canvas->SizeX + 200 &&
        screenPos.Y >= -200 && screenPos.Y <= Canvas->SizeY + 200;
        screenPositions[i] = {screenPos, bOnScreen};
    }

    // Premium skeleton colors
    FLinearColor skeletonColor = IsVisible ? 
        (Player->bEnsure ? FLinearColor(0.2f, 0.6f, 1.0f, 0.95f) : FLinearColor(0.1f, 0.9f, 0.3f, 0.95f)) : 
        (Player->bEnsure ? FLinearColor(0.5f, 0.5f, 0.7f, 0.65f) : FLinearColor(0.9f, 0.2f, 0.1f, 0.75f));
    
    for (auto& boneStructure : skeleton) {
        std::string lastBone;
        for (const std::string& currentBone : boneStructure) {
            if (!lastBone.empty()) {
                int lastBoneIndex = boneToIndex[lastBone];
                int currentBoneIndex = boneToIndex[currentBone];
                if (screenPositions[lastBoneIndex].second && screenPositions[currentBoneIndex].second) {
                    DrawLine(Canvas, screenPositions[lastBoneIndex].first, screenPositions[currentBoneIndex].first, 1.4f, skeletonColor);
                    
                    // Premium glow for skeleton
                    if (IsVisible) {
                        FLinearColor glowColor = skeletonColor;
                        glowColor.A = 0.25f;
                        DrawLine(Canvas, screenPositions[lastBoneIndex].first, screenPositions[currentBoneIndex].first, 2.2f, glowColor);
                    }
                }
            }
            lastBone = currentBone;
        }
    }
}            

if (Config.PlayerESP.Skeleton) {
    constexpr float HeadOffsetY = 12.0f;
    constexpr float StartLineY = 82.0f;

    FVector2D targetPos = { HeadPosSC.X, HeadPosSC.Y - HeadOffsetY };
    FVector2D screenCenter = { static_cast<float>(screenWidth) / 2.0f, StartLineY };

    const bool bIsBot = Player->bEnsure;
    
    // Same premium colors as skeleton
    const FLinearColor lineColor = IsVisible
        ? (bIsBot ? FLinearColor(0.2f, 0.6f, 1.0f, 0.85f) : FLinearColor(0.1f, 0.9f, 0.3f, 0.9f))
        : (bIsBot ? FLinearColor(0.5f, 0.5f, 0.7f, 0.6f) : FLinearColor(0.9f, 0.2f, 0.1f, 0.75f));

    // Main targeting line
    DrawLine(Canvas, screenCenter, targetPos, 1.6f, lineColor);

    // Premium glow effect for both enemy and bot
    if (IsVisible) {
        FLinearColor glowColor = lineColor;
        glowColor.A = 0.3f;
        DrawLine(Canvas, screenCenter, targetPos, 2.8f, glowColor);
    }
}

////==========================================================================================================//                             
if (Config.PlayerESP.Name) {
    FVector BelowRoot = Root;
    BelowRoot.Z -= 55.0f;

    FVector2D BelowRootSc;
    if (gGameplayStatics->ProjectWorldToScreen(localPlayerController, BelowRoot, false, &BelowRootSc)) {
        std::wstring wsName;
        std::wstring wsDist;
        std::wstring wsFullInfo;

        if (Player->bEnsure) {
            // Bot
            wsName = L"AI";
            wsDist = std::to_wstring((int)Distance) + L"M";   // ✅ bot ke liye bhi distance set
        } else {
            // Player
            std::wstring wsTeam = L"(" + std::to_wstring(Player->TeamID) + L")";
            wsName = Player->PlayerName.ToWString();
            wsFullInfo = wsTeam + L" - " + wsName;
            wsDist = std::to_wstring((int)Distance) + L"M";
        }

        // Adjust font size by distance
        tslFont->LegacyFontSize = std::max(5, 12 - (int)(Distance / 80));

        if (Player->bEnsure) {
            // Draw Bot name
            DrawOutlinedTextFPS(Canvas,
                FString(wsName),   // "Bot"
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_YELLOW, COLOR_BLACK, true);

            // Bot distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    COLOR_CYAN, COLOR_BLACK, true);
            }

        } else {
            // Player Name + Team
            DrawOutlinedTextFPS(Canvas,
                FString(wsFullInfo),
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_YELLOW, COLOR_BLACK, true);

            // Player distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    COLOR_CYAN, COLOR_BLACK, true);
            }

        }
    }
}

////==========================================================================================================//
    if (Config.PlayerESP.Health) {
float CurHP = Player->Health;
if (CurHP < 0.0f) CurHP = 0.0f;
else if (CurHP > Player->HealthMax) CurHP = Player->HealthMax;

float MaxHP = (Player->HealthMax > 0) ? Player->HealthMax : 1.0f;
float HealthPercentage = CurHP / MaxHP;

// MAIN COLORS
FLinearColor FillColor = FLinearColor(0.0f, 1.0f, 1.0f, 0.55f);   // CYAN (transparent)
FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.90f); // WHITE
FLinearColor BackShadow = FLinearColor(0.0f, 0.0f, 0.0f, 0.30f);  // SHADOW

// enemy ensured → green glow
if (Player->bEnsure)
    FillColor = FLinearColor(0.2f, 1.0f, 0.2f, 0.65f);

FVector AboveHead = Player->GetHeadLocation(true);
AboveHead.Z += 28.0f;

FVector2D AboveHeadSc;
if (gGameplayStatics->ProjectWorldToScreen(localPlayerController, AboveHead, false, &AboveHeadSc))
{
    float BarWidth = 58.0f;
    float BarHeight = 5.0f;

    FVector2D BarPos(
        AboveHeadSc.X - BarWidth / 2.0f,
        AboveHeadSc.Y - BarHeight * 2.0f
    );// JOIN = TG >> @JKVIPOWNER

    // BG SHADOW
    DrawTrueTransparentRect(Canvas,
        {BarPos.X - 2.0f, BarPos.Y - 2.0f},
        BarWidth + 4.0f,
        BarHeight + 4.0f,
        BackShadow
    );// JOIN = TG >> @JKVIPOWNER

    // WHITE BORDER
    DrawTrueTransparentRect(Canvas,
        {BarPos.X - 1.0f, BarPos.Y - 1.0f},
        BarWidth + 2.0f,
        BarHeight + 2.0f,
        BorderColor
    );// JOIN = TG >> @JKVIPOWNER

    // TRANSPARENT FILL
    float FillSize = BarWidth * HealthPercentage;
    DrawTrueTransparentRect(Canvas,
        {BarPos.X, BarPos.Y},
        FillSize,
        BarHeight,
        FillColor
    );// JOIN = TG >> @JKVIPOWNER
}// JOIN = TG >> @JKVIPOWNER
}// JOIN = TG >> @JKVIPOWNER
}// JOIN = TG >> @JKVIPOWNER
} // JOIN = TG >> @JKVIPOWNER

/*
if (Config.PlayerESP.MessageBox)
{
    static bool hasTriggered = false; // Add a flag to control execution
    
    if (!hasTriggered) // Check if the code has already been executed
    {
        std::vector<ASTExtraGameStateBase*> ChickenBase;
        GetAllActors(ChickenBase);

        for (auto actor = ChickenBase.begin(); actor != ChickenBase.end(); actor++) {
            auto InGame = *actor;
            
            if ((int)InGame->AliveTeamNum == 1)
            {
                MessageBoxExt(0, (char16_t *)xxx4.c_str(), (char16_t *)f3.c_str());
                hasTriggered = true; // Set the flag to true to prevent re-execution
                break; // Exit the loop after execution
            }
        }
    }
}
*/
 
if (Config.PlayerESP.Vehicle) {                
    std::vector<ASTExtraVehicleBase*> VehicleBase;
    GetAllActors(VehicleBase);
    
    for (auto actor = VehicleBase.begin(); actor != VehicleBase.end(); actor++) {
        auto Vehicle = *actor;
        if (!Vehicle->Mesh)
            continue;
        if (!Vehicle->RootComponent)
            continue;
        
        float Distance = Vehicle->GetDistanceTo(localPlayer) / 100.f;
        if (Distance > 500)
            continue;
        
        FVector2D VehiclePos;
        if (gGameplayStatics->ProjectWorldToScreen(g_PlayerController, Vehicle->RootComponent->RelativeLocation, false, &VehiclePos)) {
            std::string vehicleText = GetVehicleName(Vehicle); // Get only vehicle name
            std::string distanceText = std::to_string((int)Distance) + "M"; // Vehicle distance
           
            // Adjust font size based on distance
            tslFont->LegacyFontSize = max(6, 12 - (int)(Distance / 80));
            
            // Draw vehicle distance (on top)
            DrawOutlinedTextFPS(Canvas, FString(distanceText), 
                FVector2D(VehiclePos.X, VehiclePos.Y +20), 
                COLOR_YELLOW, COLOR_IN, true);

            // Draw vehicle name (below distance)
            DrawOutlinedTextFPS(Canvas, FString(vehicleText), 
                FVector2D(VehiclePos.X, VehiclePos.Y), // Move name down by 15 pixels
                COLOR_YELLOW, COLOR_IN, true);
        }
    }
}
                                    
                                    
                         
                                    
if (Config.PlayerESP.ItemEsp) {
    std::vector<APickUpListWrapperActor*> LootboxBase;
    GetAllActors(LootboxBase);
                   
    for (auto actor = LootboxBase.begin(); actor != LootboxBase.end(); actor++) {
        auto Pick = *actor;                                             
                                                
        if (!Pick->RootComponent)
            continue;

        float Distance = Pick->GetDistanceTo(localPlayer) / 100.0f;
                                                                    
        if (Distance > 150.0)
            continue;

        FVector2D PickUpListsPos;                  
                    
        if (W2S(Pick->K2_GetActorLocation(), &PickUpListsPos)) {
            std::string lootBoxText = "LootBox";
            std::string distanceText = std::to_string((int)Distance) + "M";

            tslFont->LegacyFontSize = max(7, 12 - (int)(Distance / 80));

            // LootBox naam draw karo
            DrawOutlinedTextFPS(Canvas, FString(distanceText), {PickUpListsPos.X, PickUpListsPos.Y +20}, COLOR_WHITE, COLOR_IN, true);
            
            // Distance naam ke niche draw karo
            DrawOutlinedTextFPS(Canvas, FString(lootBoxText), {PickUpListsPos.X, PickUpListsPos.Y }, COLOR_WHITE, COLOR_IN, true);
        }
    }
}



	
	
            
            
            
            UpdateFPS();

    std::string baseStd = "BATTLE ROYALE MODZ";

    char fpsBuffer[32];
   snprintf(fpsBuffer, sizeof(fpsBuffer), " BGMI 4.4 ", currentFPS);

    std::string combined = baseStd + fpsBuffer;

 //   Direct FString from runtime std::string
    FString displayText(combined.c_str());

    tslFont->LegacyFontSize = 16;

    float textWidth = 200.0f;
    float textHeight = 30.0f;
    float margin = 15.0f;

    float yPos = 685;
    float xPos = screenWidth - textWidth - margin;

DrawOutlinedText(Canvas, displayText, FVector2D(xPos, yPos), COLOR_CYAN, COLOR_IN, true);
            
            
            



		








				

 
	

              g_LocalPlayer = localPlayer;
g_PlayerController = localPlayerController;

        Config.PlayerESP.Line = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Skeleton = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Health = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Name = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Distance = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.TeamID = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Vehicle = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Grenade = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Weapon = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.ItemEsp = true;// JOIN = TG >> @JKVIPOWNER
        Config.PlayerESP.Alert = true;// JOIN = TG >> @JKVIPOWNER
     

          
      Config.AimBot.Enable = true;// JOIN = TG >> @JKVIPOWNER
        Config.AimBot.Radius = true;// JOIN = TG >> @JKVIPOWNER
        Config.AimBot.RecoilSet = 1.2f;    // JOIN = TG >> @JKVIPOWNER         
        Config.AimBot.IgnoreKnocked = true;// JOIN = TG >> @JKVIPOWNER
        Config.AimBot.IgnoreBot = false;// JOIN = TG >> @JKVIPOWNER
        Config.AimBot.VisCheck = true;// JOIN = TG >> @JKVIPOWNER
        Config.AimBot.Trigger = EAimTrigger::Shooting;// JOIN = TG >> @JKVIPOWNER
        Config.AimBot.Target = EAimTarget::Head;// JOIN = TG >> @JKVIPOWNER

        FOVSize = 200.f;





g_LocalPlayer = localPlayer;
g_PlayerController = localPlayerController;


std::string playerInfo;

// Total count
int totalPlayers = totalEnemies + totalBots;
std::wstring playerCount = std::to_wstring(totalPlayers);

// Box size
float rectWidth = 30.0f;
float rectHeight = 30.0f;

// Box position
FVector2D rectPos(screenWidth / 2.0f - rectWidth / 2.0f, 26.0f);

// ⭐ Cyan background
FLinearColor bgColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.f);
DrawTrueTransparentRect(Canvas, rectPos, rectWidth, rectHeight, bgColor);

// Text position
FVector2D textPos(screenWidth / 2.0f, 42.0f);

// ⭐ Black text
FLinearColor textColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.f);
FLinearColor outlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.8f);

// Smaller font
tslFont->LegacyFontSize = 14;

// Draw centered count
DrawOutlinedTextFPS(Canvas, FString(playerCount), textPos, textColor, outlineColor, true);


    }
}

/*
    
    if(Canvas) {
    g_LocalPlayer = localPlayer;
    g_PlayerController = localPlayerController;

    int screenWidth = Canvas->SizeX;
    int screenHeight = Canvas->SizeY;

    const char* Menunamee = "TERI MAA KI CHUT BHIKARI";
    
    // Set font size proportionally (e.g., 4% of screen width)
    tslFont->LegacyFontSize = screenWidth * 0.012f; // Adjust multiplier as needed

    // Calculate text width dynamically
    int textWidthMenunamee = strlen(Menunamee) * tslFont->LegacyFontSize * 0.1f; 

    // Center text dynamically
    int centerX = screenWidth / 2;
    int menunameePositionX = centerX - (textWidthMenunamee / 2);
    
    // ✅ Thoda upar shift (pehle 0.16 tha → ab 0.12)
    int menunameePositionY = screenHeight * 0.18f; 

    // Draw text with outline
    DrawOutlinedTextFPS(Canvas, Menunamee, {menunameePositionX, menunameePositionY}, COLOR_CYAN, COLOR_IN, true);

    }*/


if (XD)
{
if (g_PlayerController != nullptr)
{
if (g_LocalPlayer->bIsWeaponFiring || g_LocalPlayer->bIsGunADS) 
{
    auto MyHUD = (ASurviveHUD *)g_PlayerController->MyHUD;
    if (MyHUD == nullptr)
        return;
FLinearColor RedColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);  
FLinearColor WhiteColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);


    auto hitPerformPtr = &MyHUD->HitPerform;

    uintptr_t hitPerformAddress = *(uintptr_t *)(uintptr_t)hitPerformPtr;

    *(float *)((uintptr_t)hitPerformAddress + 0x10) = 9999.0f;
    *(float *)((uintptr_t)hitPerformAddress + 0x50) = 9999.0f;
    *(float *)((uintptr_t)hitPerformAddress + 0x90) = 9999.0f;
    *(float *)((uintptr_t)hitPerformAddress + 0xD0) = 9999.0f;

    MyHUD->SetHitPerformColor(EHitPerformColorType::EHitPerformColor_Head, RedColor);
    MyHUD->SetHitPerformColor(EHitPerformColorType::EHitPerformColor_Body, WhiteColor);
  //  XD = true;
}
}
}




if (g_LocalPlayer && g_PlayerController) {
        Config.PlayerESP.Line = true;
        Config.PlayerESP.Skeleton = true;
        Config.PlayerESP.Health = true;
        Config.PlayerESP.Name = true;
        Config.PlayerESP.Distance = true;
        Config.PlayerESP.TeamID = true;
        Config.PlayerESP.Vehicle = true;
        Config.PlayerESP.Grenade = true;
        Config.PlayerESP.Weapon = true;
        Config.PlayerESP.ItemEsp = true;
        Config.PlayerESP.Alert = true;


        
        Config.AimBot.Enable = true;
     
        Config.AimBot.Radius = true;
        Config.AimBot.RecoilSet = 1.2f;             
Config.AimBot.IgnoreKnocked = true;
Config.AimBot.IgnoreBot = false;
Config.AimBot.VisCheck = true;
Config.AimBot.Trigger = EAimTrigger::Shooting;

        FOVSize = 200.f;

}
}

bool fileExists(const std::string &filePath)
{

    std::ifstream file(filePath);

    return file.good();
}

bool isFileEmpty(const std::string &filePath)
{
    std::ifstream file(filePath);
    return file.peek() == std::ifstream::traits_type::eof();
}

bool directoryExists(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
    {
        return false;
    }
}

static char keyForLogin[64];
void GetKey()
{
    char keypath[256];

    sprintf(keypath, "/sdcard/Android/obb/%s/key.lic", Gamepackage);

    int fd = open(keypath, O_RDONLY);
    read(fd, &keyForLogin, sizeof(keyForLogin));
    close(fd);
}


void logError(const char *errorMessage)
{
    char filePath[256];
    sprintf(filePath, "/sdcard/Android/obb/%s/KeyPllzz.txt", Gamepackage);

    int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fileDescriptor != -1)
    {
        write(fileDescriptor, errorMessage, strlen(errorMessage));
        write(fileDescriptor, "\n", 1);

        close(fileDescriptor);
    }
}

bool saveClipboardTextToFile(const char *filePath, const char *clipboardText)
{
    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        return false;
    }

    size_t len = std::strlen(clipboardText);
    ssize_t written = write(fd, clipboardText, len);

    if (written != len)
    {
        return false;
    }
    close(fd);

    return true;
}
// JOIN = TG >> @JKVIPOWNER
 // JOIN = TG >> @JKVIPOWNER
// JOIN = TG >> @JKVIPOWNER


void* CreateMemoryPage(void* realAddress) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    void* aligned_address = (void*)((uintptr_t)realAddress & ~(page_size - 1));
    
    // Add small random delay
    volatile int delay = 0;
    for (int i = 0; i < (std::rand() % 50); i++) { delay++; }
    
    void* fakePage = mmap(nullptr, page_size, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (fakePage == MAP_FAILED) {
        return nullptr;
    }
    
    // Obfuscated memory copy
    char* src = (char*)aligned_address;
    char* dst = (char*)fakePage;
    for (size_t i = 0; i < page_size; i++) {
        dst[i] = src[i];
    }
    
    return fakePage;
}

template <typename T>
void VTable_Hook(void** vtable, int32_t index, T newFunction, T& oldFunction) {
    if (vtable[index] != newFunction) {
        oldFunction = reinterpret_cast<T>(vtable[index]);
        void* fakePage = CreateMemoryPage(&vtable[index]);
        if (!fakePage) return;
        ((void**)fakePage)[index] = (void*)newFunction;

        // Stealthier signal handler setup
        static bool handler_setup = false;
        if (!handler_setup) {
            struct sigaction sa;
            sa.sa_flags = SA_SIGINFO | SA_NODEFER;
            sigemptyset(&sa.sa_mask);
            sa.sa_sigaction = [](int sig, siginfo_t* si, void* ctx) {
                size_t page_size = sysconf(_SC_PAGESIZE);
                void* addr = (void*)((uintptr_t)si->si_addr & ~(page_size - 1));
                mprotect(addr, page_size, PROT_READ);
            };
            sigaction(SIGSEGV, &sa, nullptr);
            handler_setup = true;
        }

        // Change protection before assignment
        mprotect(fakePage, sysconf(_SC_PAGESIZE), PROT_READ);
        
        // Atomic pointer swap
        __atomic_store_n(&vtable[index], ((void**)fakePage)[index], __ATOMIC_SEQ_CST);
    }// JOIN = TG >> @JKVIPOWNER
}// JOIN = TG >> @JKVIPOWNER

void *(*orig_PostRender)(UGameViewportClient* ViewportClient, UCanvas* Canvas);
void *new_PostRender(UGameViewportClient* ViewportClient, UCanvas* Canvas){
    // Add benign looking operations
    if (ViewportClient && Canvas) {
        // Obfuscated ESP call
        if (Canvas->SizeX > 0 && Canvas->SizeY > 0) {
            RenderESP(Canvas, Canvas->SizeX, Canvas->SizeY);
        }// JOIN = TG >> @JKVIPOWNER
    }// JOIN = TG >> @JKVIPOWNER
    
    return orig_PostRender(ViewportClient, Canvas);
}// JOIN = TG >> @JKVIPOWNER

int GetPostRenderIndex() {
    // Obfuscated index calculation
    int base = 200;
    int offset = 66;
    volatile int dummy = base - offset;
    return dummy;
}

void PostrenderDraw() {
    // Add random timing variation
    volatile int scatter = std::rand() % 10;
    for (int i = 0; i < scatter; i++) { /* waste cycles */ }
    
    auto GViewport = GetGameViewport();
    if (GViewport) {
        auto VTable = (void **)GViewport->VTable;
        if (VTable) {
            int postrender_idx = GetPostRenderIndex();
            using PostRender_t = void*(*)(UGameViewportClient*, UCanvas*);
            VTable_Hook<PostRender_t>(VTable, postrender_idx, new_PostRender, orig_PostRender);
        }
    }
}

void FixGameCrash() {// JOIN = TG >> @JKVIPOWNER
        system("rm -rf /data/data/com.pubg.imobile/files/");
       // JOIN = TG >> @JKVIPOWNER 
        system("rm -rf /data/data/com.pubg.imobile/files/obblib");
        system("touch /data/data/com.pubg.imobile/files/obblib");
        system("chmod 000 /data/data/com.pubg.imobile/files/obblib");
        system("rm -rf /data/data/com.pubg.imobile/files/xlog");
        system("touch /data/data/com.pubg.imobile/files/xlog");
        system("chmod 000 /data/data/com.pubg.imobile/files/xlog");
        system("rm -rf /data/data/com.pubg.imobile/app_bugly");
        system("touch /data/data/com.pubg.imobile/app_bugly");
        system("chmod 000 /data/data/com.pubg.imobile/app_bugly");
        system("rm -rf /data/data/com.pubg.imobile/app_crashrecord");
        system("touch /data/data/com.pubg.imobile/app_crashrecord");
        system("chmod 000 /data/data/com.pubg.imobile/app_crashrecord");
        system("rm -rf /data/data/com.pubg.imobile/app_crashSight");
        system("touch /data/data/com.pubg.imobile/app_crashSight");
        system("chmod 000 /data/data/com.pubg.imobile/app_crashSight");
  
        system("rm -rf /data/data/com.pubg.imobile/files/ano_tmp");
        system("touch /data/data/com.pubg.imobile/files/ano_tmp");
        system("chmod 000 /data/data/com.pubg.imobile/files/ano_tmp");
  }// JOIN = TG >> @JKVIPOWNER



bool file_exists(const std::string& path) {
    return (access(path.c_str(), F_OK) != -1);
}

void clean_logs() {// JOIN = TG >> @JKVIPOWNER
    // Get the list of all PUBG Mobile games.
    std::vector<std::string> pubg_games = {
        "com.tencent.ig",
        "com.rekoo.pubgm",
        "com.vng.pubgmobile",
        "com.pubg.imobile",
        "com.pubg.krmobile",
        "com.tencent.iglite",
        "com.tencent.pubgmhd"
    };

    for (const std::string& pubg_game : pubg_games) {
        for (const std::string& file : {
                "app_flutter",
                "app_crashSight",
                "app_crashrecord",
                "app_textures",
                "app_webview",
                "cache",
                "HDmpveSDKLog",
                "code_cache",
                "no_backup",
                "files",
                "UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/RoleInfo",
                "UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/Logs",
                "UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Intermediate",
                "log",
                "hawk_data",
                "login-identifier.txt"
        }) {
            std::string path = "/data/media/0/" + pubg_game + "/" + file;
            if (file_exists(path)) {
                if (remove(path.c_str()) == 0) {
                    ("Deleted file: %s", path.c_str());
                } else {
                    ("Failed to delete file: %s", path.c_str());
                }
            }

            path = "/storage/emulated/0/" + pubg_game + "/" + file;
            if (file_exists(path)) {
                if (remove(path.c_str()) == 0) {
                    ("Deleted file: %s", path.c_str());
                } else {
                    ("Failed to delete file: %s", path.c_str());
                }
            }

            std::string package_name = "/data/data/" + pubg_game;
            path = package_name + "/" + file;
            if (file_exists(path)) {
                if (remove(path.c_str()) == 0) {
                    ("Deleted file: %s", path.c_str());
                } else {
                    ("Failed to delete file: %s", path.c_str());
                }// JOIN = TG >> @JKVIPOWNER
            }// JOIN = TG >> @JKVIPOWNER
        }// JOIN = TG >> @JKVIPOWNER
    }// JOIN = TG >> @JKVIPOWNER
}// JOIN = TG >> @JKVIPOWNER
// JOIN = TG >> @JKVIPOWNER

  
  
  
void *Chameli(void *) {// JOIN = TG >> @JKVIPOWNER
// JOIN = TG >> @JKVIPOWNER
clean_logs();
FixGameCrash();// JOIN = TG >> @JKVIPOWNER

    while (!UE4) {
        UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }    

    while (!g_App) {
        g_App = *(android_app * *)(UE4 + GNativeAndroidApp_Offset);
        
        sleep(1);
    }
   *(uintptr_t *)&MessageBoxExt = UE4 + 0x81BE3CC;// JOIN = TG >> @JKVIPOWNER
    FName::GNames = GetGNames();
    while (!FName::GNames) {
        FName::GNames = GetGNames();
        sleep(1);
    }
    UObject::GUObjectArray = (FUObjectArray * )(UE4 + GUObject_Offset);

    static bool loadFont = false;
    if (!loadFont)
    {
        pthread_t t;
        pthread_create(&t, 0, LoadFont, 0);
        loadFont = true;
    }
    PostrenderDraw();

    return 0;    
    
}// JOIN = TG >> @JKVIPOWNER






__attribute__((constructor)) void _init() {
    pthread_t Ptid;
    pthread_create(&Ptid, 0,Chameli, 0);
}

// JOIN = TG >> @JKVIPOWNER

// JOIN = TG >> @JKVIPOWNER
// JOIN = TG >> @JKVIPOWNER