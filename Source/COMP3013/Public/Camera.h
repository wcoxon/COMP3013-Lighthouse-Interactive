// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game_PaperCharacter.h"
#include "Components/SpotLightComponent.h"
#include "Camera.generated.h"

UENUM(BlueprintType)
enum class EECameraState : uint8
{
	Off		    UMETA(DisplayName = "Off"),
	Idle	    UMETA(DisplayName = "Idle"),
	Rotating	UMETA(DisplayName = "Rotating"),
	Following	UMETA(DisplayName = "Following")
};

UCLASS()
class COMP3013_API ACamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACamera();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	EECameraState CameraState;
	
	void turnTowards(float target, float deltaTime);
	void StateManager(float deltaTime);
	void setState(EECameraState newState);
	bool detectsPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float Angle1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float Angle2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float turnSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float coneLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float coneAngle;

	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	bool bIsClockwise;
	float CurrentAngle;
	float IdleDuration;
	float IdleCurrentDuration;

	UPROPERTY()
	AGame_PaperCharacter* player;
	UPROPERTY()
	USpotLightComponent* coneLight;

};