// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game_PaperCharacter.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorMesh")
	float DoorOpenAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorMesh")
	float DoorCloseAngle;
	
	UFUNCTION()
	void OnFSusMaxBroadcast();

	UFUNCTION()
	void OnFEGCOverBroadcast();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AGame_PaperCharacter* player;

};
