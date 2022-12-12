// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

#include <valarray>
#include "PaperFlipbookComponent.h"
#include "NavigationPath.h"

#include "PaperFlipbook.h"
#include "VectorTypes.h"
#include "Components/CapsuleComponent.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Engine/World.h"


ANPCBase::ANPCBase()
{
	coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	coneLight->SetupAttachment(CharacterCollider);
	coneRadius = 1000.0;
	coneAngle = 45.0;
	moveSpeed = 400;
	CharacterCollider->SetCapsuleRadius(6.6f);
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	CharacterFlipbook->BoundsScale = 10.0f;
	currentState = playerHidden;
}

bool ANPCBase::detectsPlayer()
{
	if(FVector::Distance(player->GetActorLocation(),GetActorLocation())>coneRadius)
	{
		return false;
	}
	float test = FMath::RadiansToDegrees(abs(std::atan2(player->GetActorLocation().Y-GetActorLocation().Y,player->GetActorLocation().X-GetActorLocation().X)-std::atan2(coneDirection.Y,coneDirection.X)));
	
	if(test>coneAngle && test< 360-coneAngle)
	{
		UE_LOG(LogTemp, Warning, TEXT("%f > %f OUTSIDE ANGLE"),test,coneAngle);
		return false;
	}
	FHitResult hit;
	
	bool actorHit = GetWorld()->LineTraceSingleByChannel(hit,GetActorLocation(),player->GetActorLocation(),ECC_Visibility,FCollisionQueryParams(),FCollisionResponseParams());
	
	if(actorHit && hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("OBSTACLE"),test);
		return false;
	}

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),player->GetNavAgentLocation());

	return true;
}

void ANPCBase::hearPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("hears player"));
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),player->GetNavAgentLocation());
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();
	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/WalkLeft_Anim.WalkLeft_Anim"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/WalkRight_Anim.WalkRight_Anim"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/WalkUp_Anim.WalkUp_Anim"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/WalkDown_Anim.WalkDown_Anim"), NULL, LOAD_None, NULL));

	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/LeftIdle_Anim.LeftIdle_Anim"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/RightIdle_Anim.RightIdle_Anim"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/UpIdle_Anim.UpIdle_Anim"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/Idle_Anim.Idle_Anim"), NULL, LOAD_None, NULL));

	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	
	player->PickupItemEvent.__Internal_AddDynamic(this,&ANPCBase::hearPlayer,TEXT("hearPlayer"));
	
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),player->GetNavAgentLocation());

	coneLight->SetInnerConeAngle(coneAngle);
	coneLight->bUseInverseSquaredFalloff = 0;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneRadius);
	
}

void ANPCBase::moveTowards(FVector destination,float distance)
{
	FVector3d displacement = destination - GetNavAgentLocation();
	if(displacement.Length()<distance)
	{
		SetActorLocation(GetActorLocation()+ FVector3d(1,1,0)*displacement);
		return;
	}
	//FVector3d direction = displacement.GetSafeNormal();
	direction = displacement.GetSafeNormal2D();
	coneDirection=FVector2d(direction);
	coneLight->SetRelativeRotation(FRotator(0,FMath::RadiansToDegrees(std::atan2(direction.Y,direction.X)),0));
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
	SetActorLocation(GetActorLocation()+ FVector3d(1,1,0)*(direction*distance));
}

void ANPCBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(detectsPlayer())
	{
		//currentState = seesPlayer;
		coneLight->SetLightColor(FLinearColor(1,0.0,0.0));
	}
	else
	{
		//currentState = playerHidden;
		coneLight->SetLightColor(FLinearColor(1,1,1));
	}
	if(tpath->PathPoints.Num()>1)
	{
		moveTowards(tpath->PathPoints[1],moveSpeed*DeltaSeconds);
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


