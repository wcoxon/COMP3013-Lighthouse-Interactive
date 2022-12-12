// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent_PaperCharacter.h"

#include "Components/CapsuleComponent.h"


AAgent_PaperCharacter::AAgent_PaperCharacter()
{
	CharacterCollider->SetCapsuleRadius(6.6f);
	CharacterFlipbook->SetRenderCustomDepth(true);
	CharacterFlipbook->BoundsScale = 10.0f;
}


void AAgent_PaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterCollider->SetCapsuleHalfHeight(6.6f);
	SetActorScale3D(FVector(10));
	CharacterFlipbook->SetWorldRotation(FRotator(0.0f, 0.0f, 40.0f));
}

void AAgent_PaperCharacter::moveTowards(FVector destination, float distance)
{
	
}


