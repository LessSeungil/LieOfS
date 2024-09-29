// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget.h"
#include "UI/WeaponWidget.h"
#include "Components/TileView.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (size_t i = 0; i < 10; i++)
	{
		UWeaponData* Data = NewObject<UWeaponData>();
		TV_Weapon->AddItem(Data);
	}
}
