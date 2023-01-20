// Fill out your copyright notice in the Description page of Project Settings.

#include "SecurityNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ASecurityNPC::ASecurityNPC()
{
	//assigning fields
	coneRadius = 2000.0;
	coneAngle = FMath::DegreesToRadians(60.0);
	direction=FVector(1,0,0);
	coneDirection=FVector(1,0,0);
	turnSpeed = FMath::DegreesToRadians(120);
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	//CharacterFlipbook->SetRenderCustomDepth(true);
	
	//setting up movement properties
	moveSpeed = 900;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 500.0f;
	CharacterMovementComp->BrakingDecelerationWalking = moveSpeed/0.1f;
}
void ASecurityNPC::BeginPlay()
{
	Super::BeginPlay();
	CharacterFlipbook->SetSpriteColor(FLinearColor(1.0f,0.0f,0.0f));
}
void ASecurityNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	switch(currentState)
	{
	case pursue:
		if(GetDistanceTo(player)<300.0f)
		{
			//ig they catch you in here
			UE_LOG(LogTemp, Log, TEXT("Caught you lose"));
			player->moveSpeed = 0;
			player->WalkSpeed = 0;
			player->SprintOn();
			
		}
		if(detectsActor(player))
		{
			pathToTarget(player->GetNavAgentLocation());
		}
		else if(tpath->PathPoints.Num()==1)
		{
			setState(search);
		}
		break;
	default:
		break;
	}

	switch(currentAction)
	{
	case wait:
		setDirectionalAnimation(coneDirection,"idle");
		CharacterFlipbook->SetPlayRate(1);
		break;
	default:
		if(tpath->PathPoints.Num()>1) followPath(DeltaSeconds);
		else
		{
			setDirectionalAnimation(coneDirection,"idle");
			CharacterFlipbook->SetPlayRate(1);
		}
		break;
	}

	if(!detectsActor(player)) return;
	player->isSeen = true;
	if(player->Suspicion>=100.0f) return setState(pursue);
	if(player->currentState==Run) return setState(stare);
	
}

