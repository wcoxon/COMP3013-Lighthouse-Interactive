// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FramePro/FramePro.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Called when the game starts or when spawned
AGame_PaperCharacter::AGame_PaperCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Construct Camera & Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	//Attach Components
	SpringArm->SetupAttachment(CharacterCollider);
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);

	//Spring Arm Settings
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, -30.0f, 30.0f), FRotator(-40.0f, 90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;
	SpringArm->bDoCollisionTest = false;

	//Enable Render Buffer - Used for LOS colour
	CharacterFlipbook->SetRenderCustomDepth(true);
	CharacterCollider->SetRenderCustomDepth(true);
	CharacterFlipbook->BoundsScale = 10.0f;
	
	//Collider Settings
	CharacterCollider->SetCapsuleHalfHeight(6.6f);
	CharacterCollider->SetCapsuleRadius(6.6f);

	//Movement System Settings
	//CharacterMovementComp->DefaultLandMovementMode = MOVE_Flying;
	CharacterMovementComp->MaxFlySpeed = 300.0f;
	CharacterMovementComp->BrakingDecelerationFlying = 4000.0f;
	CharacterMovementComp->bRequestedMoveUseAcceleration = false;
	CharacterMovementComp->MaxAcceleration = 8000.0f;
	CharacterMovementComp->BrakingFrictionFactor = 50.0f;
	
}

void AGame_PaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Grab character animations
	IdleDownAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/Idle_Anim.Idle_Anim"), NULL, LOAD_None, NULL);
	IdleUpAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/UpIdle_Anim.UpIdle_Anim"), NULL, LOAD_None, NULL);
	IdleLeftAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/LeftIdle_Anim.LeftIdle_Anim"), NULL, LOAD_None, NULL);
	IdleRightAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/RightIdle_Anim.RightIdle_Anim"), NULL, LOAD_None, NULL);
	MovingDownAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/WalkDown_Anim.WalkDown_Anim"), NULL, LOAD_None, NULL);
	MovingUpAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/WalkUp_Anim.WalkUp_Anim"), NULL, LOAD_None, NULL);
	MovingLeftAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/WalkLeft_Anim.WalkLeft_Anim"), NULL, LOAD_None, NULL);
	MovingRightAnim = LoadObject<UPaperFlipbook>(NULL, TEXT("/Game/PrototypeAssets/WalkRight_Anim.WalkRight_Anim"), NULL, LOAD_None, NULL);
	CharacterFlipbook->SetFlipbook(IdleDownAnim);
	CharacterFlipbook->CastShadow = true;

	//Collider Settings fix
	CharacterCollider->SetCapsuleHalfHeight(6.6f);

	//Make character face camera at all times
	FRotator SpringArmRotation = SpringArm->GetComponentRotation();
	SpringArmRotation.Roll = -SpringArmRotation.Pitch;
	SpringArmRotation.Yaw = 0.0f;
	SpringArmRotation.Pitch = 0.0f;
	CharacterFlipbook->SetWorldRotation(SpringArmRotation);
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
void AGame_PaperCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponents)
{
	Super::SetupPlayerInputComponent(InputComponents);
	
	// Bind the controls
	InputComponents->BindAxis("MoveX", this, &AGame_PaperCharacter::Move_XAxis);
	InputComponents->BindAxis("MoveY", this, &AGame_PaperCharacter::Move_YAxis);
}

void AGame_PaperCharacter::Move_XAxis(float AxisValue)
{
	if (AxisValue) PlayerDirection = Direction::MovingRight;
	if (AxisValue == -1) PlayerDirection = Direction::MovingLeft;
	
	// Move at 100 units per second forward or backward
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
	this->AddMovementInput(this->GetActorForwardVector() * CurrentVelocity.X);
}

void AGame_PaperCharacter::Move_YAxis(float AxisValue)
{
	if (AxisValue) PlayerDirection = Direction::MovingUp;
	if (AxisValue == -1) PlayerDirection = Direction::MovingDown;
	
	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
	this->AddMovementInput(this->GetActorRightVector() * CurrentVelocity.Y);
}