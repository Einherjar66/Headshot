// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged		UMETA(DisplayName = "Damaged"),
	EIR_Common		UMETA(DisplayName = "Common"),
	EIR_Uncommon	UMETA(DisplayName = "Uncommon"),
	EIR_Rare		UMETA(DisplayName = "Rare"),
	EIR_Legendary	UMETA(DisplayName = "Legendary"),

	EIR_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup			UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping	UMETA(DisplayName = "EquipInterping"),
	EIS_Pickedup		UMETA(DisplayName = "Pickedup"),
	EIS_Equipped		UMETA(DisplayName = "Equipped"),
	EIS_Falling			UMETA(DisplayName = "Falling"),

	EIS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
}; 

UCLASS()
class HEADSHOT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called in AShooterCharacter::GetPickupItem
	void PlayEquipSound();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when overlapping AreaSpere
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Called when end overlapping AreaSpere
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Sets ActiveStars array of bools based on rarity
	void SetActiveStars();

	// Sets properties of the Item's component based on State
	virtual void SetItemProperties(EItemState State);

	// Called when ItemInterpTimer is finished
	void FinishIterping();

	// Handles item interpolation when in the EquipInterping state 
	void ItemInterp(float DealtaTime);

	FVector GetInterpLocation();
	void PlayPickupSound();
	void EnableGlowMaterial();
	void UpdatePulse();
	void ResetPulseTimer();
	void StartPulseTimer();

	virtual void InitializeCustomDepth();
	virtual void OnConstruction(const FTransform& Transform) override;

private:

	/**
	 *  class Pointer
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Skeletal Mesh for the item
	class USkeletalMeshComponent* ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Line trace collides with box to show HUD widgets
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Popup widget for the player when looks at the item
	class UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Enables item tracing when overlapped
	class USphereComponent* AreaSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// The curve asset to use for  the item's Z location when interping
	class UCurveFloat* ItemZCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// curve used to scale the item when interping
	UCurveFloat* ItemScaleCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// pointer to the Character
	class AShooterCharacter* Character;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Sound played when Item is picked up
	class USoundCue* PickupSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Sound played when the Item is equipped
	USoundCue* EquipSound;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Dynamic instance that we can change at runtime
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Material instance used with the Dynamic Material instance
	UMaterialInstance* MaterialInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Curve to drive the dynamic material parameters
	class UCurveVector* PulseCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Curve to drive the dynamic material parameters
	UCurveVector* InterpPulseCurve;

	/**
	 * Variables
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// The name which on the Pickup Widget
	FString ItemName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Item count (Ammo, etc.)
	int32 ItemCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Item rarity - determines number of stars in Pickup widget
	EItemRarity ItemRartiy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// State of the Item
	EItemState ItemState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Starting location when interping begins
	FVector ItemInterpStartLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Target interp location in front of the camera
	FVector CameraTargetLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// 
	TArray<bool> ActiveStars;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// True when interping
	bool bInterping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Duration of the curve and timer
	float ZCurveTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Item Properties", meta = (AllowPrivateAccess = "true"))		// Enum for the type of item this Item is
	EItemType ItemType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Index of the interp location this intem is interping to
	int32 InterpLocIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))	// Time for the PulseTimer
	float PulseCurveTime;
	UPROPERTY(VisibleAnywhere,Category = "Item Properties", meta = (AllowPrivateAccess = "true"))						// 
	float GlowAmount;
	UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))						// 
	float FresnelExponent;
	UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))						// 
	float FresnelReflectFraction;

	FTimerHandle ItemInterpTimer; // Plays when we start interping
	FTimerHandle PulseTimer;
	bool bCanChangeCustomDepth;

	// X and Y for the Item while interping in the EquipInterping state
	float ItemInterpX;	
	float ItemInterpY;

	// Initial Yaw offset between the camera and the interping item
	float InterpInitialYawOffset;

public:

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	void SetItemState(EItemState State);
	void StartItemCurve(AShooterCharacter* Char); // Called from the AShooterCharacter class
	virtual void EnableCustomDepht();
	virtual void DisableCustomDepht();
	void DisableGlowMaterial();
};
