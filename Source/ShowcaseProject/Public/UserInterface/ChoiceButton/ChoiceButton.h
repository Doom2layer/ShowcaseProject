// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ChoiceButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceButtonClickedDelegate, class UChoiceButton*, ClickedButton, int32, ChoiceIndex);

class UTextBlock;

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UChoiceButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
    explicit UChoiceButton(const FObjectInitializer& ObjectInitializer);

    // Self-identification delegate
    UPROPERTY(BlueprintAssignable, Category = "Choice Button")
    FOnChoiceButtonClickedDelegate OnChoiceButtonClicked;

    UFUNCTION(BlueprintCallable, Category = "Choice Button")
    void SetChoiceIndex(int32 InChoiceIndex);

    UFUNCTION(BlueprintPure, Category = "Choice Button")
    int32 GetChoiceIndex() const { return ChoiceIndex; }

    UFUNCTION(BlueprintCallable, Category = "Choice Button")
    void InitializeChoiceButton(int32 InChoiceIndex, const FText& ChoiceText);

    UFUNCTION(BlueprintCallable, Category = "Choice Button")
    void SetChoiceText(const FText& InText);

    UFUNCTION(BlueprintPure, Category = "Choice Button")
    UTextBlock* GetChoiceTextWidget() const { return ChoiceTextBlock; }

    UFUNCTION(BlueprintPure, Category = "Choice Button")
    UButton* GetInternalButton() const { return InternalButton; }

protected:
    virtual void NativeConstruct() override;
    virtual void NativePreConstruct() override;

    // Widget bindings - these will be bound in Blueprint
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Choice Button")
    UButton* InternalButton;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Choice Button")
    UTextBlock* ChoiceTextBlock;

    // Fallback creation if widgets aren't bound in Blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Choice Button | Styling")
    FText DefaultChoiceText = FText::FromString(TEXT("Choice"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Choice Button | Styling")
    FSlateColor TextColor = FSlateColor(FLinearColor::White);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Choice Button | Styling")
    int32 TextSize = 16;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Choice Button", meta = (AllowPrivateAccess = "true"))
    int32 ChoiceIndex = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Choice Button", meta = (AllowPrivateAccess = "true"))
    bool bIsInitialized = false;

    UFUNCTION()
    void HandleInternalClick();

    void CreateFallbackWidgets();
    void ConfigureWidgetStyling();
};
