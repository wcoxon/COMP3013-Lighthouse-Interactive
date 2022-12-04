// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Item_Base.generated.h"

UCLASS()
class COMP3013_API AItem_Base : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Base();

	UPROPERTY(EditAnywhere)
	UItem* ItemToGive;
	
	//Bounds Box
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoundsBox;

	UPROPERTY(EditAnywhere)
	UTextRenderComponent* Text;
	
	virtual void onInteract() override;

	UFUNCTION()
	void TextOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TextOverlapExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
