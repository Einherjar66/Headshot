// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class HEADSHOT_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:
	AAmmo();
	
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	virtual void SetItemProperties(EItemState State) override;	// Override for AmmoMesh Properties

private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Ammo", meta = (AllowPrivateAccess = "true"))	// Mesh for the ammo pickup
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))		// Ammotype for the ammo
	EAmmoType AmmoType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))	// The Texrue for the Ammo
	UTexture2D* AmmoIconTexture;
public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
};
