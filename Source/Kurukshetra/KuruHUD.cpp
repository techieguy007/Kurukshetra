#include "KuruHUD.h"
#include "KuruGameMode.h"
#include "KuruHero.h"
#include "KuruController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

namespace KuruStyle
{
    const FLinearColor Paper(.94f, .87f, .71f, 1);
    const FLinearColor Gold(.9f, .58f, .20f, 1);
    const FLinearColor Muted(.63f, .68f, .7f, 1);
    const FLinearColor Panel(.023f, .04f, .055f, .96f);
}

TSharedRef<SWidget> UKuruHUD::RebuildWidget()
{
    // The native tree must exist BEFORE Slate is built, not in NativeConstruct.
    if (!WidgetTree) WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
    if (!Canvas)
    {
        Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
        Canvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        WidgetTree->RootWidget = Canvas;
    }
    return Super::RebuildWidget();
}

void UKuruHUD::NativeConstruct()
{
    Super::NativeConstruct();
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UKuruHUD::ResetScreen()
{
    if (Canvas) Canvas->ClearChildren();
    Objective = nullptr;
    Status = nullptr;
    Cooldown = nullptr;
    Health = nullptr;
    bGameplay = false;
}

void UKuruHUD::AddText(UVerticalBox* Parent, const FString& Text, int32 Size, FLinearColor Color)
{
    auto* Label = WidgetTree->ConstructWidget<UTextBlock>();
    Label->SetText(FText::FromString(Text));
    auto Font = Label->GetFont();
    Font.Size = Size;
    Label->SetFont(Font);
    Label->SetColorAndOpacity(FSlateColor(Color));
    Label->SetAutoWrapText(true);
    Label->SetVisibility(ESlateVisibility::HitTestInvisible);
    Parent->AddChildToVerticalBox(Label)->SetPadding(FMargin(0, 0, 0, 12));
}

UButton* UKuruHUD::AddButton(UVerticalBox* Parent, const FString& Label)
{
    auto* Size = WidgetTree->ConstructWidget<USizeBox>();
    Size->SetMinDesiredHeight(58);
    Parent->AddChildToVerticalBox(Size)->SetPadding(FMargin(0, 5, 0, 5));
    auto* Button = WidgetTree->ConstructWidget<UButton>();
    Button->SetBackgroundColor(FLinearColor(.35f, .22f, .09f, 1));
    Button->SetTouchMethod(EButtonTouchMethod::DownAndUp);
    Size->SetContent(Button);
    auto* Text = WidgetTree->ConstructWidget<UTextBlock>();
    Text->SetText(FText::FromString(Label));
    auto Font = Text->GetFont(); Font.Size = 20; Text->SetFont(Font);
    Text->SetColorAndOpacity(FSlateColor(KuruStyle::Paper));
    Text->SetJustification(ETextJustify::Center);
    Button->SetContent(Text);
    return Button;
}

UVerticalBox* UKuruHUD::AddPanel(const FVector2D& Anchor, const FVector2D& Alignment,
                               const FVector2D& Position, const FVector2D& Size)
{
    auto* Border = WidgetTree->ConstructWidget<UBorder>();
    Border->SetBrushColor(KuruStyle::Panel);
    Border->SetPadding(FMargin(26));
    Border->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    auto* Slot = Canvas->AddChildToCanvas(Border);
    Slot->SetAnchors(FAnchors(Anchor.X, Anchor.Y));
    Slot->SetAlignment(Alignment);
    Slot->SetPosition(Position);
    Slot->SetSize(Size);
    auto* Box = WidgetTree->ConstructWidget<UVerticalBox>();
    Box->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Border->SetContent(Box);
    return Box;
}

void UKuruHUD::AddHeroCard(UHorizontalBox* Row, EKuruHero Hero, const FString& Name,
                          const FString& Role, const FString& Description, const FString& ButtonLabel)
{
    auto* Border = WidgetTree->ConstructWidget<UBorder>();
    Border->SetBrushColor(FLinearColor(.055, .082, .10, 1));
    Border->SetPadding(FMargin(22));
    auto* Slot = Row->AddChildToHorizontalBox(Border);
    Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    Slot->SetPadding(FMargin(0, 0, 14, 0));
    auto* Box = WidgetTree->ConstructWidget<UVerticalBox>();
    Border->SetContent(Box);
    AddText(Box, Name, 32, KuruStyle::Paper);
    AddText(Box, Role, 19, KuruStyle::Gold);
    AddText(Box, Description, 18, KuruStyle::Muted);
    auto* Spacer = WidgetTree->ConstructWidget<USizeBox>();
    Box->AddChildToVerticalBox(Spacer)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this));
    AddText(Box, GM && GM->WasCompleted(Hero) ? TEXT("CHAPTER COMPLETED") : TEXT("CHAPTER 01 AVAILABLE"), 14, KuruStyle::Gold);
    auto* Button = AddButton(Box, ButtonLabel);
    if (Hero == EKuruHero::Arjuna) Button->OnClicked.AddDynamic(this, &UKuruHUD::ChooseArjuna);
    else if (Hero == EKuruHero::Krishna) Button->OnClicked.AddDynamic(this, &UKuruHUD::ChooseKrishna);
    else Button->OnClicked.AddDynamic(this, &UKuruHUD::ChooseBhishma);
}

void UKuruHUD::ShowSelection()
{
    ResetScreen();
    auto* Box = AddPanel(FVector2D(.5, .5), FVector2D(.5, .5), FVector2D::ZeroVector, FVector2D(1200, 780));
    AddText(Box, TEXT("K U R U K S H E T R A"), 42, KuruStyle::Paper);
    AddText(Box, TEXT("THREE PERSPECTIVES. ONE STORY."), 18, KuruStyle::Gold);
    AddText(Box, TEXT("Choose your perspective  /  Chapter 01: The Banner Road"), 23, KuruStyle::Paper);
    auto* Row = WidgetTree->ConstructWidget<UHorizontalBox>();
    Box->AddChildToVerticalBox(Row)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    AddHeroCard(Row, EKuruHero::Arjuna, TEXT("ARJUNA"), TEXT("THE ARCHER"),
        TEXT("Mobile archery and precise shots.\n\nFocus increases your firing speed.\n\nReach the road and defeat the opposing practice formation."), TEXT("Play as Arjuna"));
    AddHeroCard(Row, EKuruHero::Krishna, TEXT("KRISHNA"), TEXT("THE GUIDE"),
        TEXT("Guide a chariot between four banners.\n\nSwift Passage briefly increases speed.\n\nComplete the route before time runs out. No attacking."), TEXT("Play as Krishna"));
    AddHeroCard(Row, EKuruHero::Bhishma, TEXT("BHISHMA"), TEXT("THE GUARDIAN"),
        TEXT("Powerful arrows and greater endurance.\n\nSteadfast reduces incoming damage.\n\nHold the same road from the opposing perspective."), TEXT("Play as Bhishma"));
    AddText(Box, TEXT("An original practice scenario inspired by the Mahabharata. Not a depiction of a specific episode.\nEarly prototype: placeholder figures and scenery. The full campaign is still to be developed."), 15, KuruStyle::Muted);
    AddButton(Box, TEXT("Exit"))->OnClicked.AddDynamic(this, &UKuruHUD::QuitGame);
}

void UKuruHUD::ShowGameplay()
{
    ResetScreen();
    bGameplay = true;
    auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this));
    auto* Top = AddPanel(FVector2D(0, 0), FVector2D(0, 0), FVector2D(32, 24), FVector2D(760, 215));
    AddText(Top, GM ? GM->GetHeroName() + TEXT("  /  THE BANNER ROAD") : TEXT("THE BANNER ROAD"), 21, KuruStyle::Gold);
    Objective = WidgetTree->ConstructWidget<UTextBlock>();
    Objective->SetAutoWrapText(true);
    Objective->SetColorAndOpacity(FSlateColor(KuruStyle::Paper));
    Top->AddChildToVerticalBox(Objective)->SetPadding(FMargin(0, 0, 0, 12));
    Health = WidgetTree->ConstructWidget<UProgressBar>();
    Health->SetFillColorAndOpacity(FLinearColor(.26, .65, .50, 1));
    auto* HealthSize = WidgetTree->ConstructWidget<USizeBox>();
    HealthSize->SetHeightOverride(12);
    HealthSize->SetContent(Health);
    Top->AddChildToVerticalBox(HealthSize);
    Status = WidgetTree->ConstructWidget<UTextBlock>();
    Status->SetColorAndOpacity(FSlateColor(KuruStyle::Muted));
    Top->AddChildToVerticalBox(Status)->SetPadding(FMargin(0, 12, 0, 0));

    auto* Actions = AddPanel(FVector2D(1, 0), FVector2D(1, 0), FVector2D(-32, 24), FVector2D(280, 300));
    const bool bKrishna = GM && GM->SelectedHero == EKuruHero::Krishna;
    const FString Ability = bKrishna ? TEXT("SWIFT PASSAGE") : GM && GM->SelectedHero == EKuruHero::Bhishma ? TEXT("STEADFAST") : TEXT("FOCUS");
#if PLATFORM_ANDROID
    AddButton(Actions, TEXT("Pause"))->OnClicked.AddDynamic(this, &UKuruHUD::PausePressed);
    if (!bKrishna)
    {
        auto* Fire = AddButton(Actions, TEXT("SHOOT"));
        Fire->OnPressed.AddDynamic(this, &UKuruHUD::FirePressed);
        Fire->OnReleased.AddDynamic(this, &UKuruHUD::FireReleased);
    }
    AddButton(Actions, Ability)->OnClicked.AddDynamic(this, &UKuruHUD::AbilityPressed);
#else
    AddText(Actions, TEXT("CONTROLS"), 18, KuruStyle::Gold);
    AddText(Actions, TEXT("WASD  Move\nMouse  Look\nEsc  Pause"), 18, KuruStyle::Paper);
    if (!bKrishna) AddText(Actions, TEXT("Left click  Shoot"), 18, KuruStyle::Paper);
    AddText(Actions, TEXT("Q  ") + Ability, 17, KuruStyle::Gold);
#endif
    Cooldown = WidgetTree->ConstructWidget<UTextBlock>();
    Cooldown->SetColorAndOpacity(FSlateColor(KuruStyle::Gold));
    Actions->AddChildToVerticalBox(Cooldown);
    auto* Bottom = AddPanel(FVector2D(.5, 1), FVector2D(.5, 1), FVector2D(0, -24), FVector2D(640, 180));
#if PLATFORM_ANDROID
    AddText(Bottom, TEXT("LEFT STICK: MOVE   /   RIGHT STICK: LOOK"), 15, KuruStyle::Gold);
#else
    AddText(Bottom, bKrishna ? TEXT("FOLLOW THE GOLDEN MARKERS  /  Q: SWIFT PASSAGE") : TEXT("KEEP YOUR DISTANCE  /  HOLD LEFT CLICK TO SHOOT"), 14, KuruStyle::Gold);
#endif
    AddText(Bottom, GM ? GM->GetStoryLine() : TEXT(""), 16, KuruStyle::Paper);
    if (!bKrishna)
    {
        auto* Crosshair = WidgetTree->ConstructWidget<UTextBlock>();
        Crosshair->SetText(FText::FromString(TEXT("+")));
        auto Font = Crosshair->GetFont(); Font.Size = 32; Crosshair->SetFont(Font);
        Crosshair->SetColorAndOpacity(FSlateColor(KuruStyle::Paper));
        Crosshair->SetVisibility(ESlateVisibility::HitTestInvisible);
        auto* Slot = Canvas->AddChildToCanvas(Crosshair);
        Slot->SetAnchors(FAnchors(.5, .5));
        Slot->SetAlignment(FVector2D(.5, .5));
        Slot->SetAutoSize(true);
        Slot->SetPosition(FVector2D::ZeroVector);
    }
}

void UKuruHUD::NativeTick(const FGeometry& Geometry, float DeltaSeconds)
{
    Super::NativeTick(Geometry, DeltaSeconds);
    if (!bGameplay) return;
    auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this));
    auto* Hero = Cast<AKuruHero>(GetOwningPlayerPawn());
    if (!GM || !Hero) return;
    if (Objective) Objective->SetText(FText::FromString(GM->GetObjective()));
    if (Health) Health->SetPercent(Hero->HealthFraction());
    if (Status)
    {
        const float Distance = FVector::Dist2D(Hero->GetActorLocation(), GM->ObjectiveLocation) / 100.f;
        Status->SetText(FText::FromString(GM->SelectedHero == EKuruHero::Krishna || GM->Phase == EKuruPhase::Approach ?
            FString::Printf(TEXT("Golden marker: %.0f m away"), Distance) : TEXT("Hold fire to shoot repeatedly. Keep opponents at a distance.")));
    }
    if (Cooldown) Cooldown->SetText(FText::FromString(Hero->AbilityCooldown() > 0 ?
        FString::Printf(TEXT("Ability ready in %.0f s"), FMath::CeilToFloat(Hero->AbilityCooldown())) : TEXT("Ability ready")));
}

void UKuruHUD::ShowPause()
{
    ResetScreen();
    auto* Box = AddPanel(FVector2D(.5, .5), FVector2D(.5, .5), FVector2D::ZeroVector, FVector2D(600, 370));
    AddText(Box, TEXT("PAUSED"), 34, KuruStyle::Paper);
    AddText(Box, TEXT("The chapter resumes where you left it."), 18, KuruStyle::Muted);
    AddButton(Box, TEXT("Continue"))->OnClicked.AddDynamic(this, &UKuruHUD::PausePressed);
    AddButton(Box, TEXT("Restart chapter"))->OnClicked.AddDynamic(this, &UKuruHUD::Replay);
    AddButton(Box, TEXT("Choose another perspective"))->OnClicked.AddDynamic(this, &UKuruHUD::BackToSelection);
}

void UKuruHUD::ShowResult(bool bWon)
{
    ResetScreen();
    auto* Box = AddPanel(FVector2D(.5, .5), FVector2D(.5, .5), FVector2D::ZeroVector, FVector2D(780, 470));
    AddText(Box, bWon ? TEXT("THE ROAD IS CLEAR") : TEXT("THE CHAPTER IS NOT OVER"), 32, KuruStyle::Gold);
    AddText(Box, bWon ? TEXT("You completed this perspective of The Banner Road.") : TEXT("Try again. Keep your distance in combat, or follow the golden markers with the chariot."), 22, KuruStyle::Paper);
    AddText(Box, TEXT("Return to the same road through another character's eyes.\n\nThis short practice chapter establishes the controls. Cinematics, authentic character art, voice acting, and the epic's story chapters are future work."), 18, KuruStyle::Muted);
    AddButton(Box, TEXT("Choose another perspective"))->OnClicked.AddDynamic(this, &UKuruHUD::BackToSelection);
    AddButton(Box, TEXT("Replay this perspective"))->OnClicked.AddDynamic(this, &UKuruHUD::Replay);
}

void UKuruHUD::ChooseArjuna() { if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->StartChapter(EKuruHero::Arjuna); }
void UKuruHUD::ChooseKrishna() { if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->StartChapter(EKuruHero::Krishna); }
void UKuruHUD::ChooseBhishma() { if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->StartChapter(EKuruHero::Bhishma); }
void UKuruHUD::FirePressed() { if (auto* Hero = Cast<AKuruHero>(GetOwningPlayerPawn())) { Hero->SetPrimaryHeld(true); Hero->PrimaryAction(); } }
void UKuruHUD::FireReleased() { if (auto* Hero = Cast<AKuruHero>(GetOwningPlayerPawn())) Hero->SetPrimaryHeld(false); }
void UKuruHUD::AbilityPressed() { if (auto* Hero = Cast<AKuruHero>(GetOwningPlayerPawn())) Hero->UseAbility(); }
void UKuruHUD::PausePressed() { if (auto* PC = Cast<AKuruController>(GetOwningPlayer())) PC->TogglePause(); }
void UKuruHUD::Replay() { if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->StartChapter(GM->SelectedHero); }
void UKuruHUD::BackToSelection() { if (auto* GM = Cast<AKuruGameMode>(UGameplayStatics::GetGameMode(this))) GM->ReturnToSelection(); }
void UKuruHUD::QuitGame() { UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false); }
