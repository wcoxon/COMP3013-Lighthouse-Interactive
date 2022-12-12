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
	seesPlayer,
	playerHidden,
	patrol
};

UCLASS()
class COMP3013_API ANPCBase : public AAgent_PaperCharacter
{
	GENERATED_BODY()

public:
	ANPCBase();
	virtual void Tick(float DeltaSeconds) override;
	bool detectsPlayer();
	UFUNCTION()
	void hearPlayer();
	void moveTowards(FVector destination,float distance) override;
	AIState currentState;
	UPROPERTY(EditAnywhere)
	FVector2D coneDirection;
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	float moveSpeed;
	float coneRadius;
	float coneAngle;
	UPROPERTY()
	UNavigationPath *tpath;
	UPaperFlipbookComponent* CharacterFlipbook = GetSprite();
	UCapsuleComponent* CharacterCollider = GetCapsuleComponent();
	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* coneLight;
	UPROPERTY()
	AGame_PaperCharacter* player;
	
};
