// Fill out your copyright notice in the Description page of Project Settings.


#include "EntryDoors.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AEntryDoors::AEntryDoors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
}

void AEntryDoors::OnFSusMaxBroadcast() {
	DoorMesh->SetWorldRotation(FRotator(0,DoorCloseAngle,0));
}

void AEntryDoors::OnFEGCOverBroadcast() {
	DoorMesh->SetWorldRotation(FRotator(0,DoorOpenAngle,0));
}

// Called when the game starts or when spawned
void AEntryDoors::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AGame_PaperCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	player->SusMaxEvent.AddDynamic(this, &AEntryDoors::OnFSusMaxBroadcast);
	player->EGCOverEvent.AddDynamic(this, &AEntryDoors::OnFEGCOverBroadcast);
}

// Called every frame
void AEntryDoors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

