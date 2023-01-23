// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Agent_PaperCharacter.h"
#include "Game_PaperCharacter.h"
#include "PaperCharacter.h"
#include "NavigationSystem.h"
#include "Components/SpotLightComponent.h"
#include "Containers/Array.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationPath.h"
#include "VisionConeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NPCBase.generated.h"

/**
 * 
 */
UENUM()
enum AIState
{
	patrol,
	idle,
	pursue,
	search,
	stare,
	tattle
};

UCLASS()
class COMP3013_API ANPCBase : public AAgent_PaperCharacter
{
	GENERATED_BODY()

public:
	ANPCBase();
	
	UFUNCTION()
	void pathToTarget(FVector destination);

	void turnTowards(FVector destination,float deltaSec);
	void moveTowards(FVector destination,float deltaSec) override;
	
	void followPath(float deltaSec);
	
	UFUNCTION()
	void setState(AIState state);
	
	UFUNCTION()
	void playerCrimeCommitted();
	
	UPROPERTY(VisibleAnywhere)
	UVisionConeComponent* visionCone;
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	float turnSpeed;
	
	AIState currentState;
	TArray<FVector> patrolPoints;
	
	UPROPERTY()
	AGame_PaperCharacter* player;
	
	UPROPERTY()
	UNavigationPath *tpath;
	
	
};
