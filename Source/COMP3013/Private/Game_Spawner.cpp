// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Spawner.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGame_Spawner::AGame_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGame_Spawner::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		FVector SpawnPoint = GetActorLocation();
		FRotator SpawnRotate = GetActorRotation();

		APawn* CharacterPawn = GetWorld()->SpawnActor<APawn>(Character, SpawnPoint, SpawnRotate);
		CharacterPawn->SetActorScale3D(FVector(PlayerSize));

		SavedController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		SavedController->Possess(CharacterPawn);
		
	}
	
}

// Called every frame
void AGame_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

