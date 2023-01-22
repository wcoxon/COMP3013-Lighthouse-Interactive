// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Item.h"
#include "PaperCharacter.h"
#include "Agent_PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
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

UENUM(BlueprintType)
enum MovementState{
	Idle       UMETA(DisplayName="Idle"),
	Walk        UMETA(DisplayName="Walking"),
	Run        UMETA(DisplayName="Running"),
};

/*DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConcealItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshItemHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionBar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSuspicionMeterChange);*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConcealItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshItemHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionBar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSusMeterChange);

UCLASS()
class COMP3013_API AGame_PaperCharacter : public AAgent_PaperCharacter
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
	//UPROPERTY(VisibleAnywhere)
	//UStaticMeshComponent* Mesh_HeldItem;

	//Postproceser
	UPROPERTY(VisibleAnywhere)
	APostProcessVolume* PostProcess;

	//Animations
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
	//Player input functions and data
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void SprintOn();
	void SprintOff();
	void Pickup();
	void OpenInventory();
	//method to bind to conceal button
	void Conceal();
	FVector inputVector;
	bool PlayerStateChange = false;

	//method for actually concealing the item
	UFUNCTION()
	void concealItem();
	UFUNCTION()
	void grabItem(UItem* item);

	MovementState currentState;
	
	//Direct asset reference -> Inventory HUD widget
	class UClass* HUDWidgetClass;
	class UUserWidget* HUDWidgetMain;
	
	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void Destroy(UItem* Item);

	float WalkSpeed;
	
	//Suspicion meter
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sussy")
	float Suspicion= .0f;

	UPROPERTY(BlueprintAssignable)
	FRefreshItemHUD RefreshItemHUDEvent;
	
	UPROPERTY(BlueprintAssignable, Category = "Sussy")
	FSusMeterChange SusMeterChangeEvent;
	
	UPROPERTY(BlueprintAssignable)
	FInteractionBar InteractionBarEvent;

	UPROPERTY(BlueprintAssignable)
	FConcealItem ConcealItemEvent;
	//FInteractionBar actionProgressEvent;

	//UPROPERTY(BlueprintAssignable)
	//FSuspicionMeterChange suspicionEvent;
	
	//represents whether the player was seen since the last tick
	UPROPERTY(BlueprintReadOnly)
	bool isSeen = false;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void StateManager(float deltatime);

	void OcclusionPass() const;

	void SusMeterChange(float DeltaTime);

	AController* SavedController;
	
};
