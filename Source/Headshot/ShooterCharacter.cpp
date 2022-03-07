// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

#include "DrawDebugHelpers.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :

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
	MouseAimingTurnRate(.2f),
	MouseAimingLookUpRate(.2f),


	// True when aiming the weapon
	bAiming(false),

	// camera field of view values
	ZoomInterpSpeed(15.f),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(45.f),
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
	bFireButtonPressed (false),
	bShouldFire (true),
	AutomaticFireRate(.1f)

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

void AShooterCharacter::FireWeapon()
{

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());	// TODO -> FTransform <- Need a comment  
		
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

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFire)
	{
		AnimInstance->Montage_Play(HipFire);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

	// Start bullet fire timer for crosshairs
	StartCrosshairBulletFire();
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBemLocation)
{
	/**
	 *  Video 35. Directing Rifle Shots. Da mir noch einpaar sachen noch nicht ganz klar sind -> Nochmals Anshen <-
	 */

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
		CrosshairWorldDirection);						// Get the world Location of the Crosshair

	if (bScreenToWorld) // was deprojection successful?
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWoldPosition };
		const FVector End{ CrosshairWoldPosition + CrosshairWorldDirection * 50'000.f };

		// Set beam end point to line trace end point
		OutBemLocation = End;

		// Trace outward from crosshairs world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit)									// was there a trace hit?
		{
			OutBemLocation = ScreenTraceHit.Location;							// Beam end point is now trace hit location

		}

		// Perform a second trace, this time from the gun barrel
		FHitResult WeaponTraceHit;
		const FVector WaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBemLocation };
		GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);


		if (WeaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint
		{
			OutBemLocation = WeaponTraceHit.Location;
		}
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
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
	}
}

void AShooterCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
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
}

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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, & ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

}
