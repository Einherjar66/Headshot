// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AShooterCharacter::AShooterCharacter() : BaseTurnRate(45.f), BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create SpringArmComponent - pulls in towards the character if there is a collision
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->bUsePawnControlRotation = true;					// Rotate the arm based on the controller
	
	// Create Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;						// Camera does not relative to arm 

	// Don't rotate when the controller rotates. Let the Controller only affect the camera
 	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure Char Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // Lets the camera rotate in Yaw (Left & Right)
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = .2f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

void AShooterCharacter::FireWeapon()
{

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTranform = BarrelSocket->GetSocketTransform(GetMesh());
		
		if (MuzzelFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzelFlash, SocketTranform);

		}
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFire)
	{
		AnimInstance->Montage_Play(HipFire);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, & ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
}
