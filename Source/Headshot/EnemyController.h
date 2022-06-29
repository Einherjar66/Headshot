// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class HEADSHOT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyController();
	virtual void OnPossess(APawn* InPawn) override;

private:

	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComp;
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComp;

public:

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComp; }

};
