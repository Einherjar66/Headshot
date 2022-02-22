// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class HEADSHOT_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/**
	 *   Pointer
	 */

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// USpringArmComponent positioning the Camera behind the character
	class USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Camera follows the character
	class UCameraComponent* FollowCamera;

public:
	
	
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }				// Returns USpringArmComponent subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }								// Returns FollowCamera subobject
};
