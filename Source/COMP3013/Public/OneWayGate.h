// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "OneWayGate.generated.h"

UCLASS()
class COMP3013_API AOneWayGate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOneWayGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Root  = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));

	UPROPERTY(EditAnywhere)
	AActor* Player;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollider;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxTrigger;
	
	FVector ColliderSize;
	FVector TriggerSize;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GateLeft;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GateRight;
	
	


};
