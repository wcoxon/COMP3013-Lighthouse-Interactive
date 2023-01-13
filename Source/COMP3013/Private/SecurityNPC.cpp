// Fill out your copyright notice in the Description page of Project Settings.
#include "SecurityNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ASecurityNPC::ASecurityNPC()
{
	//initialising components
	//coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	//coneLight->SetupAttachment(CharacterCollider);
	//audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));

	//assigning fields
	coneRadius = 2000.0;
	coneAngle = FMath::DegreesToRadians(60.0);
	direction=FVector(1,0,0);
	coneDirection=FVector(1,0,0);
	turnSpeed = FMath::DegreesToRadians(120);
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
void ASecurityNPC::BeginPlay()
{
	Super::BeginPlay();
}
void ASecurityNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	switch(currentState)
	{
	case pursue:
		if(GetDistanceTo(player)>30.0f)
		{
			//ig they catch you in here
			UE_LOG(LogTemp, Log, TEXT("Caught you lose"));
			
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
	}
	
	if(detectsActor(player))
	{
		if(player->Suspicion>=100.0f)
		{
			setState(pursue);
		}
		else
		{
			switch(player->currentAction)
			{
			case conceal:
				player->Suspicion = 100;
				break;
			default:
				break;
			}
		
			switch (player->currentState)
			{
			case Run:
				player->Suspicion+= 10.0f*DeltaSeconds;
				setState(stare);
				break;
			default:
				break;
			}
		}
		
	}
	//if there is a path (>1 point), follow it
	if(tpath->PathPoints.Num()>1 && currentAction!=wait)
	{
		moveTowards(tpath->PathPoints[1],DeltaSeconds);
		if(FVector2d::Distance(FVector2d(GetNavAgentLocation()),FVector2d(tpath->PathPoints[1]))<1)
		{
			tpath->PathPoints.RemoveAt(0);
			setDirectionalAnimation(direction,"idle");
			CharacterFlipbook->SetPlayRate(1);
		}
	}
	else
	{
		setDirectionalAnimation(coneDirection,"idle");
		CharacterFlipbook->SetPlayRate(1);
	}
}

