#include "KuruController.h"
#include "KuruHUD.h"
#include "KuruGameMode.h"
#include "KuruHero.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/TouchInterface.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

void AKuruController::BeginPlay()
{
    Super::BeginPlay();
    bEnableTouchEvents = true;
    PlayerCameraManager->ViewPitchMin = -60;
    PlayerCameraManager->ViewPitchMax = 35;
#if PLATFORM_ANDROID
    if (GEngine && GEngine->GetGameUserSettings())
    {
        auto* Settings = GEngine->GetGameUserSettings();
        Settings->SetOverallScalabilityLevel(1);
        Settings->SetFrameRateLimit(30);
        Settings->ApplyNonResolutionSettings();
    }
#endif
    HUD = CreateWidget<UKuruHUD>(this, UKuruHUD::StaticClass());
    if (HUD) HUD->AddToViewport(10);
    ShowSelection();
}

void AKuruController::ShowSelection()
{
    SetPause(false);
    bChapterPaused = false;
    if (auto* Hero = Cast<AKuruHero>(GetPawn())) Hero->StopActions();
    SetVirtualJoystickVisibility(false);
    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());
    if (HUD) HUD->ShowSelection();
}

void AKuruController::EnterGameplay()
{
    SetPause(false);
    bChapterPaused = false;
    bShowMouseCursor = false;
#if PLATFORM_ANDROID
    if (auto* Touch = LoadObject<UTouchInterface>(nullptr, TEXT("/Engine/MobileResources/HUD/DefaultVirtualJoysticks.DefaultVirtualJoysticks"))) ActivateTouchInterface(Touch);
    SetVirtualJoystickVisibility(true);
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(true);
    SetInputMode(Mode);
#else
    SetInputMode(FInputModeGameOnly());
#endif
    if (HUD) HUD->ShowGameplay();
}

void AKuruController::ShowResult(bool bWon)
{
    if (auto* Hero = Cast<AKuruHero>(GetPawn())) Hero->StopActions();
    SetVirtualJoystickVisibility(false);
    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());
    if (HUD) HUD->ShowResult(bWon);
}

void AKuruController::TogglePause()
{
    auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GM || !GM->IsPlaying()) return;
    if (bChapterPaused) { EnterGameplay(); return; }
    if (auto* Hero = Cast<AKuruHero>(GetPawn())) Hero->StopActions();
    bChapterPaused = true;
    SetPause(true);
    SetVirtualJoystickVisibility(false);
    bShowMouseCursor = true;
    SetInputMode(FInputModeGameAndUI());
    if (HUD) HUD->ShowPause();
}
