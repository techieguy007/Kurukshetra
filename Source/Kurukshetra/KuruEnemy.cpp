#include "KuruEnemy.h"
#include "KuruHero.h"
#include "KuruGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

AKuruEnemy::AKuruEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42, 90);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCharacterMovement()->bRunPhysicsWithNoController = true;
    GetCharacterMovement()->MaxWalkSpeed = 220;
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    Body->SetupAttachment(RootComponent);
    Body->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));
    Body->SetRelativeScale3D(FVector(.65, .65, 1.2));
    Body->SetRelativeLocation(FVector(0, 0, -20));
    Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
    Head->SetupAttachment(RootComponent);
    Head->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
    Head->SetRelativeScale3D(FVector(.42));
    Head->SetRelativeLocation(FVector(0, 0, 58));
    Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKuruEnemy::Configure(bool bCaptain, bool bOpposingPandava)
{
    Health = bCaptain ? 180 : 60;
    Damage = bCaptain ? 14 : 8;
    GetCharacterMovement()->MaxWalkSpeed = bCaptain ? 185 : 240;
    if (bCaptain) SetActorScale3D(FVector(1.22));
    auto* Material = LoadObject<UMaterialInterface>(nullptr, bOpposingPandava ? TEXT("/Game/Prototype/M_Blue.M_Blue") : TEXT("/Game/Prototype/M_Red.M_Red"));
    Body->SetMaterial(0, Material);
    Head->SetMaterial(0, bCaptain ? LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Gold.M_Gold")) : Material);
}

void AKuruEnemy::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this));
    if (bDefeated || !GM || !GM->IsPlaying()) { GetCharacterMovement()->StopMovementImmediately(); return; }
    auto* Hero = Cast<AKuruHero>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Hero) return;
    FVector Delta = Hero->GetActorLocation() - GetActorLocation();
    Delta.Z = 0;
    SetActorRotation(Delta.Rotation());
    if (Delta.SizeSquared() > FMath::Square(130.f)) AddMovementInput(Delta.GetSafeNormal(), 1, true);
    AttackTimer -= DeltaSeconds;
    if (Delta.SizeSquared() < FMath::Square(155.f) && AttackTimer <= 0)
    {
        UGameplayStatics::ApplyDamage(Hero, Damage, nullptr, this, UDamageType::StaticClass());
        AttackTimer = 1.25f;
    }
}

float AKuruEnemy::TakeDamage(float Amount, const FDamageEvent& Event, AController* InstigatorController, AActor* Causer)
{
    if (bDefeated) return 0;
    Health -= FMath::Max(0.f, Amount);
    if (Health <= 0)
    {
        bDefeated = true;
        SetActorEnableCollision(false);
        SetActorHiddenInGame(true);
        if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->EnemyDefeated();
        Destroy();
    }
    return Amount;
}
