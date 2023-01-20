// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

#include <valarray>
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
	coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	coneLight->SetupAttachment(CharacterCollider);
	audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));

	//assigning fields
	coneRadius = 2000.0;
	coneAngle = FMath::DegreesToRadians(45.0);
	direction=FVector(1,0,0);
	coneDirection=FVector(1,0,0);
	turnSpeed = FMath::DegreesToRadians(180);
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(false);
	
	//setting up movement properties
	moveSpeed = 300;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 500.0f;
	CharacterMovementComp->BrakingDecelerationWalking = moveSpeed/0.1f;
}

void ANPCBase::setState(AIState state)
{
	if(currentState==state) return;
	//interrupt any actions
	endAction();
	//set state to argument
	currentState = state;
	UE_LOG(LogTemp, Log, TEXT("%i %f"), state,GetWorldTimerManager().GetTimerElapsed(actionTimerHandle));
}

bool ANPCBase::detectsActor(AActor* actor)
{

	//check if player is within vision distance
	if(FVector::Distance(player->GetActorLocation(),GetActorLocation())>coneRadius)
	{
		return false;
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Player is within vision distance %f"), FVector::Distance(player->GetActorLocation(),GetActorLocation())));
	//check if player is within vision cone
	const FVector displacement = actor->GetActorLocation()-GetActorLocation();
	if(abs(FMath::FindDeltaAngleRadians(displacement.HeadingAngle(),coneDirection.HeadingAngle()))>coneAngle)
	{
		return false;
	}

	//check for obstacles using raycast
	FHitResult hit;
	const bool actorHit = GetWorld()->LineTraceSingleByChannel(hit,GetActorLocation(),actor->GetActorLocation(),ECC_Visibility,FCollisionQueryParams(),FCollisionResponseParams());
	if(actorHit && hit.GetActor())
	{
		return false;
	}
	
	return true;
}

TArray<AActor*> ANPCBase::getVisibleActors(UClass* Type)
{
	TArray<AActor*> actorsInRange;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		coneRadius,
		{},
		Type,
		{},
		actorsInRange);
	
	TArray<AActor*> LOSActorsInRange;
	for(AActor* a : actorsInRange)
	{
		//UE_LOG(LogTemp, Log, TEXT("OverlappedActor: %s"), *a->GetName());
		if(!detectsActor(a))
		{
			actorsInRange.Remove(a);
		}
	}
	return actorsInRange;
	
}

void ANPCBase::playerPickup()
{
	if(detectsActor(player))
	{
		currentState = pursue;
	}
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();
	//setting all the animations for this npc
	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkLeft64.EmployeeWalkLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkRight64.EmployeeWalkRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkUp64.EmployeeWalkUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkDown64.EmployeeWalkDown64"), NULL, LOAD_None, NULL));

	animations.Add(FString("walkDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkDL64.EmployeeWalkDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkUL64.EmployeeWalkUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkDR64.EmployeeWalkDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/WalkAnimation/EmployeeWalkUR64.EmployeeWalkUR64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleLeft64.EmployeeIdleLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleRight64.EmployeeIdleRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleUp64.EmployeeIdleUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleDown64.EmployeeIdleDown64"), NULL, LOAD_None, NULL));

	animations.Add(FString("idleDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleDL64.EmployeeIdleDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleUL64.EmployeeIdleUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleDR64.EmployeeIdleDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/IdleAnimation/EmployeeIdleUR64.EmployeeIdleUR64"), NULL, LOAD_None, NULL));

	CharacterFlipbook->SetFlipbook(animations["idleRight"]);
	
	CharacterFlipbook->SetRenderCustomDepth(false);
	//base height of the sprite i.e. how many pixels tall it is since pixels per unit is 1
	float spriteHeight = 64.0;
	//offset from the bottom of the sprite to the feet of the character
	float spriteGroundLevel = 5.0;
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
	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	
	//player->ConcealItemEvent.__Internal_AddDynamic(this,&ANPCBase::playerPickup,TEXT("playerPickup"));

	//initialising nav path
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),GetNavAgentLocation());

	//setting the spotlight to be a vision cone
	coneLight->SetInnerConeAngle(FMath::RadiansToDegrees(coneAngle));
	coneLight->bUseInverseSquaredFalloff = 0;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneRadius);
	coneLight->SetRelativeLocation(FVector(0.0f, 4.25f, 34.0f));

	//initialising random patrol pattern containing 3 points
	int patrolCount = 2;
	for(int x=0;x<patrolCount;x++)
	{
		patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),2000));
	}
	
	//set initial AI state to patrolling
	currentState=patrol;
}

void ANPCBase::turnTowards(FVector destination, float deltaSec)
{
	//get a vector between the actor and the target
	const FVector3d displacement = destination - GetNavAgentLocation();
	
	//angle between the current vision direction and the target direction
	float deltaAngle = FMath::FindDeltaAngleRadians(coneDirection.HeadingAngle(),displacement.HeadingAngle());
	
	//the angle to rotate towards the target in this frame
	float deltaTurn = turnSpeed*deltaSec*FMath::Sign(deltaAngle);

	//if the target angle will be reached within this frame, snap the direction to the target direction
	if(abs(deltaAngle)<abs(deltaTurn))
	{
		coneDirection = displacement.GetSafeNormal2D();
	}
	
	//else rotate the vision cone towards the target using deltaTurn
	else
	{
		coneDirection=coneDirection.RotateAngleAxis(FMath::RadiansToDegrees(deltaTurn),FVector::ZAxisVector);
	}
	
	//update the spotlight to reflect this change in the vision cone
	coneLight->SetRelativeRotation(FRotator(0,FMath::RadiansToDegrees(coneDirection.HeadingAngle()),0));
}

void ANPCBase::moveTowards(FVector destination,float deltaSec)
{
	const float deltaDistance = deltaSec*CharacterMovementComp->Velocity.Length();
	const FVector3d displacement = destination - GetNavAgentLocation();
	
	//if the target position will be reached in this frame, snap position to target
	if(displacement.Length()<deltaDistance)
	{
		SetActorLocation(GetActorLocation()+ FVector3d(1,1,0)*displacement);
		return;
	}
	//given the current speed and deceleration, the distance travelled if stopped
	const float brakingDistance = 2*CharacterMovementComp->Velocity.SquaredLength()/CharacterMovementComp->BrakingDecelerationWalking;
	
	//if within braking distance of the target, brake
	if(displacement.Length()<=brakingDistance)
	{
		return;
	}

	//if not looking at the target, turn to look at the target
	if(!coneDirection.Equals(displacement.GetSafeNormal2D()))
	{
		direction = displacement.GetSafeNormal2D();
		turnTowards(destination,deltaSec);
		//setDirectionalAnimation(irection,"idle");
		//return;
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
	//move towards next point
	moveTowards(tpath->PathPoints[1],deltaSec);
	//if close to next, remove current
	if(FVector2d::Distance(FVector2d(GetNavAgentLocation()),FVector2d(tpath->PathPoints[1]))<1)
	{
		tpath->PathPoints.RemoveAt(0);
		setDirectionalAnimation(direction,"idle");
		CharacterFlipbook->SetPlayRate(1);
	}
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
		//after 2 seconds continue patrolling
		if(currentAction!=wait) beginAction(wait,2.0f,FTimerDelegate::CreateUFunction( this,FName("setState"),patrol));

		//if npc can't see player then break case
		if(!detectsActor(player)) break;

		//turn to stare at player
		turnTowards(player->GetNavAgentLocation(),DeltaSeconds);
		
		//if the player acts cringe reset timer by starting wait over
		if(player->currentState==Run) GetWorldTimerManager().ClearTimer(actionTimerHandle);
		break;
		
	default:
		break;
	}
}



