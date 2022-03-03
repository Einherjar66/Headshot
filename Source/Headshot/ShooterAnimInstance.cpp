// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)															// Wenn der ShooterCharacter null ist.
	{ 
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());							// Dann versuch die AnimInstance wieder zurückzuholen.
	}

	if (ShooterCharacter)
	{
		// Get the lateral speed of the character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		// Is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();						// Checkt ob der Character am "fallen" ist oder nicht

		// Is the Char accelerating ?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)	// Checkt ob der Char "Beschleunigt"
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotaion = ShooterCharacter->GetBaseAimRotation();										// Returns the Rotator where we are Aiming at
		FRotator MovementRotaion = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotaion, AimRotaion).Yaw;	// Give us the Movement offset Yaw
		

		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;														// LastMovementOffsetYaw saves the last moment before the player comes to a standstill
		}

		bAiming = ShooterCharacter->GetAiming();

	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());								// Gibt uns die AnimInstance vom Pawn
}
