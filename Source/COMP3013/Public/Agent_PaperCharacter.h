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

	//animation fields
	UPROPERTY()
	TMap<FString,UPaperFlipbook*> animations;
	void setAnimationRateToSpeed(UPaperFlipbookComponent* flipbook, float speed,float animationDistance);
	void setDirectionalAnimation(FVector animDirection,FString actionString);
	
	UPROPERTY()
	FVector direction;
	UPROPERTY()
	float moveSpeed;
	virtual void moveTowards(FVector destination,float distance);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItem* heldItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UStaticMeshComponent* heldItemMesh;

	//action fields
	ActionType currentAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Action")
	FTimerHandle actionTimerHandle;
	void beginAction(ActionType action, float duration, FTimerDelegate resultDelegate);
	UFUNCTION()
	void endAction();
	
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* audioSource;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	UPaperFlipbookComponent* CharacterFlipbook = GetSprite();
	UCapsuleComponent* CharacterCollider = GetCapsuleComponent();

	
};
