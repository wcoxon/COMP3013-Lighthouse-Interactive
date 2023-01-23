// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomerNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACustomerNPC::ACustomerNPC()
{
	visionCone->coneRadius = 0.0f;
	visionCone->coneAngle = FMath::DegreesToRadians(45.0);
	
	direction=FVector(1,0,0);
	
	turnSpeed = FMath::DegreesToRadians(180);
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	//CharacterFlipbook->SetRenderCustomDepth(true);
	
	//setting up movement properties
	moveSpeed = 300;
	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 20*moveSpeed;
	CharacterMovementComp->BrakingDecelerationWalking = 10*moveSpeed;
}

void ACustomerNPC::BeginPlay()
{
	Super::BeginPlay();

	int skinChoice = FMath::RandRange(0,1);
	if(skinChoice==0)
	{
		animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkLeft.CustomerMaleWalkLeft"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkRight.CustomerMaleWalkRight"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkUp.CustomerMaleWalkUp"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkDown.CustomerMaleWalkDown"), NULL, LOAD_None, NULL));

		animations.Add(FString("walkDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkDL.CustomerMaleWalkDL"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkUL.CustomerMaleWalkUL"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkDR.CustomerMaleWalkDR"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Walk/CustomerMaleWalkUR.CustomerMaleWalkUR"), NULL, LOAD_None, NULL));
	
		animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleLeft.CustomerMaleIdleLeft"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleRight.CustomerMaleIdleRight"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleUp.CustomerMaleIdleUp"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleDown.CustomerMaleIdleDown"), NULL, LOAD_None, NULL));

		animations.Add(FString("idleDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleDL.CustomerMaleIdleDL"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleUL.CustomerMaleIdleUL"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleDR.CustomerMaleIdleDR"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer1/Idle/CustomerMaleIdleUR.CustomerMaleIdleUR"), NULL, LOAD_None, NULL));
	}
	else
	{
		animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkLeft.CustomerFemaleWalkLeft"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkRight.CustomerFemaleWalkRight"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkUp.CustomerFemaleWalkUp"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkDown.CustomerFemaleWalkDown"), NULL, LOAD_None, NULL));

		animations.Add(FString("walkDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkDL.CustomerFemaleWalkDL"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkUL.CustomerFemaleWalkUL"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkDR.CustomerFemaleWalkDR"), NULL, LOAD_None, NULL));
		animations.Add(FString("walkUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Walk/CustomerFemaleWalkUR.CustomerFemaleWalkUR"), NULL, LOAD_None, NULL));
	
		animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleLeft.CustomerFemaleIdleLeft"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleRight.CustomerFemaleIdleRight"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleUp.CustomerFemaleIdleUp"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleDown.CustomerFemaleIdleDown"), NULL, LOAD_None, NULL));

		animations.Add(FString("idleDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleDL.CustomerFemaleIdleDL"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleUL.CustomerFemaleIdleUL"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleDR.CustomerFemaleIdleDR"), NULL, LOAD_None, NULL));
		animations.Add(FString("idleUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Customers/Customer2/Idle/CustomerFemaleIdleUR.CustomerFemaleIdleUR"), NULL, LOAD_None, NULL));
	
	}
	
	CharacterFlipbook->SetFlipbook(animations["idleRight"]);

	
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),GetNavAgentLocation());

	
	patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),FVector(1368.0f,0.0,0.f),3000));
	int patrolCount = 4;
	for(int x=0;x<patrolCount;x++)
	{
		patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),patrolPoints[x],3000));
	}

	audioSource->VolumeMultiplier=0.f;
	
}

void ACustomerNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch(currentAction)
	{
	case wait:
		setDirectionalAnimation(visionCone->coneDirection,"idle");
		CharacterFlipbook->SetPlayRate(1);
		break;
		
	default:
		if(tpath->PathPoints.Num()>1) followPath(DeltaSeconds);
		else
		{
			setDirectionalAnimation(visionCone->coneDirection,"idle");
			CharacterFlipbook->SetPlayRate(1);
		}
		break;
	}
	
}