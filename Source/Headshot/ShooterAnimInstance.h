// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
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
	void  TurnInPlace();			// Handle turning in place variables
	void Lean(float DeltaTime);		// Handle Calculation for leaning while running
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	/**
	 * Variables
	 */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))		// Speed of the Character
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))		// Whether or not the character is in the air
	bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))		// Whether or not the character is Moving
	bool bIsAccelerating;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))		// Offset Yaw used for strafing 
	float MovementOffsetYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))		// Offset yaw the frame before we stopped moving
	float LastMovementOffsetYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))		// 
	bool bAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) //
	float RootYawOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) // The pitch of the aim rotaiton, used for Aim Offset 
	float Pitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))			// Yaw delta used for leaning in the running blendspace
	float YawDelta;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) // True when reloading, used to prevent Aim Offset while reloading
	bool bIsReloading;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crouch", meta = (AllowPrivateAccess = "true"))		// true when Crouching
	bool bCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reaload", meta = (AllowPrivateAccess = "true"))		// true when Equipping
	bool bEquipping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Change the recoil weight based on turning in place aiming
	float RecoilWeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// True when tufning in place
	bool bTurningInPlace;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Weapon type for the currently equipped weapon
	EWeaponType EquippedWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))		// Ture when not realoding or quippuing 
	bool bShouldUseFABRIK;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true")) // Offset state ; use to detemine which Aim Offset to use
	EOffsetState OffsetState;

	float TIPCharacterYaw;				// Yaw of the Character this frame; Only updated when standing still and not in air 
	float TIPCharacterYawLastFrame;		// Yaw of the Character the previous frame; Only updated when standing still and not in air 

	FRotator CharacterRotation;					// Yaw of the Character this frame
	FRotator CharacterRotationLastFrame;		// Yaw of the Character the previous frame

	float RotationCurve;
	float RotationCurveLastFrame;

};
