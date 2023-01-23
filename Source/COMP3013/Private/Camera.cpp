// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"
#include "Game_PaperCharacter.h"
#include "CustomerNPC.h"
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

	CameraBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBody"));
	CameraBody->SetupAttachment(CameraBase);

	visionCone= CreateDefaultSubobject<UVisionConeComponent>(TEXT("VisionCone"));
	visionCone->SetupAttachment(CameraBody);
	
	visionCone->coneRadius = 1500.f;
	visionCone->coneAngle = FMath::DegreesToRadians(30);
	
}

void ACamera::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	
}

void ACamera::playerCrimeCommitted()
{
	if(visionCone->detectsActor(player))
	{
		int visibleCustomers = visionCone->getVisibleActors(ACustomerNPC::StaticClass()).Num();
		//chance of being incriminated decreases from customers also visible to the npc
		//if it rolls 0 the player gets seen stealing, the more customers around the less likely
		//halved the scale because it's a little broken if 4 customers gives you a 1/5 chance of being sussed on
		bool sawCrime = FMath::RandRange(0, visibleCustomers/2)==0?true:false;
		
		if(sawCrime)
		{
			player->Suspicion=100;
		}
	}
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	securityGuard = Cast<ASecurityNPC>(UGameplayStatics::GetActorOfClass(GetWorld(),ASecurityNPC::StaticClass()));
	visionCone->coneLight->SetIntensity(0.f);
	//player->ConcealItemEvent.AddDynamic(this,&ACamera::playerCrimeCommitted);

	//coneLight->CastShadows = false;

	
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (visionCone->detectsActor(player)) setState(EECameraState::Following);
	
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
			if (FMath::IsNearlyEqual(CameraBody->GetRelativeRotation().Yaw, Angle1, 0.1f))
			{
				setState(EECameraState::Idle);
			}
		}
		else
		{
			turnTowards(Angle2, deltaTime, turnSpeed);
			if (FMath::IsNearlyEqual(CameraBody->GetRelativeRotation().Yaw, Angle2, 0.1f))
			{
				setState(EECameraState::Idle);
			}
		}
		break;
	case EECameraState::Following:
		if (visionCone->detectsActor(player)) {
			FVector cameraLocation = CameraBody->GetComponentLocation();
			FVector playerLocation = player->GetActorLocation();
			FVector toPlayer = playerLocation - cameraLocation;
			float angle = FMath::RadiansToDegrees(FMath::Atan2(toPlayer.Y, toPlayer.X));
			turnTowards(angle-GetActorRotation().Yaw, deltaTime, 45.f);
			currentUnfollowingDuration = 0.f;
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
	visionCone->coneLight->SetLightColor(FLinearColor::White);
	CameraState = newState;
	switch (CameraState)
	{
	case EECameraState::Idle:
		IdleCurrentDuration = 0.0f;
		//Set duration between Min/Max
		IdleDuration = FMath::FRandRange(MinWait, MaxWait);
		break;
	case EECameraState::Rotating:
		bIsClockwise = !bIsClockwise;
		break;
	case EECameraState::Following:
		
		player->isSeen=true;
		if(player->Suspicion>=100)
		{
			securityGuard->pathToTarget(player->GetActorLocation());
			securityGuard->setState(pursue);
			visionCone->coneLight->SetLightColor(FLinearColor::Red);
			break;
		}
		visionCone->coneLight->SetLightColor(FLinearColor::Blue);
		break;
	}
}

/* Lerp between the input angle and current angle */
void ACamera::turnTowards(float target, float deltaTime, float speed)
{
	CurrentAngle = CameraBody->GetRelativeRotation().Yaw;
	CurrentAngle = FMath::FixedTurn(CurrentAngle, target, deltaTime * speed);
	CameraBody->SetRelativeRotation(FRotator(-20.f, CurrentAngle, 0.f));
	visionCone->coneDirection = CameraBody->GetForwardVector();
}


/* Check if the camera is detecting the player */
/*bool ACamera::detectsPlayer()
{
	//check if player is within vision distance
	if(FVector::Distance(player->GetActorLocation(),GetActorLocation())>visionCone->coneRadius)
	{
		return false;
	}

	//check if player is within vision cone
	FVector forward = CameraRoot->GetForwardVector();
	FVector toPlayer = player->GetActorLocation() - CameraRoot->GetComponentLocation();
	toPlayer.Normalize();
	float angle = FMath::Acos(FVector::DotProduct(forward, toPlayer));
	if (angle > FMath::DegreesToRadians(visionCone->coneAngle))
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
}*/