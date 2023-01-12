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
enum class EEPlayerState : uint8 {
	Idle       UMETA(DisplayName="Idle"),
	Walking        UMETA(DisplayName="Walking"),
	Running        UMETA(DisplayName="Running"),
	Concealing        UMETA(DisplayName="Concealing"),
};

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
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh_HeldItem;

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
	void Conceal();
	FVector inputVector;
	bool PlayerStateChange = false;

	//Direct asset reference -> Inventory HUD widget
	class UClass* HUDWidgetClass;
	class UUserWidget* HUDWidgetMain;

	//Direction Enum
	UPROPERTY(VisibleAnywhere, Category = "Enums")
	Direction PlayerDirection = Direction::Down;

	//Player State Enum
	UPROPERTY(VisibleAnywhere, Category = "Enums")
	EEPlayerState mPlayerState = EEPlayerState::Idle;
	UPROPERTY(VisibleAnywhere, Category = "Enums")
	EEPlayerState mPreviousState = EEPlayerState::Idle;

	
	//PLAYER VARS
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	FString P_HeldItem = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItem* Current_HeldItem;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Destroy(UItem* Item);

	UPROPERTY(BlueprintAssignable)
	FRefreshItemHUD RefreshItemHUDEvent;

	float WalkSpeed;
	

	//Suspiscison meter
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sussy")
	float SusMeter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sussy")
	float SusMeterMax = 2.0f;

	UPROPERTY(BlueprintAssignable, Category = "Sussy")
	FSusMeterChange SusMeterChangeEvent;
	
	UPROPERTY(VisibleAnywhere, Category = "Sussy")
	bool mEndGame = false;
	
	UFUNCTION(BlueprintCallable, Category = "Sussy")
	void endGamePass();
	
	//Concealing Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Concealing")
	float TimeConcealing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Concealing")
	float TimeToConceal = 3.0f;

	UPROPERTY(BlueprintAssignable)
	FConcealItem ConcealItemEvent;

	UPROPERTY(BlueprintAssignable)
	FInteractionBar InteractionBarEvent;

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void DetectionCheck(float DeltaTime);
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void StateManager(float deltatime);

	//Player Seen Data
	bool isSeen;

};
