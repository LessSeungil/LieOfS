// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UTileView;
/**
 * 
 */
UCLASS()
class LIEOFS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct()  override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TV_Weapon;
};
