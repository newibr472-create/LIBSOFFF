// sdk_functions.cpp — Only the SDK ProcessEvent functions we need
// Extracted from NIKON_Engine_functions.cpp and NIKON_ShadowTrackerExtra_functions.cpp
#include "SDK.hpp"

namespace SDK {

// ============================================================================
// UCanvas::K2_DrawLine
// ============================================================================
void UCanvas::K2_DrawLine(const struct FVector2D& ScreenPositionA, const struct FVector2D& ScreenPositionB, float Thickness, const struct FLinearColor& RenderColor) {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function Engine.Canvas.K2_DrawLine");
    if (!pFunc) return;
    UCanvas_K2_DrawLine_Params params;
    params.ScreenPositionA = ScreenPositionA;
    params.ScreenPositionB = ScreenPositionB;
    params.Thickness = Thickness;
    params.RenderColor = RenderColor;
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
}

// ============================================================================
// UCanvas::K2_DrawText
// ============================================================================
void UCanvas::K2_DrawText(class UFont* RenderFont, const struct FString& RenderText, const struct FVector2D& ScreenPosition, const struct FLinearColor& RenderColor, float Kerning, const struct FLinearColor& ShadowColor, const struct FVector2D& ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, const struct FLinearColor& OutlineColor) {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function Engine.Canvas.K2_DrawText");
    if (!pFunc) return;
    UCanvas_K2_DrawText_Params params;
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
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
}

// ============================================================================
// UCanvas::K2_DrawBox
// ============================================================================
void UCanvas::K2_DrawBox(const struct FVector2D& ScreenPosition, const struct FVector2D& ScreenSize, float Thickness) {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function Engine.Canvas.K2_DrawBox");
    if (!pFunc) return;
    UCanvas_K2_DrawBox_Params params;
    params.ScreenPosition = ScreenPosition;
    params.ScreenSize = ScreenSize;
    params.Thickness = Thickness;
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
}

// ============================================================================
// APlayerController::ProjectWorldLocationToScreen
// ============================================================================
bool APlayerController::ProjectWorldLocationToScreen(const struct FVector& WorldLocation, bool bPlayerViewportRelative, struct FVector2D* ScreenLocation) {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function Engine.PlayerController.ProjectWorldLocationToScreen");
    if (!pFunc) return false;
    APlayerController_ProjectWorldLocationToScreen_Params params;
    params.WorldLocation = WorldLocation;
    params.bPlayerViewportRelative = bPlayerViewportRelative;
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
    if (ScreenLocation) *ScreenLocation = params.ScreenLocation;
    return params.ReturnValue;
}

// ============================================================================
// AController::SetControlRotation
// ============================================================================
void AController::SetControlRotation(const struct FRotator& NewRotation, const struct FString& CallSource) {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function Engine.Controller.SetControlRotation");
    if (!pFunc) return;
    AController_SetControlRotation_Params params;
    params.NewRotation = NewRotation;
    params.CallSource = CallSource;
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
}

// ============================================================================
// AController::GetControlRotation
// ============================================================================
struct FRotator AController::GetControlRotation() {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function Engine.Controller.GetControlRotation");
    if (!pFunc) return FRotator();
    AController_GetControlRotation_Params params;
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
    return params.ReturnValue;
}

// ============================================================================
// ASTExtraPlayerCharacter::GetBonePos
// ============================================================================
struct FVector ASTExtraPlayerCharacter::GetBonePos(const struct FName& BoneName, const struct FVector& OffsetPos) {
    static UFunction* pFunc = 0;
    if (!pFunc) pFunc = UObject::FindObject<UFunction>("Function ShadowTrackerExtra.STExtraPlayerCharacter.GetBonePos");
    if (!pFunc) return FVector();
    ASTExtraPlayerCharacter_GetBonePos_Params params;
    params.BoneName = BoneName;
    params.OffsetPos = OffsetPos;
    auto flags = pFunc->FunctionFlags;
    pFunc->FunctionFlags |= 0x400;
    ((UObject*)this)->ProcessEvent(pFunc, &params);
    pFunc->FunctionFlags = flags;
    return params.ReturnValue;
}

} // namespace SDK
