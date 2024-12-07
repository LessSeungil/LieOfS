// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_RollEnd.h"

#include "Character/ABCharacterPlayer.h"

void UAnimNotify_RollEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AABCharacterPlayer* AttackPawn = Cast<AABCharacterPlayer>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->RollingEnd();
		}
	}
}
