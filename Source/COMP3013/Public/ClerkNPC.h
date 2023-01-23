// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBase.h"
#include "SecurityNPC.h"
#include "ClerkNPC.generated.h"

/**
 * 
 */
UCLASS()
class COMP3013_API AClerkNPC : public ANPCBase
{
	GENERATED_BODY()
public:
	AClerkNPC();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioComponent")
	UAudioComponent* TattleaudioComponent;

	UFUNCTION()
	void EgcOn();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
private:
	UPROPERTY()
	ASecurityNPC* securityGuard;
	UPROPERTY()
	FVector playerLastSeen;
	UPROPERTY()
	USoundCue* TattleSound;
};
