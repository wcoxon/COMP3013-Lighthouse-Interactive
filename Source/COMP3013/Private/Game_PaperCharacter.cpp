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
#include "Components/AudioComponent.h"
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
	audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));

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
	Mesh_HeldItem->SetRelativeLocationAndRotation(FVector(0, 0, 37), FRotator(0, 0, 90));
	Mesh_HeldItem->SetRelativeScale3D(FVector(0.15f,0.15f,0.15f));
	Mesh_HeldItem->CastShadow = false;
	
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	CharacterCollider->SetRenderCustomDepth(true);
	
	//Collider Settings
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Movement System Settings
	moveSpeed=800;
	WalkSpeed=500.0f;
	CharacterMovementComp->MovementMode=MOVE_Walking;
	CharacterMovementComp->MaxWalkSpeed = WalkSpeed;
	CharacterMovementComp->MaxAcceleration = 8000.0f;
	CharacterMovementComp->BrakingFrictionFactor = 2.0f;
	CharacterMovementComp->BrakingDecelerationWalking = 1200.0f;

	//Player Defaults
	SusMeter = 0.0f;
	TimeConcealing = 0.0f;
	TimeToConceal = 3.0f;
	isSeen = false;
	
	//Assign HUD element
	static ConstructorHelpers::FClassFinder<UUserWidget> hudWidgetObj (TEXT ("/Game/UserInterface/WIDGET_Inventory"));
	if (hudWidgetObj.Succeeded ()) HUDWidgetClass = hudWidgetObj.Class;
	else HUDWidgetClass = nullptr;
	
	direction = FVector::UpVector;
}

void AGame_PaperCharacter::Destroy(UItem* Item)
{
	if (Item) Item->Destroy(this);
}

void AGame_PaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Grab character animations
	animations.Add(FString("runLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunLeft64.PlayerRunLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunRight64.PlayerRunRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunUp64.PlayerRunUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunDown64.PlayerRunDown64"), NULL, LOAD_None, NULL));

	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkLeft64.PlayerWalkLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkRight64.PlayerWalkRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkUp64.PlayerWalkUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkDown64.PlayerWalkDown64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleLeft64.PlayerIdleLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleRight64.PlayerIdleRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleUp64.PlayerIdleUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleDown64.PlayerIdleDown64"), NULL, LOAD_None, NULL));

	CharacterFlipbook->SetFlipbook(animations["idleUp"]);

	PostProcess = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	
	float spriteRes = 64.0;
	float spriteGroundLevel = 5.0;
	
	float actorScale = 300.0;
	
	float spriteRoll = FMath::DegreesToRadians(CharacterFlipbook->GetComponentRotation().Roll);
	
	float halfHeight = cos(spriteRoll)*(spriteRes/2-spriteGroundLevel);
	
	CharacterCollider->SetCapsuleHalfHeight(halfHeight);
	
	SetActorScale3D(FVector(actorScale/spriteRes));

	audioSource->Sound = LoadObject<USoundBase>(NULL,TEXT("/Game/ThirdParty/Sounds/footstep.footstep"),NULL,LOAD_None,NULL);
}


void AGame_PaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Defaults
	isSeen = false;
	mPreviousState = mPlayerState;
	if(inputVector.Length()>0)
	{
		this->AddMovementInput(inputVector.GetSafeNormal2D());
		direction=CharacterMovementComp->Velocity.GetSafeNormal2D();
	}
	if(CharacterMovementComp->Velocity.Length()>moveSpeed*0.9) mPlayerState = EEPlayerState::Running;
	else if(CharacterMovementComp->Velocity.Length() > 0) mPlayerState = EEPlayerState::Walking;
	if(CharacterMovementComp->Velocity.Length() == 0 && mPlayerState != EEPlayerState::Concealing) mPlayerState = EEPlayerState::Idle;


	if (mPreviousState != mPlayerState && mPreviousState == EEPlayerState::Concealing) {
		TimeConcealing = 0;
		InteractionBarEvent.Broadcast();
	}
	StateManager(DeltaTime);
	endGamePass();
}

//UNREAL DEFAULT FUNCTION - Bind inputs to functions
void AGame_PaperCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponents)
{
	Super::SetupPlayerInputComponent(InputComponents);
	// Bind the controls
	InputComponents->BindAction("Pickup", IE_Pressed, this, &AGame_PaperCharacter::Pickup);
	InputComponents->BindAction("Inventory", IE_Pressed, this, &AGame_PaperCharacter::OpenInventory);
	InputComponents->BindAction("Conceal", IE_Pressed, this, &AGame_PaperCharacter::Conceal);
	InputComponents->BindAction("Sprint", IE_Pressed, this, &AGame_PaperCharacter::SprintOn);
	InputComponents->BindAction("Sprint", IE_Released, this, &AGame_PaperCharacter::SprintOff);
	InputComponents->BindAxis("MoveX", this, &AGame_PaperCharacter::Move_XAxis);
	InputComponents->BindAxis("MoveY", this, &AGame_PaperCharacter::Move_YAxis);
}

//Player movement functions for XY
void AGame_PaperCharacter::Move_XAxis(float AxisValue)
{
	if (AxisValue) PlayerDirection = Direction::MovingRight;
	if (AxisValue == -1) PlayerDirection = Direction::MovingLeft;
	
	inputVector.X = AxisValue;
}

void AGame_PaperCharacter::Move_YAxis(float AxisValue)
{
	if (AxisValue) PlayerDirection = Direction::MovingUp;
	if (AxisValue == -1) PlayerDirection = Direction::MovingDown;
	
	inputVector.Y = AxisValue;
}

void AGame_PaperCharacter::SprintOn() {
	CharacterMovementComp->MaxWalkSpeed = moveSpeed;
}

void AGame_PaperCharacter::SprintOff() {
	CharacterMovementComp->MaxWalkSpeed = WalkSpeed;
}

/** When player in item_base zone, place item in held_item */
void AGame_PaperCharacter::Pickup()
{
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
				RefreshItemHUDEvent.Broadcast();
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
	if (Current_HeldItem != nullptr && Inventory->Capacity > Inventory->Items.Num() && mPlayerState != EEPlayerState::Concealing)
	{
		mPlayerState = EEPlayerState::Concealing;
	}
}

/** Handles actions based on PlayerState */
void AGame_PaperCharacter::StateManager(float deltatime) {

	float animationProgress;
	switch (mPlayerState) {
	case EEPlayerState::Idle:
		setDirectionalAnimation(direction,"idle");
		CharacterFlipbook->SetPlayRate(1);
		break;
	case EEPlayerState::Walking:
		animationProgress = CharacterFlipbook->GetPlaybackPosition();
		setDirectionalAnimation(direction,"walk");
		setAnimationRateToSpeed(CharacterFlipbook,CharacterMovementComp->Velocity.Length(),300);
		CharacterFlipbook->SetPlaybackPosition(animationProgress,false);
		if(CharacterFlipbook->GetPlaybackPositionInFrames()==11 || CharacterFlipbook->GetPlaybackPositionInFrames()==26)
		{
			audioSource->Stop();
			audioSource->SetPitchMultiplier(FMath::FRandRange(1.1,1.2));
			audioSource->Play();
		}
		break;
	case EEPlayerState::Running:
		animationProgress = CharacterFlipbook->GetPlaybackPosition();
		setDirectionalAnimation(direction,"run");
		CharacterFlipbook->SetPlaybackPosition(animationProgress,false);
		setAnimationRateToSpeed(CharacterFlipbook,CharacterMovementComp->Velocity.Length(),500);
		if(CharacterFlipbook->GetPlaybackPositionInFrames()==10 || CharacterFlipbook->GetPlaybackPositionInFrames()==22)
		{
			audioSource->Stop();
			audioSource->SetPitchMultiplier(FMath::FRandRange(1.1,1.2));
			audioSource->Play();
		}
		break;
	case EEPlayerState::Concealing:
		TimeConcealing += deltatime;
		InteractionBarEvent.Broadcast();
		if (TimeConcealing >= TimeToConceal) {
			TimeConcealing = 0;
			mPlayerState = EEPlayerState::Idle;
			Inventory->AddItem(Current_HeldItem);
			Current_HeldItem = nullptr;
			ConcealItemEvent.Broadcast();
			RefreshItemHUDEvent.Broadcast();
			InteractionBarEvent.Broadcast();
			Mesh_HeldItem->SetVisibility(false);
		}
		break;
	}
	
		
}

void AGame_PaperCharacter::endGamePass() {
	if (SusMeter >= SusMeterMax && mEndGame == false) {
		mEndGame = true;
		PostProcess->Settings.VignetteIntensity += 0.10f;
	}
}

void AGame_PaperCharacter::DetectionCheck(float DeltaTime) {
	if (isSeen) return;
	
	switch (mPlayerState) {
		case EEPlayerState::Concealing:
			SusMeter += DeltaTime;
			SusMeterChangeEvent.Broadcast();
			PostProcess->Settings.VignetteIntensity += DeltaTime / 8;
			break;
		case EEPlayerState::Running:
			SusMeter += DeltaTime * 0.5f;
			SusMeterChangeEvent.Broadcast();
			PostProcess->Settings.VignetteIntensity += DeltaTime / 8;
			break;
	}
	isSeen = true;
}
