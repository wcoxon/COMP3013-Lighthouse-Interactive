// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBase.h"
#include "SecurityNPC.generated.h"

/**
 * 
 */
UCLASS()
class COMP3013_API ASecurityNPC : public ANPCBase
{
	GENERATED_BODY()
public:
	ASecurityNPC();

	void EgcOn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundFX")
	UAudioComponent* TriggerSound;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
private:
	USoundCue* PunchSoundCue;

	USoundCue* TriggerSoundCue;
	
	bool Caught = false;
};
