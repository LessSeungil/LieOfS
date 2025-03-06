// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"
#include "Animation/ABAnimInstance.h"
#include "LieOfS/UI/ABHUDWidget.h"
#include "LieOfS/CharacterStat/ABCharacterStatComponent.h"
#include "LieOfS/Interface/ABGameInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LieOfS/Physics/ABCollision.h"
#include "Kismet/KismetMathLibrary.h"
#include "LieOfS/Character/ABCharacterNonPlayer.h"
#include "Particles/ParticleSystemComponent.h"
#include "Attribute/ABCharacterAttributeSet.h"
#include "GA/ABGA_Damaged.h"
#include "Player/ABPlayerState.h"
#include "AbilitySystemBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(MyLogCategory);

AABCharacterPlayer::AABCharacterPlayer()
{
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputLockOnActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_LockOn.IA_LockOn'"));
	if (nullptr != InputLockOnActionRef.Object)
	{
		LockOnAction = InputLockOnActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputShieldActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Shield.IA_Shield'"));
	if (nullptr != InputShieldActionRef.Object)
	{
		ShieldAction = InputShieldActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShieldMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_Shield.AM_Shield'"));
	if (nullptr != ShieldMontageRef.Object)
	{
		ShieldMontage = ShieldMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DodgeMontageRef(TEXT("/ Script / Engine.AnimMontage'/Game/Animation/AM_Dodge.AM_Dodge'"));
	if (nullptr != DodgeMontageRef.Object)
	{
		DodgeMontage = DodgeMontageRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputRollingActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Roll.IA_Roll'"));
	if (nullptr != InputRollingActionRef.Object)
	{
		RollingAction = InputRollingActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputDodgeActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Dodge.IA_Dodge'"));
	if (nullptr != InputDodgeActionRef.Object)
	{
		DodgeAction = InputDodgeActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlendSpace> DodgeBlendSpaceActionRef(TEXT("/Script/Engine.BlendSpace'/Game/Animation/Roll.Roll'"));
	if (nullptr != DodgeBlendSpaceActionRef.Object)
	{
		DodgeBlendSpace = DodgeBlendSpaceActionRef.Object;
	}
	
	

	ShieldEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ShieldEffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ArenaBattle/Effect/P_ImpactSpark.P_ImpactSpark'"));
	if (ShieldEffectRef.Object)
	{
		ShieldEffect->SetTemplate(ShieldEffectRef.Object);
		ShieldEffect->bAutoActivate = false;
		ShieldEffect->SetupAttachment(GetMesh(), TEXT("EffectSocket"));
	}

	CurrentCharacterControlType = ECharacterControlType::Shoulder;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	AttributeSet = CreateDefaultSubobject<UABCharacterAttributeSet>(TEXT("HealthSet"));
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl(CurrentCharacterControlType);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (DodgeMontage && AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AABCharacterPlayer::DodgeEnd);
	}

}

void AABCharacterPlayer::SetDead()
{
	Super::SetDead();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);


		IABGameInterface* ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode());
		if (ABGameMode)
		{
			ABGameMode->OnPlayerDead();
		}
	}
}

void AABCharacterPlayer::Tick(float DeltaTime)
{
	if (bLockOn)
	{
		LookAtTarget(DeltaTime);
	}

	if(bRolling)
	{
		AddMovementInput(RolingFowardDirection, 300.f * MovementVector.X);
		AddMovementInput(RolingRightDirection, 300.f * MovementVector.Y);

		//GetCharacterMovement()->AddImpulse(FVector(MovementVector.X,MovementVector.Y,0.f) * 200.f, true);
		//LaunchCharacter(FVector(MovementVector.X,MovementVector.Y,0.f) * 10.f, false, false);
	}
}

void AABCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->SetStat(false);
}

void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
	EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::LockOn);
	EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::SwitchWeapon);
	EnhancedInputComponent->BindAction(ShieldAction, ETriggerEvent::Canceled, this, &AABCharacterPlayer::ShieldEnd);
	EnhancedInputComponent->BindAction(ShieldAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Shield);
	EnhancedInputComponent->BindAction(RollingAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Rolling);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Dodge);
	
}

float AABCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bShield)
	{

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->StopAllMontages(0.f);
			AnimInstance->Montage_Play(ShieldMontage, 1.f);
			DisableInput(Cast<APlayerController>(GetController()));

			if (ShieldEffect)
				ShieldEffect->Activate();

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AABCharacterPlayer::ShieldEndEnableInput);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, ShieldMontage);

		}

		if (bPerfectParring)
		{
			GetWorld()->GetWorldSettings()->SetTimeDilation(0.5f);
			
			GetWorld()->GetTimerManager().SetTimer(GameDelayTimeHandle, this, &AABCharacterPlayer::GameDelayNormal, 1.f, false);

		}
		else
		{
			Super::TakeDamage(FMath::Max(DamageAmount - 10, 10), DamageEvent, EventInstigator, DamageCauser);
		}
	}
	else
	{
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}

	return 0.0f;
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
			if (NewMappingContext)
			{
				Subsystem->AddMappingContext(NewMappingContext, 0);
			}
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	CameraBoom->SocketOffset = CharacterControlData->SocketOffset;
}

UAbilitySystemComponent* AABCharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void AABCharacterPlayer::PossessedBy(AController* NewController)
{
	// AABPlayerState* playerState = GetPlayerState<AABPlayerState>();
	// if(playerState != nullptr)
	// {
	// 	ASC = playerState->GetAbilitySystemComponent();
	// 	ASC->InitAbilityActorInfo(playerState, this);
	//
	// 	for(const auto& StartAbility : StartAbilities)
	// 	{
	// 		FGameplayAbilitySpec StartSpec(StartAbility);
	// 		ASC->GiveAbility(StartSpec);
	// 	}
	// }
	
	ASC->InitAbilityActorInfo(this, this);
	 for(const auto& StartAbility : StartAbilities)
	 {
	 	FGameplayAbilitySpec StartSpec(StartAbility);
	 	ASC->GiveAbility(StartSpec);
	 }
	
	// AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
	//
	// FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	// EffectContextHandle.AddSourceObject(this);
	/*FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);
	if (EffectSpecHandle.IsValid())
	{
		ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}*/
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	if(bRolling || bDodge)
	{
		return;
	}
	
	MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	RolingFowardDirection = ForwardDirection;
	RolingRightDirection = RightDirection;
	
	AddMovementInput(ForwardDirection, 200.f *MovementVector.X);
	AddMovementInput(RightDirection, 200.f *MovementVector.Y);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}

void AABCharacterPlayer::LockOn()
{
	if (bLockOn)
	{
		LockOff();
	}
	else
	{
		TArray<FHitResult> OutHitResult;
		FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;

		const FVector LockOnBox = FVector(600.f, 600.f, 300.f);
		
		const FVector CameraForwardVector = FollowCamera->GetForwardVector();
		const FVector Start = GetActorLocation() + CameraForwardVector * (GetCapsuleComponent()->GetScaledCapsuleRadius() + (LockOnBox.X * 0.5f));
		const FVector End = Start + CameraForwardVector * LockOnBox.X;
		FVector BoxOrigin = Start + (End - Start) * 0.5f;

		bool HitDetected = GetWorld()->SweepMultiByChannel(OutHitResult, BoxOrigin, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeBox(LockOnBox), Params);
		if (HitDetected)
		{
			for (auto& Hit : OutHitResult)
			{
				LockOnActor = Hit.GetActor();
				AABCharacterNonPlayer* Npc = Cast<AABCharacterNonPlayer>(LockOnActor);
				if (Npc != nullptr)
				{
					bLockOn = true;
					GetCharacterMovement()->bOrientRotationToMovement = false;
					break;
				}
					
			}
			/*AABCharacterBase* NPCCharacter = Cast<AABCharacterBase>(LockOnActor);
			if (NPCCharacter)
			{
				NPCCharacter->SetVisibleHpBar(true);
			}*/
		}

		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

		DrawDebugBox(GetWorld(), BoxOrigin, LockOnBox, DrawColor, false, 2.f);
	}
}

void AABCharacterPlayer::LockOff()
{
	// ���� ���� ����
	bLockOn = false;
	/*AABCharacterBase* NPCCharacter = Cast<AABCharacterBase>(LockOnActor);
	if (NPCCharacter)
	{
		NPCCharacter->SetVisibleHpBar(false);
	}*/

	LockOnActor = nullptr;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AABCharacterPlayer::LookAtTarget(float DeltaSeconds)
{
	if (bDodge)
		return;

	if (LockOnActor != nullptr)
	{
		FVector LockEdOnLocation = LockOnActor->GetActorLocation();
		LockEdOnLocation.Z = 100.f;

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockEdOnLocation);
		GetController()->SetControlRotation(FRotator(LookAtRotation.Pitch, LookAtRotation.Yaw, GetController()->GetControlRotation().Roll));
		
		const FRotator PawnInterpRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, GetControlRotation().Yaw, GetActorRotation().Roll), DeltaSeconds, 10.f);
		GetController()->GetPawn()->SetActorRotation(PawnInterpRotation);
	}
	
}

void AABCharacterPlayer::SwitchWeapon()
{
}

void AABCharacterPlayer::Shield()
{
	bShield = true;

	if (bPerfectParringSetting == false)
	{
		bPerfectParringSetting = true;
		// 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(PerfectParringTimeHandle, this, &AABCharacterPlayer::PerfectParringEnd, 1.5f, false);

		bPerfectParring = true;
	}
}

void AABCharacterPlayer::ShieldEnd()
{
	bShield = false;
	bPerfectParringSetting = false;
}

void AABCharacterPlayer::Rolling()
{
	if(DodgeBlendSpace)
	{
		UABAnimInstance* AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
		if(AnimInstance)
		{
			AnimInstance->bIsRolling = true;
			AnimInstance->RollingVector = FVector(GetVelocity().X,GetVelocity().Y,0.f);
			AnimInstance->RollingRotVector = GetActorRotation();

			bRolling = true;
			
			DisableInput(Cast<APlayerController>(GetController()));
			
			float AnimTime = DodgeBlendSpace->AnimLength;
			AnimTime-=0.1f;
			GetWorld()->GetTimerManager().SetTimer(RollingTimeHandle, this, &AABCharacterPlayer::RollingEnd, AnimTime, false);

			if(ASC != nullptr)
			{
				/*FGameplayAbilitySpec* DamagedGASpec = ASC->FindAbilitySpecFromClass(UABGA_Damaged::StaticClass());
				if (DamagedGASpec)
				{
					ASC->TryActivateAbility(DamagedGASpec->Handle);	
				}

				FGameplayTagContainer TargetTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Damaged")));
				ASC->TryActivateAbilitiesByTag(TargetTag);*/

				//FGameplayEventData PayloadData;
				//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(FName("Character.State.Damaged")), PayloadData);

				FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
				EffectContext.AddSourceObject(this);
				FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
				if (EffectSpecHandle.IsValid())
				{
					ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
				}
			}
			
		}
	}
	

	// UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// if (AnimInstance)
	// {
	// 	AnimInstance->StopAllMontages(0.f);
	// 	AnimInstance->Montage_Play(DodgeMontage, 1.f);
	// 	DisableInput(Cast<APlayerController>(GetController()));
	//
	// 	FOnMontageEnded EndDelegate;
	// 	EndDelegate.BindUObject(this, &AABCharacterPlayer::DodgeEndEnableInput);
	// 	AnimInstance->Montage_SetEndDelegate(EndDelegate, DodgeMontage);
	// }
}

void AABCharacterPlayer::RollingEnd()
{
	UABAnimInstance* AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	if(AnimInstance)
	{
		AnimInstance->bIsRolling = false;
	}

	bRolling = false;

	EnableInput(Cast<APlayerController>(GetController()));
}

void AABCharacterPlayer::Dodge()
{
	bDodge = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		//방향에 따라 섹션 이름 설정
		float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(MovementVector.X, MovementVector.Y));
		UE_LOG(LogTemp, Warning, TEXT("AngleDeg: %f"), AngleDeg);
		// 0 ~ 360
		AngleDeg = FMath::Fmod(AngleDeg + 360.0f, 360.0f);
		UE_LOG(LogTemp, Warning, TEXT("AngleDeg: %f"), AngleDeg);
		// 45° 단위로 8방향을 판별 > RoundToInt 로 인해서 % 에 대한 구분짓기가 가능해짐
		int32 DirectionIndex = FMath::RoundToInt(AngleDeg / 45.0f) % 8;
		
		

		const TArray<FString> Directions = {
		"Right",
		"FRight",
		"Forward",
		"FLeft",
		"Left",
		"BLeft",
		"Back",
		"BRight"
		};

		if (AnimInstance->Montage_IsPlaying(DodgeMontage))
		{
			AnimInstance->Montage_JumpToSection(FName(*Directions[DirectionIndex]), DodgeMontage);
		}
		else
		{
			AnimInstance->Montage_Play(DodgeMontage);
			AnimInstance->Montage_JumpToSection(FName(*Directions[DirectionIndex]), DodgeMontage);
		}
	}
}

void AABCharacterPlayer::DodgeEnd(UAnimMontage* Montage, bool bInterrupted)
{
	bDodge = false;
}

void AABCharacterPlayer::PerfectParringEnd()
{
	bPerfectParring = false;
}

void AABCharacterPlayer::LeadWeapon()
{
}

void AABCharacterPlayer::UnLeadWeapon()
{
}

void AABCharacterPlayer::SetupHUDWidget(UABHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		InHUDWidget->UpdateHpBar(this);

		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStat);
	}
	
}

void AABCharacterPlayer::ShieldEndEnableInput(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	EnableInput(Cast<APlayerController>(GetController()));
}


void AABCharacterPlayer::GameDelayNormal()
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.f);
}

void AABCharacterPlayer::OnOutOfHealth()
{
	SetDead();
}
