// Fill out your copyright notice in the Description page of Project Settings.

#include "SecurityNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Sound/SoundCue.h"

ASecurityNPC::ASecurityNPC()
{
	//assigning fields
	visionCone->coneRadius = 2000.0;
	visionCone->coneAngle = FMath::DegreesToRadians(60.0);
	
	direction=FVector(1,0,0);
	
	turnSpeed = FMath::DegreesToRadians(360);
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Enable Render Buffer - Used for LOS colour
	//CharacterFlipbook->SetRenderCustomDepth(true);
	
	//setting up movement properties

	moveSpeed = 650;

	CharacterMovementComp->MovementMode=MOVE_NavWalking;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 20*moveSpeed;
	CharacterMovementComp->BrakingDecelerationWalking = 10*moveSpeed;

	TriggerSound = CreateDefaultSubobject<UAudioComponent>(TEXT("TriggerSound"));
	TriggerSound->SetupAttachment(RootComponent);
	TriggerSound->bAutoActivate = false;
}

void ASecurityNPC::EgcOn() {
	visionCone->coneLight->SetIntensity(5.f);
	visionCone->coneLight->SetLightColor(FLinearColor::Yellow);
}

void ASecurityNPC::BeginPlay()
{
	Super::BeginPlay();
	player->ConcealItemEvent.AddDynamic(this,&ANPCBase::playerCrimeCommitted);

	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkLeft.SecurityWalkLeft"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkRight.SecurityWalkRight"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkUp.SecurityWalkUp"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkDown.SecurityWalkDown"), NULL, LOAD_None, NULL));

	animations.Add(FString("walkDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkDL.SecurityWalkDL"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkUL.SecurityWalkUL"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkDR.SecurityWalkDR"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Walk/SecurityWalkUR.SecurityWalkUR"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleLeft.SecurityIdleLeft"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleRight.SecurityIdleRight"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleUp.SecurityIdleUp"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleDown.SecurityIdleDown"), NULL, LOAD_None, NULL));

	animations.Add(FString("idleDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleDL.SecurityIdleDL"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleUL.SecurityIdleUL"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleDR.SecurityIdleDR"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Security/Idle/SecurityIdleUR.SecurityIdleUR"), NULL, LOAD_None, NULL));
	
	CharacterFlipbook->SetFlipbook(animations["idleRight"]);
	
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),GetNavAgentLocation());
	int patrolCount = 2;
	for(int x=0;x<patrolCount;x++)
	{
		patrolPoints.Add(navSys->GetRandomReachablePointInRadius(GetWorld(),GetNavAgentLocation(),2000));
	}

	PunchSoundCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, TEXT("/Game/ThirdParty/Sounds/SoundPunch.SoundPunch")));
	TriggerSoundCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, TEXT("/Game/ThirdParty/Sounds/Waaa.Waaa")));
	
	//TriggerSound->SetSound(TriggerSoundCue);
	player->SusMaxEvent.AddDynamic(this, &ASecurityNPC::EgcOn);
}
void ASecurityNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	switch(currentState)
	{
	case pursue:
		visionCone->coneLight->SetIntensity(10.f);
		visionCone->coneLight->SetLightColor(FLinearColor::Yellow);
		if(GetDistanceTo(player)<300.0f && !Caught)
		{
			Caught = true;
			//ig they catch you in here
			UE_LOG(LogTemp, Log, TEXT("Caught you lose"));
			player->moveSpeed = 0;
			player->WalkSpeed = 0;
			UGameplayStatics::PlaySound2D(GetWorld(), PunchSoundCue, 1.0f, 1.0f, 0.0f);
			player->SprintOn();
		}
		if(visionCone->detectsActor(player))
		{
			pathToTarget(player->GetNavAgentLocation());
			if (!TriggerSound->IsPlaying()) TriggerSound->Play();
		}
		else
		{
			tpath->PathPoints.Add(player->GetNavAgentLocation()+player->direction);
			setState(search);
		}
		
		break;
	default:
		break;
	}

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

	if(!visionCone->detectsActor(player)) return;
	player->isSeen = true;
	if(player->Suspicion>=100.0f) return setState(pursue);
	if(player->currentState==Run) return setState(stare);
	
}

