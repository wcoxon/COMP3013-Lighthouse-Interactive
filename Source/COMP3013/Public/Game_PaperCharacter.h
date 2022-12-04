// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Item.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Game_PaperCharacter.generated.h"

/**
 * 
 */

//Direction Enums
UENUM(BlueprintType)
enum class Direction : uint8 {
	Down       UMETA(DisplayName="Down"),
	Up        UMETA(DisplayName="Up"),
	Left        UMETA(DisplayName="Left"),
	Right        UMETA(DisplayName="Right"),
	MovingDown       UMETA(DisplayName="Down"),
	MovingUp        UMETA(DisplayName="Up"),
	MovingLeft        UMETA(DisplayName="Left"),
	MovingRight        UMETA(DisplayName="Right")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPickupItem);

UCLASS()
class COMP3013_API AGame_PaperCharacter : public APaperCharacter
{
	GENERATED_BODY()


public:
	AGame_PaperCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	//Spring Arm
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	
	//Camera
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	//Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	class UPlayerInvComponent* Inventory;

	//HeldItemMesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh_HeldItem;
	
	//Animations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* IdleDownAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* IdleUpAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* IdleLeftAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* IdleRightAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* MovingDownAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* MovingUpAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* MovingLeftAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animations)
	UPaperFlipbook* MovingRightAnim;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	//Player input functions and data
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void Pickup();
	void OpenInventory();
	void Conceal();
	FVector CurrentVelocity;
	bool PlayerStateChange = false;

	//Direct asset reference -> Inventory HUD widget
	class UClass* HUDWidgetClass;
	class UUserWidget* HUDWidgetMain;

	//Direction Enum
	UPROPERTY(VisibleAnywhere, Category = Enums)
	Direction PlayerDirection = Direction::Down;

	//PLAYER VARS
	UPROPERTY(VisibleAnywhere, Category = Stats)
	FString P_HeldItem = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	UItem* Current_HeldItem;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Destroy(UItem* Item);

	UPROPERTY(BlueprintAssignable)
	FPickupItem PickupItemEvent;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:

	//Grab default components of class
	UPaperFlipbookComponent* CharacterFlipbook = GetSprite();
	UCapsuleComponent* CharacterCollider = GetCapsuleComponent();
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	
};
