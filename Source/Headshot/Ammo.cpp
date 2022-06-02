// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ShooterCharacter.h"

AAmmo::AAmmo()
{
	// Constuct the mesh component and set it as the root
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("AmmoMesh"));
	// Ein RootComponent bestimmen
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());

	// Create AmmoCollisionSphere
	AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>(FName("AmmoCollisionSphere"));
	AmmoCollisionSphere->SetupAttachment(GetRootComponent());
	AmmoCollisionSphere->SetSphereRadius(50.f);


}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	// Die AmmoCollisionSphere mit der AmmoShpereOverlap funktion verknüpfen.
	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoShpereOverlap);

}

void AAmmo::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);
	switch (State)
	{
	case EItemState::EIS_Pickup:

		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_EquipInterping:

		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Equipped:

		GetPickupWidget()->SetVisibility(false);
		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:

		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		break;

	default:
		break;
	}
}

void AAmmo::AmmoShpereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 
	if (OtherActor)
	{
		auto OverlapCharacter = Cast<AShooterCharacter>(OtherActor);

		if (OverlapCharacter)
		{
			StartItemCurve(OverlapCharacter);
			AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AAmmo::EnableCustomDepht()
{
	AmmoMesh->SetRenderCustomDepth(true);
}

void AAmmo::DisableCustomDepht()
{
	AmmoMesh->SetRenderCustomDepth(false);
}
