// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_PaperCharacter.h"

#include "Item_Base.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FramePro/FramePro.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerInvComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Called when the game starts or when spawned
AGame_PaperCharacter::AGame_PaperCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Construct Components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Inventory = CreateDefaultSubobject<UPlayerInvComponent>(TEXT("Inventory"));
	Mesh_HeldItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldItem"));

	//Attach Components
	SpringArm->SetupAttachment(CharacterCollider);
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	Mesh_HeldItem->SetupAttachment(CharacterCollider);
	
	//Spring Arm Settings
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, -30.0f, 30.0f), FRotator(-40.0f, 90.0f, 0.0f));
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;
	SpringArm->bDoCollisionTest = false;

	//HeldItemLocation
	Mesh_HeldItem->SetRelativeLocationAndRotation(FVector(0, 0, 15), FRotator(0, 0, 90));
	Mesh_HeldItem->SetRelativeScale3D(FVector(0.15f,0.15f,0.15f));
	Mesh_HeldItem->CastShadow = false;

	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	CharacterCollider->SetRenderCustomDepth(true);
	//CharacterFlipbook->BoundsScale = 1.0f;
	
	//Collider Settings
	CharacterCollider->SetCapsuleRadius(25.0f);

	//Movement System Settings
	moveSpeed=500;
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
	CharacterMovementComp->MaxAcceleration = 8000.0f;
	CharacterMovementComp->BrakingFrictionFactor = 50.0f;

	//Assign HUD element
	static ConstructorHelpers::FClassFinder<UUserWidget> hudWidgetObj (TEXT ("/Game/UserInterface/WIDGET_Inventory"));
	if (hudWidgetObj.Succeeded ()) HUDWidgetClass = hudWidgetObj.Class;
	else HUDWidgetClass = nullptr;
	
}

void AGame_PaperCharacter::Destroy(UItem* Item)
{
	if (Item) Item->Destroy(this);
}

void AGame_PaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Grab character animations
	IdleDownAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunSpriteSheet97.RunSpriteSheet97"), NULL, LOAD_None, NULL);
	IdleUpAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/UpIdle_Anim.UpIdle_Anim"), NULL, LOAD_None, NULL);
	IdleLeftAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/LeftIdle_Anim.LeftIdle_Anim"), NULL, LOAD_None, NULL);
	IdleRightAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/ThirdParty/PrototypeAssets/RightIdle_Anim.RightIdle_Anim"), NULL, LOAD_None, NULL);
	
	MovingDownAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunSpriteSheet97.RunSpriteSheet97"), NULL, LOAD_None, NULL);
	MovingUpAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunSpriteSheet99.RunSpriteSheet99"), NULL, LOAD_None, NULL);
	MovingLeftAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunSpriteSheet98.RunSpriteSheet98"), NULL, LOAD_None, NULL);
	MovingRightAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunSpriteSheet100.RunSpriteSheet100"), NULL, LOAD_None, NULL);
	
	CharacterFlipbook->SetFlipbook(IdleDownAnim);
	CharacterFlipbook->CastShadow = true;

	//rescale the actor by 3.5
	float actorScale = 3.5;
	//this sprite is 96x96
	float spriteRes = 96;
	//the player animation frames are actually like 19 pixels above the bottom of the sprite, so this shifts it down
	//to touch the floor
	float spriteBottomMargin = 19;
	//fix collider
	CharacterCollider->SetCapsuleHalfHeight(spriteRes/2.0);
	SetActorScale3D(FVector(actorScale));
	
	//Make character face camera at all times
	FRotator SpringArmRotation = SpringArm->GetComponentRotation();
	SpringArmRotation.Roll = -SpringArmRotation.Pitch;
	SpringArmRotation.Yaw = 0.0f;
	SpringArmRotation.Pitch = 0.0f;
	CharacterCollider->SetCapsuleHalfHeight((spriteRes/2.0 - spriteBottomMargin) - cos(-SpringArmRotation.Pitch));
	CharacterFlipbook->SetWorldRotation(SpringArmRotation);
	//info about the animation so i can sync the strides with the movement speed (avoids feet sliding)
	float stridePixels = 75;
	float strideFrames = 10;
	CharacterFlipbook->SetPlayRate(moveSpeed/(15*actorScale*stridePixels/strideFrames));
}

void AGame_PaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	switch(PlayerDirection)
	{
	case Direction::MovingUp:
		if (CurrentVelocity.X == 0 && CurrentVelocity.Y == 0)
		{
			PlayerDirection = Direction::Up;
			CharacterFlipbook->SetFlipbook(IdleUpAnim);
		}
		else CharacterFlipbook->SetFlipbook(MovingUpAnim);
		break;
	case Direction::MovingLeft:
		if (CurrentVelocity.X == 0 && CurrentVelocity.Y == 0)
		{
			PlayerDirection = Direction::Left;
			CharacterFlipbook->SetFlipbook(IdleLeftAnim);
		}
		else CharacterFlipbook->SetFlipbook(MovingLeftAnim);
		break;
	case Direction::MovingRight:
		if (CurrentVelocity.X == 0 && CurrentVelocity.Y == 0)
		{
			PlayerDirection = Direction::Right;
			CharacterFlipbook->SetFlipbook(IdleRightAnim);
		}
		else CharacterFlipbook->SetFlipbook(MovingRightAnim);
		break;
	case Direction::MovingDown:
		if (CurrentVelocity.X == 0 && CurrentVelocity.Y == 0)
		{
			PlayerDirection = Direction::Down;
			CharacterFlipbook->SetFlipbook(IdleDownAnim);
		}
		else CharacterFlipbook->SetFlipbook(MovingDownAnim);
		break;
	}
}

//UNREAL DEFAULT FUNCTION - Bind inputs to functions
void AGame_PaperCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponents)
{
	Super::SetupPlayerInputComponent(InputComponents);
	
	// Bind the controls
	InputComponents->BindAction("Pickup", IE_Pressed, this, &AGame_PaperCharacter::Pickup);
	InputComponents->BindAction("Inventory", IE_Pressed, this, &AGame_PaperCharacter::OpenInventory);
	InputComponents->BindAction("Conceal", IE_Pressed, this, &AGame_PaperCharacter::Conceal);
	InputComponents->BindAxis("MoveX", this, &AGame_PaperCharacter::Move_XAxis);
	InputComponents->BindAxis("MoveY", this, &AGame_PaperCharacter::Move_YAxis);
}

//Player movement functions for XY
void AGame_PaperCharacter::Move_XAxis(float AxisValue)
{
	if (AxisValue) PlayerDirection = Direction::MovingRight;
	if (AxisValue == -1) PlayerDirection = Direction::MovingLeft;
	
	// Move at 100 units per second forward or backward
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * moveSpeed;
	this->AddMovementInput(this->GetActorForwardVector() * CurrentVelocity.X);
}

void AGame_PaperCharacter::Move_YAxis(float AxisValue)
{
	if (AxisValue) PlayerDirection = Direction::MovingUp;
	if (AxisValue == -1) PlayerDirection = Direction::MovingDown;
	
	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * moveSpeed;
	this->AddMovementInput(this->GetActorRightVector() * CurrentVelocity.Y);
}

/** When player in item_base zone, place item in held_item */
void AGame_PaperCharacter::Pickup()
{
	PickupItemEvent.Broadcast();
	if (Current_HeldItem == nullptr)
	{
		TArray<AActor*> Result;
		GetOverlappingActors(Result, AItem_Base::StaticClass());
		for (auto Actor : Result)
		{
			if (UKismetSystemLibrary::DoesImplementInterface(Actor, UInteraction::StaticClass()))
			{
				Mesh_HeldItem->SetVisibility(true);
				AItem_Base* CurrentItem = Cast<AItem_Base>(Actor);
				Current_HeldItem = CurrentItem->ItemToGive;
				Mesh_HeldItem->SetStaticMesh(CurrentItem->ItemToGive->Mesh);
				PickupItemEvent.Broadcast();
				break;
			}
		}
	}
}

/** Opens inventory of player */
void AGame_PaperCharacter::OpenInventory()
{
	if (HUDWidgetClass)
	{
		//Create inventory widget + swap inputs and flush inputs
		HUDWidgetMain = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		HUDWidgetMain->AddToViewport();
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerInput->FlushPressedKeys();
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeUIOnly());
	}
}

/** Player placing item in their inventory */
void AGame_PaperCharacter::Conceal()
{
	if (Current_HeldItem != nullptr && Inventory->Capacity > Inventory->Items.Num())
	{
		Inventory->AddItem(Current_HeldItem);
		Current_HeldItem = nullptr;
		PickupItemEvent.Broadcast();
		Mesh_HeldItem->SetVisibility(false);
	}
}
