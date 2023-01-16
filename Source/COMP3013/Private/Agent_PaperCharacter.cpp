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

	float angle = FMath::RadiansToDegrees(animDirection.HeadingAngle());

	if(angle>-135-45/2.0f && angle<-135+45/2.0f)
	{
		directionString="DL";
	}
	if(angle>-90-45/2.0f && angle<-90+45/2.0f)
	{
		directionString="Down";
	}
	if(angle>-45-45/2.0f && angle<-45/2.0f)
	{
		directionString="DR";
	}
	if(angle>-45/2.0f && angle<45.0f-45/2.0f)
	{
		directionString="Right";
	}
	if(angle>45.0f-45/2.0f && angle<45.0f+45/2.0f)
	{
		directionString="UR";
	}
	if(angle>90.0f-45/2.0f && angle<90.0f+45/2.0f)
	{
		directionString="Up";
	}
	if(angle>135.0f-45/2.0f && angle<135.0f+45/2.0f)
	{
		directionString="UL";
	}
	if(angle>180-45/2.0f || angle<-180+45/2.0f)
	{
		directionString="Left";
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
	CharacterFlipbook->SetWorldRotation(FRotator(0.0f, 0.0f, 10.0f));
}

void AAgent_PaperCharacter::moveTowards(FVector destination, float distance)
{
	
}


