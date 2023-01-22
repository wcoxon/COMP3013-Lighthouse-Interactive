// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBase.h"
#include "CustomerNPC.generated.h"

/**
 * 
 */
UCLASS()
class COMP3013_API ACustomerNPC : public ANPCBase
{
	GENERATED_BODY()
public:
	ACustomerNPC();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
