// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"

void UMainGameInstance::AddMoney(float amount) {
	Money += amount;
	GainLossAmount = amount;
}

void UMainGameInstance::DeductMoney(float amount) {
	Money -= amount;
	GainLossAmount = -amount;
}

void UMainGameInstance::NextDay() {
	switch (CurrentDay) {
		case Monday: CurrentDay = Tuesday; break;
		case Tuesday: CurrentDay = Wednesday; break;
		case Wednesday: CurrentDay = Thursday; break;
		case Thursday: CurrentDay = Friday; break;
		case Friday:
			CurrentDay = Monday;
			DeductMoney(NextDetuctable);
			NextDetuctable += 20;
		break;
	}
}

void UMainGameInstance::BalanceCheck() {
	if (Money <= 0) {
		ResetGame();
	}
}

void UMainGameInstance::ResetGame() {
	Money = 100.f;
	CurrentDay = Monday;
	NextDetuctable = 100.f;
}
