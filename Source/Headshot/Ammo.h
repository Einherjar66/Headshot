// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
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


public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
};
