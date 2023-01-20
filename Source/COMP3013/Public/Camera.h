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
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	EECameraState CameraState;
	
	void turnTowards(float target, float deltaTime, float speed);
	void StateManager(float deltaTime);
	void setState(EECameraState newState);
	bool detectsPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float Angle1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float Angle2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float turnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float coneLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float coneAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MinWait;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MaxWait;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Mesh")
	UStaticMeshComponent* CameraBase;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Mesh")
	UStaticMeshComponent* CameraBody;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Mesh")
	USceneComponent* CameraRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Light")
	USpotLightComponent* coneLight;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	bool bIsClockwise;
	float CurrentAngle;
	float IdleDuration;
	float IdleCurrentDuration;

	float currentUnfollowingDuration;
	float maxUnfollowDuration;
	
	UPROPERTY()
	AGame_PaperCharacter* player;
	

};