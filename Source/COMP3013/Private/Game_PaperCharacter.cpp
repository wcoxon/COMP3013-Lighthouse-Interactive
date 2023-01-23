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
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Called when the game starts or when spawned
AGame_PaperCharacter::AGame_PaperCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//Construct Components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Inventory = CreateDefaultSubobject<UPlayerInvComponent>(TEXT("Inventory"));
	heldItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldMesh"));
	audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));
	SusAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SusAudioComponent"));
	
	//Attach Components
	SpringArm->SetupAttachment(CharacterCollider);
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	heldItemMesh->SetupAttachment(CharacterCollider);
	
	//Spring Arm Settings
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, -30.0f, 30.0f), FRotator(-30.0f, 90.0f, 0.0f));
	SpringArm->TargetArmLength = 650.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 9.0f;
	SpringArm->bDoCollisionTest = false;

	//HeldItemLocation
	heldItemMesh->SetRelativeLocationAndRotation(FVector(0, 0, 37), FRotator(0, 0, 90));
	heldItemMesh->SetRelativeScale3D(FVector(0.15f,0.15f,0.15f));
	heldItemMesh->CastShadow = false;
	
	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	CharacterFlipbook->SetCustomDepthStencilValue(100);
	CharacterCollider->SetRenderCustomDepth(true);
	CharacterCollider->SetCustomDepthStencilValue(100);
	CharacterFlipbook->BoundsScale = 3.5f;
	
	//Collider Settings
	CharacterCollider->SetCapsuleRadius(6.6f);
	
	//Movement System Settings
	moveSpeed=800;
	WalkSpeed=500.0f;
	CharacterMovementComp->MovementMode=MOVE_Walking;
	CharacterMovementComp->MaxWalkSpeed = WalkSpeed;
	CharacterMovementComp->MaxAcceleration = 3000.0f;
	CharacterMovementComp->BrakingFrictionFactor = 2.0f;
	CharacterMovementComp->BrakingDecelerationWalking = 1000.0f;
	direction = FVector::UpVector;

	//Player Defaults
	Suspicion = .0f;
	isSeen = false;
	
	//Assign HUD element
	static ConstructorHelpers::FClassFinder<UUserWidget> hudWidgetObj (TEXT ("/Game/UserInterface/WIDGET_Inventory"));
	if (hudWidgetObj.Succeeded ()) HUDWidgetClass = hudWidgetObj.Class;
	else HUDWidgetClass = nullptr;
	
	
}

void AGame_PaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	SavedController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	SavedController->Possess(this);

	//UE_LOG(LogTemp, Warning, TEXT("Player Controller: %s"), *SavedController->GetName());
	
	//Grab character animations
	animations.Add(FString("runLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunLeft64.PlayerRunLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunRight64.PlayerRunRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunUp64.PlayerRunUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunDown64.PlayerRunDown64"), NULL, LOAD_None, NULL));

	animations.Add(FString("runDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunDL64.PlayerRunDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunUL64.PlayerRunUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunDR64.PlayerRunDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("runUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/RunAnimation/PlayerRunUR64.PlayerRunUR64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("walkLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkLeft64.PlayerWalkLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkRight64.PlayerWalkRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkUp64.PlayerWalkUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkDown64.PlayerWalkDown64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("walkDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkDL64.PlayerWalkDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkUL64.PlayerWalkUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkDR64.PlayerWalkDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("walkUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/WalkAnimation/PlayerWalkUR64.PlayerWalkUR64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("idleLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleLeft64.PlayerIdleLeft64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleRight64.PlayerIdleRight64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleUp64.PlayerIdleUp64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleDown64.PlayerIdleDown64"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("idleDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleDL64.PlayerIdleDL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleUL64.PlayerIdleUL64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleDR64.PlayerIdleDR64"), NULL, LOAD_None, NULL));
	animations.Add(FString("idleUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/IdleAnimation/PlayerIdleUR64.PlayerIdleUR64"), NULL, LOAD_None, NULL));

	animations.Add(FString("stealLeft"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealLeft.StealLeft"), NULL, LOAD_None, NULL));
	animations.Add(FString("stealRight"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealRight.StealRight"), NULL, LOAD_None, NULL));
	animations.Add(FString("stealUp"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealUp.StealUp"), NULL, LOAD_None, NULL));
	animations.Add(FString("stealDown"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealDown.StealDown"), NULL, LOAD_None, NULL));
	
	animations.Add(FString("stealDL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealDL.StealDL"), NULL, LOAD_None, NULL));
	animations.Add(FString("stealUL"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealUL.StealUL"), NULL, LOAD_None, NULL));
	animations.Add(FString("stealDR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealDR.StealDR"), NULL, LOAD_None, NULL));
	animations.Add(FString("stealUR"),LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/Characters/Sprites/Player/StealAnimation/StealUR.StealUR"), NULL, LOAD_None, NULL));
	
	
	CharacterFlipbook->SetFlipbook(animations["idleUp"]);

	PostProcess = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	DirectionalLight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
	
	float spriteRes = 64.0;
	float spriteGroundLevel = 5.0;
	
	float actorScale = 300.0;
	
	float spriteRoll = FMath::DegreesToRadians(CharacterFlipbook->GetComponentRotation().Roll);
	
	float halfHeight = cos(spriteRoll)*(spriteRes/2-spriteGroundLevel);
	
	CharacterCollider->SetCapsuleHalfHeight(halfHeight);
	
	SetActorScale3D(FVector(actorScale/spriteRes));

	audioSource->Sound = LoadObject<USoundBase>(NULL,TEXT("/Game/ThirdParty/Sounds/footstep.footstep"),NULL,LOAD_None,NULL);
	audioSource->VolumeMultiplier = 0.7f;
	
	isSeen=false;

	RedLUT = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/Textures/LUTs/RedLUT.RedLUT")));
	
	EGCCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, TEXT("/Game/ThirdParty/Sounds/Bang.Bang")));
	SusCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, TEXT("/Game/ThirdParty/Sounds/Sus.Sus")));
	SusAudioComponent->SetSound(SusCue);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerInput->FlushPressedKeys();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());
}


void AGame_PaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Disable Static Noise
	SusAudioComponent->SetPitchMultiplier(Suspicion /50.f);
	if (!isSeen || Suspicion > 100.f) {
		SusAudioComponent->Stop();
	}
	
	if(inputVector.Length()>0)
	{
		this->AddMovementInput(inputVector.GetSafeNormal2D());
		direction=CharacterMovementComp->Velocity.GetSafeNormal2D();
	}
	if(CharacterMovementComp->Velocity.Length()>moveSpeed*0.8) currentState = Run;
	else if(CharacterMovementComp->Velocity.Length() > 0) currentState = Walk;
	else currentState = Idle;
	
	if (currentAction == conceal && currentState != Idle) {
		endAction();
		InteractionBarEvent.Broadcast();
	}
	
	if (currentAction!=nullAction) {
		//broadcast to update ui
		InteractionBarEvent.Broadcast();
	}
	
	StateManager(DeltaTime);

	OcclusionPass();

	SusMeterChange(DeltaTime);

	endGamePass(DeltaTime);
	
	isSeen = false;

}

//UNREAL DEFAULT FUNCTION - Bind inputs to functions
void AGame_PaperCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponents)
{
	Super::SetupPlayerInputComponent(InputComponents);
	// Bind the controls
	InputComponents->BindAction("Pickup", IE_Pressed, this, &AGame_PaperCharacter::Pickup);
	InputComponents->BindAction("Inventory", IE_Pressed, this, &AGame_PaperCharacter::OpenInventory);
	InputComponents->BindAction("Drop", IE_Pressed, this, &AGame_PaperCharacter::DropItem);
	InputComponents->BindAction("Conceal", IE_Pressed, this, &AGame_PaperCharacter::Conceal);
	InputComponents->BindAction("Sprint", IE_Pressed, this, &AGame_PaperCharacter::SprintOn);
	InputComponents->BindAction("Sprint", IE_Released, this, &AGame_PaperCharacter::SprintOff);
	InputComponents->BindAxis("MoveX", this, &AGame_PaperCharacter::Move_XAxis);
	InputComponents->BindAxis("MoveY", this, &AGame_PaperCharacter::Move_YAxis);
}

//Player movement functions for XY
void AGame_PaperCharacter::Move_XAxis(float AxisValue){
	inputVector.X = AxisValue;
}
void AGame_PaperCharacter::Move_YAxis(float AxisValue){
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
	if (Suspicion >= 100.f) return;
	if (heldItem == nullptr)
	{
		TArray<AActor*> Result;
		GetOverlappingActors(Result, AItem_Base::StaticClass());
		for (auto Actor : Result)
		{
			if (UKismetSystemLibrary::DoesImplementInterface(Actor, UInteraction::StaticClass()))
			{
				AItem_Base* CurrentItem = Cast<AItem_Base>(Actor);
				beginAction(grab,0.5f,FTimerDelegate::CreateUFunction(this,FName("grabItem"),CurrentItem->ItemToGive));
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

void AGame_PaperCharacter::DropItem() {
	if (heldItem != nullptr) {
		heldItem = nullptr;
		RefreshItemHUDEvent.Broadcast();
		heldItemMesh->SetVisibility(false);
	}
}

/** Player placing item in their inventory */
//checks conditions to begin concealing
void AGame_PaperCharacter::Conceal()
{
	if (heldItem != nullptr && Inventory->Capacity > Inventory->Items.Num() && currentAction != conceal && currentState == Idle)
	{
		beginAction(conceal,heldItem->Duration,FTimerDelegate::CreateUFunction(this,FName("concealItem")));
	}
}

void AGame_PaperCharacter::grabItem(UItem* item)
{
	//function for the grab item action which puts an item object into held slot for npcs or player
	endAction();
	InteractionBarEvent.Broadcast();
	
	heldItem = item;
	RefreshItemHUDEvent.Broadcast();
	
	heldItemMesh->SetStaticMesh(item->Mesh);
	heldItemMesh->SetVisibility(true);
	
}

void AGame_PaperCharacter::concealItem()
{
	//clears the progress UI now that action is complete
	endAction();
	InteractionBarEvent.Broadcast();
	
	Inventory->AddItem(heldItem);
	heldItem = nullptr;
	RefreshItemHUDEvent.Broadcast();
	
	//broadcasts conceal event to npcs
	ConcealItemEvent.Broadcast();
	
	heldItemMesh->SetVisibility(false);
}


/** Handles actions based on PlayerState */
void AGame_PaperCharacter::StateManager(float deltatime) {

	float animationProgress;

	switch(currentAction)
	{
	case grab:
		if (currentState == MovementState::Idle) {
			setDirectionalAnimation(direction,"steal");
			CharacterFlipbook->SetPlayRate(CharacterFlipbook->GetFlipbookLength()/GetWorldTimerManager().GetTimerRate(actionTimerHandle));
			return;
		}
	default:
		break;
		
	}
	
	switch (currentState) {
	case Idle:
		setDirectionalAnimation(direction,"idle");
		CharacterFlipbook->SetPlayRate(1);
		break;
	case Walk:
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
	case Run:
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
	
	}

	
}

void AGame_PaperCharacter::endGamePass(float deltaTime) {

	if (endPass < 2) {
		if (Suspicion >= 100.0f && !endPass) {
			endPass = 1;
			DirectionalLight->GetLightComponent()->SetIntensity(0.0f);
			PostProcess->Settings.VignetteIntensity += 0.3f;
			PostProcess->Settings.ColorGradingLUT = RedLUT;
			SusMaxEvent.Broadcast();
			UGameplayStatics::PlaySound2D(GetWorld(), EGCCue, 1.0f, 1.0f, 0.0f);
		}

		if (endPass && EGCTimer > 0.0f) EGCTimer -= deltaTime;

		if (EGCTimer <= 0.0f && endPass) {
			EGCOverEvent.Broadcast();
			endPass = 2;
		}
	}
}

void AGame_PaperCharacter::OcclusionPass() const {
	
	FVector playerLocation = GetActorLocation() + FVector(0.f,0.f,20.f);
	FVector cameraLocation = Camera->GetComponentLocation();
	FHitResult hitResult;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(this, playerLocation, cameraLocation, ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::None, hitResult, true);
	if (!bHit) {
		PostProcess->Settings.WeightedBlendables.Array[2].Weight = 0;
	} else {
		PostProcess->Settings.WeightedBlendables.Array[2].Weight = 1;
	}
}

void AGame_PaperCharacter::SusMeterChange(float DeltaTime) {
	if(!isSeen) return;
	
	switch(currentAction)
	{
	case conceal:
		{
			Suspicion += 35.0f*DeltaTime;
			if (!SusAudioComponent->IsPlaying() && Suspicion < 100.f) {
				SusAudioComponent->Play();
			}
		}
		break;
				
	default:
		break;
	}
	
	switch (currentState)
	{
	case Run:
		{
			Suspicion+= 20.0f*DeltaTime;
			if (!SusAudioComponent->IsPlaying() && Suspicion < 100.f) {
				SusAudioComponent->Play();
			}
		}
		break;
				
	default:
		break;
	}
	
	SusMeterChangeEvent.Broadcast();

	if ((currentState != Run && currentAction != conceal) || Suspicion > 100.f) {
		SusAudioComponent->Stop();
	}
	//resets isSeen to false, it's more like "was seen since last suspicion checks" so like now ive done them i haven't
	//had an npc detect me since. i'll now know at the beginning of next tick if one of the npcs knocked this back to true
	isSeen = false;
}
