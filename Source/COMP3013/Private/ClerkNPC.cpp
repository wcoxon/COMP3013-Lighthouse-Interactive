// Fill out your copyright notice in the Description page of Project Settings.

#include "ClerkNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Sound/SoundCue.h"

AClerkNPC::AClerkNPC()
{
	//assigning fields
	visionCone->coneRadius = 1500.0;
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

	TattleaudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TattleAudioComponent"));
	TattleaudioComponent->SetupAttachment(RootComponent);
	TattleaudioComponent->bAutoActivate = false;
}

void AClerkNPC::EgcOn() {
	visionCone->coneLight->SetIntensity(10.f);
	visionCone->coneLight->SetLightColor(FLinearColor::Red);
}

void AClerkNPC::BeginPlay()
{
	Super::BeginPlay();
	player->ConcealItemEvent.AddDynamic(this,&ANPCBase::playerCrimeCommitted);

	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkLeft64.EmployeeWalkLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkRight64.EmployeeWalkRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkUp64.EmployeeWalkUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkDown64.EmployeeWalkDown64"), NULL, LOAD_None, NULL));

	animations.Add(FString("walkDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkDL64.EmployeeWalkDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkUL64.EmployeeWalkUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkDR64.EmployeeWalkDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/WalkAnimation/EmployeeWalkUR64.EmployeeWalkUR64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleLeft64.EmployeeIdleLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleRight64.EmployeeIdleRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleUp64.EmployeeIdleUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleDown64.EmployeeIdleDown64"), NULL, LOAD_None, NULL));

	animations.Add(FString("idleDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleDL64.EmployeeIdleDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleUL64.EmployeeIdleUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleDR64.EmployeeIdleDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Clerk/IdleAnimation/EmployeeIdleUR64.EmployeeIdleUR64"), NULL, LOAD_None, NULL));
	

	CharacterFlipbook->SetFlipbook(animations["idleRight"]);
	
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),GetNavAgentLocation());
	
	int patrolCount = 3;
	patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),3000));
	for(int x=0;x<patrolCount-1;x++)
	{
		patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),patrolPoints[x],3000));
	}
	securityGuard = Cast<ASecurityNPC>(UGameplayStatics::GetActorOfClass(GetWorld(),ASecurityNPC::StaticClass()));

	TattleSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, TEXT("/Game/ThirdParty/Sounds/Tattle.Tattle")));

	player->SusMaxEvent.AddDynamic(this, &AClerkNPC::EgcOn);
	//TattleaudioComponent->SetSound(TattleSound);
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
		
		//if the destination is close to security and isn't reached, continue following
		if(FVector::Distance(tpath->PathPoints.Last(),securityGuard->GetNavAgentLocation())<1500.f
			&&tpath->PathPoints.Num()>1) break;
		
		//else make a new path to security
		pathToTarget(securityGuard->GetNavAgentLocation());
		break;
		
	default:
		break;
	}
	
	//behaviour based on current action
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

	//behaviour based on observed player
	if(!visionCone->detectsActor(player)) return;
	player->isSeen = true;
	playerLastSeen = player->GetNavAgentLocation();
	if(player->Suspicion>=100.0f) {
		TattleaudioComponent->Play();
		return setState(tattle);
	}
	if(player->currentState==Run || player->currentAction==conceal) return setState(stare);
	
}
