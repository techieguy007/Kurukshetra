#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KuruTypes.h"
#include "KuruHUD.generated.h"

class UCanvasPanel;
class UTextBlock;
class UProgressBar;
class UButton;
class UVerticalBox;
class UBorder;

UCLASS()
class KURUKSHETRA_API UKuruHUD : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& Geometry, float DeltaSeconds) override;
    void ShowSelection();
    void ShowGameplay();
    void ShowPause();
    void ShowResult(bool bWon);
private:
    void ResetScreen();
    void AddText(UVerticalBox* Parent, const FString& Text, int32 Size, FLinearColor Color);
    UButton* AddButton(UVerticalBox* Parent, const FString& Label);
    UVerticalBox* AddPanel(const FVector2D& Anchor, const FVector2D& Alignment,
                           const FVector2D& Position, const FVector2D& Size);
    void AddHeroCard(class UHorizontalBox* Row, EKuruHero Hero, const FString& Name,
                     const FString& Role, const FString& Description, const FString& ButtonLabel);
    UFUNCTION() void ChooseArjuna();
    UFUNCTION() void ChooseKrishna();
    UFUNCTION() void ChooseBhishma();
    UFUNCTION() void FirePressed();
    UFUNCTION() void FireReleased();
    UFUNCTION() void AbilityPressed();
    UFUNCTION() void PausePressed();
    UFUNCTION() void Replay();
    UFUNCTION() void BackToSelection();
    UFUNCTION() void QuitGame();
    UPROPERTY() TObjectPtr<UCanvasPanel> Canvas;
    UPROPERTY() TObjectPtr<UTextBlock> Objective;
    UPROPERTY() TObjectPtr<UTextBlock> Status;
    UPROPERTY() TObjectPtr<UTextBlock> Cooldown;
    UPROPERTY() TObjectPtr<UProgressBar> Health;
    bool bGameplay = false;
};
