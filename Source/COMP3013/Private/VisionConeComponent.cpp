// Fill out your copyright notice in the Description page of Project Settings.


#include "VisionConeComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values for this component's properties
UVisionConeComponent::UVisionConeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = true;
	
	coneDirection=FVector(1,0,0);
	coneLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotlightComp"));
	coneLight->SetupAttachment(this);

	
	// ...
}
bool UVisionConeComponent::detectsActor(AActor* actor)
{
	const FVector displacement = actor->GetActorLocation()-GetComponentLocation();
	
	//check if actor is within vision distance
	if(displacement.Length()>coneRadius)
	{
		return false;
	}

	//if youre in their personal space they will detect u
	if(displacement.Length()<100) return true;
	
	//check if player is within vision angle
	//if(abs(FMath::FindDeltaAngleRadians(displacement.HeadingAngle(),coneDirection.HeadingAngle()))>coneAngle)
	if(FMath::Acos(coneDirection.Dot(displacement.GetSafeNormal()))>coneAngle)
	{
		return false;
	}

	
	//check for obstacles using raycast
	FHitResult hit;
	const bool actorHit = GetWorld()->LineTraceSingleByChannel(hit,GetComponentLocation(),actor->GetActorLocation(),ECC_Visibility,FCollisionQueryParams(),FCollisionResponseParams());
	if(actorHit && hit.GetActor())
	{
		return false;
	}
	
	DrawDebugLine(
			GetWorld(),
			actor->GetActorLocation()-FVector::UpVector*100,
			actor->GetActorLocation()-displacement.GetSafeNormal()*75-FVector::UpVector*100,
			coneLight->LightColor,
			false, -1, 0,
			12.333
		);
	
	return true;
}
TArray<AActor*> UVisionConeComponent::getVisibleActors(UClass* Type)
{
	TArray<AActor*> actorsInRange;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetComponentLocation(),
		coneRadius,
		{},
		Type,
		{},
		actorsInRange);
	
	TArray<AActor*> LOSActorsInRange;
	for(AActor* a : actorsInRange)
	{
		//UE_LOG(LogTemp, Log, TEXT("OverlappedActor: %s"), *a->GetName());
		if(detectsActor(a))
		{
			LOSActorsInRange.Add(a);
		}
	}
	return LOSActorsInRange;
	
}

// Called when the game starts
void UVisionConeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	coneLight->SetInnerConeAngle(FMath::RadiansToDegrees(coneAngle));
	coneLight->SetOuterConeAngle(FMath::RadiansToDegrees(coneAngle));
	coneLight->bUseInverseSquaredFalloff = 0;
	coneLight->SetLightFalloffExponent(0.25);
	coneLight->SetIntensity(5);
	coneLight->SetAttenuationRadius(coneRadius);
	coneLight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	//turning off lights for now
	//coneLight->SetIntensity(0);
	// ...
	
}


// Called every frame
void UVisionConeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

