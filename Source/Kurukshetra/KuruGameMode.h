#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KuruTypes.h"
#include "KuruGameMode.generated.h"

class AKuruEnemy;
class UStaticMesh;
class UMaterialInterface;

UCLASS()
class KURUKSHETRA_API AKuruGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AKuruGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    void StartChapter(EKuruHero Hero);
    void ReturnToSelection();
    void EnemyDefeated();
    void FailChapter();
    bool IsPlaying() const;
    bool WasCompleted(EKuruHero Hero) const;
    FString GetObjective() const;
    FString GetStoryLine() const;
    FString GetHeroName() const;

    UPROPERTY(BlueprintReadOnly) EKuruHero SelectedHero = EKuruHero::Arjuna;
    UPROPERTY(BlueprintReadOnly) EKuruPhase Phase = EKuruPhase::Selection;
    UPROPERTY(BlueprintReadOnly) int32 EnemiesRemaining = 0;
    UPROPERTY(BlueprintReadOnly) int32 WaypointIndex = 0;
    UPROPERTY(BlueprintReadOnly) float TimeRemaining = 150.f;
    UPROPERTY(BlueprintReadOnly) FVector ObjectiveLocation = FVector::ZeroVector;

private:
    void BuildArena();
    void ClearEncounter();
    void SpawnEncounter();
    void CompleteChapter();
    void PlaceMarker(const FVector& Location);
    void AddShape(UStaticMesh* Mesh, const FVector& Location, const FVector& Scale,
                  const FRotator& Rotation, UMaterialInterface* Material, bool bCollision = true);
    UPROPERTY() TObjectPtr<UStaticMesh> Cube;
    UPROPERTY() TObjectPtr<UStaticMesh> Cylinder;
    UPROPERTY() TObjectPtr<UMaterialInterface> Sand;
    UPROPERTY() TObjectPtr<UMaterialInterface> Stone;
    UPROPERTY() TObjectPtr<UMaterialInterface> Gold;
    UPROPERTY() TObjectPtr<UMaterialInterface> Red;
    UPROPERTY() TObjectPtr<UMaterialInterface> Blue;
    UPROPERTY() TObjectPtr<UKuruSave> Progress;
    UPROPERTY() TObjectPtr<AActor> Marker;
    UPROPERTY() TArray<TObjectPtr<AKuruEnemy>> Enemies;
    TArray<FVector> Route;
};
