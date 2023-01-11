// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Agent_PaperCharacter.h"
#include "Game_PaperCharacter.h"
#include "PaperCharacter.h"
#include "NavigationSystem.h"
#include "Components/SpotLightComponent.h"
#include "NPCBase.generated.h"

/**
 * 
 */

enum AIState
{
	patrol,
	idle,
	alerted,
	searching
	
};

UCLASS()
class COMP3013_API ANPCBase : public AAgent_PaperCharacter
{
	GENERATED_BODY()

public:
	ANPCBase();
	virtual void Tick(float DeltaSeconds) override;
	bool detectsPlayer();
	void moveTowards(FVector destination,float deltaSec) override;
	void turnTowards(FVector destination,float deltaSec);
	UFUNCTION()
	void playerPickup();
	UPROPERTY(EditAnywhere)
	FVector coneDirection;
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	float turnSpeed;
	float coneRadius;
	float coneAngle;
	AIState currentState;
	TArray<FVector> patrolPoints;
	float waitCounter;

	
	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* coneLight;
	UPROPERTY()
	AGame_PaperCharacter* player;
	UPROPERTY()
	UNavigationPath *tpath;
};
