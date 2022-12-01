// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

#include <string>
#include <valarray>
#include "PaperFlipbook.h"
#include "Components/CapsuleComponent.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Engine/World.h"


ANPCBase::ANPCBase()
{
	coneRadius = 1000.0;
	coneAngle = 45.0;
	//coneDirection = FVector2D(0.0,1.0);
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
	if(abs(std::atan2(player->GetActorLocation().Y-GetActorLocation().Y,player->GetActorLocation().X-GetActorLocation().X)-std::atan2(coneDirection.Y,coneDirection.X))>0.523598775598)
	{
		return false;
	}
	FHitResult hit;
	//const FName TraceTag("MyTraceTag");

	//GetWorld()->DebugDrawTraceTag = TraceTag;

	//FCollisionQueryParams CollisionParams;
	//CollisionParams.TraceTag = TraceTag;
	bool actorHit = GetWorld()->LineTraceSingleByChannel(hit,GetActorLocation(),player->GetActorLocation(),ECC_Visibility,FCollisionQueryParams(),FCollisionResponseParams());
	
	if(actorHit && hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("HIT %s"),*hit.GetActor()->GetName());
		return false;
	}
	return true;
	
}


void ANPCBase::BeginPlay()
{
	Super::BeginPlay();
	CharacterCollider->SetCapsuleHalfHeight(6.6f);
	SetActorScale3D(FVector(10));
	CharacterFlipbook->SetWorldRotation(FRotator(0.0f, 0.0f, 40.0f));
	player = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
}


void ANPCBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(detectsPlayer())
	{
		currentState = seesPlayer;
		CharacterFlipbook->SetSpriteColor(FLinearColor(1,0.0,0.0));
	}
	else
	{
		currentState = playerHidden;
		CharacterFlipbook->SetSpriteColor(FLinearColor(1,1,1));
	}
}


