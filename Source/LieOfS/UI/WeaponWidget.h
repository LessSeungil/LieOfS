// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WeaponWidget.generated.h"

UCLASS()
class LIEOFS_API UWeaponData : public UObject
{
	GENERATED_BODY()
	public:
		int Index = 0;
};


/**
 * 
 */
UCLASS()
class LIEOFS_API UWeaponWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UWeaponWidget> WeaponWidgetClass;
};
