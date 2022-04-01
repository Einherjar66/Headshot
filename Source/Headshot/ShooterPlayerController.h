// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HEADSHOT_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AShooterPlayerController();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))			// Reference to the HUD Overlay Blueprint Class 
	TSubclassOf<class UUserWidget> HUDOverlayClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))		// Variable to holds the HUD Overlay Widget after creating it
	UUserWidget* HUDOverlay;
};
