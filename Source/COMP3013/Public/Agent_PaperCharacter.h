// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Item_Base.h"
#include "Agent_PaperCharacter.generated.h"

/**
 * 
 */
enum ActionType
{
	nullAction = NULL,
	wait,
	grab,
	conceal
	
};

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
	
	void beginAction(ActionType action, float duration, FTimerDelegate resultDelegate);
	UFUNCTION()
	void endAction();
	
	void setAnimationRateToSpeed(UPaperFlipbookComponent* flipbook, float speed,float animationDistance);
	void setDirectionalAnimation(FVector animDirection,FString actionString);
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* audioSource;
	
	ActionType currentAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Action")
	FTimerHandle actionTimerHandle;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	UPaperFlipbookComponent* CharacterFlipbook = GetSprite();
	UCapsuleComponent* CharacterCollider = GetCapsuleComponent();

	
};
