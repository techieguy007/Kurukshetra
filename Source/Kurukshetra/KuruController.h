#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KuruController.generated.h"

class UKuruHUD;
UCLASS()
class KURUKSHETRA_API AKuruController : public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    void ShowSelection();
    void EnterGameplay();
    void ShowResult(bool bWon);
    void TogglePause();
    bool IsChapterPaused() const { return bChapterPaused; }
private:
    UPROPERTY() TObjectPtr<UKuruHUD> HUD;
    bool bChapterPaused = false;
};
