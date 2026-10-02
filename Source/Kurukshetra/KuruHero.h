#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KuruTypes.h"
#include "KuruHero.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

UCLASS()
class KURUKSHETRA_API AKuruHero : public ACharacter
{
    GENERATED_BODY()
public:
    AKuruHero();
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    virtual float TakeDamage(float Amount, const FDamageEvent& Event, AController* InstigatorController, AActor* Causer) override;
    void Configure(EKuruHero Hero);
    void PrimaryAction();
    void UseAbility();
    void SetPrimaryHeld(bool bHeld) { bPrimaryHeld = bHeld; }
    void StopActions();
    float HealthFraction() const { return Health / MaxHealth; }
    float AbilityCooldown() const { return AbilityTimer; }
    EKuruHero GetHero() const { return HeroType; }
private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void TurnRate(float Value);
    void LookUpRate(float Value);
    void PressPrimary();
    void ReleasePrimary();
    void Pause();
    bool CanAct() const;
    UPROPERTY(VisibleAnywhere) TObjectPtr<USpringArmComponent> Boom;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> Camera;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Body;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Head;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Bow;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Chariot;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> LeftWheel;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> RightWheel;
    EKuruHero HeroType = EKuruHero::Arjuna;
    float Health = 100.f;
    float MaxHealth = 100.f;
    float ShotTimer = 0.f;
    float AbilityTimer = 0.f;
    float AbilityActive = 0.f;
    bool bPrimaryHeld = false;
};
