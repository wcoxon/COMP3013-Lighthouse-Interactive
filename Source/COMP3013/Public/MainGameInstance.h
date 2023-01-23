// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EDay {Monday, Tuesday, Wednesday, Thursday, Friday};

UCLASS()
class COMP3013_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	TEnumAsByte<EDay> CurrentDay = EDay::Monday;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float Money = 100.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float NextDetuctable = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool CaughtState = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float GainLossAmount = 0.0f;

	UFUNCTION()
	void AddMoney(float amount);

	UFUNCTION()
	void DeductMoney(float amount);

	UFUNCTION()
	void SetCaughtState(bool state) { CaughtState = state; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void NextDay();

	UFUNCTION()
	void BalanceCheck();
	
	UFUNCTION()
	void ResetGame();
	
};
