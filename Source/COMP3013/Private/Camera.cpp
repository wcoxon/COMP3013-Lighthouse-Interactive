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

	//We dont want to call setstate in the constructor
	IdleCurrentDuration = 0.0f;
	IdleDuration = FMath::FRandRange(MinWait, MaxWait);
	CameraState = EECameraState::Idle;

	currentUnfollowingDuration = 0.0f;
	maxUnfollowDuration = 2.f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	CameraBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBase"));
	CameraBase->SetupAttachment(RootComponent);
	CameraBase->SetMobility(EComponentMobility::Movable);
	
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(CameraBase);
	
	coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	coneLight->SetupAttachment(CameraRoot);

	CameraBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBody"));
	CameraBody->SetupAttachment(CameraRoot);
}

void ACamera::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	CameraBase->SetWorldTransform(Transform);
	//CameraRoot->SetWorldTransform(Transform);
	//coneLight->SetWorldTransform(Transform);
	//CameraBody->SetWorldTransform(Transform);
	
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();
	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));

	coneLight->SetInnerConeAngle(coneAngle);
	coneLight->bUseInverseSquaredFalloff = false;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneLength);
	coneLight->SetOuterConeAngle(coneAngle + 0.1f);
	//coneLight->CastShadows = false;
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (detectsPlayer()) setState(EECameraState::Following);
	
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
			turnTowards(Angle1, deltaTime, turnSpeed);
			if (FMath::IsNearlyEqual(CameraRoot->GetComponentRotation().Yaw, Angle1, 0.1f))
			{
				setState(EECameraState::Idle);
			}
		}
		else
		{
			turnTowards(Angle2, deltaTime, turnSpeed);
			if (FMath::IsNearlyEqual(CameraRoot->GetComponentRotation().Yaw, Angle2, 0.1f))
			{
				setState(EECameraState::Idle);
			}
		}
		break;
	case EECameraState::Following:
		if (detectsPlayer()) {
			FVector cameraLocation = CameraRoot->GetComponentLocation();
			FVector playerLocation = player->GetActorLocation();
			FVector toPlayer = playerLocation - cameraLocation;
			float angle = FMath::RadiansToDegrees(FMath::Atan2(toPlayer.Y, toPlayer.X));
			turnTowards(angle, deltaTime, 45.f);
			currentUnfollowingDuration = 0.f;
			player->DetectionCheck(deltaTime);
		} else {
			currentUnfollowingDuration += deltaTime;
			if (currentUnfollowingDuration >= maxUnfollowDuration) {
				setState(EECameraState::Rotating);
				currentUnfollowingDuration = 0.f;
			}
		}
			
		break;
	case EECameraState::Off:
		break;
	}
}

void ACamera::setState(EECameraState newState)
{
	coneLight->SetLightColor(FLinearColor::White);
	CameraState = newState;
	switch (CameraState)
	{
	case EECameraState::Idle:
		IdleCurrentDuration = 0.0f;
		//Set duration between Min/Max
		IdleDuration = FMath::FRandRange(MinWait, MaxWait);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Idle Duration: %f"), IdleDuration));
		break;
	case EECameraState::Rotating:
		bIsClockwise = !bIsClockwise;
		break;
	case EECameraState::Following:
		coneLight->SetLightColor(FLinearColor::Blue);
		break;
	}
}

/* Lerp between the input angle and current angle */
void ACamera::turnTowards(float target, float deltaTime, float speed)
{
	CurrentAngle = CameraRoot->GetComponentRotation().Yaw;
	CurrentAngle = FMath::FixedTurn(CurrentAngle, target, deltaTime * speed);
	CameraRoot->SetWorldRotation(FRotator(-20.f, CurrentAngle, 0.f));
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
	FVector forward = CameraRoot->GetForwardVector();
	FVector toPlayer = player->GetActorLocation() - CameraRoot->GetComponentLocation();
	toPlayer.Normalize();
	float angle = FMath::Acos(FVector::DotProduct(forward, toPlayer));
	if (angle > FMath::DegreesToRadians(coneAngle))
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