// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequence/DoorTrigger.h"
#include "Components/BoxComponent.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

ADoorTrigger::ADoorTrigger()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	// Ʈ���� �ڽ� ����
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(30.f, 30.f, 30.f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = Trigger;

	// ����Ʈ ���� ������ ����
	LevelSequence = CreateDefaultSubobject<ULevelSequence>(TEXT("LevelSequence"));

	// ������ �ҷ��ͼ� ����
	static ConstructorHelpers::FObjectFinder<ULevelSequence> LS(TEXT("/Script/LevelSequence.LevelSequence'/Game/Sequence/TestLevelSequence.TestLevelSequence'"));
	if (LS.Succeeded())
	{
		LevelSequence = LS.Object;
	}

	LevelSequenceActor = CreateDefaultSubobject<ALevelSequenceActor>(TEXT("LevelSequenceActor"));
}

void ADoorTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;
	Settings.bPauseAtEnd = true;
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, Settings, LevelSequenceActor);
}


void ADoorTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (LevelSequencePlayer)
	{
		LevelSequencePlayer->Play();
	}
}

void ADoorTrigger::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (LevelSequencePlayer)
	{
		LevelSequencePlayer->PlayReverse();
	}
}

