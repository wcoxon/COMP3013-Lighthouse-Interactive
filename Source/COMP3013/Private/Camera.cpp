// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACamera::ACamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsClockwise = FMath::RandBool();
	turnSpeed = 10.f;
	setState(EECameraState::Idle);

	coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	coneLight->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();
	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));

	coneLight->SetInnerConeAngle(FMath::RadiansToDegrees(coneAngle));
	coneLight->bUseInverseSquaredFalloff = false;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneLength);
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	StateManager(DeltaTime);
}

/* Handles different states for the camera component */
void ACamera::StateManager(float deltaTime)
{
	switch (CameraState)
	{
	case EECameraState::Idle:
		IdleCurrentDuration += deltaTime;
		if (IdleCurrentDuration >= IdleDuration)
		{
			IdleCurrentDuration = 0.f;
			setState(EECameraState::Rotating);
		}
		break;
	case EECameraState::Rotating:
		
		if (bIsClockwise)
		{
			turnTowards(Angle1, deltaTime);
			if (FMath::IsNearlyEqual(GetActorRotation().Yaw, Angle1, 0.1f))
			{
				setState(EECameraState::Idle);
			}
		}
		else
		{
			turnTowards(Angle2, deltaTime);
			if (FMath::IsNearlyEqual(GetActorRotation().Yaw, Angle2, 0.1f))
			{
				setState(EECameraState::Idle);
			}
		}
		break;
	case EECameraState::Following:
		break;
	case EECameraState::Off:
		break;
	}
}

void ACamera::setState(EECameraState newState)
{
	switch (CameraState)
	{
	case EECameraState::Idle:
			IdleCurrentDuration = 0.0f;
			//Set duration between 2-5 seconds
			IdleDuration = FMath::FRandRange(2.0f, 5.0f);
			break;
		case EECameraState::Rotating:
			bIsClockwise = !bIsClockwise;
			break;
	}
	CameraState = newState;
}

/* Lerp between the input angle and current angle */
void ACamera::turnTowards(float target, float deltaTime)
{
	CurrentAngle = GetActorRotation().Yaw;
	
	CurrentAngle = FMath::FInterpTo(CurrentAngle, target, deltaTime, turnSpeed);

	SetActorRotation(FRotator(0.f, CurrentAngle, 0.f));
}

/* Check if the camera is detecting the player */
bool ACamera::detectsPlayer()
{
	//check if player is within vision distance
	if(FVector::Distance(player->GetActorLocation(),GetActorLocation())>coneLength)
	{
		return false;
	}

	//check if player is within vision cone
	const FVector playerDisplacement = player->GetActorLocation()-GetActorLocation();
	if(abs(FMath::FindDeltaAngleRadians(playerDisplacement.HeadingAngle(),GetActorRotation().Yaw))>coneAngle)
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