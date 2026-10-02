#include "KuruGameMode.h"
#include "KuruHero.h"
#include "KuruEnemy.h"
#include "KuruArrow.h"
#include "KuruController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

AKuruGameMode::AKuruGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = AKuruHero::StaticClass();
    PlayerControllerClass = AKuruController::StaticClass();
    Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
}

void AKuruGameMode::BeginPlay()
{
    Super::BeginPlay();
    Sand = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Sand.M_Sand"));
    Stone = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Stone.M_Stone"));
    Gold = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Gold.M_Gold"));
    Red = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Red.M_Red"));
    Blue = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Blue.M_Blue"));
    Progress = Cast<UKuruSave>(UGameplayStatics::LoadGameFromSlot(TEXT("KurukshetraProgress"), 0));
    if (!Progress) Progress = Cast<UKuruSave>(UGameplayStatics::CreateSaveGameObject(UKuruSave::StaticClass()));
    BuildArena();
    Route = { FVector(-900, -850, 20), FVector(250, 650, 20), FVector(1600, -450, 20), FVector(2750, 450, 20) };
}

void AKuruGameMode::AddShape(UStaticMesh* Mesh, const FVector& Location, const FVector& Scale,
                            const FRotator& Rotation, UMaterialInterface* Material, bool bCollision)
{
    AActor* Actor = GetWorld()->SpawnActor<AActor>(Location, Rotation);
    auto* Component = NewObject<UStaticMeshComponent>(Actor);
    Actor->SetRootComponent(Component);
    Actor->AddInstanceComponent(Component);
    Component->SetStaticMesh(Mesh);
    Component->SetMobility(EComponentMobility::Movable);
    Component->SetCollisionProfileName(bCollision ? TEXT("BlockAll") : TEXT("NoCollision"));
    Component->SetCastShadow(bCollision);
    if (Material) Component->SetMaterial(0, Material);
    Component->RegisterComponent();
    Actor->SetActorLocationAndRotation(Location, Rotation);
    Actor->SetActorScale3D(Scale);
}

void AKuruGameMode::BuildArena()
{
    AddShape(Cube, FVector(1000, 0, -70), FVector(90, 65, 1.4), FRotator::ZeroRotator, Sand);
    AddShape(Cube, FVector(1000, -3150, 140), FVector(90, 1, 4.2), FRotator::ZeroRotator, Stone);
    AddShape(Cube, FVector(1000, 3150, 140), FVector(90, 1, 4.2), FRotator::ZeroRotator, Stone);
    AddShape(Cube, FVector(-3400, 0, 140), FVector(1, 64, 4.2), FRotator::ZeroRotator, Stone);
    AddShape(Cube, FVector(5400, 0, 140), FVector(1, 64, 4.2), FRotator::ZeroRotator, Stone);
    for (int32 i = 0; i < 11; ++i)
    {
        const float X = -2200.f + i * 650.f;
        for (float Side : { -1.f, 1.f })
        {
            AddShape(Cylinder, FVector(X, Side * 1700, 180), FVector(.18, .18, 3.6), FRotator::ZeroRotator, Stone);
            AddShape(Cube, FVector(X + 65, Side * 1700, 295), FVector(1.35, .045, .8), FRotator::ZeroRotator, Side < 0 ? Blue : Red, false);
            AddShape(Cube, FVector(X, Side * 2300, 145), FVector(3, 3.2, 2.9), FRotator(0, 12.f * i, 0), Stone);
            AddShape(Cube, FVector(X, Side * 2300, 325), FVector(3.3, 3.5, .65), FRotator(0, 12.f * i, 0), Sand);
        }
    }
    // Low-cost distant silhouettes; no large army simulation in this first chapter.
    for (int32 i = 0; i < 30; ++i)
    {
        AddShape(Cylinder, FVector(3900 + (i % 3) * 180, -1300 + (i / 3) * 280, 75),
                 FVector(.38, .38, 1.5), FRotator::ZeroRotator, i % 2 ? Blue : Red, false);
    }
    UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    UMaterialInterface* Sky = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Prototype/M_Sky.M_Sky"));
    AddShape(Sphere, FVector::ZeroVector, FVector(500), FRotator::ZeroRotator, Sky, false);
    ADirectionalLight* Sun = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0, 0, 1000), FRotator(-48, -35, 0));
    Sun->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Sun->GetLightComponent()->SetIntensity(3.f);
    Sun->GetLightComponent()->SetLightColor(FLinearColor(1.f, .89f, .72f));

    Marker = GetWorld()->SpawnActor<AActor>();
    auto* MarkerMesh = NewObject<UStaticMeshComponent>(Marker);
    Marker->SetRootComponent(MarkerMesh);
    Marker->AddInstanceComponent(MarkerMesh);
    MarkerMesh->SetStaticMesh(Cylinder);
    MarkerMesh->SetMaterial(0, Gold);
    MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MarkerMesh->SetCastShadow(false);
    MarkerMesh->RegisterComponent();
    Marker->SetActorScale3D(FVector(4.4, 4.4, .12));
    Marker->SetActorHiddenInGame(true);
}

void AKuruGameMode::ClearEncounter()
{
    for (auto& Enemy : Enemies) if (IsValid(Enemy)) Enemy->Destroy();
    Enemies.Empty();
    for (TActorIterator<AKuruArrow> It(GetWorld()); It; ++It) It->Destroy();
    EnemiesRemaining = 0;
}

void AKuruGameMode::StartChapter(EKuruHero Hero)
{
    ClearEncounter();
    SelectedHero = Hero;
    WaypointIndex = 0;
    TimeRemaining = 150.f;
    Phase = EKuruPhase::Approach;
    auto* PC = Cast<AKuruController>(UGameplayStatics::GetPlayerController(this, 0));
    if (auto* Pawn = Cast<AKuruHero>(PC ? PC->GetPawn() : nullptr))
    {
        Pawn->Configure(Hero);
        Pawn->SetActorLocation(FVector(-2200, 0, 105), false, nullptr, ETeleportType::TeleportPhysics);
        Pawn->SetActorRotation(FRotator::ZeroRotator);
        PC->SetControlRotation(FRotator(-12, 0, 0));
    }
    PlaceMarker(Hero == EKuruHero::Krishna ? Route[0] : FVector(-750, 0, 12));
    if (PC) PC->EnterGameplay();
}

void AKuruGameMode::ReturnToSelection()
{
    Phase = EKuruPhase::Selection;
    ClearEncounter();
    if (Marker) Marker->SetActorHiddenInGame(true);
    if (auto* PC = Cast<AKuruController>(UGameplayStatics::GetPlayerController(this, 0))) PC->ShowSelection();
}

bool AKuruGameMode::IsPlaying() const
{
    return Phase == EKuruPhase::Approach || Phase == EKuruPhase::Encounter;
}

void AKuruGameMode::PlaceMarker(const FVector& Location)
{
    ObjectiveLocation = Location;
    if (Marker) { Marker->SetActorLocation(Location); Marker->SetActorHiddenInGame(false); }
}

void AKuruGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!IsPlaying()) return;
    auto* Pawn = Cast<AKuruHero>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Pawn) return;
    if (Pawn->GetActorLocation().Z < -500) { FailChapter(); return; }
    if (SelectedHero == EKuruHero::Krishna)
    {
        TimeRemaining = FMath::Max(0.f, TimeRemaining - DeltaSeconds);
        if (TimeRemaining <= 0) { FailChapter(); return; }
        if (FVector::Dist2D(Pawn->GetActorLocation(), ObjectiveLocation) < 250)
        {
            ++WaypointIndex;
            if (WaypointIndex >= Route.Num()) CompleteChapter();
            else PlaceMarker(Route[WaypointIndex]);
        }
    }
    else if (Phase == EKuruPhase::Approach && FVector::Dist2D(Pawn->GetActorLocation(), ObjectiveLocation) < 250)
    {
        Phase = EKuruPhase::Encounter;
        SpawnEncounter();
    }
}

void AKuruGameMode::SpawnEncounter()
{
    if (Marker) Marker->SetActorHiddenInGame(true);
    for (int32 i = 0; i < 5; ++i)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        auto* Enemy = GetWorld()->SpawnActor<AKuruEnemy>(FVector(600 + i * 250, (i % 2 ? -1 : 1) * (300 + i * 100), 105), FRotator(0, 180, 0), Params);
        if (Enemy)
        {
            Enemy->Configure(i == 4, SelectedHero == EKuruHero::Bhishma);
            Enemies.Add(Enemy);
            ++EnemiesRemaining;
        }
    }
    if (EnemiesRemaining == 0) FailChapter();
}

void AKuruGameMode::EnemyDefeated()
{
    if (Phase != EKuruPhase::Encounter) return;
    EnemiesRemaining = FMath::Max(0, EnemiesRemaining - 1);
    if (EnemiesRemaining == 0) CompleteChapter();
}

void AKuruGameMode::CompleteChapter()
{
    Phase = EKuruPhase::Complete;
    if (Marker) Marker->SetActorHiddenInGame(true);
    if (Progress)
    {
        if (SelectedHero == EKuruHero::Arjuna) Progress->bArjunaComplete = true;
        else if (SelectedHero == EKuruHero::Krishna) Progress->bKrishnaComplete = true;
        else Progress->bBhishmaComplete = true;
        UGameplayStatics::SaveGameToSlot(Progress, TEXT("KurukshetraProgress"), 0);
    }
    if (auto* PC = Cast<AKuruController>(UGameplayStatics::GetPlayerController(this, 0))) PC->ShowResult(true);
}

void AKuruGameMode::FailChapter()
{
    if (!IsPlaying()) return;
    Phase = EKuruPhase::Failed;
    if (auto* PC = Cast<AKuruController>(UGameplayStatics::GetPlayerController(this, 0))) PC->ShowResult(false);
}

bool AKuruGameMode::WasCompleted(EKuruHero Hero) const
{
    if (!Progress) return false;
    return Hero == EKuruHero::Arjuna ? Progress->bArjunaComplete :
        Hero == EKuruHero::Krishna ? Progress->bKrishnaComplete : Progress->bBhishmaComplete;
}

FString AKuruGameMode::GetHeroName() const
{
    return SelectedHero == EKuruHero::Arjuna ? TEXT("ARJUNA") : SelectedHero == EKuruHero::Krishna ? TEXT("KRISHNA") : TEXT("BHISHMA");
}

FString AKuruGameMode::GetObjective() const
{
    if (SelectedHero == EKuruHero::Krishna)
        return FString::Printf(TEXT("Guide the chariot to banner %d / 4  |  %.0f sec"), FMath::Min(WaypointIndex + 1, 4), TimeRemaining);
    if (Phase == EKuruPhase::Approach) return TEXT("Reach the golden rally marker ahead.");
    return FString::Printf(TEXT("Secure the banner road  |  %d opponents remaining"), EnemiesRemaining);
}

FString AKuruGameMode::GetStoryLine() const
{
    if (SelectedHero == EKuruHero::Krishna) return TEXT("KRISHNA'S PERSPECTIVE\nKeep the chariot moving. Find a clear passage between the banners.\nThis prototype explores guidance and movement; Krishna does not attack.");
    if (SelectedHero == EKuruHero::Bhishma) return TEXT("BHISHMA'S PERSPECTIVE\nHold the approach and preserve your strength.\nYour defensive stance reduces incoming damage for five seconds.");
    return TEXT("ARJUNA'S PERSPECTIVE\nReach the banner road, then clear the approach.\nKeep your distance, aim carefully, and use Focus for faster shots.");
}
