#include "KuruHero.h"
#include "KuruGameMode.h"
#include "KuruController.h"
#include "KuruEnemy.h"
#include "KuruArrow.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

AKuruHero::AKuruHero()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42, 96);
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->MaxWalkSpeed = 540;
    GetCharacterMovement()->BrakingDecelerationWalking = 1800;
    Boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    Boom->SetupAttachment(RootComponent);
    Boom->TargetArmLength = 460;
    Boom->SocketOffset = FVector(0, 60, 65);
    Boom->bUsePawnControlRotation = true;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    Camera->SetupAttachment(Boom);
    Camera->FieldOfView = 85;
    auto* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    auto* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    auto* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarriorBody"));
    Body->SetupAttachment(RootComponent);
    Body->SetStaticMesh(CylinderMesh);
    Body->SetRelativeScale3D(FVector(.62, .62, 1.2));
    Body->SetRelativeLocation(FVector(0, 0, -20));
    Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarriorHead"));
    Head->SetupAttachment(RootComponent);
    Head->SetStaticMesh(SphereMesh);
    Head->SetRelativeLocation(FVector(0, 0, 60));
    Head->SetRelativeScale3D(FVector(.43));
    Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Bow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BowPlaceholder"));
    Bow->SetupAttachment(RootComponent);
    Bow->SetStaticMesh(CylinderMesh);
    Bow->SetRelativeLocation(FVector(35, 40, 0));
    Bow->SetRelativeScale3D(FVector(.07, .07, 1.35));
    Bow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Chariot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChariotPlatform"));
    Chariot->SetupAttachment(RootComponent);
    Chariot->SetStaticMesh(CubeMesh);
    Chariot->SetRelativeLocation(FVector(-10, 0, -55));
    Chariot->SetRelativeScale3D(FVector(1.9, 1.55, .3));
    Chariot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWheel"));
    RightWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWheel"));
    for (auto* Wheel : { LeftWheel.Get(), RightWheel.Get() })
    {
        Wheel->SetupAttachment(RootComponent);
        Wheel->SetStaticMesh(CylinderMesh);
        Wheel->SetRelativeRotation(FRotator(0, 0, 90));
        Wheel->SetRelativeScale3D(FVector(.8, .8, .12));
        Wheel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    LeftWheel->SetRelativeLocation(FVector(0, -83, -50));
    RightWheel->SetRelativeLocation(FVector(0, 83, -50));
    Chariot->SetVisibility(false);
    LeftWheel->SetVisibility(false);
    RightWheel->SetVisibility(false);
}

void AKuruHero::Configure(EKuruHero Hero)
{
    HeroType = Hero;
    MaxHealth = Hero == EKuruHero::Bhishma ? 160.f : 100.f;
    Health = MaxHealth;
    ShotTimer = AbilityTimer = AbilityActive = 0;
    StopActions();
    GetCharacterMovement()->MaxWalkSpeed = Hero == EKuruHero::Bhishma ? 420 : Hero == EKuruHero::Krishna ? 650 : 540;
    const bool bChariot = Hero == EKuruHero::Krishna;
    Chariot->SetVisibility(bChariot);
    LeftWheel->SetVisibility(bChariot);
    RightWheel->SetVisibility(bChariot);
    Bow->SetVisibility(!bChariot);
    Boom->TargetArmLength = bChariot ? 580 : 460;
    const TCHAR* Path = Hero == EKuruHero::Arjuna ? TEXT("/Game/Prototype/M_Blue.M_Blue") :
        Hero == EKuruHero::Krishna ? TEXT("/Game/Prototype/M_Gold.M_Gold") : TEXT("/Game/Prototype/M_White.M_White");
    if (auto* Material = LoadObject<UMaterialInterface>(nullptr, Path)) { Body->SetMaterial(0, Material); Head->SetMaterial(0, Material); }
    if (auto* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Stone.M_Stone")))
    {
        Bow->SetMaterial(0, Material); Chariot->SetMaterial(0, Material);
        LeftWheel->SetMaterial(0, Material); RightWheel->SetMaterial(0, Material);
    }
}

bool AKuruHero::CanAct() const
{
    const auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this));
    return GM && GM->IsPlaying() && !UGameplayStatics::IsGamePaused(this);
}

void AKuruHero::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!CanAct()) return;
    ShotTimer = FMath::Max(0.f, ShotTimer - DeltaSeconds);
    AbilityTimer = FMath::Max(0.f, AbilityTimer - DeltaSeconds);
    AbilityActive = FMath::Max(0.f, AbilityActive - DeltaSeconds);
    if (HeroType == EKuruHero::Krishna)
    {
        GetCharacterMovement()->MaxWalkSpeed = AbilityActive > 0 ? 1000 : 650;
        const float Angle = GetVelocity().Size() * DeltaSeconds;
        LeftWheel->AddLocalRotation(FRotator(0, 0, Angle));
        RightWheel->AddLocalRotation(FRotator(0, 0, Angle));
    }
    if (bPrimaryHeld) PrimaryAction();
}

void AKuruHero::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    Input->BindAxis(TEXT("MoveForward"), this, &AKuruHero::MoveForward);
    Input->BindAxis(TEXT("MoveRight"), this, &AKuruHero::MoveRight);
    Input->BindAxis(TEXT("Turn"), this, &AKuruHero::Turn);
    Input->BindAxis(TEXT("LookUp"), this, &AKuruHero::LookUp);
    Input->BindAxis(TEXT("TurnRate"), this, &AKuruHero::TurnRate);
    Input->BindAxis(TEXT("LookUpRate"), this, &AKuruHero::LookUpRate);
    Input->BindAction(TEXT("Primary"), IE_Pressed, this, &AKuruHero::PressPrimary);
    Input->BindAction(TEXT("Primary"), IE_Released, this, &AKuruHero::ReleasePrimary);
    Input->BindAction(TEXT("Ability"), IE_Pressed, this, &AKuruHero::UseAbility);
    auto& PauseBinding = Input->BindAction(TEXT("Pause"), IE_Pressed, this, &AKuruHero::Pause);
    PauseBinding.bExecuteWhenPaused = true;
}

void AKuruHero::MoveForward(float Value)
{
    if (CanAct() && Controller) AddMovementInput(FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::X), Value);
}
void AKuruHero::MoveRight(float Value)
{
    if (CanAct() && Controller) AddMovementInput(FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::Y), Value);
}
void AKuruHero::Turn(float Value) { if (CanAct()) AddControllerYawInput(Value); }
void AKuruHero::LookUp(float Value) { if (CanAct()) AddControllerPitchInput(Value); }
void AKuruHero::TurnRate(float Value) { Turn(Value * 80.f * GetWorld()->GetDeltaSeconds()); }
void AKuruHero::LookUpRate(float Value) { LookUp(Value * 65.f * GetWorld()->GetDeltaSeconds()); }
void AKuruHero::PressPrimary() { bPrimaryHeld = true; PrimaryAction(); }
void AKuruHero::ReleasePrimary() { bPrimaryHeld = false; }
void AKuruHero::StopActions() { bPrimaryHeld = false; GetCharacterMovement()->StopMovementImmediately(); }
void AKuruHero::Pause() { if (auto* PC = Cast<AKuruController>(Controller)) PC->TogglePause(); }

void AKuruHero::PrimaryAction()
{
    if (!CanAct() || ShotTimer > 0 || HeroType == EKuruHero::Krishna) return;
    const FVector CameraLocation = Camera->GetComponentLocation();
    const FVector Forward = Camera->GetForwardVector();
    FVector Target = CameraLocation + Forward * 6500;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(KuruAim), false, this);
    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, Target, ECC_Visibility, Params)) Target = Hit.ImpactPoint;
    // Gentle aim assist in a narrow cone also makes touch aiming practical.
    float BestDot = .99f;
    for (TActorIterator<AKuruEnemy> It(GetWorld()); It; ++It)
    {
        if (It->IsActorBeingDestroyed()) continue;
        const FVector Delta = It->GetActorLocation() - CameraLocation;
        const float Dot = FVector::DotProduct(Delta.GetSafeNormal(), Forward);
        if (Dot > BestDot && Delta.SizeSquared() < FMath::Square(6500.f))
        {
            FHitResult SightHit;
            if (GetWorld()->LineTraceSingleByChannel(SightHit, CameraLocation, It->GetActorLocation(), ECC_Visibility, Params) && SightHit.GetActor() == *It)
            { BestDot = Dot; Target = It->GetActorLocation(); }
        }
    }
    const FVector Origin = GetActorLocation() + GetActorForwardVector() * 70 + FVector(0, 0, 30);
    const FVector Direction = (Target - Origin).GetSafeNormal();
    FActorSpawnParameters Spawn;
    Spawn.Owner = this;
    Spawn.Instigator = this;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    if (auto* Arrow = GetWorld()->SpawnActor<AKuruArrow>(Origin, Direction.Rotation(), Spawn))
        Arrow->Launch(Direction, HeroType == EKuruHero::Bhishma ? 45.f : 30.f);
    ShotTimer = HeroType == EKuruHero::Bhishma ? .7f : AbilityActive > 0 ? .18f : .38f;
}

void AKuruHero::UseAbility()
{
    if (!CanAct() || AbilityTimer > 0) return;
    AbilityTimer = 12;
    AbilityActive = 5;
}

float AKuruHero::TakeDamage(float Amount, const FDamageEvent& Event, AController* InstigatorController, AActor* Causer)
{
    if (!CanAct() || HeroType == EKuruHero::Krishna) return 0;
    const float Applied = FMath::Max(0.f, Amount) * (HeroType == EKuruHero::Bhishma && AbilityActive > 0 ? .25f : 1.f);
    Health = FMath::Max(0.f, Health - Applied);
    if (Health <= 0) if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->FailChapter();
    return Applied;
}
