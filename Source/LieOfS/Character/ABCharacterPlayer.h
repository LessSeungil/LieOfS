// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABCharacterBase.h"
#include "InputActionValue.h"
#include "LieOfS/Interface/ABCharacterHUDInterface.h"
#include "ABCharacterPlayer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MyLogCategory, Log, All);

/**
 * 
 */
UCLASS()
class LIEOFS_API AABCharacterPlayer : public AABCharacterBase, public IABCharacterHUDInterface
{
	GENERATED_BODY()
	
public:
	AABCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void SetDead() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

// Character Control Section
protected:
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const class UABCharacterControlData* CharacterControlData) override;

// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShieldAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SwitchWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RollingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DodgeAction;
	

// Anim Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ShieldMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DodgeMontage;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	TObjectPtr<class UParticleSystemComponent> ShieldEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBlendSpace> DodgeBlendSpace;

	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);

	ECharacterControlType CurrentCharacterControlType;

	void Attack();

	// LockOn Section
	void LockOn();
	void LockOff();
	void LookAtTarget(float DeltaSeconds);

	// Equip Section
	void SwitchWeapon();

	void Shield();
	void ShieldEnd();

public:
	void Rolling();
	void RollingEnd();

	void Dodge();
	UFUNCTION()
	void DodgeEnd(UAnimMontage* Montage, bool bInterrupted);

	void PerfectParringEnd();

	// Į �ִ� ���
	void LeadWeapon();
	void UnLeadWeapon();
public:
	bool GetIsLockOn() { return bLockOn; }
	bool GetIsShield() {
		return bShield;
	}

	bool bLockOn = false;
	bool bShield = false;

	bool bRolling = false;
	FVector RolingFowardDirection;
	FVector RolingRightDirection;

	FVector2D MovementVector;

	bool bDodge = false;

	bool bPerfectParringSetting = false;
	bool bPerfectParring = false;

	AActor* LockOnActor = nullptr;

	FTimerHandle PerfectParringTimeHandle;

	FTimerHandle GameDelayTimeHandle;

	FTimerHandle RollingTimeHandle;

// UI Section
protected:
	virtual void SetupHUDWidget(class UABHUDWidget* InHUDWidget) override;

private:
	void ShieldEndEnableInput(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void GameDelayNormal();
};
