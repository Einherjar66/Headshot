// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy.h"

AEnemyController::AEnemyController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	check(BlackboardComp);

	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	check(BehaviorTreeComp);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn == nullptr) return;
	
	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			BlackboardComp->InitializeBlackboard(*(Enemy->GetBehaviorTree()->GetBlackboardAsset()));
		}
	}
}
