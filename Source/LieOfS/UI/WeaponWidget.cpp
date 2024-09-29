// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponWidget.h"
#include "UI/LSDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UWeaponWidget::NativeConstruct()
{
	/*static ConstructorHelpers::FClassFinder<UWeaponWidget> WeaponWidgetRef(TEXT("/Game/Blueprint/UI/WeaponUI.WeaponUI_C"));
	if (WeaponWidgetRef.Class)
	{
		WeaponWidgetClass = WeaponWidgetRef.Class;
	}*/
}


void UWeaponWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{



}

void UWeaponWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (OutOperation == nullptr)
	{
		ULSDragDropOperation* Dargoperation = NewObject<ULSDragDropOperation>();
		/*Dargoperation->DefaultDragVisual = this;
		Dargoperation->Pivot = EDragPivot::MouseDown;*/

		UWeaponWidget* Widget = CreateWidget<UWeaponWidget>(GetWorld(), WeaponWidgetClass);
		Dargoperation->DefaultDragVisual = Widget;
		Dargoperation->Pivot = EDragPivot::MouseDown;
		OutOperation = Dargoperation;
	}

}

bool UWeaponWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	return false;
}

FReply UWeaponWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		FEventReply replay = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this, EKeys::LeftMouseButton);
		return replay.NativeReply;
	}


	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
