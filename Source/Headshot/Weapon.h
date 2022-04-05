// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachinGun UMETA(DisplayName = "SubmachinGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};


/**
 * 
 */
UCLASS()
class HEADSHOT_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

protected:

	void StopFalling();

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// True when moving the clip while reloading
	bool bMovingClip;				

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// Ammo cout for this Weapon
	int32 Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// Maximum ammo that our weapon can hold
	int32 MagazineCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// The type of the weapon
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// The Type of ammo for this weapon
	EAmmoType AmmoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// FName for the reload montage section
	FName ReloadMontageSection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// Name for the clip Bone
	FName ClipBoneName;
public:

	void ThrowWeapon();				// Add a Impulse to the weapon
	void DecrementAmmo();			// Called from Character class when firing weapon
	void RelaodAmmo(int32 Amount);

	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

	FORCEINLINE EWeaponType GetWeaponType()		const { return WeaponType; }
	FORCEINLINE int32 GetAmmo()					const { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity()		const { return MagazineCapacity; }
	FORCEINLINE EAmmoType GetAmmoType()			const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE FName GetClipBoneName()			const { return ClipBoneName; }
};
