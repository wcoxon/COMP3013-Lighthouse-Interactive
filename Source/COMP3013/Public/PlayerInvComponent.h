// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Components/ActorComponent.h"
#include "PlayerInvComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMP3013_API UPlayerInvComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInvComponent();

	bool AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<UItem*> BaseItems;
	
	UPROPERTY(EditDefaultsOnly)
	int Capacity;

	UPROPERTY(BlueprintAssignable)
	FInventoryUpdated InventoryUpdate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UItem*> Items;
	
};
