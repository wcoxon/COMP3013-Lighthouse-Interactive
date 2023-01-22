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

	//flavour
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (MultiLine = true))
	FText Description;

	//properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (ClampMin = 0.0))
	float Price;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (ClampMin = 0.0))
	float Weight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data", meta = (ClampMin = 0.0))
	float Duration;

	//visuals
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	UStaticMesh* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	UTexture2D* Icon;

};
