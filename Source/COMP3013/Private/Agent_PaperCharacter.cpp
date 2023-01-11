// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent_PaperCharacter.h"

#include "Components/CapsuleComponent.h"


AAgent_PaperCharacter::AAgent_PaperCharacter()
{
	CharacterCollider->SetCapsuleRadius(6.6f);
	CharacterFlipbook->SetRenderCustomDepth(true);
	//CharacterFlipbook->BoundsScale = 10.0f;
}
void AAgent_PaperCharacter::setAnimationRateToSpeed(UPaperFlipbookComponent* flipbook, float speed,float animationDistance)
{
	float animationTime = flipbook->GetFlipbookLength();
	//if the animation is 0 seconds long then there can't be a speed
	if(animationTime==0) return;
	//if the distance is 0 then the animation has no speed so isn't affected by player speed
	if(animationDistance==0) flipbook->SetPlayRate(1);
	//if the animation has a speed, adjust it for the player movement speed
	else flipbook->SetPlayRate(speed/(animationDistance/animationTime));
}

void AAgent_PaperCharacter::setDirectionalAnimation(FVector animDirection, FString actionString)
{
	FString directionString;
	
	if(abs(animDirection.Y)>abs(animDirection.X))
	{
		if(animDirection.Y>0) directionString="Up";
		else directionString="Down";
	}
	else if(abs(animDirection.Y)<abs(animDirection.X))
	{
		if(animDirection.X>0) directionString="Right";
		else directionString="Left";
	}
	if(animations.Find(actionString+directionString)!=nullptr)
	{
		CharacterFlipbook->SetFlipbook(animations[actionString+directionString]);
	}
}

void AAgent_PaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterFlipbook->CastShadow = true;
	//CharacterCollider->SetCapsuleHalfHeight(6.6f);
	//SetActorScale3D(FVector(10));
	CharacterFlipbook->SetWorldRotation(FRotator(0.0f, 0.0f, 40.0f));
}

void AAgent_PaperCharacter::moveTowards(FVector destination, float distance)
{
	
}


