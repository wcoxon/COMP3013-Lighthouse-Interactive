// Fill out your copyright notice in the Description page of Project Settings.


#include "OneWayGate.h"

// Sets default values
AOneWayGate::AOneWayGate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(Root);
	
	ColliderSize = FVector(60.0f, 50.0f, 30.0f);
	TriggerSize = FVector(60.0f, 50.0f, 30.0f);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetupAttachment(Root);
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	BoxTrigger->SetupAttachment(Root);
	
	BoxCollider->SetBoxExtent(ColliderSize);
	BoxTrigger->SetBoxExtent(TriggerSize);

	Player = CreateDefaultSubobject<AActor>(TEXT("Player"));
	
	GateLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Gate"));
	GateLeft->SetupAttachment(Root);
	GateRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Gate"));
	GateRight->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void AOneWayGate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOneWayGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(BoxTrigger->IsOverlappingActor(Player))
	{
		BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}

