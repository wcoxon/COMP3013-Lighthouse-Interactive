// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperCharacter.h"
#include "Game_Spawner.generated.h"


UCLASS()
class COMP3013_API AGame_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGame_Spawner();

	//Character to spawn
	UPROPERTY(EditAnywhere)
	TSubclassOf<APaperCharacter> Character;

	UPROPERTY(EditAnywhere)
	float PlayerSize = 1;
	UPROPERTY()
	int CustomerCount;

	float customerSpawnCooldown;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	USceneComponent* SceneComponent = GetRootComponent();
	
	AController* SavedController;
};
