// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EntryDoors.generated.h"

UCLASS()
class COMP3013_API AEntryDoors : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntryDoors();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorMesh")
	UStaticMeshComponent* DoorMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
