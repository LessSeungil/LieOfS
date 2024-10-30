// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"
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

	ShieldEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ShieldEffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ArenaBattle/Effect/P_ImpactSpark.P_ImpactSpark'"));
	if (ShieldEffectRef.Object)
	{
		ShieldEffect->SetTemplate(ShieldEffectRef.Object);
		ShieldEffect->bAutoActivate = false;
		ShieldEffect->SetupAttachment(GetMesh(), TEXT("EffectSocket"));
	}

	CurrentCharacterControlType = ECharacterControlType::Shoulder;
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

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
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
	// �̹� ���� ���¿��� �� Ȯ��
	if (bLockOn)
	{
		LockOff();
	}
	else
	{
		TArray<FHitResult> OutHitResult;
		FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;

		const FVector LockOnBox = FVector(600.f, 600.f, 300.f);
		// ���� ī�޶� ���� �������� �ڽ� ����
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
}

void AABCharacterPlayer::ShieldEnd()
{
	bShield = false;
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
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());

		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateHpBar);
	}
	
}

void AABCharacterPlayer::ShieldEndEnableInput(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	EnableInput(Cast<APlayerController>(GetController()));
}
