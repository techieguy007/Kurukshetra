#include "KuruArrow.h"
#include "KuruEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

AKuruArrow::AKuruArrow()
{
    PrimaryActorTick.bCanEverTick = false;
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);
    Collision->InitSphereRadius(8);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->OnComponentHit.AddDynamic(this, &AKuruArrow::OnHit);
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowShaft"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));
    Mesh->SetRelativeRotation(FRotator(90, 0, 0));
    Mesh->SetRelativeScale3D(FVector(.035, .035, .9));
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->UpdatedComponent = Collision;
    Movement->InitialSpeed = 5200;
    Movement->MaxSpeed = 5200;
    Movement->ProjectileGravityScale = 0;
    Movement->bRotationFollowsVelocity = true;
    InitialLifeSpan = 3;
}

void AKuruArrow::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwner()) Collision->IgnoreActorWhenMoving(GetOwner(), true);
}
void AKuruArrow::Launch(const FVector& Direction, float Damage)
{
    HitDamage = Damage;
    Movement->Velocity = Direction * Movement->InitialSpeed;
}
void AKuruArrow::OnHit(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& Hit)
{
    if (Other == GetOwner()) return;
    if (Cast<AKuruEnemy>(Other)) UGameplayStatics::ApplyDamage(Other, HitDamage, GetInstigatorController(), this, UDamageType::StaticClass());
    Destroy();
}
