#include "Offset/SDK.hpp"
using namespace SDK;
using namespace std;
#include <android/native_window_jni.h>
#include <android/log.h>
#include "Main/Tools.h"
#include "Main/Dobby/Dobby.h"
#include "Main/Includes.h"
#include "Main/StrEnc.h"
#include "Main/Vector3.hpp"
#include "Main/Vector2.hpp"
#include "Main/MemoryTools.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include "Main/android_native_app_glue.h"
#include "Main/obfuscate.h"
#include "Main/Eagle_GUI.h"
#include <curl/curl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <unordered_set>
#include <unordered_map> 
#include <thread>
#include <chrono>
#include "Main/json.hpp"
#include "Main/oxorany.cpp"
#include "Bypass.h"
bool bValid = false;
bool g_BypassDone = false; // set by KAMLESH_thread when bypasses complete
bool isLogin = false;
bool Expiry = false;
bool Fps = true;
bool deadboxskin = true;
bool GANDU = false;
bool DX4 = true;
bool Ipad;
bool NARTUO = false;
static std::string EXP = "Key Expiry";
std::string g_Token, g_Auth;
using json = nlohmann::json;
android_app *g_App = 0;
ASTExtraPlayerCharacter *g_LocalPlayer=0;
ASTExtraPlayerController *g_PlayerController =0;
void (*orig_kill_message_event)(ASTExtraPlayerController* thiz, void* FatalDamageParameter) = nullptr;// Kill Message 
#include "LoginKey.h"
#define SLEEP_TIME 1000LL / 120LL
#define TSL_FONT_DEFAULT_SIZE 12

#define PI 3.14159265358979323846
#define RAD2DEG(x) ((float)(x) * (float)(180.f / PI))

uintptr_t GNames_Offset = 0x8394964;
uintptr_t GUObject_Offset = 0xe22f8d0;
uintptr_t GetActorArray = 0xa1018ac;
uintptr_t GNativeAndroidApp_Offset = 0xdf74398;
uintptr_t Actors_Offset = 0xA0;

int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
float density = -1;
uintptr_t UE4;

float screenSizeX = 0;
float screenSizeY = 0;
float FOVsize = 100;
float Speed_Aim = 60.0;
float recoilCompensationFactor = 1.4;
float Range = 250;
bool head;
bool AimHead = true;
bool AimBody = false;
int trackingType = 1;
int scopeAndFire = 0;
float FOVSizea;

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
};


std::map<int, bool> itemConfig;

struct sConfig {
    bool Bypass;
    bool Ipad;
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

    struct sAimMenu {
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

std::string Fuck1 = "Winner Winner Chicken Dinner - #1";
std::u16string f3 = convertToUtf16(Fuck1);

std::string Op4 = "BUY KEYS AND DM FOR REBRANDING AND PANNEL";
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

void DrawFPSCounter(UCanvas* Canvas, int screenWidth, UFont* tslFont)
{
    
}

bool KillMessage = true;
std::chrono::steady_clock::time_point lastWeaponChangeTimei;
std::chrono::steady_clock::time_point lastWeaponChangeTime;
int m4v[] =  { 101004,1010041,1010042,1010043,1010044,1010045,1010046,1010047,1010049, 1101004046, 1101004062, 1101004078, 1101004086, 1101004098, 1101004138, 1101004163,1101004201,1101004209,1101004218, 1101004219,1101004246};
int scar[] = { 101003,1010031,1010032,1010033,1010034,1010035,1010036,1010037,1010039, 1101003057, 1101003070, 1101003080, 1101003119, 1101003146, 1101003167, 1101003181};
int akmv[] = { 101001,1010011,1010012,1010013,1010014,1010015,1010016,1010017,1010019,1101001089, 1101001103, 1101001116, 1101001128, 1101001143, 1101001154, 1101001174,1101001213,1101001231,1101001242,1010012561,1010012491};
int m7[] = { 101008,1101008154};
int awm[] = { 103003,1030033,1030034,1030035,1030036,1030037,1030039, 1103003022, 1103003030, 1103003042, 1103003051, 1103003062, 1103003079};
int kar[] = { 103001, 1103001060, 1103001079, 1103001101, 1103001145, 1103001160, 1103001179,1103001202};
int m24[] = { 103002,1030021,1030022,1030023,1030024,1030025,1030026,1030027, 1103002018, 1103002030, 1103002048, 1103002056, 1103002087};
int dp[] = { 105002, 1105002018, 1105002035, 1105002058, 1105002063,1105002091};
int m249[] = { 105001, 1105001020, 1105001025, 1105001048, 1105001054, 1105001069};
int groza[] = { 101005,1010051,1010052,1010053,1010054,1010055,1010056,1010057,1010059, 1101005019, 1101005025, 1101005038, 1101005043, 1101005052, 1101005082,1101005098};
int aug[] = { 101006, 1101006061, 1101006033, 1101006044, 1101006062, 1101006075};
int m16[] = { 101002, 1101002029, 1101002056, 1101002068, 1101002081, 1101002103};
int uzi[] = { 102001, 1102001024, 1102001036, 1102001058, 1102001069, 1102001089, 1102001102};
int ump[] = { 102002, 1102002043, 1102002061, 1102002090, 1102002117, 1102002124, 1102002129, 1102002136};
int bizon[] = { 102005, 1102005007, 1102005020, 1102005041, 1102005064};
int vectorr[] = { 102003, 1102003020, 1102003031, 1102003065, 1102003080, 1102003100};
//int bizon[] = { 102005, 1102005007, 1102005020, 1102005041};
int tommy[] = { 102004, 1102004018, 1102004034}; 
int honey[] = { 101012,1102004018};
int famas[] = { 101009, 1010091 };
int amr[]   = { 103012, 1030121 };
int mini[]  = { 103005, 1030051 };
int sks[]   = { 103004, 1030041 };
//int qvz[] = {1101007071
int slr[]   = { 103009, 1030091 };
int m1014[] = { 104101, 1041011 };
int ace32[] = { 101102, 1101102007, 1101102017,1011020228};
int pan[] = { 108004, 1108004125, 1108004145, 1108004160, 1108004283, 1108004337, 1108004356, 1108004365, 1108004054, 1108004008};
int P90[] = {102105,1102105028,1102105002,1102105003,1102105004, 1102105012};
int m16s[] = { 205007, 1010020292, 1010020562, 1010020682, 1010020812, 1010021032};
int m16mag[] = { 291002,204013,204011,204012, 1010020291, 1010020561, 1010020681, 1010020811, 1010021031};
int mk14[] = {103007,1030071,1030072,1030073,1030074,1030075,1030076,1030077,1103007020,1103007028};
int DBS[] = {104004,1040041,1040042,1040043,1040044,1040045,1040046,1040047,1040049,1104004035}; 
int S12K[] = {104003,1040031,1040032,1040033,1040034,1040035,1040036,1040037,1040038,1104003037};
int MG3[] = {105010,1050101,1050102,1050103,1050104,1050105,1050106,1050107,1050109,1105010019,1105010008,1105010009,1105010010,1105010011};
int emote1[] = { 2200101,12220023,12219677,12219716,12209401,12220028,12209701,12209801,12209901 };
int emote2[] = { 2200201,12210201,12210601,12220028,12219819,12211801,12212001,12212201,12212401,12209901 };
int emote3[] = { 2200301,12212601,12213201,12219715,12219814,12213601,12213801,12214001,12214201,1407277 };
int SuitX[] = { 403003,1405628,1405870,1405983,1406152,1406311,1406475,1406638,1406872 };
int Bag[] = { 501006,501005,501004,501003,501002,501001,1501003550,1501003277,1501003321,1501003443,1501003265,1501003051,1501003220,1501003174 };
int Helmet[] = { 502001,502002,502003,1502003014,1502003028,1502003023,1501002443,1502003031,1502003033,1502003069,1502003261 };

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
}

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
}

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
}

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
}

SDK::FVector AddVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
    return result;
}

SDK::FVector MultiplyVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X * b.X;
    result.Y = a.Y * b.Y;
    result.Z = a.Z * b.Z;
    return result;
}

SDK::FVector DivideVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X / b.X;
    result.Y = a.Y / b.Y;
    result.Z = a.Z / b.Z;
    return result;
}

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
}
void clampAngles(FRotator &angles) {
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

/*=============================*/






////==========================================================================================================//
void *LoadFont(void *)
{
    while (!tslFontUI || !robotoFont)
    {
        tslFontUI = UObject::FindObject<UFont>("Font Roboto.Roboto");
        robotoFont = UObject::FindObject<UFont>("Font RobotoDistanceField.RobotoDistanceField");
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
}
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
                {
                  algorithm = 2;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftCalf)
                {
                  algorithm = 3;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightCalf)
                {
                  algorithm = 4;   
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftLowerArm)
                {
                  algorithm = 5;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
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
                           float dist = v2Middle.Distance(v2Loc);
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

void kill_message_event(ASTExtraPlayerController* thiz, struct FFatalDamageParameter* FatalDamageParameter) {
    
    // 1. Basic Safety Check: Validate main pointers
    if (thiz && FatalDamageParameter && thiz->STExtraBaseCharacter) {
        
        // Check: Player is Killer AND Not in Vehicle
        if (thiz->PlayerKey == FatalDamageParameter->CauserKey && !thiz->STExtraBaseCharacter->CurrentVehicle) {
            
            // 2. CRASH FIX: Check if LocalPlayer, WeaponManager, and CurrentGun exist
            if (g_LocalPlayer && 
                g_LocalPlayer->WeaponManagerComponent && 
                g_LocalPlayer->WeaponManagerComponent->CurrentWeaponReplicated) {
                
                int g_WeaponID = g_LocalPlayer->WeaponManagerComponent->CurrentWeaponReplicated->GetWeaponID();
                
                // --- Skin Database IDs ---

                if (g_WeaponID == 101004) { // M416
                    FatalDamageParameter->CauserWeaponAvatarID = 1101004236; // Glacier
                }
                else if (g_WeaponID == 101001) { // AKM
                    FatalDamageParameter->CauserWeaponAvatarID = 1101001265; 
                }
                else if (g_WeaponID == 102002) { // UMP45
                    FatalDamageParameter->CauserWeaponAvatarID = 1102002424; 
                }
                else if (g_WeaponID == 102004) { // tommy
                    FatalDamageParameter->CauserWeaponAvatarID = 1102004018; 
                }
                else if (g_WeaponID == 110500) { // m248 ? (Check ID)
                    FatalDamageParameter->CauserWeaponAvatarID = 1105001025; 
                }
                else if (g_WeaponID == 101008) { // M762
                    FatalDamageParameter->CauserWeaponAvatarID = 1101008154; 
                }
                else if (g_WeaponID == 101003) { // SCAR-L
                    FatalDamageParameter->CauserWeaponAvatarID = 1101003219; 
                }
                else if (g_WeaponID == 103003) { // AWM
                    FatalDamageParameter->CauserWeaponAvatarID = 1103003079; 
                }
                else if (g_WeaponID == 103001) { // KAR98K
                    FatalDamageParameter->CauserWeaponAvatarID = 1103001202; 
                }
                else if (g_WeaponID == 103002) { // M24
                    FatalDamageParameter->CauserWeaponAvatarID = 1103002030; 
                }
                else if (g_WeaponID == 102001) { // UZI
                    FatalDamageParameter->CauserWeaponAvatarID = 1102001120; 
                }
                else if (g_WeaponID == 105002) { // DP-28
                    FatalDamageParameter->CauserWeaponAvatarID = 1105002091; 
                }
                else if (g_WeaponID == 101005) { // GROZA
                    FatalDamageParameter->CauserWeaponAvatarID = 1101005098; 
                }
                else if (g_WeaponID == 104004) { // DBS
                    FatalDamageParameter->CauserWeaponAvatarID = 1104004035; 
                }
                else if (g_WeaponID == 101102) { // ACE32
                    FatalDamageParameter->CauserWeaponAvatarID = 1101102025; 
                }
                else if (g_WeaponID == 101006) { // AUG
                    FatalDamageParameter->CauserWeaponAvatarID = 1101006075; 
                }
                else if (g_WeaponID == 105010) { // MG3
                    FatalDamageParameter->CauserWeaponAvatarID = 1105010019; 
                }
                else if (g_WeaponID == 105001) { // m249
                    FatalDamageParameter->CauserWeaponAvatarID = 1105001069; 
                }
                else if (g_WeaponID == 102003) { // VECTOR
                    FatalDamageParameter->CauserWeaponAvatarID = 1102003100; 
                }
                else if (g_WeaponID == 102005) { // BIZON
                    FatalDamageParameter->CauserWeaponAvatarID = 1102005064; 
                }
                else if (g_WeaponID == 101012) { // Honey Badger
                    FatalDamageParameter->CauserWeaponAvatarID = 1101012033; 
                }
                else if (g_WeaponID == 101009) { // FAMAS
                    FatalDamageParameter->CauserWeaponAvatarID = 1101009012; 
                }
                else if (g_WeaponID == 103012) { // AMR
                    FatalDamageParameter->CauserWeaponAvatarID = 1103012031; 
                }
                else if (g_WeaponID == 103005) { // Mini14
                    FatalDamageParameter->CauserWeaponAvatarID = 1103005037; 
                }
                else if (g_WeaponID == 103004) { // SKS
                    FatalDamageParameter->CauserWeaponAvatarID = 1103004058; 
                }
                else if (g_WeaponID == 103009) { // SLR
                    FatalDamageParameter->CauserWeaponAvatarID = 1103009028; 
                }
                else if (g_WeaponID == 104003) { // S12K
                    FatalDamageParameter->CauserWeaponAvatarID = 1104003037; 
                }
                else if (g_WeaponID == 104101) { // M1014
                    FatalDamageParameter->CauserWeaponAvatarID = 1104101001; 
                }
                else if (g_WeaponID == 102105) { // P90
                    FatalDamageParameter->CauserWeaponAvatarID = 1102105012; 
                }
                else if (g_WeaponID == 101002) { // M16A4
                    FatalDamageParameter->CauserWeaponAvatarID = 1101002056; 
                }
            }
        }
    }
 
    return orig_kill_message_event(thiz, FatalDamageParameter);
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
                           float dist = v2Middle.Distance(v2Loc);
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



void RenderESP(UCanvas* Canvas, int ScreenWidth, int ScreenHeight)
{

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
//=================================

if (Config.Ipad) {
      localPlayer->ThirdPersonCameraComponent->SetFieldOfView(110.0f);
     
                     }
                     
                     
                     
                     
                     
                     static std::chrono::steady_clock::time_point lastDeadboxCheck = std::chrono::steady_clock::now();
auto currentDeadboxCheck = std::chrono::steady_clock::now();

// 1 second delay check
if (deadboxskin && localPlayer->WeaponManagerComponent && 
    std::chrono::duration_cast<std::chrono::seconds>(currentDeadboxCheck - lastDeadboxCheck).count() >= 1) 
{
    lastDeadboxCheck = currentDeadboxCheck; 

    // Static variables
    static std::unordered_set<uintptr_t> processedBoxes; // Vector ki jagah Set (Faster)
    static int lastMatchKey = 0;

    // Match change detection
    if (localPlayer->PlayerKey != lastMatchKey) {
        processedBoxes.clear();
        lastMatchKey = localPlayer->PlayerKey;
    }

    auto CurrentWeapon = localPlayer->WeaponManagerComponent->CurrentWeaponReplicated;
    if (CurrentWeapon) {
        int WeaponID = CurrentWeapon->GetWeaponID();
        if (WeaponID != 0) {
            
            // Skin Mapping (Optimized lookup map)
            static const std::unordered_map<int, int> weaponSkinMap = {
           {101004, 1101004236}, // M416
            {101001, 1101001265}, // AKM
            {101003, 1101003219}, // SCAR-L
            {101008, 1101008154}, // M762
            {102002, 1102002424}, // UMP45
            {103003, 1103003079}, // AWM
            {103001, 1103001202}, // Kar98
            {105001, 1105001069}, // M249
            {103002, 1103002030}, // M24
            {102001, 1102001120}, // UZI
            {105002, 1105002091}, // DP-28
            {101102, 1101102025}, // ACE32
            {101005, 1101005098}, // Groza
            {104004, 1104004035}, // DBS
            {104003, 1104003037}, // S12K
            {105010, 1105010019}, // MG3
            {101006, 1101006075}, // AUG
            {102005, 1102005064}, // Bizon
            {102105, 1102105012}, // P90
            {102003, 1102003100}, // Vector
            {101012, 1101012033}, // Honey Badger
            {101009, 1101009012}, // FAMAS
            {103012, 1103012031}, // AMR
            {103005, 1103005037}, // Mini14
            {103004, 1103004058}, // SKS
            {103009, 1103009028}, // SLR
            {102004, 1102004018}, // Tommy Gun
            {104101, 1104101001}  // M1014
            };

            // Check if current weapon has a skin mapped
            auto skinIt = weaponSkinMap.find(WeaponID);
            if (skinIt != weaponSkinMap.end()) {
                int BoxSkinID = skinIt->second;

                auto Actors = GetActors(); // GetActors heavy ho sakta hai, isliye andar laye
                if (!Actors.empty()) {
                    for (auto Actor : Actors) {
                        if (!Actor || isObjectInvalid(Actor)) continue;

                        // Fast check: Agar pehle se processed hai, toh string check mat karo
                        if (processedBoxes.count((uintptr_t)Actor)) continue;

                        std::string name = Actor->GetName();
                        // String check
                        if (name.find("DeadInventoryBox") != std::string::npos || name.find("DeadBox") != std::string::npos) {
                            
                            auto DeadBoxComp = (UDeadBoxAvatarComponent*)Actor->GetComponentByClass(UDeadBoxAvatarComponent::StaticClass());
                            if (DeadBoxComp) {
                                DeadBoxComp->ChangeItemAvatar(BoxSkinID, true);
                                processedBoxes.insert((uintptr_t)Actor); // Mark as done
                            }
                        }
                    }
                }
            }
        }
    }
}
//=========================
if (NARTUO)
                                {
                        auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
                        if (WeaponManagerComponent) {
                            auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
                            if ((int) propSlot.GetValue() >= 1 && (int) propSlot.GetValue() <= 3) {
                                auto CurrentWeaponReplicated = (ASTExtraShootWeapon *) WeaponManagerComponent->CurrentWeaponReplicated;
                                if (CurrentWeaponReplicated) {
                                    auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
                                    if (ShootWeaponComponent) {
                                        UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
                                        if (ShootWeaponEntityComponent) {

                                            if (NARTUO) {
                                                ShootWeaponEntityComponent->GameDeviationFactor = 0.0f;                
            }
            }
            }
            }
            }
            }
            }

static float offPitch = 0;
static float offYaw = 0;
bool isADS = localPlayer->bIsGunADS;
        
if(!isADS){ 
offPitch = 0;
offYaw = 0;
}


if (Config.AimBot.Enable) {
ASTExtraPlayerCharacter* Target = GetTargetForAim();
if (Target && (localPlayer->bIsWeaponFiring)) {
FVector targetAimPos = Target->GetBonePos("Head", {});
auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
if (WeaponManagerComponent) {
auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
if ((int)propSlot.GetValue() >= 1 && (int)propSlot.GetValue() <= 3) {
auto CurrentWeaponReplicated = (ASTExtraShootWeapon *) WeaponManagerComponent->CurrentWeaponReplicated;
if (CurrentWeaponReplicated) {
auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
if (ShootWeaponComponent) {
auto ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
if (ShootWeaponEntityComponent) {
auto CurrentVehicle = Target->CurrentVehicle;
if (CurrentVehicle) {
FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;
float dist = localPlayer->GetDistanceTo(Target);
auto timeToTravel = dist / ShootWeaponEntityComponent->BulletFireSpeed;
targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos,UKismetMathLibrary::Multiply_VectorFloat(LinearVelocity, timeToTravel));
} else {
FVector Velocity = Target->GetVelocity();
float dist = localPlayer->GetDistanceTo(Target);
auto timeToTravel = dist / ShootWeaponEntityComponent->BulletFireSpeed;
targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(Velocity, timeToTravel));
}
if (Config.AimBot.Enable) {
float dist = localPlayer->GetDistanceTo(Target) / 100.f;
targetAimPos.Z -= dist * 1.0f;
}
FVector gunLocation = g_PlayerController->PlayerCameraManager->CameraCache.POV.Location;
FRotator desired = ToRotator(gunLocation, targetAimPos);
FRotator current = g_PlayerController->PlayerCameraManager->CameraCache.POV.Rotation;

if(isADS){
float dPitch = desired.Pitch - current.Pitch;
float dYaw = desired.Yaw - current.Yaw;
while (dPitch > 180) dPitch -= 360;
while (dPitch < -180) dPitch += 360;
while (dYaw > 180) dYaw -= 360;
while (dYaw < -180) dYaw += 360;

offPitch += dPitch * 0.12f;
offYaw += dYaw * 0.12f;

if (offPitch > 30) offPitch = 30;
if (offPitch < -30) offPitch = -30;
if (offYaw > 30) offYaw = 30;
if (offYaw < -30) offYaw = -30;
}

FRotator apply;
apply.Pitch = desired.Pitch + offPitch;
apply.Yaw = desired.Yaw + offYaw;
apply.Roll = 0;

float s = 1.0f;
if (s < 1) s = 1;

FRotator out;
out.Pitch = current.Pitch + (apply.Pitch - current.Pitch) / s;
out.Yaw = current.Yaw + (apply.Yaw - current.Yaw) / s;
out.Roll = 0;

clampAngles(out);

g_PlayerController->ControlRotation = out;
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

FLinearColor lineColor = IsVisible ? 
(Player->bEnsure ? White : Green) : 
(Player->bEnsure ? Black : Red);
for (auto& boneStructure : skeleton) {
std::string lastBone;
for (const std::string& currentBone : boneStructure) {
if (!lastBone.empty()) {
int lastBoneIndex = boneToIndex[lastBone];
int currentBoneIndex = boneToIndex[currentBone];
if (screenPositions[lastBoneIndex].second && screenPositions[currentBoneIndex].second) {
DrawLine(Canvas, screenPositions[lastBoneIndex].first, screenPositions[currentBoneIndex].first, 1.0f, lineColor);
}
}
lastBone = currentBone;
}
}
                }            

if (Config.PlayerESP.Skeleton) {
    constexpr float HeadOffsetY = 10.0f;
    constexpr float StartLineY = 80.0f;

    FVector2D targetPos = { HeadPosSC.X, HeadPosSC.Y - HeadOffsetY };  // Just above head
    FVector2D screenCenter = { static_cast<float>(screenWidth) / 2.0f, StartLineY };

    const bool bIsBot = Player->bEnsure;
    const FLinearColor lineColor = IsVisible
        ? (bIsBot ? COLOR_WHITE : COLOR_GREEN)
        : (bIsBot ? COLOR_BLACK : COLOR_RED);

    DrawLine(Canvas, screenCenter, targetPos, 1.5f, lineColor);
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
            wsName = L"Bot";
            wsDist = std::to_wstring((int)Distance) + L"M";   // ✅ bot ke liye bhi distance set
        } else {
            // Player
            std::wstring wsTeam = L"(" + std::to_wstring(Player->TeamID) + L")";
            wsName = Player->PlayerName.ToWString();
            wsFullInfo = wsTeam + L" - " + wsName;
            wsDist = std::to_wstring((int)Distance) + L"M";
        }

        // Adjust font size by distance
        tslFontUI->LegacyFontSize = std::max(5, 12 - (int)(Distance / 80));

        if (Player->bEnsure) {
            // Draw Bot name
            DrawOutlinedTextFPS(Canvas,
                FString(wsName),   // "Bot"
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_WHITE, COLOR_IN, true);

            // Bot distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    COLOR_WHITE, COLOR_IN, true);
            }

        } else {
            // Player Name + Team
            DrawOutlinedTextFPS(Canvas,
                FString(wsFullInfo),
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_WHITE, COLOR_IN, true);

            // Player distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    COLOR_WHITE, COLOR_IN, true);
            }

        }
    }
}

////==========================================================================================================//
    if (Config.PlayerESP.Health) {
float CurHP = Player->Health;

if (CurHP < 0.0f)  
    CurHP = 0.0f;  
else if (CurHP > Player->HealthMax)  
    CurHP = Player->HealthMax;  

float MaxHP = Player->HealthMax > 0 ? Player->HealthMax : 1.0f;  
float HealthPercentage = CurHP / MaxHP;  

FLinearColor ColorHP;  
if (Player->bEnsure) {  
    ColorHP = FLinearColor(0.0f, 1.0f, .0f, 1.0f);  
} else {  
    if (HealthPercentage > 0.5f) {  
        ColorHP = FLinearColor(0.0f, 1.0f, .0f, 1.0f);  
    } else if (HealthPercentage > 0.2f) {  
        ColorHP = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);  
    } else {  
        ColorHP = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);  
    }  
}  

FVector AboveHead = Player->GetHeadLocation(true);  
AboveHead.Z += 25.0f;  

FVector2D AboveHeadSc;  
if (gGameplayStatics->ProjectWorldToScreen(localPlayerController, AboveHead, false, &AboveHeadSc)) {  
    float BarWidth = 50.0f;  
    float BarHeight = 3.0f;  

     FVector2D BarPos;  
    BarPos.X = AboveHeadSc.X - BarWidth / 2.0f;  
    BarPos.Y = AboveHeadSc.Y - BarHeight * 1.5f;  

    FLinearColor BorderColor = Player->bEnsure ? COLOR_IN : COLOR_IN;  
    DrawFilledRect(Canvas, {BarPos.X - 1.0f, BarPos.Y - 1.0f}, BarWidth + 2.0f, BarHeight + 2.0f, BorderColor); // bg  
    DrawFilledRect(Canvas, {BarPos.X, BarPos.Y}, HealthPercentage * BarWidth, BarHeight, ColorHP); // fill  
}
}
}
} 

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
            tslFontUI->LegacyFontSize = max(6, 12 - (int)(Distance / 80));
            
            // Draw vehicle distance (on top)
            DrawOutlinedTextFPS(Canvas, FString(distanceText), 
                FVector2D(VehiclePos.X, VehiclePos.Y +20), 
                COLOR_WHITE, COLOR_IN, true);

            // Draw vehicle name (below distance)
            DrawOutlinedTextFPS(Canvas, FString(vehicleText), 
                FVector2D(VehiclePos.X, VehiclePos.Y), // Move name down by 15 pixels
                COLOR_WHITE, COLOR_IN, true);
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

            tslFontUI->LegacyFontSize = max(7, 12 - (int)(Distance / 80));

            // LootBox naam draw karo
            DrawOutlinedTextFPS(Canvas, FString(distanceText), {PickUpListsPos.X, PickUpListsPos.Y +20}, COLOR_WHITE, COLOR_IN, true);
            
            // Distance naam ke niche draw karo
            DrawOutlinedTextFPS(Canvas, FString(lootBoxText), {PickUpListsPos.X, PickUpListsPos.Y }, COLOR_WHITE, COLOR_IN, true);
        }
    }
}
                if (Config.PlayerESP.Grenade) {
    std::vector<ASTExtraGrenadeBase*> allGrenades;
    GetAllActors(allGrenades);

FVector myPos;
if (localPlayer->RootComponent)
    myPos = localPlayer->RootComponent->RelativeLocation;
else {
    myPos.X = 0.0f;
    myPos.Y = 0.0f;
    myPos.Z = 0.0f;
}

    for (auto* grenade : allGrenades) {
        if (!grenade || !grenade->RootComponent || grenade->bHidden || grenade->IsActorBeingDestroyed())
            continue;

        FVector grenadePos = grenade->RootComponent->RelativeLocation;
        FVector delta = grenadePos - myPos;
        float distance = sqrtf(delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z) / 100.0f;

        if (distance > 100.f)
            continue;

        FVector2D screenPos;
        if (!W2S(grenadePos, &screenPos))
            continue;

        std::string name = grenade->GetName();
        std::string label;
        FLinearColor color = COLOR_YELLOW;
        bool drawLine = false;

        // 🎯 Frag Grenade
        if (name.find("Frag") != std::string::npos) {
            label = "Frag Grenade - [" + std::to_string((int)distance) + "M]";
            color = COLOR_RED;
        }

        // 🎯 Smoke Grenade
        else if (name.find("Smoke") != std::string::npos) {
            label = "Smoke Grenade - [" + std::to_string((int)distance) + "M]";
            color = COLOR_RED;
            drawLine = false;  // ✅ No line for smoke
        }

        // 🎯 Molotov / Burn
        else if (name.find("Burn") != std::string::npos || name.find("Molotov") != std::string::npos) {
            label = "Molotov - [" + std::to_string((int)distance) + "M]";
            color = COLOR_RED;
        }

        // 🔕 Skip unknown types
        else {
            continue;
        }

        tslFontUI->LegacyFontSize = 10;

        // ✅ Draw line if allowed
        if (drawLine) {
            FVector2D playerScreen;
            if (W2S(myPos, &playerScreen)) {
            Canvas->K2_DrawLine(playerScreen, screenPos, 1.5f, COLOR_RED);  // Unreal's native
            }
        }

        // ✅ Draw text
        DrawOutlinedTextFPS(Canvas, FString(label), screenPos, color, COLOR_OUTLINE, true);
    }
                }                 

if (KillMessage){
            if (localPlayer && localPlayer->WeaponManagerComponent && localPlayer->WeaponManagerComponent->CurrentWeaponReplicated) {
        
        int weapowep = localPlayer->WeaponManagerComponent->CurrentWeaponReplicated->GetWeaponID();
        
        // 2. CRASH FIX: Use Reference (&) and Check Array Size limits
        // (Agar memory corrupt ho to loop 100 se zyada nahi chalega -> No Freeze)
        auto& landchud = localPlayer->WeaponManagerComponent->CurrentWeaponReplicated->synData;
        
        if (landchud.Num() > 0 && landchud.Num() < 100) {

            auto currentTime = std::chrono::steady_clock::now();
            auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastWeaponChangeTime).count();

            // 1 Second Delay Logic (To prevent lag)
            if (timeDiff > 1000) {
                
                bool bHasChanged = false; // Flag to check if update is needed

                for (int j = 0; j < landchud.Num(); j++) {
                    auto& weaponInfo = landchud[j]; // Access original memory
                    auto weaponid = weaponInfo.DefineID.TypeSpecificID;

                    if (weaponid != 0) {

                        // --- AR GUNS ---

                        // M416 (Glacier)
                        if (weapowep == 101004) {
                            for (int i = 0; i < sizeof(m4v) / sizeof(m4v[0]); i++) {
                                if (weaponid == m4v[i] || weaponid == 101004) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101004236;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // AKM (Glacier)
                        else if (weapowep == 101001) {
                            for (int i = 0; i < sizeof(akmv) / sizeof(akmv[0]); i++) {
                                if (weaponid == akmv[i] || weaponid == 101001) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101001265;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // SCAR-L
                        else if (weapowep == 101003) {
                            for (int i = 0; i < sizeof(scar) / sizeof(scar[0]); i++) {
                                if (weaponid == scar[i] || weaponid == 101003) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101003219;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // GROZA
                        else if (weapowep == 101005) {
                            for (int i = 0; i < sizeof(groza) / sizeof(groza[0]); i++) {
                                if (weaponid == groza[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101005098;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // AUG
                        else if (weapowep == 101006) {
                            for (int i = 0; i < sizeof(aug) / sizeof(aug[0]); i++) {
                                if (weaponid == aug[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101006075;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // M16A4
                        else if (weapowep == 101002) {
                            for (int i = 0; i < sizeof(m16) / sizeof(m16[0]); i++) {
                                if (weaponid == m16[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101002056;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // M762
                        else if (weapowep == 101008) {
                            for (int i = 0; i < sizeof(m7) / sizeof(m7[0]); i++) {
                                if (weaponid == m7[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101008154;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // ACE32
                        else if (weapowep == 101102) {
                            for (int i = 0; i < sizeof(ace32) / sizeof(ace32[0]); i++) {
                                if (weaponid == ace32[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101102025;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // HONEY BADGER
                        else if (weapowep == 101012) {
                            for (int i = 0; i < sizeof(honey) / sizeof(honey[0]); i++) {
                                if (weaponid == honey[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101012033;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // FAMAS
                        else if (weapowep == 101009) {
                            for (int i = 0; i < sizeof(famas) / sizeof(famas[0]); i++) {
                                if (weaponid == famas[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1101009012;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // --- SMG GUNS ---

                        // UMP45
                        else if (weapowep == 102002) {
                            for (int i = 0; i < sizeof(ump) / sizeof(ump[0]); i++) {
                                if (weaponid == ump[i] || weaponid == 102002) {
                                    weaponInfo.DefineID.TypeSpecificID = 1102002424;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // UZI
                        else if (weapowep == 102001) {
                            for (int i = 0; i < sizeof(uzi) / sizeof(uzi[0]); i++) {
                                if (weaponid == uzi[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1102001120;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // VECTOR
                        else if (weapowep == 102003) {
                            for (int i = 0; i < sizeof(vectorr) / sizeof(vectorr[0]); i++) {
                                if (weaponid == vectorr[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1102003100;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // BIZON
                        else if (weapowep == 102005) {
                            for (int i = 0; i < sizeof(bizon) / sizeof(bizon[0]); i++) {
                                if (weaponid == bizon[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1102005064;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // TOMMY
                        else if (weapowep == 102004) {
                            for (int i = 0; i < sizeof(tommy) / sizeof(tommy[0]); i++) {
                                if (weaponid == tommy[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1102004018;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // P90
                        else if (weapowep == 102105) {
                            for (int i = 0; i < sizeof(P90) / sizeof(P90[0]); i++) {
                                if (weaponid == P90[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1102105012;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // --- SNIPER / DMR ---

                        // AWM
                        else if (weapowep == 103003) {
                            for (int i = 0; i < sizeof(awm) / sizeof(awm[0]); i++) {
                                if (weaponid == awm[i] || weaponid == 103003) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103003079;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // Kar98K
                        else if (weapowep == 103001) {
                            for (int i = 0; i < sizeof(kar) / sizeof(kar[0]); i++) {
                                if (weaponid == kar[i] || weaponid == 103001) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103001202;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 8;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // M24
                        else if (weapowep == 103002) {
                            for (int i = 0; i < sizeof(m24) / sizeof(m24[0]); i++) {
                                if (weaponid == m24[i] || weaponid == 103002) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103002030;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // MK14
                        else if (weapowep == 103007) {
                            for (int i = 0; i < sizeof(mk14) / sizeof(mk14[0]); i++) {
                                if (weaponid == mk14[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103007020;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // AMR
                        else if (weapowep == 103012) {
                            for (int i = 0; i < sizeof(amr) / sizeof(amr[0]); i++) {
                                if (weaponid == amr[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103012031;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // MINI14
                        else if (weapowep == 103005) {
                            for (int i = 0; i < sizeof(mini) / sizeof(mini[0]); i++) {
                                if (weaponid == mini[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103005037;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // SKS
                        else if (weapowep == 103004) {
                            for (int i = 0; i < sizeof(sks) / sizeof(sks[0]); i++) {
                                if (weaponid == sks[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103004058;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // SLR
                        else if (weapowep == 103009) {
                            for (int i = 0; i < sizeof(slr) / sizeof(slr[0]); i++) {
                                if (weaponid == slr[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1103009028;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // --- LMG / SHOTGUNS ---

                        // DP-28
                        else if (weapowep == 105002) {
                            for (int i = 0; i < sizeof(dp) / sizeof(dp[0]); i++) {
                                if (weaponid == dp[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1105002091;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // M249
                        else if (weapowep == 105001) {
                            for (int i = 0; i < sizeof(m249) / sizeof(m249[0]); i++) {
                                if (weaponid == m249[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1105001069;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // MG3
                        else if (weapowep == 105010) {
                            for (int i = 0; i < sizeof(MG3) / sizeof(MG3[0]); i++) {
                                if (weaponid == MG3[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1105010019;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // DBS
                        else if (weapowep == 104004) {
                            for (int i = 0; i < sizeof(DBS) / sizeof(DBS[0]); i++) {
                                if (weaponid == DBS[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1104004035;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // S12K
                        else if (weapowep == 104003) {
                            for (int i = 0; i < sizeof(S12K) / sizeof(S12K[0]); i++) {
                                if (weaponid == S12K[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1104003037;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // M1014
                        else if (weapowep == 104101) {
                            for (int i = 0; i < sizeof(m1014) / sizeof(m1014[0]); i++) {
                                if (weaponid == m1014[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1104101001;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // --- MISC ---

                        // PAN
                        else if (weapowep == 108004) {
                            for (int i = 0; i < sizeof(pan) / sizeof(pan[0]); i++) {
                                if (weaponid == pan[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1108004356;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // X-SUIT
                        else if (weapowep == 403003) {
                            for (int i = 0; i < sizeof(SuitX) / sizeof(SuitX[0]); i++) {
                                if (weaponid == SuitX[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1406872;
                                    *(int*)((uintptr_t)&weaponInfo + 12) = 7;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // BAG
                        else if (weapowep == 501006 || weapowep == 501001) {
                            for (int i = 0; i < sizeof(Bag) / sizeof(Bag[0]); i++) {
                                if (weaponid == Bag[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1501003550;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }

                        // HELMET
                        else if (weapowep == 502001 || weapowep == 502002 || weapowep == 502003) {
                            for (int i = 0; i < sizeof(Helmet) / sizeof(Helmet[0]); i++) {
                                if (weaponid == Helmet[i]) {
                                    weaponInfo.DefineID.TypeSpecificID = 1502003261;
                                    bHasChanged = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                
                // 3. FPS & LAG FIX: Only call update function if data actually changed
                if (bHasChanged) {
                    localPlayer->WeaponManagerComponent->CurrentWeaponReplicated->DelayHandleAvatarMeshChanged();
                    lastWeaponChangeTime = currentTime;
                }
            }
        }
    }
}																												
            
            
            
            /*======================*/
            
             
    /*======================================m416end=======================*/
				/*
							                    if (Magic){
                        uintptr_t Mesh = *(uintptr_t *)((uintptr_t)localPlayer + 0x4B8);
                        if (Mesh != 0){
                            uintptr_t Skeletal = *(uintptr_t *)(Mesh + 0x8B8);
                            if (Skeletal != 0){
                                uintptr_t Asset = *(uintptr_t *)(Skeletal + 0x138);
                                if (Asset != 0){
                                    uintptr_t ArraySkeletal = *(uintptr_t *)(Asset + 0x38);
                                    if (ArraySkeletal != 0){
                                        uintptr_t SkeletalBodySetup = *(uintptr_t *)(ArraySkeletal + sizeof(uintptr_t) * 14);
                                        if (SkeletalBodySetup != 0){
                                            uintptr_t BoxElems = *(uintptr_t *)(SkeletalBodySetup + 0x38);
                                            if (BoxElems != 0){
                                                *(float *)(BoxElems + 0x88) = 120.0f;
                                                *(float *)(BoxElems + 0x8c) = 208.0f;
                                                *(float *)(BoxElems + 0x90) = 408.0f;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
												
																*/
																				
																												
bool Magic = false;

if (Magic){
uintptr_t Mesh = *(uintptr_t *)((uintptr_t)localPlayer + 0x4a8);
if (Mesh != 0){
uintptr_t Skeletal = *(uintptr_t *)(Mesh + 0x898);
if (Skeletal != 0){
uintptr_t Asset = *(uintptr_t *)(Skeletal + 0x138);
if (Asset != 0){
uintptr_t ArraySkeletal = *(uintptr_t *)(Asset + 0x38);
if (ArraySkeletal != 0){
uintptr_t SkeletalBodySetup = *(uintptr_t *)(ArraySkeletal + sizeof(uintptr_t) * 14);
if (SkeletalBodySetup != 0){
uintptr_t BoxElems = *(uintptr_t *)(SkeletalBodySetup + 0x38);
if (BoxElems != 0){
                        *(float *)(BoxElems + 0x88) = 120.0f;
                                                *(float *)(BoxElems + 0x8c) = 208.0f;
                                                *(float *)(BoxElems + 0x90) = 408.0f;
}}}}}}}

                           
                g_LocalPlayer = localPlayer;
g_PlayerController = localPlayerController;

int kill_message_event_idx = 999;
auto VTable = (void **) localPlayerController->VTable;
auto f_mprotect = [](uintptr_t addr, size_t len, int32_t prot) -> int32_t {
 static_assert(PAGE_SIZE == 4096);
 constexpr size_t page_size = static_cast<size_t>(PAGE_SIZE);
 void* start = reinterpret_cast<void*>(addr & -page_size);
 uintptr_t end = (addr + len + page_size - 1) & -page_size;
 return mprotect(start, end - reinterpret_cast<uintptr_t>(start), prot);
 };
 
 if (VTable && (VTable[kill_message_event_idx] != kill_message_event)) {
orig_kill_message_event = decltype(orig_kill_message_event)(VTable[kill_message_event_idx]);
f_mprotect((uintptr_t)(&VTable[kill_message_event_idx]), sizeof(uintptr_t), PROT_READ | PROT_WRITE);
VTable[kill_message_event_idx] = (void *) kill_message_event;
}


std::string playerInfo;
    if (totalEnemies > 0 || totalBots > 0) {
    playerInfo += "E";
    playerInfo += std::to_string(totalEnemies);
    playerInfo += "丨";
    playerInfo += "B";
    playerInfo += std::to_string(totalBots);

    tslFontUI->LegacyFontSize = 18;
     DrawOutlinedTextFPS(Canvas, FString(playerInfo), {(float)screenWidth / 2, 100}, COLOR_WHITE, COLOR_IN, true);

    }
}
}



/*
g_LocalPlayer = localPlayer;
g_PlayerController = localPlayerController;

std::string playerInfo;

if (totalEnemies != 0 || totalBots != 0) {
    float boxWidth  = 50.0f;  // 🔽 Chota width
    float boxHeight = 35.0f;  // 🔽 Chota height
    float centerX   = screenWidth / 2.0f;
    float topY      = 20.0f;  // Position upar hi rahegi

    tslFontUI->LegacyFontSize = 20; // Slightly smaller font to fit

    // Perfect center Y for text
 //   float textCenterY = topY + (boxHeight / 2.0f) - (tslFont->LegacyFontSize / 2.0f);
float textCenterY = topY + (boxHeight / 2.0f) - (tslFontUI->LegacyFontSize / 2.0f) + 7.0f; // niche 2px
 
 
 
    // Colors
    FLinearColor enemyColor = FLinearColor(0.4f, 0.6f, 1.0f, 0.5f); // Sky blue
    FLinearColor botColor   = FLinearColor(0.0f, 0.0f, 0.0f, 0.35f); // Transparent black

    // Dark shades for outline
    FLinearColor enemyBorder = FLinearColor(enemyColor.R * 0.6f, enemyColor.G * 0.6f, enemyColor.B * 0.6f, 1.0f);
    FLinearColor botBorder   = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f); // dark grey

    // Enemy Box (left)
    float enemyX = centerX - boxWidth;
    DrawFilledRect(Canvas, FVector2D(enemyX - 1.0f, topY - 1.0f), boxWidth + 1.0f, boxHeight + 2.0f, enemyBorder);
    DrawFilledRect(Canvas, FVector2D(enemyX, topY), boxWidth, boxHeight, enemyColor);
    DrawOutlinedTextFPS(Canvas,
        FString(std::to_string((int)totalEnemies).c_str()),
        FVector2D(enemyX + boxWidth / 2.0f, textCenterY),
        COLOR_BLACK, FLinearColor(enemyColor.R, enemyColor.G, enemyColor.B, 1.0f), true);

    // Bot Box (right)
    float botX = centerX;
    DrawFilledRect(Canvas, FVector2D(botX, topY - 1.0f), boxWidth + 1.0f, boxHeight + 2.0f, botBorder);
    DrawFilledRect(Canvas, FVector2D(botX, topY), boxWidth, boxHeight, botColor);
    DrawOutlinedTextFPS(Canvas,
        FString(std::to_string((int)totalBots).c_str()),
        FVector2D(botX + boxWidth / 2.0f, textCenterY),
        FLinearColor(1.f, 1.f, 1.f, 1.f), COLOR_BLACK, true);

    // Middle separator line
    DrawFilledRect(Canvas, FVector2D(centerX - 0.5f, topY - 1.0f), 1.0f, boxHeight + 2.0f, FLinearColor(0.f, 0.f, 0.f, 1.f));

    tslFontUI->LegacyFontSize = TSL_FONT_DEFAULT_SIZE;
}}}





*/

/*
if (totalEnemies > 0 || totalBots > 0) {
    // When enemies or bots exist
    playerInfo = std::to_string(totalEnemies) + " | " + std::to_string(totalBots);

    tslFontUI->LegacyFontSize = 18;

    float posX = (float)screenWidth / 2;
    float posY = 70.0f;

    float padding = 20.0f;
    float bgWidth = (playerInfo.length() * 12.0f) + (padding * 2);
    float bgHeight = 30.0f;

    // 🔴 Draw Red Background
    DrawFilledRect(Canvas,
                   FVector2D(posX - bgWidth / 2, posY - bgHeight / 2),
                   bgWidth, bgHeight,
                   FLinearColor(0.4f, 0.6f, 1.0f, 0.5f));

    // White text
    float textOffsetX = (playerInfo.length() * 1.0f);
    DrawOutlinedTextFPS(Canvas, FString(playerInfo),
                        FVector2D(posX - textOffsetX, posY),
                        FLinearColor(1.f, 1.f, 1.f, 1.f),
                        FLinearColor(0.f, 0.f, 0.f, 0.f),
                        true);
} else {
    // ✅ No enemies or bots
    playerInfo = "CLEAR";

    tslFontUI->LegacyFontSize = 14;

    float posX = (float)screenWidth / 2;
    float posY = 70.0f;

    float padding = 20.0f;
    float bgWidth = (playerInfo.length() * 12.0f) + (padding * 2);
    float bgHeight = 30.0f;

    // 🟡 Draw Yellow Background
    DrawFilledRect(Canvas,
                   FVector2D(posX - bgWidth / 2, posY - bgHeight / 2),
                   bgWidth, bgHeight,
                   FLinearColor(0.4f, 0.6f, 1.0f, 0.5f));

    // Black text
    float textOffsetX = (playerInfo.length() * 1.0f);
    DrawOutlinedTextFPS(Canvas, FString(playerInfo),
                        FVector2D(posX - textOffsetX, posY),
                        FLinearColor(0.f, 0.f, 0.f, 1.f),
                        FLinearColor(0.f, 0.f, 0.f, 0.f),
                        true);
    
     }
}
}
*/
    if(Canvas) {
    char Menunamee[256];
    sprintf(Menunamee, OBFUSCATE("BATTLE ROYALE MODZ"));

    // Dynamically adjust font size based on screen width
    int screenWidth = Canvas->SizeX;
    int screenHeight = Canvas->SizeY;
    
    // Set font size proportionally (e.g., 4% of screen width)
    tslFontUI->LegacyFontSize = screenWidth * 0.012f; // Adjust multiplier as needed

    // Calculate text width dynamically
    int textWidthMenunamee = strlen(Menunamee) * tslFontUI->LegacyFontSize * 0.1f; 

    // Center text dynamically
    int centerX = screenWidth / 2;
    int menunameePositionX = centerX - (textWidthMenunamee / 2);
    
    // ✅ Thoda upar shift (pehle 0.16 tha → ab 0.12)
    int menunameePositionY = screenHeight * 0.10f; 

    // Draw text with outline
    DrawOutlinedTextFPS(Canvas, Menunamee, {menunameePositionX, menunameePositionY}, COLOR_RED, COLOR_IN, true);

    }
    
    
    if (DX4)
{
    // Check local player to avoid lobby crash
    if (g_LocalPlayer) 
    {
        auto AllActors = GetActors(); 
        for (auto Actor : AllActors)
        {
            if (isObjectInvalid(Actor)) continue;

            if (Actor->IsA(ASTExtraGameStateBase::StaticClass()))
            {
                auto InGame = (ASTExtraGameStateBase *)Actor;
                
                // Fix: AliveTeamNum == 1 (Sirf tab dikhega jab aap jeet gaye ho)
                if ((int)InGame->AliveTeamNum == 1) 
                {
                    tslFontUI->LegacyFontSize = 35; 
                    float fontSize = 35.0f;
                    float fontRatio = 0.6f; // Font width estimation multiplier
                    float centerY = screenHeight / 3; 

                    // --- Line 1 ---
                    std::string text1 = "<< Winner Winner Chicken Dinner >>";
                    float width1 = text1.length() * fontSize * fontRatio;
                    DrawOutlinedTextFPS(Canvas, FString(text1.c_str()), 
                                        FVector2D((screenWidth / 2) - (width1 / 2), centerY), 
                                        COLOR_YELLOW, COLOR_BLACK, true);
                    
                    // --- Line 2 ---
                    std::string text2 = "<<    Restart Your Game    >>";
                    float width2 = text2.length() * fontSize * fontRatio;
                    DrawOutlinedTextFPS(Canvas, FString(text2.c_str()), 
                                        FVector2D((screenWidth / 2) - (width2 / 2), centerY + 50), 
                                        COLOR_RED, COLOR_BLACK, true);
                    
                    // --- Line 3 ---
                    std::string text3 = "<< Sed Feedback @JKVIPOWNER >>";
                    float width3 = text3.length() * fontSize * fontRatio;
                    DrawOutlinedTextFPS(Canvas, FString(text3.c_str()), 
                                        FVector2D((screenWidth / 2) - (width3 / 2), centerY + 100), 
                                        COLOR_GREEN, COLOR_BLACK, true);
                }
                break; 
            }
        }
    }
}


UpdateFPS();

    std::string baseStd = " FPS";

    char fpsBuffer[32];
   snprintf(fpsBuffer, sizeof(fpsBuffer), " ||  %.0f ", currentFPS);

    std::string combined = baseStd + fpsBuffer;

 //   Direct FString from runtime std::string
    FString displayText(combined.c_str());

    tslFontUI->LegacyFontSize = 25;

    float textWidth = 200.0f;
    float textHeight = 30.0f;
    float margin = 15.0f;

    float yPos = 685;
    float xPos = screenWidth - textWidth - margin;

DrawOutlinedText(Canvas, displayText, FVector2D(xPos, yPos), COLOR_YELLOW, COLOR_IN, true);

if (Canvas) {
char Menunameed[256];
sprintf(Menunameed, OBFUSCATE("{ BATTLE ROYALE MODZ }"));
int screenWidth = Canvas->SizeX;
int screenHeight = Canvas->SizeY;
tslFontUI->LegacyFontSize = screenWidth * 0.0010f; // Adjusted multiplier for smaller size
int textWidthMenunamee = strlen(Menunameed) * tslFontUI->LegacyFontSize * 0.1f; // Adjust multiplier for better fitting
int menunameePositionX = 200.0f; // 20 pixels from the left edge
int menunameePositionY = screenHeight - 80; // 30 pixels from the bottom (adjust as needed)
DrawOutlinedText(Canvas, Menunameed, {menunameePositionX, menunameePositionY}, COLOR_YELLOW, COLOR_BLACK, true);
}




   // tslFontUI->LegacyFontSize = tslFontUI_DEFAULT_SIZE;
	/*
if (Canvas) {
    char Menunamee[256];
    sprintf(Menunamee, OBFUSCATE("ADMIN SERVER"));

    // Dynamically adjust font size based on screen width
    int screenWidth = Canvas->SizeX;
    int screenHeight = Canvas->SizeY;
    
    // Set font size proportionally (e.g., 4% of screen width)
    tslFontUI->LegacyFontSize = screenWidth * 0.012f; // Adjust multiplier as needed

    // Calculate text width dynamically
    int textWidthMenunamee = strlen(Menunamee) * tslFontUI->LegacyFontSize * 0.1f; 

    // Center text dynamically
    int centerX = screenWidth / 2;
    int menunameePositionX = centerX - (textWidthMenunamee / 2);
    
    // ✅ Thoda upar shift (pehle 0.16 tha → ab 0.12)
    int menunameePositionY = screenHeight * 0.04f; 

    // Draw text with outline
    DrawOutlinedTextFPS(Canvas, Menunamee, {menunameePositionX, menunameePositionY}, COLOR_CYAN, COLOR_IN, true);

    }

*/

if(Fps) {
static USTExtraGameInstance *Instance = nullptr;
if (!Instance)
    Instance = UObject::FindObject<USTExtraGameInstance>("STExtraGameInstance Transient.UAEGameEngine_1.STExtraGameInstance_1");
if (Instance != nullptr)
{
    auto& UserSettings = Instance->UserDetailSetting;
    UserSettings.PUBGDeviceFPSDef = 120;
    UserSettings.PUBGDeviceFPSLow = 120;
    UserSettings.PUBGDeviceFPSMid = 120;
    UserSettings.PUBGDeviceFPSHigh = 120;
    UserSettings.PUBGDeviceFPSHDR = 120;
    UserSettings.PUBGDeviceFPSUltralHigh = 120;
    UserSettings.DeviceMaxQualityLevel = 3;
}}



if (GANDU)
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
        Config.Ipad = true;
       // Config.Magic = true;
      //  Config.GANDU = true;
        Config.PlayerESP.Alert = true;
        
       Config.PlayerESP.MessageBox = true;
       Config.SilentAim.Enable = true;
       Config.SilentAim.IgnoreKnocked = true;
        
        
        Config.AimBot.Enable = true;
     
        Config.AimBot.Radius = true;
        Config.AimBot.RecoilSet = 1.1f;             
Config.AimBot.IgnoreKnocked = true;
Config.AimBot.IgnoreBot = false;
Config.AimBot.VisCheck = true;
Config.AimBot.Trigger = EAimTrigger::Any;

        FOVSizea = 100.f;

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

    sprintf(keypath, "/sdcard/Android/obb/%s/Key.lic", Gamepackage);

    int fd = open(keypath, O_RDONLY);
    read(fd, &keyForLogin, sizeof(keyForLogin));
    close(fd);
}


void logError(const char *errorMessage)
{
    char filePath[256];
    sprintf(filePath, "/sdcard/Android/obb/%s/Error.txt", Gamepackage);

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

 


void *LoginThread(void *arg)
{
    // Login disabled - always valid
    sleep(2); // wait for game init
    bValid = true;
    isLogin = true;
    g_Token = "1";
    g_Auth = "1";
    return 0;
}

void* CreateMemoryPage(void* realAddress) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    void* aligned_address = (void*)((uintptr_t)realAddress & ~(page_size - 1));
    void* fakePage = mmap(nullptr, page_size, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (fakePage == MAP_FAILED) {
        return nullptr;
    }
    memcpy(fakePage, aligned_address, page_size);
    return fakePage;
}
template <typename T>
void VTable_Hook(void** vtable, int32_t index, T newFunction, T& oldFunction) {
    if (vtable[index] != newFunction) {
        oldFunction = reinterpret_cast<T>(vtable[index]);
        // Direct VTable patch - no SIGSEGV handler
        size_t page_size = sysconf(_SC_PAGESIZE);
        void* page_start = (void*)((uintptr_t)&vtable[index] & ~(page_size - 1));
        mprotect(page_start, page_size, PROT_READ | PROT_WRITE);
        vtable[index] = (void*)newFunction;
        mprotect(page_start, page_size, PROT_READ);
    }
}

void *(*orig_PostRender)(UGameViewportClient* ViewportClient, UCanvas* Canvas);
void *new_PostRender(UGameViewportClient* ViewportClient, UCanvas* Canvas){
	if (Canvas) {
RenderESP(Canvas, Canvas->SizeX, Canvas->SizeY);
    }
	
    return orig_PostRender(ViewportClient, Canvas);
}

int GetPostRenderIndex() {
    return (200 - 66);
}


void PostrenderDraw() {
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

void FixGameCrash() {
        system("rm -rf /data/data/com.pubg.imobile/files/");
        
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
  }

void banfixer()
        {
system("rm -rf /data/data/com.pubg.imobile/shared_prefs");
system("mkdir /data/data/com.pubg.imobile/shared_prefs");
system("chmod 777 /data/data/com.pubg.imobile/shared_prefs");
system("rm -rf /data/data/com.pubg.imobile/files");
system("rm -rf /data/data/com.pubg.imobile/databases");
system("rm -rf /data/data/com.pubg.imobile/files/login-identifier.txt");
system("rm -rf /data/data/com.pubg.imobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Intermediate");
system("rm -rf /data/data/com.pubg.imobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Intermediate");
system("rm -rf /data/data/com.pubg.imobile/files/TGPA");
system("rm -rf /data/data/com.pubg.imobile/files/TGPA");
system("rm -rf /data/data/com.pubg.imobile/files/ProgramBinaryCache");
system("rm -rf /data/data/com.pubg.imobile/files/ProgramBinaryCache");
system("chmod -R 000 /data/data/com.pubg.imobile/files/ano_tmp/ano.ano3.dat");

        }

bool file_exists(const std::string& path) {
    return (access(path.c_str(), F_OK) != -1);
}

void clean_logs() {
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
                }
            }
        }
    }
}


  
  
  
void *Chameli(void *) {

// clean_logs(); // disabled - deletes game runtime files
// banfixer(); // disabled - causes crash
// FixGameCrash(); // disabled - causes crash

    while (!UE4) {
        UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }    

    while (!g_App) {
        g_App = *(android_app * *)(UE4 + GNativeAndroidApp_Offset);
        
        sleep(1);
    }

   *(uintptr_t *)&MessageBoxExt = UE4 + 0x81BE3CC;
    FName::GNames = GetGNames();
    while (!FName::GNames) {
        FName::GNames = GetGNames();
        sleep(1);
    }
    UObject::GUObjectArray = (FUObjectArray * )(UE4 + GUObject_Offset);
//DobbyHook((void *) DobbySymbolResolver(OBFUSCATE("/apex/com.android.runtime/lib64/bionic/libc.so"), OBFUSCATE("AnoSDKSetUserInfo")), (void *) userinfo, (void **) &ouserinfo);
    static bool loadFont = false;
    if (!loadFont)
    {
        pthread_t t;
        pthread_create(&t, 0, LoadFont, 0);
        loadFont = true;
    }
    // Wait for KAMLESH_thread bypass to complete
    while (!g_BypassDone) { sleep(2); }
    sleep(5); // extra settle time
    PostrenderDraw();

    return 0;    
}


__attribute__((constructor)) void _init() {
	pthread_t Ptid;
    pthread_create(&Ptid, 0,Chameli, 0);
   // pthread_create(&Ptid, 0, Vivid_thread, 0);
	
    pthread_create(&Ptid, 0, LoginThread, 0);
}


