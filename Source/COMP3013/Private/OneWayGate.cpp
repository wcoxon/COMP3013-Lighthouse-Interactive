// Fill out your copyright notice in the Description page of Project Settings.


#include "OneWayGate.h"

#include "ClerkNPC.h"
#include "CustomerNPC.h"
#include "Game_PaperCharacter.h"
#include "NPCBase.h"

// Sets default values
AOneWayGate::AOneWayGate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate"));
	Gate->SetupAttachment(RootComponent);

	BoundsBoxTop = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBoxTop"));
	BoundsBoxTop->SetupAttachment(RootComponent);

	BoundsBoxBottom = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBoxBottom"));
	BoundsBoxBottom->SetupAttachment(RootComponent);

	BoundsBoxTop->SetBoxExtent(FVector(20,20,20));
	BoundsBoxBottom->SetBoxExtent(FVector(20,20,20));
	
}

// Called when the game starts or when spawned
void AOneWayGate::BeginPlay()
{
	Super::BeginPlay();

	BoundsBoxTop->OnComponentBeginOverlap.AddDynamic(this, &AOneWayGate::BoundBoxTopEnter);
	BoundsBoxTop->OnComponentEndOverlap.AddDynamic(this, &AOneWayGate::BoundBoxTopExit);

	BoundsBoxBottom->OnComponentBeginOverlap.AddDynamic(this, &AOneWayGate::BoundBoxBottomEnter);
	BoundsBoxBottom->OnComponentEndOverlap.AddDynamic(this, &AOneWayGate::BoundBoxBottomExit);

	currentAngle = this->GetActorRotation().Yaw;
	targetAngleUp = currentAngle + OpenAngleUp;
	targetAngleDown = currentAngle - OpenAngleDown;
	defaultAngle = currentAngle;
	
}

// Called every frame
void AOneWayGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (GateState) {
	case  EGateState::Opening:
		if (abs(currentAngle) != abs(targetAngleUp) && Up) {
			TurnTowards(targetAngleUp, DeltaTime);
		}

		else if (abs(currentAngle) != abs(targetAngleDown) && !Up) {
			TurnTowards(targetAngleDown, DeltaTime);
		}
		
		if (abs(currentAngle) == abs(targetAngleUp) || abs(currentAngle) == abs(targetAngleDown)) {
			GateState = EGateState::Open;
			Timer = 2.f;
		}
		break;
	case EGateState::Closing:

		if (currentAngle != defaultAngle) {
			TurnTowards(defaultAngle, DeltaTime);
		}
		else {
			GateState = EGateState::Idle;

		}
		break;
	}
	if (Timer > 0) {
		Timer -= DeltaTime;
		
	}
	if (Timer <= 0 && GateState == EGateState::Open) {
		GateState = EGateState::Closing;
	}
	if (GateState == EGateState::Idle && currentAngle != defaultAngle) {
		GateState = EGateState::Closing;
	}
	
}


void AOneWayGate::onInteract()
{
	
}

void AOneWayGate::BoundBoxTopEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->IsA(ACustomerNPC::StaticClass()) || OtherActor->IsA(AClerkNPC::StaticClass()) || OtherActor->IsA(AGame_PaperCharacter::StaticClass()) || OtherActor->IsA(ASecurityNPC::StaticClass())) {
		if (GateState != EGateState::Open && GateState != EGateState::Opening) {
			GateState = EGateState::Opening;
			Up = true;
		}
	}
	
}

void AOneWayGate::BoundBoxTopExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	Timer = 2.f;
}

void AOneWayGate::BoundBoxBottomEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA(ACustomerNPC::StaticClass()) || OtherActor->IsA(AClerkNPC::StaticClass()) || OtherActor->IsA(AGame_PaperCharacter::StaticClass()) || OtherActor->IsA(ASecurityNPC::StaticClass())) {
		if (GateState != EGateState::Open && GateState != EGateState::Opening) {
			GateState = EGateState::Opening;
			Up = false;
		}
	}
}

void AOneWayGate::BoundBoxBottomExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	Timer = 2.f;
	
	
}

void AOneWayGate::TurnTowards(float target, float DeltaTime) {
	currentAngle = FMath::FixedTurn(currentAngle, target, DeltaTime * 350);
	Gate->SetWorldRotation(FRotator(0, currentAngle, 0));
	
}

