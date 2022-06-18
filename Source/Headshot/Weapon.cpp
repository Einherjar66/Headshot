// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon() :
	ThrowWeaponTime(.7f),
	bFalling(false),
	Ammo(30),
	MagazineCapacity(30),
	WeaponType(EWeaponType::EWT_SubmachinGun),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadMontageSection(FName(TEXT("ReloadSMG"))),
	ClipBoneName(FName(TEXT("smg_clip")))
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}


bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	
	// Direction in which we throw the weapon
	FVector ImpulsDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	ImpulsDirection = ImpulsDirection.RotateAngleAxis(FMath::FRandRange(0.f, 30.f), FVector(0.f, 0.f, 1.f));
	ImpulsDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulsDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
	EnableGlowMaterial();
}

void AWeapon::DecrementAmmo()
{
	if (Ammo -1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::RelaodAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity,TEXT("Attempted to reload with mor than magazine capacity"))
	Ammo += Amount;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString WeaponPath{ TEXT("DataTable'/Game/_Game/DataTable/WeaponDataTable.WeaponDataTable'") };								// Der Pfard wo der DataTable zu finden ist
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponPath));	// 
	
	if (WeaponDataTable)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;

		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachinGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
			break;

		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;
		}
		if (WeaponDataTable)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazinCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetIconItem(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);
		}
	}
}
