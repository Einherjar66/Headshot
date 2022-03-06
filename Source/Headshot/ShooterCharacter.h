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
	void FireWeapon();				// Called when fire Button is pressed
	void AimingButtonPressed();		// SetbAmin to true
	void AimingButtonReleased();	// Set bAiming to false

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

	/**
	 * Rotate controller based on mouse X movement
	 * @param Value		The input value from mouse movement
	 */
	void Turn(float Value);

	/**
	 * Rotate controller based on mouse Y movement
	 * @param Value		The input value from mouse movement
	 */
	void LookUp(float Value);

	void CalculateCrosshairSpread(float DeltaTime);
	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Particles spawned upon bullet Impact
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Somke trail for bullets
	UParticleSystem* BeamParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Montage for firing the weapon
	class UAnimMontage* HipFire;

	/**
	 * Functions
	 */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBemLocation);
	void CameraZoomIn(float DeltaTime);
	void SetLookUpRates();	//Set BaseTurnRate an BaseLookUpRate based on Aiming

																				
	/**
	 *  Variables
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Base turn rate, in deg/sec. Other scaling may affect final turn rate
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Base look up/down rate, in deg/sec. Other scaling may affect final turn rate
	float BaseLookUpRate;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Combat", meta = (AllowPrivateAccess = "true"))		// true when aiming
	bool bAiming;																								
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Interp speed for zooming when aiming
	float ZoomInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Turn rate while not aiming
	float HipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Look rate while not aiming
	float HipLookUpRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Turn rate when aiming
	float AimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))	// Turn rate when aiming
	float AimingLookUpRate;
	
	/*
	 *	Scale factor mouse look sensitivity.Turn rate when not aiming -
	 *	ClampMin/Max beschränkt den werte Bereich. UIMin/Max beschränkt den Slider im Editor auf jeweiligen werten Bereich
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0" ,UIMax = "1.0"))	
	float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))	// Scale factor mouse look sensitivity. Look up rate when not aiming
	float MouseHipLookUpRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))	// Scale factor mouse look sensitivity. Turn rate when  aiming
	float MouseAimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))	// Scale factor mouse look sensitivity. Look up rate when  aiming
	float MouseAimingLookUpRate;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))		// Determines the spread of the crosshairs
	float CrosshairSpreadMultiplier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))	// Velocity component for crosshairs spread
	float CrosshairVelocityFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))	// In air component for crosshairs spread
	float CrosshairInAirFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))	// Aim component for crosshairs spread
	float CrosshairAimFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))	// Shooting component for crosshairs spread
	float CrosshairShootingFactor;



	float CameraDefaultFOV;							// Default  camera field of view
	float CameraZoomedFOV;							// Field of view value for when zoomed in	
	float CameraCurrentFOV;							// Current field of view this frame

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

public:

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }				// Returns USpringArmComponent subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }								// Returns FollowCamera subobject
	FORCEINLINE bool GetAiming()const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
};
