// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitZone.h"

#include "Game_PaperCharacter.h"
#include "MainGameInstance.h"
#include "PlayerInvComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AExitZone::AExitZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	BoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBox"));
	BoundsBox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AExitZone::BeginPlay()
{
	Super::BeginPlay();

	BoundsBox->OnComponentBeginOverlap.AddDynamic(this, &AExitZone::BoundBoxEnter);
	
}

// Called every frame
void AExitZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExitZone::onInteract() {
}

void AExitZone::BoundBoxEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA(AGame_PaperCharacter::StaticClass())) {
		UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
		float valuables = 0.0f;
		for (auto Item : Cast<AGame_PaperCharacter>(OtherActor)->Inventory->Items) {
			valuables += Item->Price;
		}
		if (GI) {
			GI->AddMoney(valuables);
			GI->SetCaughtState(false);
		}
		UGameplayStatics::OpenLevel(GetWorld(), "TransitionLevel", true);
	}
}

