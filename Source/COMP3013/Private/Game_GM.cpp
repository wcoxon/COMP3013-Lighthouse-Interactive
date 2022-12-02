// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_GM.h"

#include "PlayerHUD.h"


AGame_GM::AGame_GM()
{
	PrimaryActorTick.bCanEverTick = false;

	HUDClass = APlayerHUD::StaticClass();
}
