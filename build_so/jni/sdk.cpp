// sdk.cpp — UObject name resolution + ProcessEvent helpers
#include "sdk.h"
#include "Main/Tools.h"

// Globals
FUObjectArray* UObject::GUObjectArray = nullptr;
TNameEntryArray* UObject::GNames = nullptr;
uintptr_t UE4 = 0;
TNameEntryArray* (*GetGNamesFunc)() = nullptr;

std::string UObject::GetName() const {
    if (!GNames) return "";
    auto entry = GNames->GetById(NamePrivate.ComparisonIndex);
    if (!entry) return "";
    if (entry->IsWide()) return ""; // skip wide names
    std::string name(entry->GetAnsiName());
    if (NamePrivate.Number > 0)
        name += "_" + std::to_string(NamePrivate.Number);
    return name;
}

std::string UObject::GetFullName() const {
    std::string name;
    if (ClassPrivate) {
        std::string temp;
        for (auto p = OuterPrivate; p; p = p->OuterPrivate) {
            temp = p->GetName() + "." + temp;
        }
        name = ((UObject*)ClassPrivate)->GetName();
        name += " ";
        name += temp;
        name += GetName();
    }
    return name;
}

bool UObject::IsA(UClass* cmp) const {
    // Simplified — just check ClassPrivate pointer equality
    // Full version would walk SuperField chain
    return ClassPrivate == cmp;
}

// K2_DrawLine via ProcessEvent
static UFunction* pK2DrawLine = nullptr;
void UCanvas::K2_DrawLine(const FVector2D& A, const FVector2D& B, float Thickness, const FLinearColor& Color) {
    if (!pK2DrawLine) {
        pK2DrawLine = UObject::FindObject<UFunction>("Function Engine.Canvas.K2_DrawLine");
        if (!pK2DrawLine) return;
    }
    UCanvas_K2_DrawLine_Params params;
    params.ScreenPositionA = A;
    params.ScreenPositionB = B;
    params.Thickness = Thickness;
    params.RenderColor = Color;
    
    auto flags = pK2DrawLine->FunctionFlags;
    pK2DrawLine->FunctionFlags |= 0x400;
    ProcessEvent(pK2DrawLine, &params);
    pK2DrawLine->FunctionFlags = flags;
}

// ProjectWorldLocationToScreen via ProcessEvent
struct ProjectW2S_Params {
    FVector WorldLocation;
    bool bPlayerViewportRelative;
    FVector2D ScreenPosition;
    bool ReturnValue;
};

static UFunction* pProjectW2S = nullptr;
bool APlayerController::ProjectWorldLocationToScreen(FVector WorldLocation, bool bPlayerViewportRelative, FVector2D& ScreenPosition) {
    if (!pProjectW2S) {
        pProjectW2S = UObject::FindObject<UFunction>("Function Engine.PlayerController.ProjectWorldLocationToScreen");
        if (!pProjectW2S) return false;
    }
    ProjectW2S_Params params;
    params.WorldLocation = WorldLocation;
    params.bPlayerViewportRelative = bPlayerViewportRelative;
    params.ScreenPosition = {0,0};
    params.ReturnValue = false;
    
    auto flags = pProjectW2S->FunctionFlags;
    pProjectW2S->FunctionFlags |= 0x400;
    ProcessEvent(pProjectW2S, &params);
    pProjectW2S->FunctionFlags = flags;
    
    ScreenPosition = params.ScreenPosition;
    return params.ReturnValue;
}

// GetBonePos (calls GetSocketLocation which is ProcessEvent based)
struct GetBonePos_Params {
    FName BoneName; // This is complex — needs FName construction
    FVector ReturnValue;
};

// Simplified: read bone transform from mesh component directly
FVector ASTExtraPlayerCharacter::GetBonePos(const char* BoneName, FVector DefaultPos) {
    // For now, use direct bone array read approach
    // The full SDK version uses ProcessEvent("GetSocketLocation")
    // We'll implement this via raw memory in the ESP function
    return DefaultPos;
}
