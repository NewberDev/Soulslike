// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h" //위젯 접근 헤더
#include "BossHPWidget.h"

#include "SoulHUD.generated.h"

/**
 * 
 */


class AMyBossCharacter;


UCLASS()
class SOULSLIKE_API ASoulHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AMyBossCharacter* BossCharacter;


public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SoulBaseWidget;

	UPROPERTY()
	UUserWidget* SoulBaseWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBossHPWidget> BossHPWidget;

	UPROPERTY()
	UBossHPWidget* BossHPWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> StartWidget;

	UPROPERTY()
	UUserWidget* StartWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> EndWidget;

	UPROPERTY()
	UUserWidget* EndWidgetInstance;

public:

	void SetBoss(AMyBossCharacter* Character);

	UFUNCTION(BlueprintCallable) 
	void ShowSoulBase();

	UFUNCTION(BlueprintCallable)
	void ShowBossHP();

	//UFUNCTION(BlueprintCallable)
	//void ShowEnemyFindUi();

	UFUNCTION(BlueprintCallable)
	void RemoveUI(UUserWidget* WidgetInstance);

	
};
