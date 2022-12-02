// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class COMP3013_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	APlayerHUD();

public:
	class UClass* WidgetClass;
	class UUserWidget* WidgetMain;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
};
