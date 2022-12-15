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
	
	coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	coneLight->SetupAttachment(CharacterCollider);
	audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));
	
	coneRadius = 2000.0;
	coneAngle = FMath::DegreesToRadians(45.0);
	direction=FVector(1,0,0);
	coneDirection=FVector(1,0,0);
	turnSpeed = 180;
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	moveSpeed = 500;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 500.0f;
	CharacterMovementComp->BrakingDecelerationWalking = moveSpeed/0.1f;
}

bool ANPCBase::detectsPlayer()
{
	if(FVector::Distance(player->GetActorLocation(),GetActorLocation())>coneRadius)
	{
		return false;
	}
	
	const FVector playerDisplacement = player->GetActorLocation()-GetActorLocation();
	if(abs(FMath::FindDeltaAngleRadians(playerDisplacement.HeadingAngle(),coneDirection.HeadingAngle()))>coneAngle)
	{
		return false;
	}
	
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
		//if the player is seen picking up an item it will alert the npc
		//yeah ik concealing items should be what aggros
		currentState = alerted;
	}
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();
	//setting all the animations for this npc
	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeWalkLeft.EmployeeWalkLeft"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeWalkRight.EmployeeWalkRight"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeWalkUp.EmployeeWalkUp"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeWalkDown.EmployeeWalkDown"), NULL, LOAD_None, NULL));

	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeIdleLeft.EmployeeIdleLeft"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeIdleRight.EmployeeIdleRight"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeIdleUp.EmployeeIdleUp"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Employees/EmployeeIdleDown.EmployeeIdleDown"), NULL, LOAD_None, NULL));

	float actorScale = 4;
	//this sprite is 96x96
	float spriteRes = 96;
	//the player animation frames are actually like 19 pixels above the bottom of the sprite, so this shifts it down
	//to touch the floor
	float spriteBottomMargin = 16;
	
	//fix collider
	CharacterCollider->SetCapsuleHalfHeight(spriteRes/2.0-spriteBottomMargin);
	SetActorScale3D(FVector(actorScale));
	
	const float stridePixels = 70;
	const float strideFrames = 16;
	CharacterFlipbook->SetPlayRate(moveSpeed/(15*actorScale*stridePixels/strideFrames));
	
	audioSource->Sound = LoadObject<USoundBase>(NULL,TEXT("/Game/ThirdParty/Sounds/footstep.footstep"),NULL,LOAD_None,NULL);
	
	
	//finding player pawn and binding pickup delegate
	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	player->PickupItemEvent.__Internal_AddDynamic(this,&ANPCBase::playerPickup,TEXT("playerPickup"));

	//initialising nav path
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),GetNavAgentLocation());

	//setting the spotlight to be a better vision cone
	coneLight->SetInnerConeAngle(FMath::RadiansToDegrees(coneAngle));
	coneLight->bUseInverseSquaredFalloff = 0;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneRadius);

	//initialising random patrol pattern
	int patrolCount = 3;
	for(int x=0;x<patrolCount;x++)
	{
		patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),1000));
	}
	//setting npc sprite
	CharacterFlipbook->SetFlipbook(animations["idleRight"]);
	//initial state is patrolling
	currentState=patrol;
	waitCounter=0;
	
}

void ANPCBase::turnTowards(FVector destination, float deltaSec)
{
	const FVector3d displacement = destination - GetNavAgentLocation();
	float deltaAngle = FMath::DegreesToRadians(turnSpeed*deltaSec);
	float directionAngle = std::atan2(coneDirection.Y,coneDirection.X);
	float targetAngle = std::atan2(displacement.Y,displacement.X);
	
	if(abs(FMath::FindDeltaAngleRadians(targetAngle,directionAngle))<deltaAngle)
	{
		coneDirection = displacement.GetSafeNormal2D();
	}
	else if(FMath::FindDeltaAngleRadians(targetAngle,directionAngle)<0)
	{
		coneDirection=FVector(cos(directionAngle+deltaAngle),sin(directionAngle+deltaAngle),0);
	}
	else
	{
		coneDirection=FVector(cos(directionAngle-deltaAngle),sin(directionAngle-deltaAngle),0);
	}
	coneLight->SetRelativeRotation(FRotator(0,FMath::RadiansToDegrees(std::atan2(coneDirection.Y,coneDirection.X)),0));
}

void ANPCBase::moveTowards(FVector destination,float deltaSec)
{
	float distance = deltaSec*CharacterMovementComp->Velocity.Length();
	const FVector3d displacement = destination - GetNavAgentLocation();
	if(displacement.Length()<distance)
	{
		SetActorLocation(GetActorLocation()+ FVector3d(1,1,0)*displacement);
		return;
	}
	/*float initVelocity = CharacterMovementComp->Velocity.Length();
	float timeToDecel = initVelocity/CharacterMovementComp->BrakingDecelerationWalking;
	float distToStop = 2*CharacterMovementComp->Velocity.Length()*timeToDecel;
	if(displacement.Length()<distToStop)
	{
		return;
	}*/
	
	if(!coneDirection.Equals(displacement.GetSafeNormal2D()))
	{
		direction = displacement.GetSafeNormal2D();
		turnTowards(destination,deltaSec);
	}

	//set sprite to walking in current direction
	if(abs(direction.Y)>abs(direction.X))
	{
		if(direction.Y>0) CharacterFlipbook->SetFlipbook(animations["walkUp"]);
		else CharacterFlipbook->SetFlipbook(animations["walkDown"]);
	}
	else if(abs(direction.Y)<abs(direction.X))
	{
		if(direction.X>0) CharacterFlipbook->SetFlipbook(animations["walkRight"]);
		else CharacterFlipbook->SetFlipbook(animations["walkLeft"]);
	}
	//footstep sound effects at frame 13 and 30
	if(CharacterFlipbook->GetPlaybackPositionInFrames()==13 || CharacterFlipbook->GetPlaybackPositionInFrames()==30)
	{
		audioSource->Stop();
		audioSource->SetPitchMultiplier(FMath::FRandRange(1.1,1.2));
		audioSource->Play();
	}
	CharacterFlipbook->SetPlayRate(CharacterMovementComp->Velocity.Length()/(15*4*70/16));
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
			
			if(abs(direction.Y)>abs(direction.X))
			{
				if(direction.Y>0) CharacterFlipbook->SetFlipbook(animations["idleUp"]);
				else CharacterFlipbook->SetFlipbook(animations["idleDown"]);
			}
			else if(abs(direction.Y)<abs(direction.X))
			{
				if(direction.X>0) CharacterFlipbook->SetFlipbook(animations["idleRight"]);
				else CharacterFlipbook->SetFlipbook(animations["idleLeft"]);
			}
		}
		
	}
}


