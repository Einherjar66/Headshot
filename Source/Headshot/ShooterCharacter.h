// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

class AWeapon;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied			UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInPorgess	UMETA(DisplayName = "FireTimerInPorgess"),
	ECS_Reloading			UMETA(DisplayName = "Reloading"),

	ECS_MAX					UMETA(DisplayName = "DefaultMAX")
};
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

	void MoveForward(float Value);				// Called for Forward / backward input
	void MoveRight(float Value);				// Called for side to side input
	void FireWeapon();							// Called when fire Button is pressed
	void AimingButtonPressed();					// SetbAmin to true
	void AimingButtonReleased();				// Set bAiming to false
	void SetLookUpRates();						// Set BaseTurnRate an BaseLookUpRate based on Aiming
	void TraceForItems();						// Trace for Item if OverlappedItemCount >0
	void EquipWeapon(AWeapon* WeaponToEquip);	// Takes a weapon an attaches it to the mesh
	void DropWeapon();							// Detach weapon and let it fall to the ground
	void SwapWeapon(AWeapon* WeaponToSwap);		// Drops currently equipped Weapon and Equips TraceHitItem 
	void InitializeAmmoMap();					// Init the Ammo Map with ammo values
	void FireButtonPressed();
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector OutHitLacation);	// Line trace for items under the crosshairs

	AWeapon* SpawnDefaultWeapon();				// Spawns a default weapon and equips it

	void FireButtonReleased();
	void StartFireTimer();
	void CalculateCrosshairSpread(float DeltaTime);
	void StartCrosshairBulletFire();
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBemLocation);
	void CameraZoomIn(float DeltaTime);

	void SelectButtonPressed();
	void SelectButtonReleased();

	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();

	void ReloadButtonPressed();
	void ReloadWeapon();		// Handle reloading of the weapon
	bool CarryingAmmo();		// Checks to see if we have ammo of the EquippedWeapon's ammo type
	UFUNCTION(BlueprintCallable)
	void FinishReload();

	UFUNCTION()
	void AutoFireReset();
	UFUNCTION()
	void FinishCrosshairBulletFire();

	/**
	 *  Called from Animation Blueprint with Grab Clip notify
	 */
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/**
	 *  Called from Animation Blueprint with Release Clip notify
	 */
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

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

	/**
	 * Check to make sure our weapon has ammo
	 */
	bool WeaponHasAmmo();

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Smoke trail for bullets
	UParticleSystem* BeamParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Montage for firing the weapon
	class UAnimMontage* HipFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Montage for Reload animation
	UAnimMontage* ReloadMontage;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Currently equipped Weapon
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))		// The AItem we hit last frame
	class AItem* TraceHitItemLastFrame;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))	// The item currently hit by our trace in TraceForItems (could be null)
	AItem* TraceHitItem;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))	// Set this in Blueprints for the default Weapon class
	TSubclassOf<AWeapon> DefaultWeaponClass;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))			// Distance outward from the camera for the interp destination
	float CameraInterpDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))			// Distance upward from the camera for the interp destination
	float CameraIterpElevation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))			// Map to keep track of ammo of the different ammo types
	TMap<EAmmoType, int32> AmmoMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items", meta = (AllowPrivateAccess = "true"))			// Stating amount of 9mm ammo 
	int32 Starting9mmAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items", meta = (AllowPrivateAccess = "true"))			// Stating amount of AR ammo
	int32 StartingARAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Combat state, can only fire or reload if Unoccupied
	ECombatState CombatState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Transform of the clip when we first grab the clip during reloading
	FTransform ClipTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Scene component to attach to the Charater's hand during reloading
	USceneComponent* HandSceneComponent;

	float CameraDefaultFOV;					// Default camera field of view
	float CameraZoomedFOV;					// Field of view value for when zoomed in	
	float CameraCurrentFOV;					// Current field of view this frame

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	bool bFireButtonPressed;				// Left mouse button or right console trigger pressed 
	bool bShouldFire;						// True when we can fire. False when waiting for the timer
	float AutomaticFireRate;				// Rate of automatic gun fire
	FTimerHandle AutoFireTimer;				// Sets a timer between gunshots

	bool bShouldTraceForItems;				// True if we should trace every frame for items
	int8 OverlappedItemCount;				// Number of overlapped AItems


public:

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }				// Returns USpringArmComponent subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }								// Returns FollowCamera subobject
	FORCEINLINE bool GetAiming()const { return bAiming; }
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	
	void IncrementOverlappedItemCount(int8 Amount);			// Adds/ subtracts to/from OverlappedItemCount and updates bShouldTraceForItems
	void GetPickupItem(AItem* Item);

	FVector GetCameraInterpLocation();
};
