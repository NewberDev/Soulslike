// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h" //위젯 접근 헤더
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AMyHUD : public AHUD
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> EnemyFindWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> EndingWidget;

	UPROPERTY()
	UUserWidget* EnemyFindWidgetInstance;
	
public:
	void ShowEndingUi();

	void ShowEnemyFindUi();

	void RemoveUI();
};
