// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

#include "Item.h"
#include "Ammo.h"
#include "Weapon.h"
#include "DrawDebugHelpers.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :

	// Starting Ammo Amounts
	Starting9mmAmmo(85),
	StartingARAmmo(120),

	// Base rate for turning/look up 
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),

	// Turn rates for Aiming/not Aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),

	// Mouse look sensitivity scale factors
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimingTurnRate(.5f),
	MouseAimingLookUpRate(.5f),

	// True when aiming the weapon
	bAiming(false),

	// Combat State
	CombatState(ECombatState::ECS_Unoccupied),

	// Movement
	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrounchingGroundFriction(100.f),

	// camera field of view values
	ZoomInterpSpeed(15.f),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(30.f),
	CameraCurrentFOV(0.f),

	// Crosshair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),

	// Bullet fire timer variables
	ShootTimeDuration(0.05f),
	bFiringBullet(false),

	// Automatic fire variables
	bFireButtonPressed(false),
	bShouldFire(true),
	AutomaticFireRate(.1f),

	// Item trace variables
	bShouldTraceForItems(false),
	OverlappedItemCount(0),

	// Camera interp location variables
	CameraInterpDistance(250.f),
	CameraIterpElevation(65.f),
	bCrouching(false),
	bAimingButtonPressed(true),

	// Pickup sound timer properties
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	EquipSoundResetTime(.2f),
	PickupSoundResetTime(.2f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create SpringArmComponent - pulls in towards the character if there is a collision
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 180.f;
	SpringArmComponent->bUsePawnControlRotation = true;					// Rotate the arm based on the controller
	SpringArmComponent->SocketOffset = FVector(0., 50.f, 70.f);

	// Create Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;						// Camera does not relative to arm 

	// Don't rotate when the controller rotates. Let the Controller only affect the camera
 	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure Char Movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);	// Lets the camera rotate in Yaw (Left & Right)
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = .2f;

	//Create HandSceneComponent
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	// Create Interp Componens
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(FName("WeaponInterpComp"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(FName("InterpComp1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(FName("InterpComp2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(FName("InterpComp3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(FName("InterpComp4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(FName("InterpComp5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(FName("InterpComp6"));
	InterpComp6->SetupAttachment(GetFollowCamera());

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	// Spawn the default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepht();
	EquippedWeapon->DisableGlowMaterial();

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	// Create FInterpLocation structs for each interp location. Add to array
	InitializeInterpLocations();
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle interpolation for zoom when aiming
	CameraZoomIn(DeltaTime);
	// Change look sensitivity based on aiming
	SetLookUpRates();
	// Calculate crosshair spread multiplier
	CalculateCrosshairSpread(DeltaTime);
	// Check OverlappingItemCount, then trace for items
	TraceForItems();
	// Interpolate the capsule half height based on crouching / Standing
	InterpCapsuleHalfHeight(DeltaTime);
}

void AShooterCharacter::MoveForward(float Value)						// Value, ist mit dem input wert der in der UE hinterlegt ist gebunden. (Das ist die 1.0 im input Fenster)
{

	if ((Controller != nullptr) && (Value != 0.f))						// Verify that the controller is valid & if Value != 0.f then do nothing
	{
		// Find out witch way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };	// Get the Controller Rotation
		const FRotator YawRotation{ 0.f,Rotation.Yaw, 0.f };			// From the controller rotation, We want only the yaw Rotation

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction,Value);								// Moves the Character
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))						// Verify that the controller is valid & if Value != 0.f then do nothing
	{
		// Find out witch way is Right
		const FRotator Rotation{ Controller->GetControlRotation() };	// Get the Controller Rotation
		const FRotator YawRotation{ 0.f,Rotation.Yaw, 0.f };			// From the controller rotation, We want only the yaw Rotation

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);								// Moves the Character
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information 
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());  //deg/sec * sec/frame  // GetWorld->GetDeltaSeconds(), gibt uns die Delta Time
}

void AShooterCharacter::LooUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleRate{};
	if (bAiming)
	{
		TurnScaleRate = MouseAimingTurnRate;
	} 
	else
	{
		TurnScaleRate = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleRate);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpScaleRate{};
	if (bAiming)
	{
		LookUpScaleRate = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleRate = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleRate);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f,600.f };
	FVector2D VelocityMultiplierRange{ 0.f,1.0f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// Calculate crosshair in air factor
	if (GetCharacterMovement()->IsFalling()) // is in air
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);		// Spread the crosshair slowly while in air
		
	}
	else //Character is on the ground
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, .0f, DeltaTime, 30.f);		// Shrink the crosshairs rapidly while on the ground
	}


	// Calculate crosshair aim factor
	if (bAiming)	// Aiming
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.2f, DeltaTime, 30.f);			// Shrink the crosshair while Aiming
	}
 	else			// Not Aiming
 	{
 		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, .0f, DeltaTime, 30.f);			// Spread the crosshair to normal while not Aiming
 
 	}


	// true 0.05 sec after firing
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	} 
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	// Index for the clip bone on the Equipped Weapon
	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };

	// Store the transform of the clip
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRuls(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRuls, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunfireMontage();
		EquippedWeapon->DecrementAmmo();
		
		StartFireTimer();
	}
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBemLocation)
{
	/**
	 *  Video 35. Directing Rifle Shots. Da mir noch einpaar sachen noch nicht ganz klar sind -> Nochmals Anshen <-
	 */

	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshaitHit = TraceUnderCrosshairs(CrosshairHitResult, OutBemLocation);

	if (bCrosshaitHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBemLocation = CrosshairHitResult.Location;
	}

	else	// No crosshair trace hit
	{
			// OutBeamLocation is the End location for line trace
	}

	// Perform a second trace, this time from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBemLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);


	if (WeaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint
	{
		OutBemLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;
}

void AShooterCharacter::CameraZoomIn(float DeltaTime)
{
	// Set current camera field of view
	if (bAiming)
	{
		// Interpolate to zoomed FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{

		// Interpolate to default FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);
	}
}

void AShooterCharacter::SelectButtonReleased()
{

}

void AShooterCharacter::PlayFireSound()
{
	// Play Fire sound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterCharacter::SendBullet()
{
	// Send bullet
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());	// TODO -> FTransform <- Need a comment  

		if (MuzzelFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzelFlash, SocketTransform);

		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void AShooterCharacter::PlayGunfireMontage()
{
	//Play Hip Fire Montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFire)
	{
		AnimInstance->Montage_Play(HipFire);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;

	// Do we have ammo for the correct type?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull()) 
	{
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimIntance = GetMesh()->GetAnimInstance();
		if (AnimIntance && ReloadMontage)
		{
			AnimIntance->Montage_Play(ReloadMontage);
			AnimIntance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}

	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrounchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalftHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime,20.f) };
	const float DeltaCapsuleHalfHeight{ InterpHalftHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() }; // Negative value if crouching; Positve value if standing
	const FVector MeshOffset{ 0.f,0.f, -DeltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalftHeight);
}

void AShooterCharacter::PickupAmmo(class AAmmo* Ammo)
{
	// check to see if AmmoMap contains Ammo's Ammotype
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// check amount of amm in our AmmoMap for this Ammo Type
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();

		//Set the amount of ammmo in the Map for this type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}
	
	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		// check to see if the gun is empty
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocations{ WeaponInterpComp, 0 };
	InterpLocations.Add(WeaponLocations);

	FInterpLocation InterpLoc1{ InterpComp1, 0 };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2, 0 };
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3, 0 };
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4, 0 };
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5, 0 };
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6, 0 };
	InterpLocations.Add(InterpLoc6);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}
	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::FinishReload()
{
	// Update the combat state
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr) return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };
	// Update the AmmoMap
	if (AmmoMap.Contains(AmmoType))
	{
		// amount of Ammo the Character is carrying of the EquippedWeapon type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// Space left in the magazine of EquippedWeapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			// Reload the Magazine with all the ammo we are carrying
			EquippedWeapon->RelaodAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			// fill the Magazine
			EquippedWeapon->RelaodAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::SetLookUpRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInPorgess;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				
				// Show Item's Pickup widget
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepht();
			}

			// We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame from last frame. Or AItem is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepht();
				}
			}

			// store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;
		}
	}

	else if (TraceHitItemLastFrame)
	{
		// No longer overlapping any item, Item last frame should not show widget
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepht();
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector OutHitLacation)
{	// Video 62. Trace for Widget - 63. Refactor Trace Under Crosshairs - Nochmals Ansehen 
	// Get Current size of the viewport 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)									// GEngine is a Global Engine pointer there hold the viewport
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);				// Get the viewport size and save in "ViewportSize"
	}

	// Get Screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);	// Get the middle of Viewport
	FVector CrosshairWoldPosition;
	FVector CrosshairWorldDirection;

	// Get world position an direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWoldPosition,
		CrosshairWorldDirection);											// Get the world Location of the Crosshair


	if (bScreenToWorld)
	{
		// trace from Crosshairs world location outward
		const FVector Start{ CrosshairWoldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLacation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLacation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	// Check TSubclassOf Variable
	if (DefaultWeaponClass)
	{
		// Spawn the Weapon
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);

	}
	return nullptr;
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		/*WeaponToEquip->GetItemState();*/

		// Get the Hand Socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			// Attach the Weapon to the hand socket (RightHandSocket)
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		// Set EquippedWeapon to the newly spawned Weapon
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTranformRules(EDetachmentRule::KeepWorld, true); // Mehr infos reinholen
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTranformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();				// Drop the current weapon
	EquipWeapon(WeaponToSwap);	// Take the new one
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.FindOrAdd(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.FindOrAdd(EAmmoType::EAT_AR, StartingARAmmo);
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}

	else
	{
		// Reload Weapon
		ReloadWeapon();
	}
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipsoundTimer()
{
	bShouldPlayEquipSound = true;
}

FInterpLocation AShooterCharacter::GetFInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	return FInterpLocation();
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipsoundTimer, EquipSoundResetTime);
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{

	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	Item->PlayEquipSound();

	AWeapon* Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		 if (	Inventory.Num() < INVENTORY_CAPACITY)
		 {
			 Weapon->SetSlotIndex(Inventory.Num());
			 Inventory.Add(Weapon);
			 Weapon->SetItemState(EItemState::EIS_Pickedup);
		 }

		 else // Inventory is full! Swap with equippen weapon
		 {
			 SwapWeapon(Weapon);
		 }
	}

	AAmmo* Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}

}

// No longer needed; AItem has GetInterpLocation
// 
// FVector AShooterCharacter::GetCameraInterpLocation()
// {
// 	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
// 	const FVector CameraForward{ FollowCamera->GetForwardVector() };
// 	// DesiredLocation = CameraWorldLocationv + CameraForward * CameraInterpDistance + CameraIterpElevation
// 	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraIterpElevation);
// }

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent)

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LooUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this,&AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, & ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);
}
