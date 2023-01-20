// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent_PaperCharacter.h"

#include "Components/CapsuleComponent.h"


AAgent_PaperCharacter::AAgent_PaperCharacter()
{
	CharacterCollider->SetCapsuleRadius(6.6f);
	//CharacterFlipbook->SetRenderCustomDepth(true);
}

void AAgent_PaperCharacter::beginAction(ActionType action, float duration, FTimerDelegate resultDelegate)
{
	//this is cool too because if we wanna we can make functions to call at the end of these timers,
	//like picking up or concealing performing the action at the end of the timer
	currentAction = action;
	
	GetWorldTimerManager().SetTimer(
		actionTimerHandle,
		resultDelegate,
		duration,
		false,
		duration
		);
	
}
void AAgent_PaperCharacter::endAction()
{
	currentAction=nullAction;
	GetWorldTimerManager().ClearTimer(actionTimerHandle);
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

	float angle = FMath::RadiansToDegrees(animDirection.HeadingAngle())+180;
	std::vector<FString> directionStrings = {"Left","DL","Down","DR","Right","UR","Up","UL"};

	for(int x = 0; x<8;x++)
	{
		if(abs(FMath::FindDeltaAngleDegrees(angle,x*45))<45.0f/2.0f)
		{
			directionString=directionStrings[x];
			break;
		}
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

void AAgent_PaperCharacter::Tick(float DeltaTime)
{
	if(currentAction!=nullAction && GetWorldTimerManager().GetTimerElapsed(actionTimerHandle)==-1.0f)
	{
		endAction();
	}
}

void AAgent_PaperCharacter::moveTowards(FVector destination, float distance)
{
	
}


