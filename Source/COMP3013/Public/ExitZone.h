// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ExitZone.generated.h"

UCLASS()
class COMP3013_API AExitZone : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExitZone();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Bounds Box
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoundsBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void onInteract() override;

	UFUNCTION()
	void BoundBoxEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
