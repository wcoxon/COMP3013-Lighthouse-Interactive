// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Spawner.h"

#include "CustomerNPC.h"
#include "Game_PaperCharacter.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGame_Spawner::AGame_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnerRootComponent"));
	//SetRootComponent(SceneComponent);
	RootComponent = SceneComponent;

}

// Called when the game starts or when spawned
void AGame_Spawner::BeginPlay()
{
	Super::BeginPlay();

	AGame_PaperCharacter* CharacterPawn = GetWorld()->SpawnActor<AGame_PaperCharacter>( GetActorLocation(), GetActorRotation());
	//CharacterPawn->SetActorScale3D(FVector(PlayerSize));

		
	SavedController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	SavedController->Possess(CharacterPawn);

	customerSpawnCooldown = 5.0f;
	//UE_LOG(LogTemp, Warning, TEXT("Player Controller: %s"), *SavedController->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Player Pawn: %s"), *CharacterPawn->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Player C Character: %s"), *SavedController->GetPawn()->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Player C Character: %s"), *SavedController->GetPawn()->GetActorLocation().ToString());
	
}

// Called every frame
void AGame_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	customerSpawnCooldown-= DeltaTime;
	if(customerSpawnCooldown<0.f && CustomerCount<=30)
	{
		GetWorld()->SpawnActor<ACustomerNPC>( GetActorLocation(), GetActorRotation());
		customerSpawnCooldown = 5.0f;
		CustomerCount+=1;
	}
}

