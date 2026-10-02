#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KuruArrow.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
UCLASS()
class KURUKSHETRA_API AKuruArrow : public AActor
{
    GENERATED_BODY()
public:
    AKuruArrow();
    virtual void BeginPlay() override;
    void Launch(const FVector& Direction, float Damage);
private:
    UFUNCTION() void OnHit(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComponent,
                          FVector Impulse, const FHitResult& Hit);
    UPROPERTY(VisibleAnywhere) TObjectPtr<USphereComponent> Collision;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UProjectileMovementComponent> Movement;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Mesh;
    float HitDamage = 30.f;
};
