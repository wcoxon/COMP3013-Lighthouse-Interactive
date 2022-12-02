// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "Blueprint/UserWidget.h"

APlayerHUD::APlayerHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> hudWidgetObj (TEXT ("/Game/UserInterface/UserHUD"));
	if (hudWidgetObj.Succeeded ()) WidgetClass = hudWidgetObj.Class;
	else WidgetClass = nullptr;
}


void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass)
	{
		WidgetMain = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		WidgetMain->AddToViewport();
	}

}
