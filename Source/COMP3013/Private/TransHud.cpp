// Fill out your copyright notice in the Description page of Project Settings.


#include "TransHud.h"

#include "Blueprint/UserWidget.h"

ATransHud::ATransHud() {
	static ConstructorHelpers::FClassFinder<UUserWidget> hudWidgetObj (TEXT ("/Game/UserInterface/Transition/TransUI"));
	if (hudWidgetObj.Succeeded ()) WidgetClass = hudWidgetObj.Class;
	else WidgetClass = nullptr;
}

void ATransHud::BeginPlay() {
	Super::BeginPlay();

	if (WidgetClass)
	{
		WidgetMain = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		WidgetMain->AddToViewport();
	}
}
