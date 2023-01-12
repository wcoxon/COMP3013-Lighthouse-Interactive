// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

#include <valarray>
#include "PaperFlipbookComponent.h"
#include "NavigationPath.h"
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
	CharacterFlipbook->SetRenderCustomDepth(true);
	
	//setting up movement properties
	moveSpeed = 500;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 500.0f;
	CharacterMovementComp->BrakingDecelerationWalking = moveSpeed/0.1f;
}

bool ANPCBase::detectsPlayer()
{
	//check if player is within vision distance
	if(FVector::Distance(player->GetActorLocation(),GetActorLocation())>coneRadius)
	{
		return false;
	}

	//check if player is within vision cone
	const FVector playerDisplacement = player->GetActorLocation()-GetActorLocation();
	if(abs(FMath::FindDeltaAngleRadians(playerDisplacement.HeadingAngle(),coneDirection.HeadingAngle()))>coneAngle)
	{
		return false;
	}

	//check for obstacles using raycast
	FHitResult hit;
	const bool actorHit = GetWorld()->LineTraceSingleByChannel(hit,GetActorLocation(),player->GetActorLocation(),ECC_Visibility,FCollisionQueryParams(),FCollisionResponseParams());
	if(actorHit && hit.GetActor())
	{
		return false;
	}
	
	return true;
}

void ANPCBase::playerPickup()
{
	if(detectsPlayer())
	{
		currentState = alerted;
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
	player->ConcealItemEvent.__Internal_AddDynamic(this,&ANPCBase::playerPickup,TEXT("playerPickup"));

	//initialising nav path
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),GetNavAgentLocation());

	//setting the spotlight to be a vision cone
	coneLight->SetInnerConeAngle(FMath::RadiansToDegrees(coneAngle));
	coneLight->bUseInverseSquaredFalloff = 0;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneRadius);

	//initialising random patrol pattern containing 3 points
	int patrolCount = 3;
	for(int x=0;x<patrolCount;x++)
	{
		patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),1000));
	}
	
	//set initial AI state to patrolling
	currentState=patrol;
	
	//counter used to handle pauses in the NPC's behaviour
	waitCounter=0;
	
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

void ANPCBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	switch(currentState)
	{
	case patrol:
		if(waitCounter<5)
		{
			waitCounter+=DeltaSeconds;
			break;
		}
		waitCounter=0;
		for(int patrolIndex = 0; patrolIndex<patrolPoints.Num();patrolIndex++)
		{
			if(tpath->PathPoints.Last().Equals(patrolPoints[patrolIndex]))
			{
				if(patrolPoints.Num()-1==patrolIndex)
				{
					Algo::Reverse(patrolPoints);
					patrolIndex=-1;
				}
				if(tpath->PathPoints.Num()==1)
				{
					UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
					tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),patrolPoints[patrolIndex+1]);
				}
				break;
			}
			else if(patrolIndex==patrolPoints.Num()-1)
			{
				UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
				tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),patrolPoints[0]);
			}
			
		}
		break;
		
	case alerted:
		if(detectsPlayer())
		{
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
			tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),player->GetNavAgentLocation());
		}
		else if(tpath->PathPoints.Num()==1)
		{
			currentState=searching;
			waitCounter=0;
		}
		break;
		
	case searching:
		if(detectsPlayer())
		{
			currentState=alerted;
		}
		else if(tpath->PathPoints.Num()==1 && fmod(waitCounter,1)+DeltaSeconds>1)
		{
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
			tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),navSys->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),500));
		}
		else if(waitCounter>15)
		{
			waitCounter=0;
			currentState=patrol;
			break;
		}
		waitCounter+=DeltaSeconds;
		break;
		
		default:
			break;
	}
	
	if(detectsPlayer())
	{
		player->DetectionCheck(DeltaSeconds);
		if(currentState==alerted)
		{
			coneLight->SetLightColor(FLinearColor(1,0.0,0.0));
		}
		else coneLight->SetLightColor(FLinearColor(1,1.0,0.0));
	}
	else
	{
		if(currentState==searching) coneLight->SetLightColor(FLinearColor(1,0.0,1.0));
		else coneLight->SetLightColor(FLinearColor(1,1,1));
	}
	if(tpath->PathPoints.Num()>1)
	{
		moveTowards(tpath->PathPoints[1],DeltaSeconds);
		if(FVector2d::Distance(FVector2d(GetNavAgentLocation()),FVector2d(tpath->PathPoints[1]))<1)
		{
			tpath->PathPoints.RemoveAt(0);
			setDirectionalAnimation(direction,"idle");
			CharacterFlipbook->SetPlayRate(1);
		}
	}
}


