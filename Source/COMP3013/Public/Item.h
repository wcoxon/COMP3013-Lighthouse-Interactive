// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class COMP3013_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UItem();

	virtual class UWorld* GetWorld() const {return World;};

	UPROPERTY(Transient)
	UWorld* World;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (ClampMin = 0.0))
	float Value;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (ClampMin = 0.0))
	float Weight;

	UPROPERTY()
	class UPlayerInvComponent* Inventory;

	virtual void Destroy(class AGame_PaperCharacter* Character) PURE_VIRTUAL (UItem);
};
