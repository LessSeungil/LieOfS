// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABItemData.h"
#include "../GameData/ABCharacterStat.h"
#include "ABWeaponItemData.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Sword,
	Shield,
};

/**
 * 
 */
UCLASS()
class LIEOFS_API UABWeaponItemData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABWeaponItemData();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());
	}
	
public:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<UStaticMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;

	UPROPERTY(EditAnywhere, Category = Socket)
	FString SocketName;

	UPROPERTY(EditAnywhere, Category = WeaponType)
	EWeaponType WeaponType;
	
};
