// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
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
	UStaticMesh* ItemMesh;
	
	//Bounds Box
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoundsBox;

	//Bounds Box
	UPROPERTY(EditAnywhere)
	FString ItemName = "";

	UPROPERTY(EditAnywhere)
	UTextRenderComponent* Text;
	
	virtual void onInteract() override;

	UFUNCTION()
	void TextOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TextOverlapExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	FString GetItemName();
	UStaticMesh* GetItemMesh();
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
