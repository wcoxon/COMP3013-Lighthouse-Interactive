// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Base.h"

#include "Game_PaperCharacter.h"

// Sets default values
AItem_Base::AItem_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBounds"));
	BoundsBox->SetBoxExtent(FVector(80.0, 270, 200));
	
	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DisplayText"));
	Text->SetupAttachment(BoundsBox);
	Text->SetRelativeLocationAndRotation(BoundsBox->GetComponentLocation() + FVector(0, 0, 240.0), FRotator(40,-90,0));
	Text->SetText(FText::FromString("Press E to Pickup " + ItemName));
	Text->HorizontalAlignment = EHTA_Center;
	Text->XScale = 2;
	Text->YScale = 2;
	Text->SetTextRenderColor(FColor::Black);
	Text->SetVisibility(false);

}

void AItem_Base::onInteract()
{
	
}

void AItem_Base::TextOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AGame_PaperCharacter>(OtherActor))
	{
		Text->SetVisibility(true);
	}
}

void AItem_Base::TextOverlapExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Text->SetVisibility(false);
}

FString AItem_Base::GetItemName()
{
	return ItemName;
}

UStaticMesh* AItem_Base::GetItemMesh()
{
	return ItemMesh;
}

// Called when the game starts or when spawned
void AItem_Base::BeginPlay()
{
	Super::BeginPlay();
	BoundsBox->OnComponentBeginOverlap.AddDynamic(this, &AItem_Base::TextOverlap);
	BoundsBox->OnComponentEndOverlap.AddDynamic(this, &AItem_Base::TextOverlapExit);
	Text->SetText(FText::FromString("Press E to Pickup " + ItemName));
	
}

// Called every frame
void AItem_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

