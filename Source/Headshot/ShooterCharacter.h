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


	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);	// Called for Forward / backward input
	void MoveRight(float Value);	// Called for side to side input
	
	/**
	 * Called via input to turn at a given rate
	 * @param Rate This is a normalized rate i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate); 

	/**
	 * Called via input to up/down at a given rate
	 * @param Rate This is a normalized rate i.e. 1.0 means 100% of desired  rate
	 */
	void LooUpAtRate(float Rate);

	void FireWeapon();				// Called when fire Button is pressed
private:

	/**
	 *   Class Pointer
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// USpringArmComponent positioning the Camera behind the character
	class USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Camera follows the character
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Randomized gunshot sound cue
	class USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Flash spawned at BarrelSocket
	class UParticleSystem* MuzzelFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Montage for firing the weapon
	class UAnimMontage* HipFire;


	/**
	 *  Variables
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Base turn rate, in deg/sec. Other scaling may affect final turn rate
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Base look up/down rate, in deg/sec. Other scaling may affect final turn rate
	float BaseLookUpRate;

public:

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }				// Returns USpringArmComponent subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }								// Returns FollowCamera subobject

};
