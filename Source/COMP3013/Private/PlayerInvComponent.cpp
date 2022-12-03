// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInvComponent.h"

// Sets default values for this component's properties
UPlayerInvComponent::UPlayerInvComponent()
{
	Capacity = 16;
	

}

bool UPlayerInvComponent::AddItem(UItem* Item)
{
	if (Items.Num() < Capacity && Item)
	{
		Item->World = GetWorld();
		Item->Inventory = this;
		Items.Add(Item);
		InventoryUpdate.Broadcast();
		return true;
	}
	return false;
}

bool UPlayerInvComponent::RemoveItem(UItem* Item)
{
	if (Item)
	{
		Item->World = nullptr;
		Item->Inventory = nullptr;
		Items.RemoveSingle(Item);
		InventoryUpdate.Broadcast();
		return true;
	}
	return false;
}


// Called when the game starts
void UPlayerInvComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

