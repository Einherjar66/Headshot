// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))		// Ammo cout for this Weapon
	int32 Ammo;

public:

	void ThrowWeapon();			// Add a Impulse to the weapon
	void DecrementAmmo();		// Called from Character class when firing weapon

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
};
