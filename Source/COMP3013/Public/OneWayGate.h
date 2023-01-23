// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "OneWayGate.generated.h"


enum class EGateState : uint8
{
	Idle,
	Opening,
	Closing,
	Open
};

UCLASS()
class COMP3013_API AOneWayGate : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOneWayGate();

	EGateState GateState = EGateState::Idle;

	float currentAngle;

	float targetAngleUp;
	float targetAngleDown;
	float defaultAngle;

	bool Up = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void onInteract() override;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoundsBoxTop;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoundsBoxBottom;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Gate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	float OpenAngleUp = 89.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	float OpenAngleDown = 89.f;

	
	UFUNCTION()
	void BoundBoxTopEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BoundBoxTopExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void BoundBoxBottomEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BoundBoxBottomExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void GateStateManager();

	void TurnTowards(float target, float DeltaTime);

private:
	float Timer = 2.f;
};
