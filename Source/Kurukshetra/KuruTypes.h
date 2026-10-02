#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KuruTypes.generated.h"

UENUM(BlueprintType)
enum class EKuruHero : uint8 { Arjuna, Krishna, Bhishma };

UENUM(BlueprintType)
enum class EKuruPhase : uint8 { Selection, Approach, Encounter, Complete, Failed };

UCLASS()
class KURUKSHETRA_API UKuruSave : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) bool bArjunaComplete = false;
    UPROPERTY(SaveGame) bool bKrishnaComplete = false;
    UPROPERTY(SaveGame) bool bBhishmaComplete = false;
};
