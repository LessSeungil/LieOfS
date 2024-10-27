// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_AttackInRange.h"
#include "ABAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LieOfS/Interface/ABCharacterAIInterface.h"
#include "Kismet/KismetMathLibrary.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return false;
	}

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == Target)
	{
		return false;
	}

	float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	float AttackRangeWithRadius = AIPawn->GetAIAttackRange();
	bResult = (DistanceToTarget <= AttackRangeWithRadius);

	if (bResult == false)
	{
		FVector TargetPos = Target->GetTransform().GetLocation();
		FVector AIPos = ControllingPawn->GetTransform().GetLocation();

		FVector Dir = UKismetMathLibrary::GetDirectionUnitVector(AIPos,TargetPos);
		FVector CalPos = AIPos + (Dir * 200.f);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_FOLLOWPOS, CalPos);
	}

	return bResult;
}
