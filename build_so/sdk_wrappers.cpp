// Minimal ProcessEvent wrappers for functions used by main.cpp
// These replace the massive Engine_functions.cpp and ShadowTrackerExtra_functions.cpp
#include "SDK.hpp"
namespace SDK {

// === GetBonePos (ShadowTrackerExtra) ===
FVector ASTExtraPlayerCharacter::GetBonePos(const FName& BoneName, const FVector& DefaultValue) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function ShadowTrackerExtra.STExtraPlayerCharacter.GetBonePos");
    struct { FName BoneName; FVector DefaultValue; FVector ReturnValue; } params;
    params.BoneName = BoneName;
    params.DefaultValue = DefaultValue;
    params.ReturnValue = FVector();
    if (pFunc) ProcessEvent(pFunc, &params);
    return params.ReturnValue;
}

// === K2_DrawLine (Engine.Canvas) ===
void UCanvas::K2_DrawLine(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB, float Thickness, const FLinearColor& RenderColor) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.Canvas.K2_DrawLine");
    struct { FVector2D ScreenPositionA; FVector2D ScreenPositionB; float Thickness; FLinearColor RenderColor; } params;
    params.ScreenPositionA = ScreenPositionA;
    params.ScreenPositionB = ScreenPositionB;
    params.Thickness = Thickness;
    params.RenderColor = RenderColor;
    if (pFunc) ProcessEvent(pFunc, &params);
}

// === K2_DrawText (Engine.Canvas) ===
void UCanvas::K2_DrawText(class UFont* RenderFont, const FString& RenderText, const FVector2D& ScreenPosition, const FLinearColor& RenderColor, float Kerning, const FLinearColor& ShadowColor, const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, const FLinearColor& OutlineColor) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.Canvas.K2_DrawText");
    struct {
        UFont* RenderFont;
        FString RenderText;
        FVector2D ScreenPosition;
        FLinearColor RenderColor;
        float Kerning;
        FLinearColor ShadowColor;
        FVector2D ShadowOffset;
        bool bCentreX;
        bool bCentreY;
        bool bOutlined;
        FLinearColor OutlineColor;
    } params;
    params.RenderFont = RenderFont;
    params.RenderText = RenderText;
    params.ScreenPosition = ScreenPosition;
    params.RenderColor = RenderColor;
    params.Kerning = Kerning;
    params.ShadowColor = ShadowColor;
    params.ShadowOffset = ShadowOffset;
    params.bCentreX = bCentreX;
    params.bCentreY = bCentreY;
    params.bOutlined = bOutlined;
    params.OutlineColor = OutlineColor;
    if (pFunc) ProcessEvent(pFunc, &params);
}

// === ProjectWorldLocationToScreen (Engine.PlayerController) ===
bool APlayerController::ProjectWorldLocationToScreen(const FVector& WorldLocation, bool bPlayerViewportRelative, FVector2D* ScreenLocation) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.PlayerController.ProjectWorldLocationToScreen");
    struct { FVector WorldLocation; FVector2D ScreenLocation; bool ReturnValue; } params;
    params.WorldLocation = WorldLocation;
    params.ScreenLocation = FVector2D();
    params.ReturnValue = false;
    if (pFunc) ProcessEvent(pFunc, &params);
    if (ScreenLocation) *ScreenLocation = params.ScreenLocation;
    return params.ReturnValue;
}

// === GetControlRotation (Engine.Controller) ===
FRotator AController::GetControlRotation() {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.Controller.GetControlRotation");
    struct { FRotator ReturnValue; } params;
    params.ReturnValue = FRotator();
    if (pFunc) ProcessEvent(pFunc, &params);
    return params.ReturnValue;
}

// === SetControlRotation (Engine.Controller) ===
void AController::SetControlRotation(const FRotator& NewRotation, const FString& CallSource) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.Controller.SetControlRotation");
    struct { FRotator NewRotation; FString CallSource; } params;
    params.NewRotation = NewRotation;
    params.CallSource = CallSource;
    if (pFunc) ProcessEvent(pFunc, &params);
}

// === GetDistanceTo (Engine.Actor) ===
float AActor::GetDistanceTo(class AActor* OtherActor) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.Actor.GetDistanceTo");
    struct { AActor* OtherActor; float ReturnValue; } params;
    params.OtherActor = OtherActor;
    params.ReturnValue = 0;
    if (pFunc) ProcessEvent(pFunc, &params);
    return params.ReturnValue;
}

// === GetVelocity (Engine.Actor) ===
FVector AActor::GetVelocity() {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.Actor.GetVelocity");
    struct { FVector ReturnValue; } params;
    params.ReturnValue = FVector();
    if (pFunc) ProcessEvent(pFunc, &params);
    return params.ReturnValue;
}

} // namespace SDK

// === GetAllActorsOfClass (Engine.GameplayStatics) ===
namespace SDK {
void UGameplayStatics::GetAllActorsOfClass(UObject* WorldContextObject, UClass* ActorClass, TArray<AActor*>* OutActors) {
    static UFunction* pFunc = nullptr;
    if (!pFunc)
        pFunc = UObject::FindObject<UFunction>("Function Engine.GameplayStatics.GetAllActorsOfClass");
    struct { UObject* WorldContextObject; UClass* ActorClass; TArray<AActor*> OutActors; } params;
    params.WorldContextObject = WorldContextObject;
    params.ActorClass = ActorClass;
    if (pFunc) {
        UGameplayStatics* gps = (UGameplayStatics*)UGameplayStatics::StaticClass();
        if (gps) gps->ProcessEvent(pFunc, &params);
    }
    if (OutActors) *OutActors = params.OutActors;
}
}
