// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"
UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class HEADSHOT_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:
	void  TurnInPlace();	// Handle turning in place variables
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	/**
	 * Variables
	 */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Speed of the Character
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Whether or not the character is in the air
	bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Whether or not the character is Moving
	bool bIsAccelerating;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Offset Yaw used for strafing 
	float MovementOffsetYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Offset yaw the frame before we stopped moving
	float LastMovementOffsetYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// 
	bool bAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) // The pitch of the aim rotaiton, used for Aim Offset 
	float Pitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) // True when reloading, used to prevent Aim Offset while reloading
	bool bIsReloading;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) // Offset state ; use to detemine which Aim Offset to use
	EOffsetState OffsetState;

	float CharacterYaw;				// Yaw of the Character this frame
	float CharacterYawLastFrame;	// Yaw of the Character the previous frame

	float RotationCurve;
	float RotationCurveLastFrame;

};
