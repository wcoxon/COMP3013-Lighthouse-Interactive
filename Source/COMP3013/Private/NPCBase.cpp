// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

#include <valarray>

#include "CustomerNPC.h"
#include "PaperFlipbookComponent.h"
#include "Components/AudioComponent.h"
#include "PaperFlipbook.h"
#include "VectorTypes.h"
#include "Components/CapsuleComponent.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Algo/Reverse.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"


ANPCBase::ANPCBase()
{
	//initialising components
	visionCone= CreateDefaultSubobject<UVisionConeComponent>(TEXT("VisionCone"));
	visionCone->SetupAttachment(RootComponent);
	
	audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));
	audioSource->SetupAttachment(RootComponent);
	
	
	//assigning fields
	visionCone->coneRadius = 2000.0;
	visionCone->coneAngle = FMath::DegreesToRadians(45.0);
	
	direction=FVector(1,0,0);
	
	turnSpeed = FMath::DegreesToRadians(180);
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(false);
	
	//setting up movement properties
	moveSpeed = 300;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 500.0f;
	CharacterMovementComp->BrakingDecelerationWalking = 10*moveSpeed;

	CharacterCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	audioSource->VolumeMultiplier=1.f;
	
	
}

void ANPCBase::setState(AIState state)
{
	if(currentState==state) return;
	//interrupt any actions
	endAction();
	//set state to argument
	currentState = state;
	//UE_LOG(LogTemp, Log, TEXT("%i %f"), state,GetWorldTimerManager().GetTimerElapsed(actionTimerHandle));
}

void ANPCBase::playerCrimeCommitted()
{
	if(visionCone->detectsActor(player))
	{
		int visibleCustomers = visionCone->getVisibleActors(ACustomerNPC::StaticClass()).Num();
		//chance of being incriminated decreases from customers also visible to the npc
		//if it rolls 0 the player gets seen stealing, the more customers around the less likely
		//halved the scale because it's a little broken if 4 customers gives you a 1/5 chance of being sussed on
		bool sawCrime = FMath::RandRange(0, visibleCustomers/2)==0?true:false;
		UE_LOG(LogTemp, Warning, TEXT("customer camoflauge: %i"), visibleCustomers);
		UE_LOG(LogTemp, Warning, TEXT("crime seen: %i"), sawCrime);
		if(sawCrime)
		{
			player->Suspicion=100;
		}
	}
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	CharacterFlipbook->SetRenderCustomDepth(false);
	//base height of the sprite i.e. how many pixels tall it is since pixels per unit is 1
	float spriteHeight = 64.0;
	//offset from the bottom of the sprite to the feet of the character
	float spriteGroundLevel = 3.0;
	//scale to resize the actor
	float actorScale = 300.0;
	//the angle that we set the sprite to
	float spriteRoll = FMath::DegreesToRadians(CharacterFlipbook->GetComponentRotation().Roll);

	//apply rotation and foot offset to half height of sprite
	float halfHeight = cos(spriteRoll)*(spriteHeight/2-spriteGroundLevel);

	//set collider height to offset the sprite so it touches the ground
	CharacterCollider->SetCapsuleHalfHeight(halfHeight);
	
	//scales up the actor
	SetActorScale3D(FVector(actorScale/spriteHeight));

	//load up footstep audio
	audioSource->Sound = LoadObject<USoundBase>(NULL,TEXT("/Game/ThirdParty/Sounds/footstep.footstep"),NULL,LOAD_None,NULL);
	
	//finding player pawn and binding pickup delegate
	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	
	visionCone->coneLight->SetLightColor(FLinearColor(1,1,0));
	visionCone->coneLight->SetIntensity(0.f);
	
	//set initial AI state to patrolling
	currentState=patrol;
}

void ANPCBase::turnTowards(FVector destination, float deltaSec)
{
	//get a vector between the actor and the target
	const FVector3d displacement = destination - GetNavAgentLocation();
	
	//angle between the current vision direction and the target direction
	float deltaAngle = FMath::FindDeltaAngleRadians(visionCone->coneDirection.HeadingAngle(),displacement.HeadingAngle());
	
	//the angle to rotate towards the target in this frame
	float deltaTurn = turnSpeed*deltaSec*FMath::Sign(deltaAngle);

	//if the target angle will be reached within this frame, snap the direction to the target direction
	if(abs(deltaAngle)<abs(deltaTurn))
	{
		visionCone->coneDirection = displacement.GetSafeNormal2D();
	}
	
	//else rotate the vision cone towards the target using deltaTurn
	else
	{
		visionCone->coneDirection=visionCone->coneDirection.RotateAngleAxis(FMath::RadiansToDegrees(deltaTurn),FVector::ZAxisVector);
	}
	
	//update the spotlight to reflect this change in the vision cone
	visionCone->coneLight->SetRelativeRotation(FRotator(0,FMath::RadiansToDegrees(visionCone->coneDirection.HeadingAngle()),0));
}

void ANPCBase::moveTowards(FVector destination,float deltaSec)
{
	const FVector3d displacement = destination - GetNavAgentLocation();

	//given the current speed and deceleration, the distance travelled if stopped
	const float brakingDistance = 2*CharacterMovementComp->Velocity.SquaredLength()/CharacterMovementComp->BrakingDecelerationWalking;
	
	//if approaching final point in path
	if(tpath->PathPoints.Num()<=2)
	{
		//if within braking distance of the target, brake
		if(displacement.Length()<=brakingDistance)
		{
			return;
		}
	}
	//if not looking at the target, turn to look at the target
	if(!visionCone->coneDirection.Equals(displacement.GetSafeNormal2D()))
	{
		direction = displacement.GetSafeNormal2D();
		turnTowards(destination,deltaSec);
		
	}

	//set sprite animation to walking in current direction
	setDirectionalAnimation(direction,"walk");
	//alter the animation speed to reflect the velocity of the player
	setAnimationRateToSpeed(CharacterFlipbook,CharacterMovementComp->Velocity.Length(),300);
	
	//footstep sound effects at frame 13 and 30
	if(CharacterFlipbook->GetPlaybackPositionInFrames()==13 || CharacterFlipbook->GetPlaybackPositionInFrames()==30)
	{
		audioSource->Stop();
		audioSource->SetPitchMultiplier(FMath::FRandRange(1.1,1.2));
		audioSource->Play();
	}

	//move actor in direction
	CharacterMovementComp->RequestPathMove(direction);
}

void ANPCBase::pathToTarget(FVector destination)
{
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),destination);
	endAction();
}
void ANPCBase::followPath(float deltaSec)
{
	float distanceToNextPathNode = FVector2d::Distance(FVector2d(GetNavAgentLocation()),FVector2d(tpath->PathPoints[1]));
	
	if(distanceToNextPathNode>deltaSec*moveSpeed) return moveTowards(tpath->PathPoints[1],deltaSec);
	tpath->PathPoints.RemoveAt(0);
	//if the target position will be reached in this frame, snap position to target
	SetActorLocation(FVector(tpath->PathPoints[0].X,tpath->PathPoints[0].Y,GetActorLocation().Z));
	setDirectionalAnimation(direction,"idle");
	CharacterFlipbook->SetPlayRate(1);
}

void ANPCBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	switch(currentState)
	{
	case patrol:
		//for each point in patrol route
		for(int patrolIndex = 0; patrolIndex<patrolPoints.Num();patrolIndex++)
		{
			//if current destination isn't this patrol point, continue to next
			if(!tpath->PathPoints.Last().Equals(patrolPoints[patrolIndex])) continue;
			
			//if destination point isn't reached, return to continue along it
			if(tpath->PathPoints.Num()>1) return;
			
			//if point is last element in patrol route
			if(patrolIndex==patrolPoints.Num()-1)
			{
				//reverse the patrol route
				Algo::Reverse(patrolPoints);
				patrolIndex=0;
			}
			
			//wait 3 seconds to make path to next patrol point
			if(currentAction!=wait) beginAction(wait,3.0f,FTimerDelegate::CreateUFunction( this,FName("pathToTarget"),patrolPoints[patrolIndex+1]));

			return;
			
			
			
		}
		//if current path destination didn't match any patrol points, path to first patrol point
		pathToTarget(patrolPoints[0]);
		break;
		
	case search:
		//if our path has a next point, break case
		if(tpath->PathPoints.Num()>1) break;
		//wait 1 second before making path to random nearby position
		if(currentAction!=wait) beginAction(wait,1.0f,FTimerDelegate::CreateUFunction( this,FName("pathToTarget"),UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),1000)));
		
		break;
		
	case stare:
		visionCone->coneLight->SetLightColor(FLinearColor(1, 1-player->Suspicion/100.f, 0,0.5));
		//after 2 seconds continue patrolling
		if(currentAction!=wait) beginAction(wait,2.0f,FTimerDelegate::CreateUFunction( this,FName("setState"),patrol));

		//if npc can't see player then break case
		if(!visionCone->detectsActor(player)) break;

		//turn to stare at player
		turnTowards(player->GetNavAgentLocation(),DeltaSeconds);
		
		//if the player acts cringe reset timer by starting wait over
		if(player->currentState==Run || player->currentAction==conceal) GetWorldTimerManager().ClearTimer(actionTimerHandle);
		break;
		
	default:
		break;
	}
}



