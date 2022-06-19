#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachinGun UMETA(DisplayName = "SubmachinGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	EWT_Pistol		 UMETA(DisplayName = "Pistol"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};