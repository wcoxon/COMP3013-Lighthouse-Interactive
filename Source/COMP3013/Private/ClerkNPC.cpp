// Fill out your copyright notice in the Description page of Project Settings.

#include "ClerkNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AClerkNPC::AClerkNPC()
{
	//assigning fields
	coneRadius = 2000.0;
	coneAngle = FMath::DegreesToRadians(45.0);
	direction=FVector(1,0,0);
	coneDirection=FVector(1,0,0);
	turnSpeed = FMath::DegreesToRadians(180);
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	
	//setting up movement properties
	moveSpeed = 300;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 500.0f;
	CharacterMovementComp->BrakingDecelerationWalking = moveSpeed/0.1f;
}
void AClerkNPC::BeginPlay()
{
	Super::BeginPlay();
	securityGuard = Cast<ASecurityNPC>(UGameplayStatics::GetActorOfClass(GetWorld(),ASecurityNPC::StaticClass()));
}
void AClerkNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//behaviour based on current state
	//subclass-specific state cases, clerks will tattle on you to security when they see you
	switch(currentState)
	{
	case tattle:
		//if close to security, bring them to last known player position and break
		if(GetDistanceTo(securityGuard)<300.0f)
		{
			securityGuard->pathToTarget(playerLastSeen);
			securityGuard->setState(pursue);
			pathToTarget(playerLastSeen);
			setState(search);
			break;
		}
		//if have path, break case
		if(tpath->PathPoints.Num()>1) break;
		//make path to security
		pathToTarget(securityGuard->GetNavAgentLocation());
		break;
		
	default:
		break;
	}
	
	//behaviour based on current action
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

	//behaviour based on observed player
	if(!detectsActor(player)) return;
	player->isSeen = true;
	playerLastSeen = player->GetNavAgentLocation();
	if(player->Suspicion>=100.0f) return setState(tattle);
	if(player->currentState==Run) return setState(stare);
	
}
