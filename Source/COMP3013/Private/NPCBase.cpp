// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

#include <string>
#include <valarray>

#include "NavigationPath.h"
#include "PaperFlipbook.h"
#include "RenderGraphResources.h"
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
	CharacterCollider->SetRenderCustomDepth(true);
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
	CharacterCollider->SetCapsuleHalfHeight(6.6f);
	SetActorScale3D(FVector(10));
	CharacterFlipbook->SetWorldRotation(FRotator(0.0f, 0.0f, 40.0f));
	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	//player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(),AGame_PaperCharacter::StaticClass()));
	
	player->PickupItemEvent.__Internal_AddDynamic(this,&ANPCBase::hearPlayer,TEXT("hearPlayer"));
	
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	tpath=navSys->FindPathToLocationSynchronously(GetWorld(),GetNavAgentLocation(),player->GetNavAgentLocation());

	//coneLight->SetRelativeLocation(FVector(0,0,-6));
	coneLight->SetInnerConeAngle(coneAngle);
	coneLight->bUseInverseSquaredFalloff = 0;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneRadius);
	//coneLight = NewObject<USpotLightComponent>();
	//coneLight->SetupAttachment(CharacterCollider);
	//coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotlightComponent"));
	
}

void ANPCBase::moveTowardsPoint(FVector point,float distance)
{
	FVector3d displacement = point - GetNavAgentLocation();
	if(displacement.Length()<distance)
	{
		SetActorLocation(GetActorLocation()+ FVector3d(1,1,0)*displacement);
		return;
	}
	FVector3d direction = displacement.GetSafeNormal();
	coneDirection=FVector2d(direction);
	coneLight->SetRelativeRotation(FRotator(0,FMath::RadiansToDegrees(std::atan2(direction.Y,direction.X)),0));
	SetActorLocation(GetActorLocation()+ FVector3d(1,1,0)*(direction*distance));
}

void ANPCBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(detectsPlayer())
	{
		//currentState = seesPlayer;
		CharacterFlipbook->SetSpriteColor(FLinearColor(1,0.0,0.0));
	}
	else
	{
		//currentState = playerHidden;
		CharacterFlipbook->SetSpriteColor(FLinearColor(1,1,1));
	}
	if(tpath->PathPoints.Num()>1)
	{
		moveTowardsPoint(tpath->PathPoints[1],moveSpeed*DeltaSeconds);
		if(FVector2d::Distance(FVector2d(GetNavAgentLocation()),FVector2d(tpath->PathPoints[1]))<1)
		{
			tpath->PathPoints.RemoveAt(0);
		}
	}
}


