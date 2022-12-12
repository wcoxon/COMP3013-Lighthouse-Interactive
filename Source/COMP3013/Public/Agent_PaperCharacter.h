// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Agent_PaperCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMP3013_API AAgent_PaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	AAgent_PaperCharacter();
	UPROPERTY()
	TMap<FString,UPaperFlipbook*> animations;
	UPROPERTY()
	FVector direction;
	UPROPERTY()
	float moveSpeed;
	
	virtual void moveTowards(FVector destination,float distance);
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	UPaperFlipbookComponent* CharacterFlipbook = GetSprite();
	UCapsuleComponent* CharacterCollider = GetCapsuleComponent();
	
};
