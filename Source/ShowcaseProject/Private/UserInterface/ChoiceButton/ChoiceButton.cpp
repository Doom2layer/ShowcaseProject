#include "UserInterface/ChoiceButton/ChoiceButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

UChoiceButton::UChoiceButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , InternalButton(nullptr)
    , ChoiceTextBlock(nullptr)
    , DefaultChoiceText(FText::FromString(TEXT("Choice")))
    , TextColor(FLinearColor::White)
    , TextSize(32.0f)
    , ChoiceIndex(-1)
    , bIsInitialized(false)
{
    UE_LOG(LogTemp, Warning, TEXT("ChoiceButton: Creating fallback widgets due to binding failures"));
}

void UChoiceButton::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    // NativePreConstruct should NOT create widgets
    // Only prepare data or handle design-time setup
    UE_LOG(LogTemp, VeryVerbose, TEXT("ChoiceButton: NativePreConstruct called"));
}

void UChoiceButton::NativeConstruct()
{
    Super::NativeConstruct();

    // Widget binding validation with detailed logging
    bool bHasValidBindings = true;

    if (!InternalButton)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChoiceButton: InternalButton binding failed - widget name should be 'InternalButton'"));
        bHasValidBindings = false;
    }

    if (!ChoiceTextBlock)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChoiceButton: ChoiceTextBlock binding failed - widget name should be 'ChoiceTextBlock'"));
        bHasValidBindings = false;
    }

    // Create fallback widgets only if necessary and WidgetTree is valid
    if (!bHasValidBindings)
    {
        if (WidgetTree)
        {
            UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Creating fallback widgets with valid WidgetTree"));
            CreateFallbackWidgets();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ChoiceButton: Cannot create fallback widgets - WidgetTree is null"));
            return;
        }
    }

    // Bind button events - Proper UE delegate binding
    if (InternalButton)
    {
        // Clear any existing bindings to prevent double-binding
        InternalButton->OnClicked.RemoveAll(this);
        
        // Add the new binding
        InternalButton->OnClicked.AddDynamic(this, &UChoiceButton::HandleInternalClick);
        UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Button click delegate bound"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ChoiceButton: Cannot bind click delegate - InternalButton is null"));
    }

    // Apply styling and finalize
    ConfigureWidgetStyling();
    bIsInitialized = true;

    UE_LOG(LogTemp, Log, TEXT("ChoiceButton: NativeConstruct completed successfully"));
}

void UChoiceButton::CreateFallbackWidgets()
{
    if (!WidgetTree)
    {
        UE_LOG(LogTemp, Error, TEXT("ChoiceButton: WidgetTree is null during fallback creation"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Creating fallback widgets"));

    // Ensure root widget exists
    UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
    if (!RootCanvas)
    {
        RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
        WidgetTree->RootWidget = RootCanvas;
    }

    // Create InternalButton if missing
    if (!InternalButton && RootCanvas)
    {
        InternalButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("InternalButton"));
        if (InternalButton)
        {
            UCanvasPanelSlot* ButtonSlot = RootCanvas->AddChildToCanvas(InternalButton);
            ButtonSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
            ButtonSlot->SetOffsets(FMargin(0.0f));
            UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Created fallback InternalButton"));
        }
    }

    // Create ChoiceTextBlock if missing
    if (!ChoiceTextBlock)
    {
        ChoiceTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ChoiceTextBlock"));
        if (ChoiceTextBlock)
        {
            if (InternalButton)
            {
                // Add as child to button
                InternalButton->AddChild(ChoiceTextBlock);
                UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Added ChoiceTextBlock to InternalButton"));
            }
            else if (RootCanvas)
            {
                // Add directly to canvas as fallback
                UCanvasPanelSlot* TextSlot = RootCanvas->AddChildToCanvas(ChoiceTextBlock);
                TextSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
                TextSlot->SetOffsets(FMargin(10.0f));
                UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Added ChoiceTextBlock to Canvas"));
            }
        }
    }
}

void UChoiceButton::ConfigureWidgetStyling()
{
    if (ChoiceTextBlock)
    {
        ChoiceTextBlock->SetText(DefaultChoiceText);
        ChoiceTextBlock->SetColorAndOpacity(TextColor);
        ChoiceTextBlock->SetJustification(ETextJustify::Center);

        // Configure font
        FSlateFontInfo FontInfo = ChoiceTextBlock->GetFont();
        FontInfo.Size = TextSize;
        ChoiceTextBlock->SetFont(FontInfo);

        UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Styling applied - Text: %s, Size: %d"), 
               *DefaultChoiceText.ToString(), TextSize);
    }

    if (InternalButton)
    {
        // Apply any button-specific styling here
        UE_LOG(LogTemp, VeryVerbose, TEXT("ChoiceButton: Button styling configured"));
    }
}

void UChoiceButton::SetChoiceIndex(int32 InChoiceIndex)
{
    ChoiceIndex = InChoiceIndex;
    UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Index set to %d"), ChoiceIndex);
}

void UChoiceButton::InitializeChoiceButton(int32 InChoiceIndex, const FText& ChoiceText)
{
    SetChoiceIndex(InChoiceIndex);
    SetChoiceText(ChoiceText);
    UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Initialized with index %d and text: %s"), 
           InChoiceIndex, *ChoiceText.ToString());
}

void UChoiceButton::SetChoiceText(const FText& InText)
{
    // Always store the text for later use
    DefaultChoiceText = InText;

    if (ChoiceTextBlock)
    {
        ChoiceTextBlock->SetText(InText);
        UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Text set to: %s"), *InText.ToString());
    }
    else
    {
        // Widget might not be constructed yet - text will be applied in ConfigureWidgetStyling
        UE_LOG(LogTemp, Warning, TEXT("ChoiceButton: ChoiceTextBlock null, text stored for later application: %s"), 
               *InText.ToString());
    }

}

void UChoiceButton::HandleInternalClick()
{
    if (ChoiceIndex >= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChoiceButton[%d]: Broadcasting click event"), ChoiceIndex);
        OnChoiceButtonClicked.Broadcast(this, ChoiceIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ChoiceButton: Click ignored - invalid choice index: %d"), ChoiceIndex);
    }
}