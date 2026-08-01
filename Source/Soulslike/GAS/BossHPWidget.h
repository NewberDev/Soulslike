// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHPWidget.generated.h"



/**
 * 
 */

class AMyBossCharacter;

UCLASS()
class SOULSLIKE_API UBossHPWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AMyBossCharacter* Boss;

public:
	void SetBoss(AMyBossCharacter* InBoss);

};
