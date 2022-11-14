// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game_PaperCharacter.h"
#include "Game_Spawner.h"
#include "PaperCharacter.h"
#include "NPCBase.generated.h"

/**
 * 
 */

enum AIState
{
	seesPlayer,
	playerHidden
};

UCLASS()
class COMP3013_API ANPCBase : public APaperCharacter
{
	GENERATED_BODY()

public:
	ANPCBase();
	virtual void Tick(float DeltaSeconds) override;
	bool detectsPlayer();
	AIState currentState;

	UPROPERTY(EditAnywhere)
	FVector2D coneDirection;
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	float coneRadius;
	float coneAngle;
	UPaperFlipbookComponent* CharacterFlipbook = GetSprite();
	UCapsuleComponent* CharacterCollider = GetCapsuleComponent();
	APawn* player;
	
};
