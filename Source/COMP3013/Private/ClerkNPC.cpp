// Fill out your copyright notice in the Description page of Project Settings.

#include "ClerkNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AClerkNPC::AClerkNPC()
{
	//initialising components
	//coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	//coneLight->SetupAttachment(CharacterCollider);
	//audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));

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

	//subclass-specific state cases, clerks will tattle on you to security when they see you
	switch(currentState)
	{
	case tattle:
		if(GetDistanceTo(securityGuard)<50.0f)
		{
			securityGuard->pathToTarget(playerLastSeen);
			securityGuard->setState(pursue);
			setState(search);
		}
		else if(tpath->PathPoints.Num()==1)
		{
			pathToTarget(securityGuard->GetNavAgentLocation());
		}
		break;
	}
	
	if(detectsActor(player))
	{
		playerLastSeen=player->GetNavAgentLocation();
		if(player->Suspicion>=100.0f)
		{
			setState(tattle);
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
}
