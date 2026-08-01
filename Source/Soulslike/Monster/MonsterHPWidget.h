// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHPWidget.generated.h"

/**
 * 
 */

class AMonsterBase;

UCLASS()
class SOULSLIKE_API UMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMonsterBase> Monster;

public:
	UFUNCTION(BlueprintCallable)
	void SetMonster(AMonsterBase* InMonster);
	
};
