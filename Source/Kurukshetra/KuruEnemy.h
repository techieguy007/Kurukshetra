#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KuruEnemy.generated.h"

class UStaticMeshComponent;
UCLASS()
class KURUKSHETRA_API AKuruEnemy : public ACharacter
{
    GENERATED_BODY()
public:
    AKuruEnemy();
    virtual void Tick(float DeltaSeconds) override;
    virtual float TakeDamage(float Amount, const FDamageEvent& Event, AController* InstigatorController, AActor* Causer) override;
    void Configure(bool bCaptain, bool bOpposingPandava);
private:
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Body;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Head;
    float Health = 60.f;
    float AttackTimer = 0.f;
    float Damage = 8.f;
    bool bDefeated = false;
};
