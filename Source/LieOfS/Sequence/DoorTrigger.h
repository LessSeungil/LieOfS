// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorTrigger.generated.h"

UCLASS()
class LIEOFS_API ADoorTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoorTrigger();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
		class ALevelSequenceActor* LevelSequenceActor;

		UPROPERTY(VisibleAnywhere)
		class ULevelSequence* LevelSequence;

		UPROPERTY(VisibleAnywhere)
		class ULevelSequencePlayer* LevelSequencePlayer;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
